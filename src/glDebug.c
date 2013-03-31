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

#include "glDebug.h"

#include <stdio.h>
#include <string.h>

void glFormatError(GLenum err, char *errorMessage) {

    char *selectedError;
    switch (err) {
        case GL_INVALID_ENUM:
            selectedError = "Invalid enum";
            break;
        case GL_INVALID_VALUE:
            selectedError = "Invalid value";
            break;
        case GL_INVALID_OPERATION:
            selectedError = "Invalid operation";
            break;
        case GL_OUT_OF_MEMORY:
            selectedError = "Out of memory";
            break;
        default:
            selectedError = NULL;
            break;
    }

    if (selectedError == NULL) {
        sprintf(errorMessage, "OpenGL error %d (0x%x): (no description available)", err, err);
    } else {
        sprintf(errorMessage, "OpenGL error %d (0x%x): %s", err, err, selectedError);
    }
}

void glDebugFn(const char *fileName, int line)
{
    GLenum err = glGetError();
    if (GL_NO_ERROR == err)
        return;

    char errorMessage[1024 + 512];

    sprintf(errorMessage, "%s:%d: ", fileName, line);

    glFormatError(err, errorMessage + strlen(errorMessage));
    fprintf(stderr, "%s\n", errorMessage);
}
