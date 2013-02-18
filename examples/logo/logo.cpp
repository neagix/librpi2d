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

#include <cstdlib>

#include "rpi2d.h"
#include <stdio.h>

using namespace std;

/*
 * display logo
 */
int main(int argc, char** argv) {

    Raspberry2D *rpi2D = new Raspberry2D(640, 480);

    // enable alpha channel (transparency)
    rpi2D->AlphaSize = 8;

    // set background color to blue
    rpi2D->BackgroundColor[2] = 1;

    // attach surface
    if (!rpi2D->Attach()) {
        delete rpi2D;

        fprintf(stderr, "Could not attach surface\n");
        return -1;
    }

    // create texture
    Texture2D *tex = new Texture2D("rpilogo.png");

    // PNGs are stored vertically
    rpi2D->DrawVFlip(*tex);

    rpi2D->SwapBuffers();

    printf("Logo will be on display for 5 seconds\n");

    sleep(5);

    delete tex;

    delete rpi2D;

    return 0;
}
