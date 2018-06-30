# Dumageview: The Dumb Image Viewer
Dumageview is a simple Qt-based image viewer. There are many like it, but this
one has "dum" in the name. It was created as a test case for my [HEIF image
plugin]. It is generally functional and decently quick.

[HEIF image plugin]: https://github.com/jakar/qt-heif-image-plugin

![Screenshot](doc/screenshot.png?raw=true)

## Dependencies
Prerequisites:
- Boost
- Qt 5

Included as submodules:
- [glm](https://github.com/g-truc/glm)
- [fmt](https://github.com/fmtlib/fmt)
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

Step three, put it in its place:
```console
$ sudo make install
```

## Usage
We leave this as an exercise for the reader.
Hint: try a variety of mouse buttons and keys.
