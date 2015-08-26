Quokka3D is a cross-platform, lightweight software-only 3D polygon rendering engine written in C++.

It is a C++ port of the 3D engine described in the book ["Developing Games in Java"](http://www.brackeen.com/javagamebook/) by David Brackeen. The engine features texture mapping, light mapping, BSP trees (2D only currently, like Doom), basic OBJ file parsing, basic collision detection and basic 3D animation. It uses standard C++ and is designed to be cross-platform. To achieve this the engine uses other open source libraries to abstract away the underlying hardware. Currently [PixelToaster](http://www.pixeltoaster.com/) is used as the output framebuffer and [OIS](http://sourceforge.net/projects/wgois/) to control input devices.

_Please note there is no 3D hardware acceleration in this engine - it is purely a software engine._

You can write simple 3D games or simulations with Quokka3D, or use it as a learning tool for 3D graphics.

The original Java source code can be downloaded from http://www.brackeen.com/javagamebook/

A [quokka](http://en.wikipedia.org/wiki/Quokka) is a small, cute marsupial found in Rottnest Island, Western Australia.