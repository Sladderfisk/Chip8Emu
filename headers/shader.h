#pragma once

typedef struct {
    unsigned int ID;
    const char *vertSrc;
    const char *fragSrc;
} shader;

void CompileShader(shader* sh, const char* vertPath, const char* fragPath);
void DeleteShader(shader* sh);