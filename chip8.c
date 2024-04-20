#include "headers/chip8.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "headers/fileReader.h"
#include "headers/texture.h"
#include "headers/frame.h"

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

void SetFrame(){
    for (int y = 0; y < 32; y++){
        for (int x = 0; x < 64; x++){
            byte pixel = emu.display[y * 64 + x];
            if (pixel & 0xFF){
                SetFramePixel(x, y);
            }
        }
    }
    ReBindFrame();
}

void InitChip8(SDL_Window *win){
    // Putting font in memory at location 0x50. Why? Idk people usually do that, but it's suposed to be somwhere around 0x000 to 0x1FF.
    memcpy(emu.memory + 0x50, font, sizeof(font));
    // The program counter (PC) should start at 0x200, because that's where the instructions start.
    emu.PC = 0x200;

    char *file;
    int size;
    FILE *fptr = OpenExplorer(win, "rb");
    if (fptr == NULL) return;
    GetFileToStr(fptr, &file, &size);
    fclose(fptr);

    memcpy(emu.memory + 0x200, file, size);
    free(file);

    emu.active = true;
    //Fetch();
}

void Fetch(){
    // The Op code is 16 bits, and we need to get both bytes from memory where PC is pointing.
    uint16_t opcode = (emu.memory[emu.PC] << 8) | emu.memory[emu.PC + 1];
    // We need to append PC by 2 bytes to for next op code fetch.
    emu.PC += 2;
    Decode(opcode);
    // printf("%d\n", (0xA050 & 0xF000) >> 12);
    // Decode(0xA050);
    // Decode(0xD375);
}

void Decode(uint16_t opcode){
    switch((opcode & 0xF000) >> 12){
        case 0x00: {
            opcode &= 0x0FFF;
            switch(opcode){
                case 0xE0: // CLS:      Clear screen.
                ClearFrame();
                break;

                case 0xEE: // RET:      Return from subroutine.
                emu.PC = emu.stack[emu.stackPointer];
                emu.stackPointer--;
                break;
            }
        }
        break;

        case 0x01: { // JP addr:        Sets PC to addr.
            emu.PC = opcode & 0x0FFF;
        }
        break;

        case 0x02: { // CALL addr:      Call subroutine at addr.
            emu.stackPointer++;
            emu.stack[emu.stackPointer] = emu.PC;
        }
        break;

        case 0x03: { // SE Vx, byte:    Skip next instruction if Vx = kk.
            byte Vx = emu.variableRegister[(opcode & 0x0F00) >> 8];
            byte kk = opcode & 0x00FF;
            if (Vx == kk) emu.PC += 2;
        }
        break;

        case 0x04: { // SNE Vx, byte:    Skip next instruction if Vx != kk.
            byte Vx = emu.variableRegister[(opcode & 0x0F00) >> 8];
            byte kk = opcode & 0x00FF;
            if (Vx != kk) emu.PC += 2;
        }
        break;

        case 0x05: {
            byte Vx = emu.variableRegister[(opcode & 0x0F00) >> 8];
            byte Vy = emu.variableRegister[(opcode & 0x00F0) >> 4];
            if (Vx == Vy) emu.PC += 2;
        }
        break;

        case 0x06: {
            byte Vx = (opcode & 0x0F00) >> 8;
            byte kk = opcode & 0x00FF;
            emu.variableRegister[Vx] = kk;
        }
        break;

        case 0x07: {
            byte Vx = (opcode & 0x0F00) >> 8;
            byte kk = opcode & 0x00FF;
            emu.variableRegister[Vx] += kk;
        }
        break;

        case 0x08: {
            
        }
        break;

        case 0x09: {
            byte Vx = emu.variableRegister[(opcode & 0x0F00) >> 8];
            byte Vy = emu.variableRegister[(opcode & 0x00F0) >> 4];
            if (Vx != Vy) emu.PC++;
        }
        break;

        case 0x0A: {
            uint16_t nnn = opcode & 0x0FFFu;
            emu.stackPointer = nnn;
            printf("Sets stack pointer to: %#3x\n", nnn);
        }
        break;

        case 0x0D: {
            byte Vx = (opcode & 0x0F00) >> 8;
            byte Vy = (opcode & 0x00F0) >> 4;
            byte n = opcode & 0x000F;

            byte x = emu.variableRegister[Vx] % 64;
            byte y = emu.variableRegister[Vy] % 32;

            emu.variableRegister[0x0F] = 0;

            for (int row = 0; row < n; row++){
                byte currentByte = emu.memory[emu.stackPointer + row];

                for (int col = 0; col < 8; col++){
                    byte *pixel = &emu.display[(row + y) * 64 + (col + x)];
                    if (currentByte & (0x80 >> col)){
                    *pixel ^= 0xFF;

                        if (*pixel & 0xFF) emu.variableRegister[0x0F] = 1;
                    } 
                }
            }

            SetFrame();
        }
        break;
        default:
        printf("%d not implimented\n", (opcode & 0xF000) >> 12);
        break;
    }
}