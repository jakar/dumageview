# Dumageview: The Dumb Image Viewer
Dumageview is a simple Qt-based image viewer.
It was created partially as a test case for my [HEIF image plugin], partially
to experiment with C++17, and partially for an assortment of other less
concrete reasons. Roughly a 20/30/50 split. It is, however, functional and
reasonably fast.

[HEIF image plugin]: https://github.com/jakar/qt-heif-image-plugin

![Screenshot](doc/screenshot.png?raw=true)

## Dependencies
Prerequisites:
- Boost
- Qt 5

Included as submodules:
- [fmt](https://github.com/fmtlib/fmt)
- [glm](https://github.com/g-truc/glm)
- [spdlog](https://github.com/gabime/spdlog)

## Installation
Step one, clone it:
```console
$ git clone --recursive https://github.com/jakar/dumageview.git
```

Step two, build it:
```console
$ mkdir build
$ cd build
$ cmake ..
$ make -j$(nproc)
```

Step three, install it:
```console
$ sudo make install
```

## Usage
We leave this as an exercise for the reader.
Hint: try a variety of mouse buttons and keys.
