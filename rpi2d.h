/*  rpi2d - Raspberry Pi 2D  lightweight library to access Raspberry PI's
    hardware video surface through OpenGL ES.
    Copyright (C) 2013 neagix

        https://github.com/neagix/librpi2d

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef TEST_H
#define	TEST_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include  "bcm_host.h"

#include "Texture2D.h"

#include "Rect.h"

class Raspberry2D {
public:
    Raspberry2D(int surfaceWidth, int surfaceHeight);
    void Draw(const Texture2D &tex, Rect src, Rect dest) const;
    void Draw(const Texture2D &tex) const;
    void DrawTile(const Texture2D &tex, int tileHeight, int tileIndex, int x, int y) const;
    void DrawTile(const Texture2D &tex, Rect srcRect, int x, int y) const;
    void DrawTile(const Texture2D &tex, int x, int y) const;
    void DrawHFlip(const Texture2D &tex) const;
    void DrawVFlip(const Texture2D &tex) const;
    void DrawHVFlip(const Texture2D &tex) const;
    virtual ~Raspberry2D();

    // attach the native display, centered viewport and no scaling
    bool Attach();

    // attach the native display with hardware-accelerated automatic scaling
    bool Attach(int scaleW, int scaleH);

    // remember to call this, unless you prefer staring at a black screen
    void SwapBuffers();

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

#endif	/* TEST_H */

