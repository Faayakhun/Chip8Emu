#include "chip8.h"
#include "AudioManager.h"
#include <SDL.h>
#include <iostream>

const int SCALE = 10;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "argc: " << argc << std::endl;
        for (int i = 0; i < argc; i++) {
            std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
        }

        if (argc != 2) {
            std::cout << "Usage: Chip8Emulator <ROM>\n";
            return 1;
        }
    }

    Chip8 chip8;
    chip8.LoadROM(argv[1]);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
    SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * SCALE, 32 * SCALE, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_GameController* controller = nullptr;
    if (SDL_NumJoysticks() > 0) {
        if (SDL_IsGameController(0)) {
            controller = SDL_GameControllerOpen(0);
            if (!controller) {
                std::cerr << "Could not open gamecontroller: " << SDL_GetError() << std::endl;
            }
        }
    }


    bool quit = false;
    SDL_Event event;

    AudioManager audio;
    bool beepPlaying = false;
    uint32_t lastTimerUpdate = SDL_GetTicks();

    while (!quit) {
        chip8.EmulateCycle();
        uint32_t currentTime = SDL_GetTicks();
        if (currentTime - lastTimerUpdate >= 1000 / 60) { // ~16ms
            if (chip8.delay_timer > 0)
                chip8.delay_timer--;

            if (chip8.sound_timer > 0)
                chip8.sound_timer--;

            lastTimerUpdate = currentTime;
        }

        if (chip8.sound_timer > 0) {
            if (!beepPlaying) {
                audio.playBeep();
                beepPlaying = true;
            }
        }
        else {
            if (beepPlaying) {
                audio.stopBeep();
                beepPlaying = false;
            }
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;

            else if (event.type == SDL_KEYDOWN) {
                // Escape key quits the program
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
                // Forward other keypresses to CHIP-8
                chip8.handleKeyPress(event.key.keysym.sym);
            }
            // User releases a key
            else if (event.type == SDL_KEYUP) {
                chip8.handleKeyRelease(event.key.keysym.sym);
            }

            else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                SDL_GameControllerButton button = static_cast<SDL_GameControllerButton>(event.cbutton.button);
                chip8.handleGamepadPress(button);
            }
            // Controller button released
            else if (event.type == SDL_CONTROLLERBUTTONUP) {
                SDL_GameControllerButton button = static_cast<SDL_GameControllerButton>(event.cbutton.button);
                chip8.handleGamepadRelease(button);
            }
        }


        if (chip8.drawFlag) {
            chip8.drawFlag = false;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            for (int y = 0; y < 32; y++) {
                for (int x = 0; x < 64; x++) {
                    if (chip8.gfx[y * 64 + x]) {
                        SDL_Rect pixel = { x * SCALE, y * SCALE, SCALE, SCALE };
                        SDL_RenderFillRect(renderer, &pixel);
                    }
                }
            }

            SDL_RenderPresent(renderer);
        }

        SDL_Delay(2);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if (controller) SDL_GameControllerClose(controller);
    SDL_Quit();

    return 0;
}
