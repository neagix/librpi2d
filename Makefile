INCLUDES    = -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux

OBJECTS	    = glDebug.o pngread.o Texture2D.o rpi2d.o

CCC        = g++
CC         = gcc

all:	$(OBJECTS) librpi2d tests

tests: abcd logo
	
glDebug.o:
	$(CC) glDebug.c -Wall -lGL $(INCLUDES) -c -o $@

pngread.o:
	$(CC) pngread.c -Wall -lGL $(INCLUDES) -c -o $@

Texture2D.o:
	$(CCC) Texture2D.cpp $(CFLAGS) -c $(INCLUDES) -o $@

rpi2d.o:
	$(CCC) rpi2d.cpp $(CFLAGS) -c $(INCLUDES) -o $@
	
librpi2d:
	$(AR) rcs $@.so $(OBJECTS)
	
abcd:
	$(CCC) examples/basic/$@.cpp $(CFLAGS) -I. -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d $(INCLUDES) -o examples/basic/$@

logo:
	wget --continue "http://www.raspberrypi.org/wp-content/uploads/2011/10/Raspi-PGB001.png" -O examples/logo/rpilogo.png
	$(CCC) examples/logo/$@.cpp $(CFLAGS) -I. -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d $(INCLUDES) -o examples/logo/$@

clean:
	rm -f *.o librpi2d.so
