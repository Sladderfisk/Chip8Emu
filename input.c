#include "headers/input.h"

#include<stdio.h>

SDL_Event ev[64] = {0};
int amount;
void SaveEvents(SDL_Event* e, int i){
    if (i > 64) return;
    amount++;
    ev[i] = *e;
}

void EndInput(){
    amount = 0;
}

bool GetKeyDown(SDL_Keycode key){
    for (int i = 0; i < amount; i++){
        if (key == ev[i].key.keysym.sym) return true;
    }
    return false;
}