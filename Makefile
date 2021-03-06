CC=gcc
SOURCE=src/def.c src/cvar.c src/v_*.c src/g_*.c src/level.c src/sys_sdl.c 
INCLUDE=-I./include
CFLAGS=-DM_PI=3.1415926535 -DGLEW_STATIC -DSDL_MAIN_HANDLED -Wall -std=c99 -g
LIBS=-lSDL2 -lGLEW -lGL -lassimp -llua -lm
PROGRAM=3D

build:
	$(CC) $(CFLAGS) $(SOURCE) $(INCLUDE) $(LIBS) -o $(PROGRAM)
