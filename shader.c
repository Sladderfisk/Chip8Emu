#include "headers/shader.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/glew.h>

#include "headers/fileReader.h"

void checkCompileErrors(unsigned int shader, const char* type)
{
    int success;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM"))
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
}

void CompileShader(shader *sh, const char *vertPath, const char *fragPath)
{
    FILE *vertFile = fopen(vertPath, "r");
    FILE *fragFile = fopen(fragPath, "r");

    if (vertFile == NULL){
        printf("Vertex File: %s\nCould not be opend", vertPath);
        return;
    }
    if (fragFile == NULL){
        printf("Fragment File: %s\nCould not be opend", fragPath);
        return;
    }

    char *vertCode, *fragCode;
    GetFileToStr(vertFile, &vertCode);
    GetFileToStr(fragFile, &fragCode);
    sh->vertSrc = vertCode;
    sh->fragSrc = fragCode;

    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &sh->vertSrc, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &sh->fragSrc, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    sh->ID = glCreateProgram();
    glAttachShader(sh->ID, vertex);
    glAttachShader(sh->ID, fragment);
    glLinkProgram(sh->ID);
    checkCompileErrors(sh->ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    //free(vertCode);
    //free(fragCode);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}