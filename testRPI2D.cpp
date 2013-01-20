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
 * 
 */
int main(int argc, char** argv) {

    Raspberry2D *rpi2D = new Raspberry2D(320, 240);

    // enable support of alpha channel
    rpi2D->AlphaSize = 8;

    // set background color to green
    rpi2D->BackgroundColor[1] = 1;

    // attach surface
    if (!rpi2D->Attach(0, 0)) {
        delete rpi2D;

        fprintf(stderr, "Could not attach surface\n");
        return -1;
    }

    // create texture
    Texture2D *tex = new Texture2D("abcd.png");

    rpi2D->DrawTile(*tex, 0, 0);

    //    rpi2D->DrawTile(*tex, 32, 2, 130, 100);
    //    rpi2D->DrawTile(*tex, 32, 3, 140, 100);
    rpi2D->DrawTile(*tex, 32, 0, 0, 0);
    rpi2D->DrawTile(*tex, 32, 1, 120, 100);

    rpi2D->SwapBuffers();

    printf("Test will be on display for 10 seconds\n");

    sleep(10);

    delete tex;

    delete rpi2D;

    return 0;
}
