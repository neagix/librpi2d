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

};

#endif	/* TEXTURE2D_H */

