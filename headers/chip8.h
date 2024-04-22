#pragma once

#define frameRate 60;
#define instructionsPerSec 700;

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "texture.h"

typedef struct{
    bool active;
    byte spritesThisSec;
    byte input[0x10];
    int PC;
    byte memory[4096]; // Memory is 4kB
    int stack[16];
    uint16_t I;
    byte stackPointer;
    byte delayTimer;
    byte soundTimer;
    byte variableRegister[16];
    byte display[64 * 32];
} chip8;

extern chip8 emu;

void InitChip8(SDL_Window *win);
void Fetch();
void Decode(uint16_t opernad);
void SetFrame();
