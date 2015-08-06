gcc -o 3D.exe src/v_*.c src/g_*.c src/sys_sdl.c ^
	-I./include ^
	-DM_PI=3.1415926535 -DGLEW_STATIC -DSDL_MAIN_HANDLED ^
	-Wall -std=c99 ^
	-L./winlibs -lSDL2 -lglew32 -lOpenGL32 -lassimp

pause