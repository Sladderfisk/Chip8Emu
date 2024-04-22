#include "headers/frame.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <string.h>

#include "headers/shader.h"

const pixel black = {0, 0, 0};
const pixel white = {255, 255, 255};

pixel* clearImage = {0};

vec2 framePos = {0, 0};
bool grabbing = false;
int width = 0;
int height = 0;

texture tex = {0};
shader sh = {0};
unsigned int VBO = 0, VAO = 0, EBO = 0;


void CreateBlankImage(){
    clearImage = malloc(width * height * 3);

    for (int i = 0; i < height * width;i++){
        memcpy(&clearImage[i], black, 3);
    }
}

void CreateFrame(int x, int y){
    width = x;
    height = y;
    CreateTexture(&tex, x, y, NULL);
    CreateBlankImage();
    ClearFrame();

    CompileShader(&sh, "shaders/sh.vert", "shaders/sh.frag");
    float halfX = x /2;
    float halfY = y /2;
    float vertices[] = {
        -halfX, halfY, 0.0f,  0.0f, 1.0f,
        halfX, halfY, 0.0f,   1.0f, 1.0f,
        -halfX, -halfY, 0.0f, 0.0f, 0.0f,
        halfX, -halfY, 0.0f, 1.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        1, 3, 2
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    ClearFrame();
}

void DrawFrame(SDL_Window *win){
    
    DrawTex(&tex);

    glUseProgram(sh.ID);
    mat4 proj = {0};
    
    int win_width, win_height;
    SDL_GetWindowSize(win, &win_width, &win_height);
    glm_ortho(-win_width / 2, win_width / 2, -win_height / 2, win_height / 2, -1000.0f, 1000.0f, proj); 

    mat4 view = {0};
    vec3 eye = {0}; eye[0] = 0.0f; eye[1] = 0.0f; eye[2] = 1.0f;
    vec3 center = {0}; center[0] = 0.0f; center[1] = 0.0f; center[2] = 0.0f;
    vec3 up = {0}; up[0] = 0.0f; up[1] = 1.0f; up[2] = 0.0f;
    glm_lookat(eye, center, up, view);
    
    mat4 tra = {0};
    glm_mat4_identity(tra);
    float newPos[] = {framePos[0], framePos[1], 0.0f};
    glm_translate(tra, newPos);

    glm_mat4_identity(view);

    glUniformMatrix4fv(glGetUniformLocation(sh.ID, "model"), 1, GL_FALSE, &tra[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh.ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sh.ID, "projection"), 1, GL_FALSE, &proj[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ClearFrame(){
    SetImage(&tex, clearImage);
    //ReBindTex(&tex, &sh);
}

void TryToGrab(SDL_Window *win){
    int mX;
    int mY;
    SDL_GetMouseState(&mX, &mY);
    int win_width, win_height;
    SDL_GetWindowSize(win, &win_width, &win_height);
    mY = win_height - mY;

    int halfX = win_width / 2 + framePos[0];
    int halfY = win_height / 2 + framePos[1];
    int halfWidth = width / 2;
    int halfHeight = height / 2;

    if (halfX - halfWidth < mX && halfX + halfWidth > mX && halfY - halfHeight < mY && halfY + halfHeight > mY) grabbing = true;
    else grabbing = false;
}

void Release(){
    grabbing = false;
}

void DeleteFrame(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    DeleteTex(&tex);
    DeleteShader(&sh);
    free(clearImage);
}

void SetFramePixel(int xPos, int yPos, bool on){
    yPos = 31 - yPos;
    int size = width / 64;
    for (int y = 0; y < size; y++){
        for (int x = 0; x < size; x++){
            SetTexPixel(&tex, (byte*)(on ? white : black), xPos * size + x, yPos * size + y);
        }
    }
}

void ReBindFrame(){
    ReBindTex(&tex, &sh);
}