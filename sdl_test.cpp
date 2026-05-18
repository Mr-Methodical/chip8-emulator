#include <SDL2/SDL.h>
#include <iostream>

int main() {
  // we need to boot up the video display:
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Simple Direct Media Layer 2 failed to start: "
              << SDL_GetError() << "\n";
    return 1;
  }

  // creating the window:
  SDL_Window *window = SDL_CreateWindow(
      "SDL Test",
      0, 0,
      640, 320
      SDL_WINDOW_SHOWN
  );
  
  if (window == nullptr) {
    std::cout << "Window creation failed" << SDL_GetError() << "\n";
    return 1;
  }

  // we need to have a chance to see it:
  SDL_Delay(3000);
  // Clean up time:
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  std::cout << "SDL works!\n";
  return 0; // for success
}
