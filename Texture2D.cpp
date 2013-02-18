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

#include "Texture2D.h"

#include "glDebug.h"

Texture2D::Texture2D(int width, int height, int bytesPerPixel = 3, GLenum pixelFormat = GL_RGB, GLenum pixelType = GL_UNSIGNED_BYTE) {
    if (((width << 1) >> 1) != width || ((height << 1) >> 1) != height)
        throw "Width and height should be powers of 2";

    this->Width = width;
    this->Height = height;
    this->BytesPerPixel = bytesPerPixel;
    this->PixelFormat = pixelFormat;
    this->PixelType = pixelType;

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
    glTexImage2D(GL_TEXTURE_2D, 0, PixelFormat, Width, Height, 0, PixelFormat, PixelType, texels);
    __GL_DEBUG__;

    // Set the filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    __GL_DEBUG__;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    __GL_DEBUG__;
}

Texture2D::Texture2D(const char *PNGfileName) {
    gl_texture_t *png_tex = loadPNGTexture(PNGfileName);

    if (((png_tex->Width << 1) >> 1) != png_tex->Width || ((png_tex->Height << 1) >> 1) != png_tex->Height)
        throw "Width and height should be powers of 2";

    Width = png_tex->Width;
    Height = png_tex->Height;
    PixelFormat = png_tex->PixelFormat;
    PixelType = GL_UNSIGNED_BYTE;
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

void Texture2D::Render(void *pixelData) {
    Render(pixelData, 0, 0, Width, Height);
}

/*
 * overwrite texels data of the texture
 */
void Texture2D::Render(void *pixelData, int x, int y, int w, int h) {
    // Use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, BytesPerPixel);
    __GL_DEBUG__;

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureId);
    __GL_DEBUG__;

    // Load the texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, PixelFormat, PixelType, pixelData);
    __GL_DEBUG__;
}

void Texture2D::WriteTileRect(int tileHeight, int tileIndex, Rect *r) const {

    tileIndex = (Height / tileHeight) - tileIndex - 1;

    r->x1 = 0;
    r->x2 = Width;
    r->y1 = tileIndex * tileHeight;
    r->y2 = r->y1 + tileHeight;

    // normalize
    r->x1 /= Width;
    r->x2 /= Width;
    r->y1 /= Height;
    r->y2 /= Height;
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &textureId);
    __GL_DEBUG__;
}

