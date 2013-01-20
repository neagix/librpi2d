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

    GLubyte *texels;
};

struct gl_texture_t *loadPNGTexture(const char *filename);

#ifdef	__cplusplus
}
#endif

#endif	/* PNGREAD_H */

