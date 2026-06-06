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
  want.channels = 1; // only one speaker
  want.samples = 2048; // the number of sound instructions it will take at once
  want.callback = Callback; // the function to call when 2048 buffer used up
  want.userdata = this; // because we do static but still want to access fields
                        // in the current instance (we basically store Sound *)
                        // SDL treats it as a void *
  SDL_AudioSpec have;
  // this will return audio device handle that we can call on later
  deviceId = SDL_OpenAudioDevice(nullptr, // default audio device
                                 0, // no microphone only speakers
                                 &want, // what we are are asking for
                                 &have, // SDL fills with what we have
                                 0); // don't let SDL change our settings
  // We want unpaused so we can start playing (0 is for unpause):
  SDL_PauseAudioDevice(deviceId, 0);
}

void Sound::SetOn(bool on) {
  isOn = on;
}

Sound::~Sound() {
  // stops callback from being called, just closes the connection to audio 
  //   device
  SDL_CloseAudioDevice(deviceId);
  // shuts down the audio subsystem:
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

// Goal: keep computer's sound card fed with raw audio data so it never runs
//   dry. 
// SDL handles the multithreading, we are just giving it this function so it
//   knows what to do to get more sound data
void Sound::Callback(
    void *userData, // pointer to the specific instance of the sound object
                    // allowing us to access the fields
    uint8_t *buffer, // the memory needed to be filled with sound values
    int length) { // the number of bytes that the buffer is
  // We want to recover the current object:
  Sound *sound = static_cast<Sound*>(userData); // static cast better safer
                                                // than C cast
  // we need a signed int so we use SDL's type
  // now when we put data in it won't be a hastle, it will just take up 16 bits
  //   instead of 8 bits (like how it normally would)
  Sint16 *stream = reinterpret_cast<Sint16*>(buffer);
  // each sample is two bytes:
  int samples = length / 2;
  for (int i = 0; i < samples; ++i) {
    if (sound->isOn) {
      // samplePos is where we are and sample_rate is samples per second
      //   so we can cancel them out and get time
      double time = static_cast<double>(sound->samplePos) / SAMPLE_RATE;
      // multiplying 2pi by frequency makes it complete 441 cycles every 
      //   second, and then multiplying by time gets us what part of 
      //   cycle we are on
      stream[i] = static_cast<Sint16>(
        AMPLITUDE * std::sin(2.0 * M_PI * FREQUENCY * time)
      );
      sound->samplePos += 1;
    } else {
      stream[i] = 0; // silence
      sound->samplePos = 0; // reset so it can start cleanly on the next go
    }
  }
}
