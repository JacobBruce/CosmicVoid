# Cosmic Void

Cosmic Void is an experimental procedural universe engine which uses OpenCL to quickly generate the universe as the camera moves and OpenGL to render things like star sprites. OpenGL and OpenCL can share memory on the GPU which means you can avoid sending data to system memory and solve problems in interesting ways. OpenCL is used to draw distant stars which cover less than a pixel (allowing for a twinkling effect) and OpenCL is also used to render planets using ray-tracing on analytical spheres (the fastest ray intersection test). There are still a few bugs and many things which need to be finished but you can already explore a virtually endless universe full of stars.

OpenCL does the ray-tracing required at fast enough speeds since we only need to test against a small number of planets for a loaded solar system, however at some point the plan is to shift over to an RTX API for ray-tracing since AMD seems to be phasing out OpenCL. The portability, interoperability with OpenGL, and general computing capabilities of OpenCL is really nice though and I hope we get a suitable replacement at some point because it's nice to solve general problems outside of the graphics pipeline using a simple C-like language which OpenCL and CUDA provide.

## DEFAULT CONTROLS:

Control settings can be changed in the settings.cfg file. Scroll lock will enable/disable mouse control. Click a star, planet, or moon to select it and L to lock onto it. WASD for movement, up and down keys for vertical movement, page up to increase speed, page down to decrease speed. You'll need to decrease a lot to enter a solar system, and planets will be a bit hard to find even though they are unrealistically enlarged. Planet sizes and other settings can be changed by editing Resource.h and the compute.cl kernel.

## DEPENDENCIES:

- [SoLoud](http://sol.gfxile.net/soloud/)
- [Freetype GL](https://github.com/rougier/freetype-gl)
- [SOIL2](https://bitbucket.org/SpartanJ/soil2/)
- [GLFW3](https://www.glfw.org/)
- [GLM](https://glm.g-truc.net/)
- OpenCL & OpenGL (from [AMD APP SKD 2.9.1](https://github.com/ghostlander/AMD-APP-SDK/releases/tag/v2.9.1))
- OpenCL C++ Bindings (from AMD APP SKD 2.9.1)
- GLEW (from AMD APP SKD 2.9.1)

## COMPILING:

Ideally you should try to build Cosmic Void in Code::Blocks with gcc by using the included project file (update the search directories for the AMD APP SDK and the project program arguments if necessary). I developed this on Windows using the mingw64 gcc compiler but I tried to keep the code portable so any problems should be easy to fix.

All libraries should be compiled as static libraries using the -static flag or an available flag.

Freetype GL must be built with -DGLEW_STATIC flag and with USE_VAO enabled.

### Windows:

Install [MSYS2](https://www.msys2.org/) and run this command in the shell to get started:

```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-libtool mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-freetype mingw-w64-x86_64-glfw
```

Usually you can create a makefile with cmake and then use the `mingw32-make` command to compile the required libraries.

To build Freetype GL configure cmake so that the FreeType, GLFW, and GLEW paths link to the static mingw64 libraries.
 
Build SoLoud using the cmake files in the contrib folder and with SOLOUD_BACKEND_WINMM defined.

HarfBuzz should be built using only the freetype, glib, and graphite2 dependencies, and ensure GRAPHITE2_STATIC is defined.

Also make sure the freetype, glib, and graphite2 paths point to the static mingw64 library files and not the .dll.a files.

## NOTE:

The fonts and audio files are not included in the repository but can be obtained by downloading the Windows release.
