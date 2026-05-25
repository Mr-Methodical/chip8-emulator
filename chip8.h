#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <random>

class Chip8 {
public:
  uint8_t registers[16]{}; // the cpu's scratchpad: 16 8-bit registers
  uint8_t memory[4096]{}; // 4KB of Chip-8 Memory with certain parts reserved
  uint16_t index{}; // register for holding addresses since memory up to 0xFFF
  uint16_t pc{}; // program counter holds address of next instruction
  uint16_t stack[16]{}; // holds pc of where to RET after CALL
  uint8_t sp{}; // keeps track of which level we are on in the stack
  uint8_t delayTimer{}; // counts down from whatever value put in it
  uint8_t soundTimer{}; // timer for how long to play song (will count down)
  uint8_t keypad[16]{}; // will have 1 or 0 in depending which key pressed
  uint32_t video[64 * 32]{}; // will only use 1 or 0 for black or white
  uint16_t opcode; // the instruction being done on that operation (2 bytes)
  // loads game into memory:
  void LoadROM(char const *filename);
  std::default_random_engine randGen;
  std::uniform_int_distribution<uint8_t> randByte{0, 255U};
};


#endif
