#include "sound.h"
// to get the sin function and pi to make a nicer sound
#include <cmath>

Sound::Sound() {
  // start up the audio hardware:
  SDL_InitSubSystem(SDL_INIT_AUDIO);

  // a structure of specifications of what we want from the audio (eg. freq):
  SDL_AudioSpec want;
  SDL_zero(want); // we don't want garbage values in it
  want.freq = SAMPLE_RATE; // 44100 audio snapshots every second
  // Sys is just telling computer to use whatever the system byte order is
  //   like to use little endian or big endian first (little endian would
  //   do the reverse way it would like do 0x1234 would send 34 then 12)
  want.format = AUDIO_S16SYS; // sound represented as 16 bit numbers
  want.channel = 1; // only one speaker
  want.samples = 2048; // the number of sound instructions it will take at once
  want.callback = Callback; // the function to call when 2048 buffer used up
  a
  SDL_AudioSpec have;

}

void Sound::SetOn(bool on) {
  isOn = on;
}
