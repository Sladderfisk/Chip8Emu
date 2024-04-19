#pragma once

#define frameRate 60;
#define instructionsPerSec 700;

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "texture.h"

typedef struct{
    bool active;
    int PC;
    byte memory[4096]; // Memory is 4kB
    uint16_t stackPointer;
    byte stack[16];
    byte delayTimer;
    byte soundTimer;
    byte variableRegister[16];
    byte display[64 * 32 / 8]; // Devides to make it 64 * 32 bits   1: on  0: off
} chip8;

extern chip8 emu;

void InitChip8(SDL_Window *win);
void Fetch();
void Decode(uint16_t opernad);