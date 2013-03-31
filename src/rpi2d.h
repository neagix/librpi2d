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

#ifndef RPI2D_H
#define	RPI2D_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include  "bcm_host.h"

#include "Texture2D.h"

#include "Rect.h"

class Raspberry2D {
public:
    // create instance with specified definition
    // this will not be the displayed size but the
    // size of surface we will be writing into
    // after construction the display size
    // will be available for reading
    Raspberry2D(int surfaceWidth, int surfaceHeight);

    // attach the native display, centered viewport and no scaling
    // to be called right after construction
    bool Attach();

    // attach the native display with hardware-accelerated automatic scaling
    // specified size will be used on the destination display
    // to be called right after construction
    bool Attach(int scaleW, int scaleH);

    // swap surface buffer to display
    // remember to call this, unless you prefer staring at a black screen
    void SwapBuffers();

    // following methods provide different ways of rendering a texture to screen
    void Draw(const Texture2D &tex, Rect src, Rect dest) const;
    void Draw(const Texture2D &tex) const;
    void DrawHFlip(const Texture2D &tex) const;
    void DrawVFlip(const Texture2D &tex) const;
    void DrawHVFlip(const Texture2D &tex) const;

    // following methods provide different ways of rendering a tile to screen
    void DrawTile(const Texture2D &tex, int tileHeight, int tileIndex, int x, int y) const;
    void DrawTile(const Texture2D &tex, Rect srcRect, int x, int y) const;
    void DrawTile(const Texture2D &tex, int x, int y) const;
    virtual ~Raspberry2D();

    
    //TODO: method to get screenshot? useful for remote testing/streaming
    
    EGLint RedSize;
    EGLint GreenSize;
    EGLint BlueSize;

    // set below variables to 8 if needed
    EGLint AlphaSize;
    EGLint DepthSize;
    EGLint StencilSize;
    bool MultiSample;

    /// desired drawing surface
    // READONLY
    int Width, Height;

    // dispmanx real display size
    // filled in after construction
    uint32_t DisplayWidth;
    uint32_t DisplayHeight;

    float BackgroundColor[3];

private:
    GLuint programObject;

    // Attribute locations
    GLint positionLoc;
    GLint texCoordLoc;
    // Sampler location
    GLint samplerLoc;

    /// debugging stuff
    char *infoLog;

    /// native stuff (dispmanx)
    DISPMANX_ELEMENT_HANDLE_T dispman_element;
    DISPMANX_DISPLAY_HANDLE_T dispman_display;
    DISPMANX_UPDATE_HANDLE_T dispman_update;

    /// OpenGL ES stuff
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGLConfig config;
    EGLNativeWindowType hWnd;
    EGL_DISPMANX_WINDOW_T nativeW;

    /// 2D rendering stuff
    // Handle to a program object
    GLuint vertexShader, fragmentShader;

    bool CreateNativeWindow(int scaleW, int scaleH);
    bool CreateEGLContext();
    void Create2DShader();
    void loadProgram(const char *vShaderStr, const char *fShaderStr);
    GLuint esLoadShader(GLenum type, const char *shaderSrc);
    void makeVertices(Rect src, Rect dst, GLfloat *vVertices) const;
    void normalizeRect(Rect *r) const;
};

#endif	/* RPI2D_H */

