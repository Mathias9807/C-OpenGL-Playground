cd build
gcc ../src/v_*.c ../src/g_*.c ../src/sys_sdl.c ^
	-I../include ^
	-DM_PI=3.1415926535 -DGLEW_STATIC ^
	-Wall -std=c99 -c

pause