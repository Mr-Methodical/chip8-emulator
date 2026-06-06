#ifndef SOUND_H
#define SOUND_H

#include <SDL.h>
#include <cstdint>

class Sound {
public:
  Sound();
  ~Sound();
  void set_on(bool on);

private:
  // refills the audio buffer tray:
  //   the only reason it is static because we feeding this into SDL and 
  //   SDL is C library and without static 'this' gets put in, and C doesn't
  //   know about that
  static void Callback(void * );
  SDL_AudioDeviceID deviceId{}; // handle to audio device to open/close/pause
  bool isOn{false};


  static constexpr int SAMPLE_RATE = 44100; // audio points per second
  static constexpr int AMPLITUDE = 28000; // the volume out of 32000
  static constexpr double FREQUENCY = 441.0; // wave cycles per second (pitch)
};

#endif
