#include <chrono>
#include <iostream>
#include "chip8.h"
#include "platform.h"

int main(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
    std::exit(EXIT_FAILURE);
  }
  int videoScale = std::stoi(argv[1]);
  int cycleDelay = std::stoi(argv[2]); // number of milliseconds to wait
  char const *romFilename = argv[3];
  
  // window will appear, renderer gets created and texture gets created:
  Platform platform("CHIP-8 Emulator", Chip8::VIDEO_WIDTH * videoScale, 
                    Chip8::VIDEO_HEIGHT * videoScale, Chip8::VIDEO_WIDTH, 
                    Chip8::VIDEO_HEIGHT);
  // seeds random value with time, sets pc to 0x200, loads in fonts, and
  //   wires up function pointer table:
  Chip8 chip8;
  // load the instructions into memory:
  chip8.LoadROM(romFilename);
  // pitch is just the number of bytes per row:
  int videoPitch = sizeof(*chip8.GetVideo()) * Chip8::VIDEO_WIDTH;
  // getting the precise time:
  auto lastCycleTime = std::chrono::high_resolution_clock::now();
  
  while (true) {
    // we end if user exits:
    if (platform.ProcessInput(chip8.GetKeypad())) break;
    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float,std::chrono::milliseconds::period>
      (currentTime - lastCycleTime).count();

    if (dt > cycleDelay) {
      lastCycleTime = currentTime;
      // Does one instruction, it goes through that fetch, decode,
      //   execute, and store cycle
      chip8.Cycle();
      // make the video buffer visible to the screen:
      platform.Update(chip8.GetVideo(), videoPitch);
    }
  }
  return 0;
}
