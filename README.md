rpi2d
=====

Raspberry2D lightweight library to access Raspberry PI's hardware video surface through OpenGL ES. Supports textures and tile-based drawing, easy to integrate

The library is C++ (with some C), not really packaged for portability but you should be able to use it with little efforts. There are some nice&interesting features, like:

* support for hardware-provided scaling
* tileset loading from PNG and index-based tile selection for drawing
* render-to-texture (this is what you will use to port other software to Raspberry PI)
