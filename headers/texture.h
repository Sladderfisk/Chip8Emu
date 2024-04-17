#pragma once

#include "shader.h"

typedef unsigned char   byte;
typedef byte            pixel[3];

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int ID;
    void *texture;
} texture;

void CreateTexture(texture *tex, unsigned int x, unsigned int y, pixel image[]);

void SetImage(texture *tex, pixel image[]);

void SetTexPixel(texture *tex, pixel pixel, unsigned int x, unsigned int y);

void ReBindTex(texture *tex, shader *s);

void DrawTex(texture *tex);