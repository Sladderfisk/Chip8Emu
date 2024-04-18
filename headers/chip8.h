#pragma once

#define frameRate 60;
#define instructionsPerSec 700;

#include "headers/texture.h"

typedef struct{
    void *PC;
    byte memory[4096]; // Memory is 4kB
    byte I[2];
    byte stack[2];
    byte delayTimer;
    byte soundTimer;
    byte variableRegister[16];
    byte display[64 * 32 / 8]; // Devides to make it 64 * 32 bits   1: on  0: off
} chip8;

void Decode();