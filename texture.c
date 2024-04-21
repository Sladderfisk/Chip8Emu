#include "headers/texture.h"

#include <GL/glew.h>
#include <cglm/cglm.h>

#include <stdlib.h>
#include <string.h>

void CreateTexture(texture *tex, unsigned int x, unsigned int y, pixel image[]){
    tex->x = x;
    tex->y = y;
    tex->texture = malloc(x * y * 3);
    if (tex->texture == NULL) {printf("Could not create texture!\n"); return;}

    if (image != NULL) memcpy(tex->texture, image[0], x * y * 3);

    glGenTextures(1, &tex->ID);
    glBindTexture(GL_TEXTURE_2D, tex->ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->texture);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void SetTexPixel(texture *tex, pixel pix, unsigned int x, unsigned int y){
    x = glm_clamp(x, 0, tex->x - 1);
    y = glm_clamp(y, 0, tex->y - 1);

    int offset = y * tex->x + x; 
    memcpy(tex->texture + offset, pix, 3);

}

void SetImage(texture *tex, pixel *image){
    memcpy(tex->texture, image, tex->x * tex->y * 3);
}

void ReBindTex(texture *tex, shader *s){
    glBindTexture(GL_TEXTURE_2D, tex->ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex->x, tex->y, GL_RGB, GL_UNSIGNED_BYTE, tex->texture);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawTex(texture *tex){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->ID);
}

void DeleteTex(texture *tex){
    glDeleteTextures(1, &tex->ID);
    free(tex->texture);
}