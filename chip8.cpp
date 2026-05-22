#include "chip8.h"
#include <fstream>

const unsigned int START_ADDRESS = 0x200;

Chip8::Chip8() {
  // where first instruction will be read:
  pc = START_ADDRESS;
}

// will modify memory to load in the ROM game instructions:
void Chip8::LoadROM(char const *filename) {
  
}
