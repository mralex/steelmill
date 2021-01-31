#pragma once

#include <cstdint>
#include <memory>

#include <SDL.h>

class Sample {
public:
    Sample(int sampleRate, int length) : sampleRate(sampleRate), length(length) {
        size = sampleRate * length;
        raw = (float *)malloc(sizeof(float) * size);
    }

    ~Sample() {
        free(raw);
    }

    float& operator [](const int i) {
        return raw[i];
    }
    float operator [](const int i) const
    {
        return raw[i];
    }

    int sampleRate;
    int length;
    int size;
    float *raw;
    int playbackPosition;
    
    void play();
    void stop();

private:
    float audioInitialized = false;
    SDL_AudioDeviceID audioDevice;
    
    static void SDLAudioCallback(void *data, Uint8 *buffer, int length);
};

