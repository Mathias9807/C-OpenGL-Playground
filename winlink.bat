gcc -o 3D build/*.o ^
	-L./winlibs -lSDL2 -lglew32 -lOpenGL32 -lassimp

pause