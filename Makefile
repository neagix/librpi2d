
INCLUDES    = -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux

OBJECTS	    := src/glDebug.o src/pngread.o src/Texture2D.o src/rpi2d.o

CCC        = g++
CC         = gcc

all:	$(OBJECTS) librpi2d tests

tests: abcd logo
	
src/glDebug.o:
	$(CC) src/glDebug.c -Wall -lGL $(INCLUDES) -c -o $@

src/pngread.o:
	$(CC) src/pngread.c -Wall -lGL $(INCLUDES) -c -o $@

src/Texture2D.o:
	$(CCC) src/Texture2D.cpp $(CFLAGS) -c $(INCLUDES) -o $@

src/rpi2d.o:
	$(CCC) src/rpi2d.cpp $(CFLAGS) -c $(INCLUDES) -o $@
	
librpi2d:
	$(AR) rcs $@.so $(OBJECTS)
	
abcd:
	$(CCC) examples/basic/$@.cpp $(CFLAGS) -Isrc -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d $(INCLUDES) -o examples/basic/$@

logo:
	wget -q --continue "http://www.raspberrypi.org/wp-content/uploads/2011/10/Raspi-PGB001.png" -O examples/logo/rpilogo.png
	$(CCC) examples/logo/$@.cpp $(CFLAGS) -Isrc -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d $(INCLUDES) -o examples/logo/$@

clean:
	rm -f src/*.o librpi2d.so examples/basic/basic examples/logo/logo
