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

#ifndef PNGREAD_H
#define	PNGREAD_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <png.h>

#include <GLES2/gl2.h>

/* OpenGL texture info */
struct gl_texture_t {
    GLsizei Width;
    GLsizei Height;

    GLenum PixelFormat;
    GLint BytesPerPixel;
    GLuint textureId;

/*    int bit_depth; */

    // this will be a placeholder
    GLubyte texels[4];
};

#define TEXTURE_SIZE(t) sizeof(struct gl_texture_t) - 4*sizeof(GLubyte) + TEXELS_SIZE(t)

struct gl_texture_t *loadPNGTexture(const char *filename);

#define TEXELS_SIZE(t)    sizeof (GLubyte) * t.Width * t.Height * t.BytesPerPixel

#ifdef	__cplusplus
}
#endif

#endif	/* PNGREAD_H */

