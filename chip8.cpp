#include "chip8.h"
#include <cassert>
#include <cstring>
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

// Clears the display:
void Chip8::OP_00E0() {
  memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE() {
  assert(sp > 0 && "stack underflow");
  --sp;
  pc = stack[sp];
}

void Chip8::OP_1nnn() {
  uint16_t address = opcode & 0x0FFFu;
  pc = address;
}

void Chip8::OP_2nnn() {
  assert(sp < 16 && "stack overflow");
  uint16_t address = opcode & 0x0FFFu;
  // note that pc will have been increased by 2 so we don't run into 
  //   an infinite loop
  stack[sp++] = pc;
  pc = address;
}

void Chip8::OP_3xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t val = opcode & 0x00FFu;
  if (registers[Vx] == val) {
    // since the cycle would have already put us on next instruction
    //   we just need to add 2 to it to skip this next instruction:
    pc += 2;
  }
}

void Chip8::OP_4xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t val = opcode & 0x00FFu;
  if (registers[Vx] != val) {
    pc += 2;
  }
}

void Chip8::OP_5xy0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  if (registers[Vx] == registers[Vy]) {
    pc += 2;
  }
}

void Chip8::OP_6xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;
  registers[Vx] = byte;
}

void Chip8::OP_7xkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = opcode & 0x00FFu;
  registers[Vx] += byte;
}

void Chip8::OP_8xy0() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  uint16_t sum = registers[Vx] + registers[Vy];
  registers[0xFu] = (sum > 255u) ? 1 : 0;
  registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  // if underflows then Vf is set to 0 (only underflows if less):
  register[0xFu] = (registers[Vx] >= registers[Vy]) ? 1 : 0;
  register[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  registers[0xFu] = registers[Vx] & 0x1u;
  // essentially dividing the number by 2:
  registers[Vx] >>= 1;
}

void Chip8::OP_8xy7() {
  uint8_t Vx = (opcode & 0x0F00) >> 8u;
  uint8_t Vy = (opcode & 0x00F0) >> 4u;
  // 1 if good enough to not have underflow
  registers[0xFu] = (Vy >= Vx) ? 1 : 0;
  registers[Vx] = registers[Vy] - registers[Vx];
}
