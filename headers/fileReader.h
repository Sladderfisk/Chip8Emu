#pragma once

#include <windows.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <stdio.h>

FILE* OpenExplorer(SDL_Window* win, const char * flag);
void GetFileToStr(FILE *f, char **stor);