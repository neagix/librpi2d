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

