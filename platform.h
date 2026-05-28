#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL.h>
#include <cstdint>

class Platform {
public:
  Platform(char const *title, int windowWidth, int windowHeight,
           int textureWidth, int textureHeight); // the constructor
  ~Platform(); // the destructor
  void Update(void const *buffer, int pitch);
  bool ProcessInput(uint8_t *keys);
private:
  SDL_Window *window{}; // the actual window that I will see. SHOWS IT!
  SDL_Renderer *renderer{}; // draws up texture scaled up and talks to GPU
                            // to accelerate rendering. DRAWS IT!
  SDL_Texture *texture{}; // our 64 by 32 screen sitting in GPU memory
};

#endif
