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

const int ALL_FRAMES = 1000;

void *context;

void my_free (void *data, void *hint)
{
    free (data);
}

void *producer_thread( void *arg ) {
    printf("Producer thread\n");

    void *publisherSocket = zmq_socket (context, ZMQ_PUB);
    int rc = zmq_bind (publisherSocket, "tcp://*:5555");
    if (rc != 0) {
		fprintf( stderr, "zmq_bind: %s\n", zmq_strerror( zmq_errno() ) );
		return 1;
    }


    void *data = malloc(6);
    memcpy(data, "Hello!", 6);

    zmq_msg_t msg;

    int rc = zmq_msg_init_data (&msg, data, 6, my_free, NULL);
    assert (rc == 0);

    for(int i =0; i < 6; i++) {
        printf("producer: send msg %d\n", i);
        zmq_send (responder, &msg, 0);
        sleep (1); // Do some 'work'
    }
}

void *consumer_thread( void *arg ) {
    zmq_msg_t inMsg;
    zmq_msg_init(&inMsg);
    printf("Consumer thread\n");

    for(int i =0; i < 6; i++) {
        int rc = zmq_recv (responder, &inMsg, 0);
        assert(rc == 0);
        printf ("consumer: received message: ?\n");
    }
}

/*
 * display animation
 */
int main(int argc, char** argv) {

         // Socket to talk to clients
    context = zmq_init (1);
    void *receiver = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    if (rc != 0) {
		fprintf( stderr, "zmq_bind: %s\n", zmq_strerror( zmq_errno() ) );
		return 1;
    }

    int n;
    pthread_t producer, consumer;

	if ( n = pthread_create(&producer, NULL, producer_thread, NULL ) ) {
		fprintf( stderr, "pthread_create: %s\n", strerror( n ) );
		return 1;
	}

	if ( n = pthread_create(&consumer, NULL, consumer_thread, NULL ) ) {
		fprintf( stderr, "pthread_create: %s\n", strerror( n ) );
		return 1;
	}

	pthread_join(consumer, NULL);
	return 0;


    Raspberry2D *rpi2D = new Raspberry2D(480, 360);

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



    printf("Pre-loading %d frames\n", ALL_FRAMES);

    // one thread will load frames, the other thread will display them with no delay
    char frameFileName[200];
    Texture2D *allFrames[ALL_FRAMES];
    for(int i=0;i<ALL_FRAMES;i++){

        sprintf(frameFileName, "monarch-frames/pict%04d.png", i);

        // create texture
        allFrames[i] = new Texture2D(frameFileName);
    }

    printf("Running animation at full speed\n");
    float startTime = (float)clock()/CLOCKS_PER_SEC;

    for(int i=0;i<ALL_FRAMES;i++){

        // PNGs are stored vertically
        rpi2D->DrawVFlip(*allFrames[i]);

        rpi2D->SwapBuffers();

        usleep(500);
    }
    float endTime = (float)clock()/CLOCKS_PER_SEC;

    float timeElapsed = endTime - startTime;

    printf("%d frames in %.2f seconds, %.2f FPS", ALL_FRAMES, timeElapsed, ALL_FRAMES/timeElapsed);

    for(int i=0;i<ALL_FRAMES;i++){
        delete allFrames[i];
    }
    delete rpi2D;

    return 0;
}
