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

#ifndef TEXTURE2D_H
#define	TEXTURE2D_H

#include <GLES2/gl2.h>
#include <stddef.h>

#include "pngread.h"

#include "Rect.h"

class Texture2D {
public:
    Texture2D(const char *PNGfileName);
    Texture2D(int textureWidth, int textureHeight, int bytesPerPixel, GLenum pixelFormat, GLenum pixelType);
    virtual ~Texture2D();
    void Bind() const;
    void WriteTileRect(int tileHeight, int tileIndex, Rect *r) const;
    void Render(void *pixelData);
    void Render(void *pixelData, int x, int y, int w, int h);

    int Width, Height;

private:
    int BytesPerPixel, PixelFormat, PixelType;
    // Texture object handle
    GLuint textureId;
    void createTexture(void *texels);
    void glHandleError();

};

#endif	/* TEXTURE2D_H */

