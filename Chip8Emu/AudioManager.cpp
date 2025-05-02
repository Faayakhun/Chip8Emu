// AudioManager.cpp
#include "AudioManager.h"

AudioManager::AudioManager() {
    SDL_AudioSpec want, have;
    SDL_zero(want);

    want.freq = 44100;
    want.format = AUDIO_U8;
    want.channels = 1;
    want.samples = 2048;
    want.callback = nullptr;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audioDevice == 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << "\n";
        return;
    }

    // Generate a simple square wave beep (approx 440Hz tone)
    int sampleRate = have.freq;
    int waveLength = sampleRate / 440; // 440Hz
    beepBuffer.resize(sampleRate / 10); // 0.1s beep

    for (size_t i = 0; i < beepBuffer.size(); ++i) {
        beepBuffer[i] = (i % waveLength < waveLength / 2) ? 255 : 0;
    }
}

AudioManager::~AudioManager() {
    if (audioDevice != 0) {
        SDL_CloseAudioDevice(audioDevice);
    }
}

void AudioManager::playBeep() {
    if (SDL_GetQueuedAudioSize(audioDevice) == 0) {
        SDL_QueueAudio(audioDevice, beepBuffer.data(), beepBuffer.size());
        SDL_PauseAudioDevice(audioDevice, 0); // Unpause
    }
}

void AudioManager::stopBeep() {
    SDL_ClearQueuedAudio(audioDevice);
    SDL_PauseAudioDevice(audioDevice, 1); // Pause
}
