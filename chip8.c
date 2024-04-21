#include "headers/chip8.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "headers/input.h"
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

uint16_t Getnnn(uint16_t opcode){
    return opcode & 0x0FFF;
}

byte Getn(uint16_t opcode){
    return opcode & 0x000F;
}

byte *GetVx(uint16_t opcode){
    return &emu.variableRegister[(opcode & 0x0F00) >> 8];
}

byte *GetVy(uint16_t opcode){
    return &emu.variableRegister[(opcode & 0x00F0) >> 4];
}

byte Getkk(uint16_t opcode){
    return opcode & 0x00FF;
}

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

byte Chip8KeyDown(byte key){
    printf("Inpuzzy\n");
    switch(key){
        case 0x0:
            return GetKeyDown(SDLK_x) | (key << 4);
        break;

        case 0x1:
            return GetKeyDown(SDLK_1) | (key << 4);
        break;

        case 0x2:
            return GetKeyDown(SDLK_2) | (key << 4);
        break;

        case 0x3:
            return GetKeyDown(SDLK_3) | (key << 4);
        break;

        case 0x4:
            return GetKeyDown(SDLK_q) | (key << 4);
        break;

        case 0x5:
            return GetKeyDown(SDLK_w) | (key << 4);
        break;

        case 0x6:
            return GetKeyDown(SDLK_e) | (key << 4);
        break;

        case 0x7:
            return GetKeyDown(SDLK_a) | (key << 4);
        break;

        case 0x8:
            return GetKeyDown(SDLK_s) | (key << 4);
        break;

        case 0x9:
            return GetKeyDown(SDLK_d) | (key << 4);
        break;

        case 0xA:
            return GetKeyDown(SDLK_z) | (key << 4);
        break;

        case 0xB:
            return GetKeyDown(SDLK_c) | (key << 4);
        break;
        
        case 0xC:
            return GetKeyDown(SDLK_4) | (key << 4);
        break;

        case 0xD:
            return GetKeyDown(SDLK_r) | (key << 4);
        break;

        case 0xE:
            return GetKeyDown(SDLK_f) | (key << 4);
        break;

        case 0xF:
            return GetKeyDown(SDLK_v) | (key << 4);
        break;

        default: 
            return 0x0000;
        break;
    }
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
}

void Fetch(){
    // The Op code is 16 bits, and we need to get both bytes from memory where PC is pointing.
    uint16_t opcode = (emu.memory[emu.PC] << 8) | emu.memory[emu.PC + 1];
    // We need to append PC by 2 bytes to for next op code fetch.
    emu.PC += 2;
    Decode(opcode);
}

void Decode(uint16_t opcode){
    printf("%#2x\n", (opcode & 0xF000) >> 12);
    switch((opcode & 0xF000) >> 12){
        case 0x00: {
            printf("opcode: %#4x\n", opcode);
            switch(opcode){
                case 0x00E0: // CLS:      Clear screen.
                ClearFrame();
                break;

                case 0x00EE: // RET:      Return from subroutine.
                printf("Return from subroutine %d\n", emu.stackPointer);
                emu.stackPointer--;
                emu.PC = emu.stack[emu.stackPointer];
                break;
            }
        }
        break;

        case 0x01: { // JP addr:        Sets PC to addr.
            emu.PC = Getnnn(opcode);
        }
        break;

        case 0x02: { // CALL addr:      Call subroutine at addr.
            printf("PC : %#8x\n", emu.PC);
            emu.stack[emu.stackPointer] = emu.PC;
            ++emu.stackPointer;
            emu.PC = Getnnn(opcode);
        }
        break;

        case 0x03: { // SE Vx, byte:    Skip next instruction if Vx = kk.
            byte *Vx = GetVx(opcode);
            byte kk = Getkk(opcode);
            if (*Vx == kk) emu.PC += 2;
        }
        break;

        case 0x04: { // SNE Vx, byte:    Skip next instruction if Vx != kk.
            byte *Vx = GetVx(opcode);
            byte kk = Getkk(opcode);
            if (*Vx != kk) emu.PC += 2;
        }
        break;

        case 0x05: {
            byte *Vx = GetVx(opcode);
            byte *Vy = GetVy(opcode);
            if (*Vx == *Vy) emu.PC += 2;
        }
        break;

        case 0x06: {
            byte *Vx = GetVx(opcode);
            byte kk = Getkk(opcode);
            *Vx = kk;
        }
        break;

        case 0x07: {
            byte *Vx = GetVx(opcode);
            byte kk = Getkk(opcode);
            *Vx += kk;
        }
        break;

        case 0x08: {
            byte *Vx = GetVx(opcode);
            byte *Vy = GetVy(opcode);
            
            switch(opcode & 0xF){
                case 0x0: {
                    *Vx = *Vy;
                }
                break;

                case 0x1: {
                    *Vx |= *Vy;
                }
                break;

                case 0x2: {
                    *Vx &= *Vy;
                }
                break;

                case 0x3: {
                    *Vx ^= *Vy;
                }
                break;

                case 0x4: {
                    int16_t val = (*Vx + *Vy);
                    *Vx = val & 0xFF;
                    emu.variableRegister[0xF] = val > 255;
                }
                break;

                case 0x5: {
                    int16_t val = *Vx - *Vy;
                    *Vx = val;
                    emu.variableRegister[0xF] = val >= 0;
                }
                break;

                case 0x6: {
                    int16_t val = *Vx >> 1;
                    byte ogVx = *Vx;
                    *Vx = val;
                    emu.variableRegister[0xF] = ogVx & 0x01;
                }
                break;

                case 0x7: {
                    int16_t val = *Vy - *Vx;
                    *Vx = val;
                    emu.variableRegister[0xF] = val >= 0;
                }
                break;

                case 0xE: {
                    int16_t val = *Vx << 1;
                    byte ogVx = *Vx;
                    *Vx = val;
                    emu.variableRegister[0xF] = (ogVx >> 7) & 0x01;
                }
                break;
            }
        }
        break;

        case 0x09: {
            byte *Vx = GetVx(opcode);
            byte *Vy = GetVy(opcode);
            if (*Vx != *Vy) emu.PC += 2;
        }
        break;

        case 0x0A: {
            uint16_t nnn = Getnnn(opcode);
            emu.I = nnn;
        }
        break;

        case 0x0B: {
            uint16_t nnn = Getnnn(opcode);
            byte V0 = emu.variableRegister[0];
            emu.PC = nnn + V0;
        }
        break;

        case 0x0C: {
            byte *Vx = GetVx(opcode);
            byte kk = Getkk(opcode);
            srand(time(NULL));
            *Vx = (rand() % 255 + 0) & kk;
        }
        break;

        case 0x0D: {
            byte *Vx = GetVx(opcode);
            byte *Vy = GetVy(opcode);
            byte n = Getn(opcode);

            byte x = *Vx % 64;
            byte y = *Vy % 32;

            emu.variableRegister[0x0F] = 0;

            for (int row = 0; row < n; row++){
                byte currentByte = emu.memory[emu.I + row];

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

        case 0x0E: {
            byte *Vx = GetVx(opcode);

            switch(opcode & 0x00FF) {
                case 0x9E: {
                    if (Chip8KeyDown(*Vx) & 0x1) emu.PC += 2;
                }
                break;

                case 0xA1: {
                    if (!(Chip8KeyDown(*Vx) & 0x1)) emu.PC += 2;
                }
                break;
            }
        }
        break;

        case 0x0F: {
            byte *Vx = GetVx(opcode);

            switch(opcode & 0x00FF) {
                case 0x07: {
                    *Vx = emu.delayTimer;
                }
                break;

                case 0x0A: {
                    printf("Waiting for input\n");
                    byte val = 0;
                    for (int i = 0; i < 0x10; i++){
                        val = Chip8KeyDown(i);
                        if (val & 0x1) break;
                    }

                    if (!(val & 0x1)) emu.PC -= 2;
                    else *Vx = (val >> 4);
                }
                break;

                case 0x15: {
                    emu.delayTimer = *Vx;
                }
                break;

                case 0x18: {
                    emu.soundTimer = *Vx;
                }
                break;

                case 0x1E: {
                    int16_t val = emu.I + *Vx;
                    emu.variableRegister[0x0F] = val > 0x1000;
                    emu.I = val;
                }
                break;

                case 0x29: {
                    emu.I = 0x50 + *Vx * 5;
                }
                break;

                case 0x33: {
                    uint8_t val = *Vx;
                    emu.memory[emu.I + 2] = val % 10;

                    val /= 10;
                    emu.memory[emu.I + 1] = val % 10;

                    val /= 10;
                    emu.memory[emu.I] = val % 10;
                }
                break;

                case 0x55: {
                    byte x = (opcode & 0x0F00) >> 8; 
                    for (int i = 0; i <= x; i++){
                        emu.memory[emu.I + i] = emu.variableRegister[i];
                    }
                }
                break;

                case 0x65: {
                    byte x = (opcode & 0x0F00) >> 8; 
                    for (int i = 0; i <= x; i++){
                        emu.variableRegister[i] = emu.memory[emu.I + i];
                    }
                }
                break;
            }
        }
        break;

        default:
        printf("%d not implimented\n", (opcode & 0xF000) >> 12);
        break;
    }
}