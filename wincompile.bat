gcc -o 3D.exe -static-libgcc -static-libstdc++ ^
	src/sys_sdl.c src/cvar.c src/v_*.c src/g_*.c src/def.c src/level.c ^
	-Iinclude/ -Lwinlibs/ -L. -lglew32 -lopengl32 -lSDL2 -lassimp ^
	-DM_PI=3.1415926535 -DGLEW_STATIC -DSDL_MAIN_HANDLED ^
	-std=c99 -Wall

pause
