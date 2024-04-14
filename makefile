OBJS = main.c

CC = gcc

INCLUDE_PATHS = -I"C:\mingw-x86_64\include\SDL2"

LIB_PATH = -L"C:\mingw-x86_64\lib\SDL2"

FLAGS = -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

TARGET = main

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIB_PATH) $(LINKER_FLAGS) -o $(TARGET)