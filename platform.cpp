#include "platform.h"

// constructor:
Platform::Platform(char const *title, int windowWidth, int windowHeight,
                   int textureWidth, int textureHeight) {
  // window width/heigh will be number of pixels on our actual screen
  // but texture is the size in bytes of how long our thing is in the GPU
  
  // Starting Simple directmedia layer video:
  SDL_Init(SDL_INIT_VIDEO);
  // x and y for where on screen in top left to display and shown right away:
  window = SDL_CreateWindow(title, 0, 0, windowWidth,
                            windowHeight, SDL_WINDOW_SHOWN);
  // (which window to render to, -1 for first available rendering driver, ...)
  // this is what actually will do the drawing to the window
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  texture = SDL_CreateTexture(
      renderer, // which renderer is responsible to draw it
      SDL_PIXELFORMAT_RGBA8888, // pixel format matches video buffer
      SDL_TEXTUREACCESS_STREAMING, // saying we will update texture a lot
      textureWidth,
      textureHeight);
}

// destructor:
Platform::~Platform() {
  // texture depends on renderer which depends on window which depends on SDL:
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

// void *buffer because it is generic SDL can take different video buffers
//   not just uint32_t and pitch is just bytes per row
void Platform::Update(void const *buffer, int pitch) {
  // Gives GPU a copy of our video state
  SDL_UpdateTexture(texture, // which texture we are updating
                    nullptr, // specific rectangle but we do nullptr as we
                             // are updating the whole screen
                    buffer, // source pixel data
                    pitch);
  SDL_RenderClear(renderer); // cleans it up
  SDL_RenderCopy(renderer,
                    texture,
                    nullptr, // source rectangle from texture
                    nullptr); // destination rectangle (nullptr = fill window)
  SDL_RenderPresent(renderer); // show it on screen
}

// returns true if the user wants to exit the game:
// it takes a pointer so it can actually update our array in chip8
bool Platform::ProcessInput(uint8_t *keys) {
  // SDL_Event is a union which means it only allocates memory for the
  //   biggest thing in the struct, since only one button can be pressed
  //   at a time
  SDL_Event event;
  // there is a queue of things the user typed in that frame, so 
  //   SDL_PollEvent will put the first thing in the queue into event
  //   and then remove that item from the queue and keep doing that
  while (SDL_PollEvent(&event)) { // returns 0 if queue is empty
    switch (event.type) {
      case SDL_QUIT: {
        return true;
      }
      case SDL_KEYDOWN: {
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE: {
            return true;
          }
          case SDLK_x: {
            keys[0] = 1;
            break;
          }
          case SDLK_1: {
            keys[1] = 1;
            break;
          }
          case SDLK_2: {
            keys[2] = 1;
            break;
          }
          case SDLK_3: {
            keys[3] = 1;
            break;
          }
          case SDLK_q: {
            keys[4] = 1;
            break;
          }
          case SDLK_w: {
            keys[5] = 1;
            break;
          }
          case SDLK_e: {
            keys[6] = 1;
            break;
          }
          case SDLK_a: {
            keys[7] = 1;
            break;
          }
          case SDLK_s: {
            keys[8] = 1;
            break;
          }
          case SDLK_d: {
            keys[9] = 1;
            break;
          }
          case SDLK_z: {
            keys[0xA] = 1;
            break;
          }
          case SDLK_c: {
            keys[0xB] = 1;
            break;
          }
          case SDLK_4: {
            keys[0xC] = 1;
            break;
          }
          case SDLK_r: {
            keys[0xD] = 1;
            break;
          }
          case SDLK_f: {
            keys[0xE] = 1;
            break;
          }
          case SDLK_v: {
            keys[0xF] = 1;
            break;
          }
        }
      } break;
      case SDL_KEYUP: {
        switch (event.key.keysym.sym) {
          case SDLK_x: {
            keys[0] = 0;
            break;
          }
          case SDLK_1: {
            keys[1] = 0;
            break;
          }
          case SDLK_2: {
            keys[2] = 0;
            break;
          }
          case SDLK_3: {
            keys[3] = 0;
            break;
          }
          case SDLK_q: {
            keys[4] = 0;
            break;
          }
          case SDLK_w: {
            keys[5] = 0;
            break;
          }
          case SDLK_e: {
            keys[6] = 0;
            break;
          }
          case SDLK_a: {
            keys[7] = 0;
            break;
          }
          case SDLK_s: {
            keys[8] = 0;
            break;
          }
          case SDLK_d: {
            keys[9] = 0;
            break;
          }
          case SDLK_z: {
            keys[0xA] = 0;
            break;
          }
          case SDLK_c: {
            keys[0xB] = 0;
            break;
          }
          case SDLK_4: {
            keys[0xC] = 0;
            break;
          }
          case SDLK_r: {
            keys[0xD] = 0;
            break;
          }
          case SDLK_f: {
            keys[0xE] = 0;
            break;
          }
          case SDLK_v: {
            keys[0xF] = 0;
            break;
          }
        }
      } break;
    }
  }
  return false;
}
