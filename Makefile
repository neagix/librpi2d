
INCLUDES    = -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads

OBJECTS	    = glDebug.o pngread.o Texture2D.o rpi2d.o

CCC        = g++
CC         = gcc

# NOTE: testRPI2D is not compiled with the library
all:	$(OBJECTS) librpi2d
	
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
	
testRPI2D:
	$(CCC) $@.cpp $(CFLAGS) -L/opt/vc/lib -L./ -lpng -lGLESv2 -lEGL -lrpi2d $(INCLUDES) -o $@
	
clean:
	rm -f *.o testRPI2D