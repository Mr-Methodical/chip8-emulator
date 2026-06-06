# CHIP-8 Emulator

A CHIP-8 emulator written in C++ using SDL2. Built to learn how emulators work at a low level, the CPU fetch-decode-execute-store cycle, how graphic and sound gets rendered from raw pixel and audio buffers, and how registers, program counters, stack pointers, call stack, and a little bit of assembly code works. I also learned about bit manipulation (masking and shifting) for opcode decoding, C++ encapsulation, and audio callbacks (function that gets called when audio hardware needs more data).

An emulator pretends to be hardware. These chip8 games expect a CPU, registers, etc. so we can fake these with using different data types like arrays and uint8_t's to represent them. At its core, you can also think of it as an interpreter since it is taking these instructions reading them off and interpreting what things to perform (the fake hardware just helps us do this). 

## Demo
[![UFO gameplay with sound](https://img.youtube.com/vi/VoAYsSW74Mg/maxresdefault.jpg)](https://www.youtube.com/watch?v=VoAYsSW74Mg)
*UFO working with audio, the buzzer fires on every shot*

## How it works
The emulator implements the full CHIP-8 instruction set (34 opcodes) using a function pointer dispatch table (opcode maps to memory address of a function in the table) instead of switch statement (which can get super ugly say if we go to NES which has 256 opcodes, the table just keeps it more clean). The main loop runs a fetch-decode-execute where the speed of this can be chosen. SDL2 handles graphics, input, and audio on a separate system. For the audio to make it smooth I do use this samplePos to keep track of where we are in the sine wave so that the audio is more smooth instead of the other option of using a square wave where we just alternate between a max and a min value (maybe we lost some authenticity with this though lol). The audio turns on or off based on the chip8 sound timer. 

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
# 2nd command is the scale for how big you want the game on the screen (10 = 640 by 320)
# and the third command is how fast you want the game to play (lower is faster)
# the number (3) represents number of milliseconds between every CPU cycle
./build/chip8 10 3 'UFO [Lutz V, 1992].ch8' #play game, it will open window and play sound
```
