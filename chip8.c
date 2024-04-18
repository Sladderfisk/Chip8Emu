#include "headers/chip8.h"

#include <string.h>

#include "headers/texture.h"

chip8 emu = {0};

const byte font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void InitChip8(){
    // Putting font in memory at location 0x50. Why? Idk people usually do that, but it's suposed to be somwhere around 0x000 to 0x1FF.
    memcpy(emu.memory + 0x50, font, sizeof(font));
    // The program counter (PC) should start at 0x200, because that's where the instructions start.
    emu.PC = emu.memory + 0x200;
}

void Fetch(){
    byte instruction[2] = emu.PC;
    emu.PC += 2;

    Decode(instruction);
}

void Decode(byte opcode[2]){
    switch(opcode[0]){
        
    }
}