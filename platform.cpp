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
  SDL_RendererClear(renderer); // cleans it up
  SDL_RendererCopy(renderer,
                    texture,
                    nullptr, // source rectangle from texture
                    nullptr); // destination rectangle (nullptr = fill window)
  SDL_RendererPresent(renderer); // show it on screen
}


