# Intel 8080 Disassembler and Emulator

Work in progress for a disassembler and emulator of the Intel 8080 8-bit microprocessor. Can run early 8-bit games such as Space invaders!

## How to build

The emulator uses SDL2, an abstraction layer for low-level machine
access on Linux, OS X, and Windows. It can be obtained from the
project's website:

https://libsdl.org/

### Build and install SDL2

```
tar zxvf SDL2-2.0.14.tar.gz
cd SDL2-2.0.14
mkdir build
cd build

../configure --prefix=/opt/sdl2

make
make install
```

### Build and install SDL image
```
tar zxvf SDL2_image-2.0.5.tar.gz
cd SDL2_mixer-2.0.5
mkdir build
cd build
../configure --prefix=/opt/sdl2 --with-sdl-prefix=/opt/sdl2

make
make install
```

### Build and install SDL mixer
```
tar zxvf SDL2_mixer-2.0.4.tar.gz
cd SDL2_mixer-2.0.4
mkdir build
cd build
../configure --prefix=/opt/sdl2 --with-sdl-prefix=/opt/sdl2

make
make install
```

### Build emulator

To build the disassembler and emulator, run:
```
make
```

This will result in a program called 'emulator8080', which can then be run in the command line with the path to the rom file to emulate:

```
./emulator8080 <filename>
```
