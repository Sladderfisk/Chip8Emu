#pragma once

#include <cglm/cglm.h>
#include <SDL2/SDL.h>

#include "texture.h"

extern vec2 framePos;
extern bool grabbing;
extern int width;
extern int height;

void CreateFrame(int x, int y);
void DrawFrame(SDL_Window *win);
void Clear();
void TryToGrab(SDL_Window *win);
void Release();
void DeleteFrame();