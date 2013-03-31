## librpi2d Makefile

INCLUDES    = -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux

OBJECTS	    := src/glDebug.o src/pngread.o src/Texture2D.o src/rpi2d.o

SOURCES	    := src/glDebug src/pngread src/Texture2D src/rpi2d

CCC        = g++
CC         = gcc

all:	$(SOURCES) librpi2d animation

tests: logo abcd
	
src/glDebug: src/glDebug.c src/glDebug.h
	$(CC) $@.c -Wall -lGL $(INCLUDES) -c -o $@.o

src/pngread: src/pngread.c src/pngread.h
	$(CC) $@.c -Wall -lGL $(INCLUDES) -c -o $@.o

src/Texture2D: src/Texture2D.cpp src/Texture2D.h
	$(CCC) $@.cpp $(CFLAGS) -c $(INCLUDES) -o $@.o

src/rpi2d: src/rpi2d.cpp src/rpi2d.h
	$(CCC) $@.cpp $(CFLAGS) -c $(INCLUDES) -o $@.o
	
librpi2d:
	$(AR) rcs $@.so $(OBJECTS)
	
abcd:
	$(CCC) examples/basic/$@.cpp $(CFLAGS) -Isrc -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d $(INCLUDES) -o examples/basic/$@

logo:
	wget -q --continue "http://www.raspberrypi.org/wp-content/uploads/2011/10/Raspi-PGB001.png" -O examples/logo/rpilogo.png
	$(CCC) examples/logo/$@.cpp $(CFLAGS) -Isrc -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d $(INCLUDES) -o examples/logo/$@

animation:
	$(CCC) examples/animation/$@-client.cpp $(CFLAGS) -Isrc -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d -lzmq $(INCLUDES) -o examples/animation/$@-client
	$(CCC) examples/animation/$@-server.cpp $(CFLAGS) -Isrc -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d -lzmq $(INCLUDES) -o examples/animation/$@-server

clean:
	rm -f src/*.o librpi2d.so examples/basic/basic examples/logo/logo
