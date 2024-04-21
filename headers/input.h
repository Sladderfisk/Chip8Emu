#pragma once

#include <stdbool.h>
#include<SDL2/SDL.h>

void SaveEvents(SDL_Event* e, int index);
void EndInput();
bool GetKeyDown(SDL_Keycode key);