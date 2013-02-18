/*  librpi2d - Raspberry Pi 2D  lightweight library to access Raspberry PI's
    hardware video surface through OpenGL ES.
    Copyright (C) 2013 neagix

	https://github.com/neagix/librpi2d

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "rpi2d.h"
#include "glDebug.h"

const GLushort indices[] = {0, 1, 2, 0, 2, 3};

const GLbyte vShaderSrc[] =
        "attribute vec4 a_position;   \n"
        "attribute vec2 a_texCoord;   \n"
        "varying vec2 v_texCoord;     \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = a_position; \n"
        "   v_texCoord = a_texCoord;  \n"
        "}                            \n";

const GLbyte fShaderSrc[] =
        "precision mediump float;                            \n"
        "varying vec2 v_texCoord;                            \n"
        "uniform sampler2D s_texture;                        \n"
        "void main()                                         \n"
        "{                                                   \n"
        "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
        "}                                                   \n";


static GLfloat fullVertices[] = {
    -1, 1, 0.0f, // Position 0
    0.0f, 0.0f, // TexCoord 0 
    -1, -1, 0.0f, // Position 1
    0.0f, 1.0f, // TexCoord 1
    1, -1, 0.0f, // Position 2
    1.0f, 1.0f, // TexCoord 2
    1, 1, 0.0f, // Position 3
    1.0f, 0.0f // TexCoord 3
};

Raspberry2D::Raspberry2D(int surfaceWidth, int surfaceHeight) {
    this->Width = surfaceWidth;
    this->Height = surfaceHeight;

    RedSize = 5;
    GreenSize = 6;
    BlueSize = 5;

    BackgroundColor[0] = BackgroundColor[1] = BackgroundColor[2] = .0f;

    AlphaSize = EGL_DONT_CARE;
    DepthSize = EGL_DONT_CARE;
    StencilSize = EGL_DONT_CARE;
    MultiSample = false;

    infoLog = NULL;
    programObject = fragmentShader = vertexShader = 0;

    // initialize BCM host driver
    bcm_host_init();
    
    // get size of LCD (index 0)
    int success = graphics_get_display_size(0, &DisplayWidth, &DisplayHeight);
    if (success < 0)
        throw "Could not get display size";
}

bool Raspberry2D::Attach() {
    return Attach(DisplayWidth, DisplayHeight);
}

bool Raspberry2D::Attach(int scaleW, int scaleH) {
    if (!this->CreateNativeWindow(scaleW, scaleH)) {
        //        fprintf(stderr, "Could not create native window\n");
        return false;
    }
    if (!this->CreateEGLContext()) {
        //        fprintf(stderr, "Could not create EGL context\n");
        return false;
    }

    // we use a simple shader to render the 2D texture
    Create2DShader();

    // set everything to black
    glClearColor(BackgroundColor[0], BackgroundColor[1], BackgroundColor[2], 1.0f);
    __GL_DEBUG__;
    
    // Set the viewport
    glViewport(0, 0, this->Width, this->Height);
    __GL_DEBUG__;

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    __GL_DEBUG__;

    if (AlphaSize > 0) {
        glClear(GL_DEPTH_BUFFER_BIT);
        __GL_DEBUG__;
        //        glShadeModel(GL_SMOOTH);

        glEnable(GL_DEPTH_TEST);
        __GL_DEBUG__;

        glEnable(GL_BLEND);
        __GL_DEBUG__;
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        __GL_DEBUG__;
    }

    // Use the program object
    glUseProgram(programObject);
    __GL_DEBUG__;    

    return true;
}

/*
 * initialize BCM native display
 */
bool Raspberry2D::CreateNativeWindow(int scaleW, int scaleH) {
    VC_RECT_T dst_rect;
    VC_RECT_T src_rect;

    dst_rect.x = (DisplayWidth - scaleW) / 2;
    dst_rect.y = (DisplayHeight - scaleH) / 2;
    dst_rect.width = scaleW;
    dst_rect.height = scaleH;

    src_rect.x = 0;
    src_rect.y = 0;
    
    //TODO: is this always desirable/correct?
    src_rect.width = Width << 16;
    src_rect.height = Height << 16;

    dispman_display = vc_dispmanx_display_open(0 /* LCD */);
    dispman_update = vc_dispmanx_update_start(0);

    dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display,
            0/*layer*/, &dst_rect, 0/*src*/,
            &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T) 0/*transform*/);

    nativeW.element = dispman_element;
    nativeW.width = Width;
    nativeW.height = Height;
    vc_dispmanx_update_submit_sync(dispman_update);

    // store handle to native window
    this->hWnd = &nativeW;

    return true;
}

bool Raspberry2D::CreateEGLContext() {
    EGLint numConfigs;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    // get default display
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY)
        return false;

    // initialize EGL
    if (!eglInitialize(display, &majorVersion, &minorVersion)) {
        return false;
    }

    // get available configurations
    if (!eglGetConfigs(display, NULL, 0, &numConfigs))
        return false;

    EGLint attributesList[] = {
        EGL_RED_SIZE, RedSize,
        EGL_GREEN_SIZE, GreenSize,
        EGL_BLUE_SIZE, BlueSize,
        EGL_ALPHA_SIZE, AlphaSize,
        EGL_DEPTH_SIZE, DepthSize,
        EGL_STENCIL_SIZE, StencilSize,
        EGL_SAMPLE_BUFFERS, MultiSample ? 1 : 0,
        EGL_NONE
    };

    // choose 1st available configuration with our requirements attributes
    if (!eglChooseConfig(display, attributesList, &config, 1, &numConfigs))
        return false;

    // create a rendering surface
    surface = eglCreateWindowSurface(display, config, hWnd, NULL);
    if (surface == EGL_NO_SURFACE)
        return false;

    // create GL context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
        return false;

    // set such context as current
    if (!eglMakeCurrent(display, surface, surface, context))
        return false;

    return true;
}

Raspberry2D::~Raspberry2D() {
    if (infoLog != NULL)
        free(infoLog);

    /// (1) drop all 2D rendering shaders and program
    if (programObject)
        glDeleteProgram(programObject);

    // Free up no longer needed shader resources
    /*    if (vertexShader)
            glDeleteShader(vertexShader);
        if (fragmentShader)
            glDeleteShader(fragmentShader); */

    /// (2) drop OpenGL rendering context

    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);

    eglTerminate(display);

    // (3) drop native display

    vc_dispmanx_element_remove(dispman_update, dispman_element);

    bcm_host_deinit();
}

/*
 * swap buffers e.g. cause a display repaint from drawing buffer
 */
void Raspberry2D::SwapBuffers() {
    eglSwapBuffers(display, surface);
    __GL_DEBUG__;
}

void Raspberry2D::makeVertices(Rect src, Rect dst, GLfloat *vVertices) const {
    int i = 0;
    // -1, 1, 0.0f, // Position 0
    vVertices[i++] = dst.x1;
    vVertices[i++] = dst.y2;
    vVertices[i++] = 0.0f;
    // 0.0f, 0.0f, // TexCoord 0 
    vVertices[i++] = src.x1;
    vVertices[i++] = src.y1;
    // -1, -1, 0.0f, // Position 1
    vVertices[i++] = dst.x1;
    vVertices[i++] = dst.y1;
    vVertices[i++] = 0.0f;
    // 0.0f, 1.0f, // TexCoord 1
    vVertices[i++] = src.x1;
    vVertices[i++] = src.y2;
    // 1, -1, 0.0f, // Position 2
    vVertices[i++] = dst.x2;
    vVertices[i++] = dst.y1;
    vVertices[i++] = 0.0f;
    // 1.0f, 1.0f, // TexCoord 2
    vVertices[i++] = src.x2;
    vVertices[i++] = src.y2;
    // 1, 1, 0.0f, // Position 3
    vVertices[i++] = dst.x2;
    vVertices[i++] = dst.y2;
    vVertices[i++] = 0.0f;
    // 1.0f, 0.0f // TexCoord 3
    vVertices[i++] = src.x2;
    vVertices[i++] = src.y1;

#if     0
    printf("vVertices = {");
    printf("\tp0: %.2f\t%.2f\t%.2f\n", vVertices[0], vVertices[1], vVertices[2]);
    printf("\tt0: %.2f\t%.2f\n", vVertices[3], vVertices[4]);
    printf("\tp1: %.2f\t%.2f\t%.2f\n", vVertices[5], vVertices[6], vVertices[7]);
    printf("\tt1: %.2f\t%.2f\n", vVertices[8], vVertices[9]);
    printf("\tp2: %.2f\t%.2f\t%.2f\n", vVertices[10], vVertices[11], vVertices[12]);
    printf("\tt2: %.2f\t%.2f\n", vVertices[13], vVertices[14]);
    printf("\tp3: %.2f\t%.2f\t%.2f\n", vVertices[15], vVertices[16], vVertices[17]);
    printf("\tt3: %.2f\t%.2f\n", vVertices[18], vVertices[19]);
    printf("}\n");
#endif
}

/*
 * draw texture with scaling, fully covering viewport
 */
void Raspberry2D::Draw(const Texture2D &tex) const {
    Draw(tex, fullSource, fullDest);
}

void Raspberry2D::DrawHFlip(const Texture2D &tex) const {
    Draw(tex, fullSource, fullHflipDest);
}

void Raspberry2D::DrawVFlip(const Texture2D &tex) const {
    Draw(tex, fullSource, fullVflipDest);
}

void Raspberry2D::DrawHVFlip(const Texture2D &tex) const {
    Draw(tex, fullSource, fullHVflipDest);
}

void Raspberry2D::normalizeRect(Rect *r) const {
    // invert y and normalize
    r->x1 = -1 + r->x1 * 2 / Width;
    r->x2 = -1 + r->x2 * 2 / Width;
    r->y1 = -1 + (2 - r->y1 * 2 / Height);
    r->y2 = -1 + (2 - r->y2 * 2 / Height);
}

void Raspberry2D::DrawTile(const Texture2D &tex, int tileHeight, int tileIndex, int x, int y) const {
    Rect tileRect;
    tex.WriteTileRect(tileHeight, tileIndex, &tileRect);

    printf("tileRect is %.2f, %.2f -> %.2f, %.2f\n", tileRect.x1, tileRect.y1, tileRect.x2, tileRect.y2);

    Rect dst;

    dst.x1 = x;
    dst.y1 = y;
    dst.x2 = x + tex.Width;
    dst.y2 = y + tileHeight;

    // normalize to a centered viewport
    normalizeRect(&dst);

    Draw(tex, tileRect, dst);
}

void Raspberry2D::DrawTile(const Texture2D &tex, Rect srcRect, int x, int y) const {
    Rect dst;

    dst.x1 = x;
    dst.y1 = y;
    dst.x2 = x + tex.Width;
    dst.y2 = y + tex.Height;

    // normalize to a centered viewport
    normalizeRect(&dst);

    Draw(tex, srcRect, dst);
}

void Raspberry2D::DrawTile(const Texture2D &tex, int x, int y) const {
    DrawTile(tex, fullSource, x, y);
}

void Raspberry2D::Draw(const Texture2D &tex, Rect src, Rect dst) const {
    // create the vertices
    GLfloat vVertices[20];
    makeVertices(src, dst, vVertices);

    // Load the vertex position
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT,
            GL_FALSE, 5 * sizeof (GLfloat), vVertices);
    __GL_DEBUG__;
    // Load the texture coordinate
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT,
            GL_FALSE, 5 * sizeof (GLfloat), &vVertices[3]);
    __GL_DEBUG__;

    glEnableVertexAttribArray(positionLoc);
    __GL_DEBUG__;
    glEnableVertexAttribArray(texCoordLoc);
    __GL_DEBUG__;

    // bind to selected texture
    tex.Bind();

    // Set the sampler texture unit to 0
    glUniform1i(samplerLoc, 0);
    __GL_DEBUG__;

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    __GL_DEBUG__;
}

//
///
/// \brief Load a shader, check for compile errors, print error messages to output log
/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
/// \param shaderSrc Shader source string
/// \return A new shader object on success, 0 on failure
//

GLuint Raspberry2D::esLoadShader(GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
        return 0;

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);
    __GL_DEBUG__;

    // Compile the shader
    glCompileShader(shader);
    __GL_DEBUG__;

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    __GL_DEBUG__;

    if (!compiled) {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        __GL_DEBUG__;

        if (infoLen > 1) {
            if (infoLog != NULL)
                free(infoLog);
            infoLog = (char *) malloc(sizeof (char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            throw infoLog;
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

//
///
/// \brief Load a vertex and fragment shader, create a program object, link program.
//         Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
//

void Raspberry2D::loadProgram(const char *vShaderStr, const char *fShaderStr) {
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = esLoadShader(GL_VERTEX_SHADER, vShaderStr);
    if (vertexShader == 0)
        throw "Could not create vertex shader";

    fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fShaderStr);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        throw "Could not create fragment shader";
    }

    // Create the program object
    programObject = glCreateProgram();
    __GL_DEBUG__;
    if (programObject == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        throw "Could not create program object";
    }

    glAttachShader(programObject, vertexShader);
    __GL_DEBUG__;
    glAttachShader(programObject, fragmentShader);
    __GL_DEBUG__;

    // Link the program
    glLinkProgram(programObject);
    __GL_DEBUG__;

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    __GL_DEBUG__;

    if (!linked) {
        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            if (infoLog != NULL)
                free(infoLog);

            infoLog = (char *) malloc(sizeof (char) * infoLen);

            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);

            throw ( "Error linking program:\n%s\n", infoLog);
        }

        glDeleteProgram(programObject);

        throw "Could not link program object";
    }

    // Free up no longer needed shader resources
    glDeleteShader(vertexShader);
    __GL_DEBUG__;
    glDeleteShader(fragmentShader);
    __GL_DEBUG__;
}

void Raspberry2D::Create2DShader() {
    // Load the shaders and get a linked program object
    loadProgram((const char *) vShaderSrc, (const char *) fShaderSrc);

    __GL_DEBUG__;
    // Get the attribute locations
    positionLoc = glGetAttribLocation(programObject, "a_position");
    __GL_DEBUG__;

    if (-1 == positionLoc)
        throw "Could not get a_position";

    texCoordLoc = glGetAttribLocation(programObject, "a_texCoord");
    __GL_DEBUG__;
    if (-1 == texCoordLoc)
        throw "Could not get a_texCoord";

    // Get the sampler location
    samplerLoc = glGetUniformLocation(programObject, "s_texture");
    __GL_DEBUG__;
    if (-1 == samplerLoc)
        throw "Could not get s_texture";

}