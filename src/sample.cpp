#include "sample.hpp"
#include <iostream>
#include <cstdio>

static const int F_TO_8 = sizeof(float) / sizeof(Uint8);

void Sample::play()
{
    // Uint8 *samples = (Uint8*)malloc(sizeof(Uint8) * size);
    // for(int i = 0; i < length; i++) {
    //     int out;
    //     float data = raw[i];

    //     if (data >= 1.0)
    //         out = 254;
    //     else if (data <= -1.0)
    //         out = 0;
    //     else
    //         out = (int)((data + 1.0) * 255.0);

    //     samples[i] = out;
    // }

    // int result = Mix_OpenAudio(sampleRate, AUDIO_F32, 2, 512);
    // if (result < 0)
    // {
    //     fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
    //     exit(-1);
    // }

    // result = Mix_AllocateChannels(2);
    // if (result < 0)
    // {
    //     fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
    //     exit(-1);
    // }

    // Mix_Chunk *mixSample;
    // // memset(mixSample, 0, sizeof(Mix_Chunk));
    // mixSample = Mix_QuickLoad_RAW(samples, length);
    // if (mixSample == nullptr)
    // {
    //     fprintf(stderr, "Unable to load raw data: %s\n", SDL_GetError());
    //     exit(-1);
    // }

    // Mix_PlayChannel(-1, mixSample, 0);

    if (!audioInitialized) {

        // int i, audioDeviceCount = SDL_GetNumAudioDevices(0);
        // for (i = 0; i < audioDeviceCount; ++i)
        // {
        //     printf("Audio device %d: %s\n", i, SDL_GetAudioDeviceName(i, 0));
        // }

        SDL_AudioSpec have;
        SDL_AudioSpec spec = {0};
        spec.freq = sampleRate;
        spec.format = AUDIO_F32;
        spec.channels = 1;
        spec.samples = 512;
        spec.callback = SDLAudioCallback;
        spec.userdata = this;

        // const unsigned int deviceIndex = 0;
        // const char* deviceName = SDL_GetAudioDeviceName(deviceIndex, 0);
        // if (deviceName == nullptr) {
        //     printf("Failed to find audio device: %s", SDL_GetError());
        //     return;
        // }
        // printf("Device: %s\n", deviceName);
        audioDevice = SDL_OpenAudioDevice(NULL, 0, &spec, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
        if (audioDevice == 0)
        {
            printf("Failed to open audio: %s\n", SDL_GetError());
            return;
        }

        if (spec.format != have.format) {
            printf("Didn't get Float32 audio: %d\n", have.format);
        }

        audioInitialized = true;
    }

    playbackPosition = 0;
    SDL_PauseAudioDevice(audioDevice, 0);

    // SDL_CloseAudioDevice(device);
}

void Sample::stop() {
    if (!audioInitialized) {
        return;
    }

    SDL_PauseAudioDevice(audioDevice, 1);
}

void Sample::SDLAudioCallback(void *data, Uint8 *buffer, int length) {
    float* fBuffer = reinterpret_cast<float*>(buffer);
    Sample *sample = reinterpret_cast<Sample*>(data);

    int len = length / F_TO_8;

    for (int i = 0; i < len; i++)
    {
        if (sample->playbackPosition < sample->length) {
            fBuffer[i] = sample->raw[sample->playbackPosition];
            sample->playbackPosition++;
        } else {
            fBuffer[i] = 0;
        }
    }
}
