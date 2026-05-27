#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <random>

const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;
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
  uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{}; // will only use 1 or 0 for black or white
  uint16_t opcode; // the instruction being done on that operation (2 bytes)
  // loads game into memory:
  void LoadROM(char const *filename);
  std::default_random_engine randGen;
  std::uniform_int_distribution<uint8_t> randByte{0, 255u};
  // now we need to implement the 34 different possible instructions
  // the ROM could have
  // Clear the display:
  void OP_00E0();
  // returning from a subroutine:
  void OP_00EE();
  // Jump to location nnn:
  void OP_1nnn();
  // call subroutine at nnn:
  void OP_2nnn();
  // skip next instruction if register x is kk
  void OP_3xkk();
  // skip if not equal to:
  void OP_4xkk();
  // skip next instruction if Vx = Vy
  void OP_5xy0();
  // set Vx to kk
  void OP_6xkk();
  // add kk to register x, no carry flag
  void OP_7xkk();
  // set Vx = Vy
  void OP_8xy0();
  // Set Vx = Vx OR Vy:
  void OP_8xy1();
  // Set Vx = Vx AND Vy:
  void OP_8xy2();
  // Set Vx = Vx XOR Vy:
  void OP_8xy3();
  // Set Vx = Vx + Vy and Vf = carry:
  void OP_8xy4();
  // set Vx = Vx - Vy and Vf will be 1 if we did not have underflow; 1 else:
  void OP_8xy5();
  // right shift on Vx and will carry 1 if it was there:
  void OP_8xy6();
  // Vy - Vx stored in Vx but if underflow put 0 in Vf, else 1:
  void OP_8xy7();
  // left shift and will carry 1 into Vf for most significant digit:
  void OP_8xyE();
  // skip if Vx != Vy:
  void OP_9xy0();
  // set the index register to nnn:
  void OP_Annn();
  // jump to location nnn + V0:
  void OP_Bnnn();
  // Set Vx to a random byte AND kk:
  void OP_Cxkk();
  // Starting at the memory location in the index register, we are going
  //   to be reading off n bytes and putting those into the video array
  //   starting at (Vx, Vy) in the video array as the top left. For
  //   collisions we set Vf to 1:
  void OP_Dxyn();
  // skip next instruction if key with value in Vx is pressed:
  void OP_Ex9E();
  // skip next instruction if key with value in Vx is not pressed:
  void OP_ExA1();
  // Set the number in register Vx to be the delaytimer:
  void OP_Fx07();
  // basically creates an infinite loop till the user presses a key:
  void OP_Fx0A();
  // set delay timer to Vx:
  void OP_Fx15();
  // set sound timer to Vx:
  void OP_Fx18();
  // increase index register by Vx:
  void OP_Fx1E();
  // setting the index register to the start address of the sprite for digit
  //   Vx:
  void OP_Fx29();
  // Stores the binary coded decimal starting at index register. So it will put
  //   the hundred's digit at the index register address, then 10's at 
  //   I + 1 and 1's at I + 2:
  void OP_Fx33();
  // stores registers V0 to Vx in memory starting at the index register
  void OP_Fx55();
  // read the memory into the registers from V0 up to Vx:
  void OP_Fx65();
  // the tables
  using Chip8Func = void (Chip8::*)();
  Chip8Func table[0xF + 1];
	Chip8Func table0[0xE + 1];
	Chip8Func table8[0xE + 1];
	Chip8Func tableE[0xE + 1];
	Chip8Func tableF[0x65 + 1];
  // the functions that table will be calling when it
  //   wants to go to a subtable:
  // The idea is basically we are bitmasking to find the specific index
  //   and then using it to go into the table we want to pull the function
  //   pointer and then we are dereferencing to get us the function and 
  //   then calling it on our current object
  void Table0() { ((*this).*(table0[opcode & 0x000Fu]))(); }
  void Table8() { ((*this).*(table8[opcode & 0x000Fu]))(); }
  void TableE() { ((*this).*(tableE[opcode & 0x000Fu]))(); }
  void TableF() { ((*this).*(tableF[opcode & 0x00FFu]))(); }
  void OP_NULL() {}

  void Cycle();
};


#endif
