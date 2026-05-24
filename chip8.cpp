#include "chip8.h"
#include <stdexcept>
#include <fstream>

const unsigned int START_ADDRESS = 0x200;

Chip8::Chip8() { // this is the constructor
  // where first instruction will be read:
  pc = START_ADDRESS;
}

// will modify memory to load in the ROM game instructions:
void Chip8::LoadROM(char const *filename) {
  // open the file, this creates the connection to it
  // we read it only as binary and start at the end
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (file.is_open()) {
    // we need to know size of file so we get tellg to us where the end is
    std::streampos size = file.tellg();
    // we want a space large enough to hold ROM contents
    char *buffer = new char[size];
    // we need to go back to the beginning:
    file.seekg(0, std::ios::beg);
    // put the data into the buffer:
    file.read(buffer, size);
    file.close(); // no longer need it
    if (size > (4096 - START_ADDRESS)) {
      throw std::runtime_error("ROM too large!");
    }
    for (size_t i = 0; i < size; ++i) {
      memory[START_ADDRESS + i] = buffer[i];
    }
    delete[] buffer;
  }
}
