rpi2d
=====

Raspberry2D lightweight library to access Raspberry PI's hardware video surface through OpenGL ES. Supports textures and tile-based drawing, easy to integrate

The library is C++ (with some C), not really packaged for portability but you should be able to use it with little efforts. There are some nice&interesting features, like:

* support for hardware-provided scaling
* tileset loading from PNG and index-based tile selection for drawing
* render-to-texture (this is what you will use to port other software to Raspberry PI)

Library is licensed under LGPL v2 license

NOTE: author is not officially endorsed in Raspberry Pi project, nor this library

Getting started
=====

	git clone https://github.com/neagix/librpi2d.git && \
	cd librpi2d && \
	make && \
	cd examples/logo && \
	./logo

The above will download the source, compile and then display the
nice Raspberry Pi project logo :)
