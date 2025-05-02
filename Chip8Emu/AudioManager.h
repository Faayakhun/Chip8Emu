// AudioManager.h
#pragma once
#include <SDL.h>
#include <vector>
#include <iostream>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void playBeep();
    void stopBeep();

private:
    SDL_AudioDeviceID audioDevice = 0;
    std::vector<uint8_t> beepBuffer;
};
