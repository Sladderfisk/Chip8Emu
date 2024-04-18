OBJS = main.c shader.c texture.c fileReader.c frame.c chip8.c

CC = gcc

INCLUDE_PATHS = -I"Dependencies/include"

LIB_PATH = -L"Dependencies\lib"

FLAGS = -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lGLEW32 -lcomdlg32

TARGET = main

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIB_PATH) $(LINKER_FLAGS) -o $(TARGET)