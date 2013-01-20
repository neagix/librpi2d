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

#ifndef GLDEBUG_H
#define	GLDEBUG_H

#ifdef	__cplusplus
extern "C" {
#endif

void glDebugFn(const char *fileName, int line);

// neagix: enable following to debug OpenGL calls
//#define __GL_DEBUG__ glDebugFn(__FILE__, __LINE__)
#define __GL_DEBUG__


#ifdef	__cplusplus
}
#endif

#endif	/* GLDEBUG_H */

