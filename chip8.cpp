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
  // setting up a function pointer table:
  table[0x0] = &Chip8::Table0;
  table[0x1] = &Chip8::OP_1nnn;
  table[0x2] = &Chip8::OP_2nnn;
  table[0x3] = &Chip8::OP_3xkk;
  table[0x4] = &Chip8::OP_4xkk;
  table[0x5] = &Chip8::OP_5xy0;
  table[0x6] = &Chip8::OP_6xkk;
  table[0x7] = &Chip8::OP_7xkk;
  table[0x8] = &Chip8::Table8;
  table[0x9] = &Chip8::OP_9xy0;
  table[0xA] = &Chip8::OP_Annn;
  table[0xB] = &Chip8::OP_Bnnn;
  table[0xC] = &Chip8::OP_Cxkk;
  table[0xD] = &Chip8::OP_Dxyn;
  table[0xE] = &Chip8::TableE;
  table[0xF] = &Chip8::TableF;
  
  // initializing tables to the null function:
  for (size_t i = 0; i <= 0xE; i++) {
    table0[i] = &Chip8::OP_NULL;
    table8[i] = &Chip8::OP_NULL;
    tableE[i] = &Chip8::OP_NULL;
  }
  for (size_t i = 0; i <= 0x65; i++) {
    tableF[i] = &Chip8::OP_NULL;
  }

  // now putting addresses into the tables:
  table0[0x0] = &Chip8::OP_00E0;
  table0[0xE] = &Chip8::OP_00EE;

  table8[0x0] = &Chip8::OP_8xy0;
  table8[0x1] = &Chip8::OP_8xy1;
  table8[0x2] = &Chip8::OP_8xy2;
  table8[0x3] = &Chip8::OP_8xy3;
  table8[0x4] = &Chip8::OP_8xy4;
  table8[0x5] = &Chip8::OP_8xy5;
  table8[0x6] = &Chip8::OP_8xy6;
  table8[0x7] = &Chip8::OP_8xy7;
  table8[0xE] = &Chip8::OP_8xyE;

  tableE[0x1] = &Chip8::OP_ExA1;
  tableE[0xE] = &Chip8::OP_Ex9E;

  tableF[0x07] = &Chip8::OP_Fx07;
  tableF[0x0A] = &Chip8::OP_Fx0A;
  tableF[0x15] = &Chip8::OP_Fx15;
  tableF[0x18] = &Chip8::OP_Fx18;
  tableF[0x1E] = &Chip8::OP_Fx1E;
  tableF[0x29] = &Chip8::OP_Fx29;
  tableF[0x33] = &Chip8::OP_Fx33;
  tableF[0x55] = &Chip8::OP_Fx55;
  tableF[0x65] = &Chip8::OP_Fx65;
}

// will modify memory to load in the ROM game instructions:
void Chip8::LoadROM(char const *filename) {
  // open the file, this creates the connection to it
  // we read it only as binary and start at the end
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  // there could be the case that the file is not in the directory, so
  //   then this wouldn't run
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
  registers[0xFu] = (registers[Vx] >= registers[Vy]) ? 1 : 0;
  registers[Vx] -= registers[Vy];
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
  registers[0xFu] = (registers[Vy] >= registers[Vx]) ? 1 : 0;
  registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  // what is at the end
  registers[0xFu] = (registers[Vx] & 0x80u) >> 7u;
  registers[Vx] <<= 1;
}

void Chip8::OP_9xy0() {
  // remember that pc would have already been incremented 
  //   2 to be on the next instruction
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  if (registers[Vx] != registers[Vy]) pc += 2;
}

void Chip8::OP_Annn() {
  uint16_t addr = (opcode & 0x0FFFu);
  index = addr;
}

void Chip8::OP_Bnnn() {
  uint16_t address = (opcode & 0x0FFFu);
  pc = registers[0] + address;
}

void Chip8::OP_Cxkk() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t byte = (opcode & 0x00FFu);
  registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode & 0x00F0u) >> 4u;
  uint8_t height = (opcode & 0x000Fu);
  // we wrap if it goes past our video
  uint8_t x_pos = registers[Vx] % VIDEO_WIDTH;
  uint8_t y_pos = registers[Vy] % VIDEO_HEIGHT;
  // assume there have been no collisions:
  registers[0xFu] = 0;
  for (uint8_t row = 0; row < height; ++row) {
    // index register is where we want to start pulling sprite from:
    uint8_t sprite_byte = memory[index + row];
    for (uint8_t col = 0; col < 8; ++col) {
      // sprite_pixel will either be 0 or non_zero (not necessarily 1)
      uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
      // coordinates for the video:
      uint8_t draw_x = (x_pos + col) % VIDEO_WIDTH;
      uint8_t draw_y = (y_pos + row) % VIDEO_HEIGHT;
      uint32_t *screen_pixel = &video[draw_y * VIDEO_WIDTH + draw_x];
      // if sprite_pixel is off we don't need to change anything
      if (sprite_pixel) {
        if (*screen_pixel) {
          // there has been a collision:
          registers[0xFu] = 1;
        }
        // we are XORing it with all 1's, so we flip what it currently is
        *screen_pixel ^= 0xFFFFFFFF;
      }
    }
  }
}

void Chip8::OP_Ex9E() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  // 16 different keys that could be getting pressed:
  uint8_t key = registers[Vx];
  if (keypad[key]) pc += 2;
}

void Chip8::OP_ExA1() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  // 16 different keys that could be getting pressed:
  uint8_t key = registers[Vx];
  if (!keypad[key]) pc += 2;
}

void Chip8::OP_Fx07() {
  // we need to know which register:
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A() {
  // which register to put the key that gets pressed in:
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0]) {
		registers[Vx] = 0;
	} else if (keypad[1]) {
		registers[Vx] = 1;
	} else if (keypad[2]) {
		registers[Vx] = 2;
	}	else if (keypad[3]) {
		registers[Vx] = 3;
	}	else if (keypad[4])	{
		registers[Vx] = 4;
	} else if (keypad[5]) {
		registers[Vx] = 5;
	} else if (keypad[6]) {
		registers[Vx] = 6;
	} else if (keypad[7]) {
		registers[Vx] = 7;
	}	else if (keypad[8]) {
		registers[Vx] = 8;
	}	else if (keypad[9]) {
		registers[Vx] = 9;
	}	else if (keypad[10]) {
		registers[Vx] = 10;
	}	else if (keypad[11]) {
		registers[Vx] = 11;
	}	else if (keypad[12]) {
		registers[Vx] = 12;
	}	else if (keypad[13]) {
		registers[Vx] = 13;
	}	else if (keypad[14]) {
		registers[Vx] = 14;
	}	else if (keypad[15]) {
		registers[Vx] = 15;
	}	else {
    // because Cycle() went two forward we will go back 2 to keep on 
    //   this instruction till a key is pressed:
		pc -= 2;
	}
}

void Chip8::OP_Fx15() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  delayTimer = registers[Vx];
}

void Chip8::OP_Fx18() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  index += registers[Vx];
}

void Chip8::OP_Fx29() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t digit = registers[Vx];
  // each of our sprite digits are only 5 bytes apart, so we can do
  //   math to know where they are
  index = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx33() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  uint8_t value = registers[Vx];
  memory[index + 2] = value % 10;
  value /= 10;
  memory[index + 1] = value % 10;
  value /= 10;
  memory[index] = value % 10;
}

void Chip8::OP_Fx55() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i <= Vx; ++i) {
    memory[index + i] = registers[i];
  }
}

void Chip8::OP_Fx65() {
  uint8_t Vx = (opcode & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i <= Vx; ++i) {
    registers[i] = memory[index + i];
  }
}

void Chip8::Cycle() {
  // Fetch:
  opcode = (static_cast<uint16_t>(memory[pc]) << 8u) | memory[pc + 1];
  // pc should be on next instruction after we got our current one:
  pc += 2;
  // decode and execute:
  // 'this' is the current instance of the chip8 hardware and we are
  //   are calling the specific function that this opcode represents on 
  //   our emulated hardware:
  ((*this).*(table[(opcode & 0xF000u) >> 12u]))();
  if (delayTimer > 0) --delayTimer;
  if (soundTimer > 0) --soundTimer;
}
