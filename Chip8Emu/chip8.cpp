#include "chip8.h"
#include <fstream>
#include <iostream>

uint8_t fontset[80] = {
  0xF0,0x90,0x90,0x90,0xF0, // 0
  0x20,0x60,0x20,0x20,0x70, // 1
  0xF0,0x10,0xF0,0x80,0xF0, // 2
  0xF0,0x10,0xF0,0x10,0xF0, // 3
  0x90,0x90,0xF0,0x10,0x10, // 4
  0xF0,0x80,0xF0,0x10,0xF0, // 5
  0xF0,0x80,0xF0,0x90,0xF0, // 6
  0xF0,0x10,0x20,0x40,0x40, // 7
  0xF0,0x90,0xF0,0x90,0xF0, // 8
  0xF0,0x90,0xF0,0x10,0xF0, // 9
  0xF0,0x90,0xF0,0x90,0x90, // A
  0xE0,0x90,0xE0,0x90,0xE0, // B
  0xF0,0x80,0x80,0x80,0xF0, // C
  0xE0,0x90,0x90,0x90,0xE0, // D
  0xF0,0x80,0xF0,0x80,0xF0, // E
  0xF0,0x80,0xF0,0x80,0x80  // F
};

Chip8::Chip8() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;
    gfx.fill(0);
    memory.fill(0);
    V.fill(0);
    stack.fill(0);
    for (int i = 0; i < 80; ++i)
        memory[i] = fontset[i];
    delay_timer = 0;
    sound_timer = 0;
}

void Chip8::LoadROM(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        std::cerr << "Failed to open ROM\n";
        return;
    }

    fread(&memory[0x200], 1, 3584, file);
    fclose(file);
}

void Chip8::EmulateCycle() {
    // Fetch opcode (each is 2 bytes)
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode and execute
    ExecuteOpcode();

    // Update timers
    if (delay_timer > 0) --delay_timer;
    if (delay_timer > 0) --delay_timer;
}

void Chip8::setKey(uint8_t keyIndex, bool pressed) {
    if (keyIndex < 16) {
        key[keyIndex] = pressed ? 1 : 0;
    }
}

void Chip8::resetKeys() {
    for (int i = 0; i < 16; i++) {
        key[i] = 0;
    }
}

void Chip8::handleKeyPress(SDL_Keycode keycode) {
    // Map SDL keyboard keys to CHIP-8 keys
    switch (keycode) {
    case SDLK_1: key[0x1] = 1; break;
    case SDLK_2: key[0x2] = 1; break;
    case SDLK_3: key[0x3] = 1; break;
    case SDLK_4: key[0xC] = 1; break;

    case SDLK_q: key[0x4] = 1; break;
    case SDLK_w: key[0x5] = 1; break;
    case SDLK_e: key[0x6] = 1; break;
    case SDLK_r: key[0xD] = 1; break;

    case SDLK_a: key[0x7] = 1; break;
    case SDLK_s: key[0x8] = 1; break;
    case SDLK_d: key[0x9] = 1; break;
    case SDLK_f: key[0xE] = 1; break;

    case SDLK_z: key[0xA] = 1; break;
    case SDLK_x: key[0x0] = 1; break;
    case SDLK_c: key[0xB] = 1; break;
    case SDLK_v: key[0xF] = 1; break;

    default: break;
    }
}

void Chip8::handleKeyRelease(SDL_Keycode keycode) {
    // Map SDL keyboard keys to CHIP-8 keys
    switch (keycode) {
    case SDLK_1: key[0x1] = 0; break;
    case SDLK_2: key[0x2] = 0; break;
    case SDLK_3: key[0x3] = 0; break;
    case SDLK_4: key[0xC] = 0; break;

    case SDLK_q: key[0x4] = 0; break;
    case SDLK_w: key[0x5] = 0; break;
    case SDLK_e: key[0x6] = 0; break;
    case SDLK_r: key[0xD] = 0; break;

    case SDLK_a: key[0x7] = 0; break;
    case SDLK_s: key[0x8] = 0; break;
    case SDLK_d: key[0x9] = 0; break;
    case SDLK_f: key[0xE] = 0; break;

    case SDLK_z: key[0xA] = 0; break;
    case SDLK_x: key[0x0] = 0; break;
    case SDLK_c: key[0xB] = 0; break;
    case SDLK_v: key[0xF] = 0; break;

    default: break;
    }
}

//expand to support controller

void Chip8::handleGamepadPress(SDL_GameControllerButton button) {
    switch (button) {
    case SDL_CONTROLLER_BUTTON_A:
        key[0x0] = 1; break;
    case SDL_CONTROLLER_BUTTON_B:
        key[0x1] = 1; break;
    case SDL_CONTROLLER_BUTTON_X:
        key[0x2] = 1; break;
    case SDL_CONTROLLER_BUTTON_Y:
        key[0x3] = 1; break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        key[0x4] = 1; break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        key[0x5] = 1; break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        key[0x6] = 1; break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        key[0x7] = 1; break;
    default: break;
    }
}

void Chip8::handleGamepadRelease(SDL_GameControllerButton button) {
    switch (button) {
    case SDL_CONTROLLER_BUTTON_A:
        key[0x0] = 0; break;
    case SDL_CONTROLLER_BUTTON_B:
        key[0x1] = 0; break;
    case SDL_CONTROLLER_BUTTON_X:
        key[0x2] = 0; break;
    case SDL_CONTROLLER_BUTTON_Y:
        key[0x3] = 0; break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        key[0x4] = 0; break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        key[0x5] = 0; break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        key[0x6] = 0; break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        key[0x7] = 0; break;
    default: break;
    }
}


void Chip8::ExecuteOpcode() {
    uint16_t nnn = opcode & 0x0FFF;      // Address
    uint8_t  n = opcode & 0x000F;        // Lowest nibble
    uint8_t  x = (opcode & 0x0F00) >> 8; // Register X
    uint8_t  y = (opcode & 0x00F0) >> 4; // Register Y
    uint8_t  kk = opcode & 0x00FF;       // Byte

    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode & 0x00FF) {
        case 0x00E0:  // CLS
            std::fill(std::begin(gfx), std::end(gfx), 0);
            drawFlag = true;
            pc += 2;
            break;
        case 0x00EE:  // RET
            --sp;
            pc = stack[sp];
            pc += 2;
            break;
        default:
            std::cout << "Unknown opcode [0x0000]: " << std::hex << opcode << "\n";
            pc += 2;
            break;
        }
        break;

    case 0x1000:  // JP addr
        pc = nnn;
        break;

    case 0x2000:  // CALL addr
        stack[sp] = pc;
        ++sp;
        pc = nnn;
        break;

    case 0x3000:  // SE Vx, byte - Skip next instruction if Vx = kk
        pc += (V[x] == kk) ? 4 : 2;
        break;

    case 0x4000:  // SNE Vx, byte - Skip next instruction if Vx != kk
        pc += (V[x] != kk) ? 4 : 2;
        break;

    case 0x5000:  // SE Vx, Vy - Skip next instruction if Vx = Vy
        pc += (V[x] == V[y]) ? 4 : 2;
        break;

    case 0x6000:  // LD Vx, byte
        V[x] = kk;
        pc += 2;
        break;

    case 0x7000:  // ADD Vx, byte
        V[x] += kk;
        pc += 2;
        break;

    case 0x8000: {
        switch (opcode & 0x000F) {
        case 0x0: V[x] = V[y]; pc += 2; break;
        case 0x1: V[x] |= V[y]; pc += 2; break;
        case 0x2: V[x] &= V[y]; pc += 2; break;
        case 0x3: V[x] ^= V[y]; pc += 2; break;
        case 0x4: {
            uint16_t sum = V[x] + V[y];
            V[0xF] = (sum > 255) ? 1 : 0;
            V[x] = sum & 0xFF;
            pc += 2;
            break;
        }
        case 0x5: V[0xF] = (V[x] > V[y]) ? 1 : 0; V[x] -= V[y]; pc += 2; break;
        case 0x6: V[0xF] = V[x] & 0x1; V[x] >>= 1; pc += 2; break;
        case 0x7: V[0xF] = (V[y] > V[x]) ? 1 : 0; V[x] = V[y] - V[x]; pc += 2; break;
        case 0xE: V[0xF] = (V[x] & 0x80) >> 7; V[x] <<= 1; pc += 2; break;
        default:
            std::cerr << "Unknown 8XY_ opcode: " << std::hex << opcode << "\n";
            pc += 2;
        }
        break;
    }

    case 0x9000:  // SNE Vx, Vy - Skip next instruction if Vx != Vy
        if ((opcode & 0x000F) == 0x0) {
            pc += (V[x] != V[y]) ? 4 : 2;
        }
        else {
            std::cout << "Unknown opcode [0x9000]: " << std::hex << opcode << "\n";
            pc += 2;
        }
        break;

    case 0xA000:  // LD I, addr
        I = nnn;
        pc += 2;
        break;

    case 0xB000:  // JP V0, addr - Jump to location nnn + V0
        pc = nnn + V[0];
        break;

    case 0xC000:  // RND Vx, byte - Set Vx = random byte AND kk
        V[x] = (rand() % 256) & kk;
        pc += 2;
        break;

    case 0xD000: {  // DRW Vx, Vy, nibble
        uint8_t xPos = V[x] % 64;
        uint8_t yPos = V[y] % 32;
        uint8_t height = n;
        V[0xF] = 0;

        for (int row = 0; row < height; ++row) {
            uint8_t spriteByte = memory[I + row];
            for (int col = 0; col < 8; ++col) {
                if ((spriteByte & (0x80 >> col)) != 0) {
                    int index = (xPos + col + ((yPos + row) * 64)) % (64 * 32);
                    if (gfx[index] == 1) V[0xF] = 1;
                    gfx[index] ^= 1;
                }
            }
        }

        drawFlag = true;
        pc += 2;
        break;
    }

    case 0xE000:
        switch (kk) {
        case 0x9E:  // SKP Vx - Skip next instruction if key with value Vx is pressed
            pc += key[V[x]] ? 4 : 2;
            break;
        case 0xA1:  // SKNP Vx - Skip next instruction if key with value Vx is not pressed
            pc += !key[V[x]] ? 4 : 2;
            break;
        default:
            std::cout << "Unknown opcode [0xE000]: " << std::hex << opcode << "\n";
            pc += 2;
        }
        break;

    case 0xF000:
        switch (kk) {
        case 0x07:  // LD Vx, DT - Set Vx = delay timer value
            V[x] = delay_timer;
            pc += 2;
            break;
        case 0x0A: {  // LD Vx, K - Wait for a key press, store the value of the key in Vx
            bool keyPressed = false;
            for (int i = 0; i < 16; ++i) {
                if (key[i]) {
                    V[x] = i;
                    keyPressed = true;
                    break;
                }
            }
            // Only increment PC if a key was pressed
            if (keyPressed) {
                pc += 2;
            }
            break;
        }
        case 0x15:  // LD DT, Vx - Set delay timer = Vx
            delay_timer = V[x];
            pc += 2;
            break;
        case 0x18:  // LD ST, Vx - Set sound timer = Vx
            sound_timer = V[x];
            pc += 2;
            break;
        case 0x1E:  // ADD I, Vx - Set I = I + Vx
            // Some implementations set VF if I overflows, but this is uncommon
            I += V[x];
            pc += 2;
            break;
        case 0x29:  // LD F, Vx - Set I = location of sprite for digit Vx
            I = V[x] * 5;  // Each font character is 5 bytes
            pc += 2;
            break;
        case 0x33: {  // LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2
            uint8_t value = V[x];
            memory[I] = value / 100;
            memory[I + 1] = (value / 10) % 10;
            memory[I + 2] = value % 10;
            pc += 2;
            break;
        }
        case 0x55: {  // LD [I], Vx - Store registers V0 through Vx in memory starting at location I
            for (int i = 0; i <= x; ++i) {
                memory[I + i] = V[i];
            }
            pc += 2;
            break;
        }
        case 0x65: {  // LD Vx, [I] - Read registers V0 through Vx from memory starting at location I
            for (int i = 0; i <= x; ++i) {
                V[i] = memory[I + i];
            }
            pc += 2;
            break;
        }
        default:
            std::cout << "Unknown opcode [0xF000]: " << std::hex << opcode << "\n";
            pc += 2;
        }
        break;

    default:
        std::cout << "Unknown opcode: " << std::hex << opcode << "\n";
        pc += 2;
        break;
    }
}


// reserved for audio