#include "chip8.h"
#include <stdexcept>
#include <cstdint>
#include <fstream>
#include <chrono>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;

uint8_t fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
  { // this is the constructor
  // where first instruction will be read:
  pc = START_ADDRESS;

  // loading fonts into the memory:
  for (size_t i = 0; i < FONTSET_SIZE; ++i) {
    memory[FONTSET_START_ADDRESS + i] = fontset[i]; 
  }
}

// will modify memory to load in the ROM game instructions:
void Chip8::LoadROM(char const *filename) {
  // open the file, this creates the connection to it
  // we read it only as binary and start at the end
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (file.is_open()) {
    // we need to know size of file so we get tellg to us where the end is
    std::streampos size = file.tellg();
    if (size > (4096 - START_ADDRESS)) {
      throw std::runtime_error("ROM too large!");
    }
    // we want a space large enough to hold ROM contents
    char *buffer = new char[size];
    // we need to go back to the beginning:
    file.seekg(0, std::ios::beg);
    // put the data into the buffer:
    file.read(buffer, size);
    file.close(); // no longer need it
    for (size_t i = 0; i < size; ++i) {
      memory[START_ADDRESS + i] = buffer[i];
    }
    delete[] buffer;
  }
}
