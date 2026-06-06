# CHIP-8 Emulator

A CHIP-8 emulator written in C++ using SDL2. Built to learn how emulators work at a low level, the CPU fetch-decode-execute-store cycle, how graphic and sound gets rendered from raw pixel and audio buffers, and how registers, program counters, stack pointers, call stack, and a little bit of assembly code works. I also learned about bit manipulation (masking and shifting) for opcode decoding, C++ encapsulation, and audio callbacks (function that gets called when audio hardware needs more data).

## Demo
[![UFO gameplay with sound](https://img.youtube.com/vi/VoAYsSW74Mg/maxresdefault.jpg)](https://www.youtube.com/watch?v=VoAYsSW74Mg)
*UFO working with audio, the buzzer fires on every shot*

## How it works
The emulator implements the full CHIP-8 instuction set (34) opcodes.

## ROMS (Read only memory-the game instructions)
- Games: https://github.com/dmatlack/chip8/tree/master/roms/games
- Test ROM: https://github.com/corax89/chip8-test-rom

## Build and Run
Requires CMake3.10+, SDL2 (`sudo apt install libsdl2-dev`)
Commands (do in WSL):
```bash
mkdir build && cd build
cmake .. #creates the make file 
make #creates the executable chip8
cd ..
wget 'https://github.com/dmatlack/chip8/raw/refs/heads/master/roms/games/UFO%20%5BLutz%20V,%201992%5D.ch8' #to play the UFO game
./build/chip8 10 3 'UFO [Lutz V, 1992].ch8' #play game, it will open window and play sound
```
second command is the scale for how big you want the game on the screen (10 = 640 by 320)
third command (3) is how fast you want the game to play (lower is faster) - the number represents the number of milliseconds between every CPU cycle
