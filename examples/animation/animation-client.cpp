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

#include <stdio.h>
#include <zmq.h>
#include <assert.h>

#include "animation.h"

using namespace std;

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

    if (argc < 2) {
        fprintf(stderr, "Please specify server\n");
        return -1;
    }

    char server[1024];
    sprintf(server, "tcp://%s:5555", argv[1]);

    // Socket to talk to clients
    context = zmq_init (1);

    printf("Producer thread\n");

    void *publisherSocket = zmq_socket (context, ZMQ_PUB);
    int rc = zmq_bind (publisherSocket, server);
    if (rc != 0) {
		fprintf( stderr, "zmq_bind: %s\n", zmq_strerror( zmq_errno() ) );
		return NULL;
    }

    zmq_msg_t msg;


    // one thread will load frames, the other thread will display them with no delay
    char frameFileName[200];
    for(int i=0;i<ALL_FRAMES;i++){

        sprintf(frameFileName, "monarch-frames/pict%04d.png", i);

        // load texture and send
        struct gl_texture_t *png = loadPNGTexture(frameFileName);

        rc = zmq_msg_init_data (&msg, png, TEXTURE_SIZE((*png)), my_free, NULL);

        assert (rc == 0);

        printf("producer: sending frame %d\n", i);
        zmq_send (publisherSocket, &msg, 0);

        // mark as no more needed
        zmq_msg_close(&msg);

//        usleep(500);
    }

    printf("Producer: completed sending %d frames\n", ALL_FRAMES);

    return 0;
}
