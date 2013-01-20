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

#include "Texture2D.h"

#include "glDebug.h"

Texture2D::Texture2D(int textureWidth, int textureHeight, int bytesPerPixel = 3, int pixelFormat = GL_RGB, int textureFormat = GL_UNSIGNED_BYTE) {
    if (((textureWidth << 1)>>1) != textureWidth || ((textureHeight << 1)>>1) != textureHeight)
        throw "Width and height should be powers of 2";
    
    this->Width = textureWidth;
    this->Height = textureHeight;
    this->BytesPerPixel = bytesPerPixel;
    this->PixelFormat = pixelFormat;
    this->TextureFormat = textureFormat;

    createTexture(NULL);
}

void Texture2D::createTexture(void *texels) {
    __GL_DEBUG__;
    // Generate a texture object
    glGenTextures(1, &textureId);
    __GL_DEBUG__;
    // Use tightly packed data
    //    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureId);
    __GL_DEBUG__;

    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the texture
    glTexImage2D(GL_TEXTURE_2D, 0, PixelFormat, Width, Height, 0, PixelFormat, TextureFormat, texels);
    __GL_DEBUG__;

    // Set the filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    __GL_DEBUG__;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    __GL_DEBUG__;
}

Texture2D::Texture2D(const char *PNGfileName) {
    gl_texture_t *png_tex = loadPNGTexture(PNGfileName);

    if (((png_tex->Width << 1)>>1) != png_tex->Width || ((png_tex->Height << 1)>>1) != png_tex->Height)
        throw "Width and height should be powers of 2";

    Width = png_tex->Width;
    Height = png_tex->Height;
    PixelFormat = png_tex->PixelFormat;
    TextureFormat = GL_UNSIGNED_BYTE;
    textureId = png_tex->textureId;
    BytesPerPixel = png_tex->BytesPerPixel;

    createTexture(png_tex->texels);

    // Generate texture 
    /*        glGenTextures(1, &png_tex->textureId);
            glBindTexture(GL_TEXTURE_2D, png_tex->textureId);

            // Setup some parameters for texture filters and mipmapping 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, png_tex->PixelFormat,png_tex->Width, png_tex->Height, 0, png_tex->TextureFormat,               GL_UNSIGNED_BYTE, png_tex->texels);
     */
    /* OpenGL has its own copy of texture data */
    free(png_tex->texels);
    free(png_tex);

    printf("Created %dx%d texture with %d bytes per pixel\n", Width, Height, BytesPerPixel);
}

void Texture2D::Bind() const {
    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    __GL_DEBUG__;
    glBindTexture(GL_TEXTURE_2D, textureId);
    __GL_DEBUG__;
}

/*
 * overwrite texels data of the texture
 */
void Texture2D::Render(GLuint textureId, void *pixelData) {
    // Use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, BytesPerPixel);
    __GL_DEBUG__;

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureId);
    __GL_DEBUG__;

    // Load the texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, PixelFormat, TextureFormat, pixelData);
    __GL_DEBUG__;
}

void Texture2D::WriteTileRect(int tileHeight, int tileIndex, Rect *r) const {
    
    tileIndex = (Height/tileHeight) - tileIndex - 1;
    
    r->x1 = 0;
    r->x2 = Width;
    r->y1 = tileIndex * tileHeight;
    r->y2 = r->y1 + tileHeight;
    
    // normalize
    r->x1 /= Width; r->x2 /= Width;
    r->y1 /= Height; r->y2 /= Height;    
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &textureId);
    __GL_DEBUG__;
}

