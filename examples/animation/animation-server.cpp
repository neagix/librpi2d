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
#include <zmq.h>
#include <assert.h>
#include <pthread.h>

using namespace std;

#include "animation.h"

void *context;

void my_free (void *originalPng, void *x)
{
    // this will release the original png
    free (originalPng);
}

/*
 * display animation
 */
int main(int argc, char** argv) {

    // Socket to talk to clients
    context = zmq_init (1);

    Raspberry2D *rpi2D = new Raspberry2D(ANIMATION_W, ANIMATION_H);

    // enable alpha channel (transparency)
//    rpi2D->AlphaSize = 8;

    // set background color to red
    rpi2D->BackgroundColor[0] = 1;

    // attach surface
    if (!rpi2D->Attach()) {
        delete rpi2D;

        fprintf(stderr, "Could not attach surface\n");
        return -1;
    }



    zmq_msg_t inMsg;
    zmq_msg_init(&inMsg);

    void *subscriberSocket = zmq_socket (context, ZMQ_SUB);
    int rc = zmq_connect(subscriberSocket, "tcp://*:5555");
    if (rc != 0) {
		fprintf( stderr, "zmq_connect: %s\n", zmq_strerror( zmq_errno() ) );
		return NULL;
    }

    // subscribe to all messages
    zmq_setsockopt(subscriberSocket, ZMQ_SUBSCRIBE, NULL, 0);

    float startTime = (float)clock()/CLOCKS_PER_SEC;

    for(int i=0;i<ALL_FRAMES-10;i++){
        int rc = zmq_recv (subscriberSocket, &inMsg, 0);
        assert(rc == 0);
        printf ("consumer: received frame\n");
    }

    float endTime = (float)clock()/CLOCKS_PER_SEC;

    float timeElapsed = endTime - startTime;

    printf("%d frames in %.2f seconds, %.2f FPS", ALL_FRAMES, timeElapsed, ALL_FRAMES/timeElapsed);

    delete rpi2D;

    return 0;
}
