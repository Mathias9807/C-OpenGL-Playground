// v_main.c - Main file for rendering

#ifndef V_MAIN_H
#define V_MAIN_H


#include "def.h"
#include <linmath.h>

#define V_WIDTH 800
#define V_HEIGHT 600

bool V_reloadShaders;
bool V_rendererUp;

void V_SetProj(float fov);
void V_SetVSync(bool vsync);

void V_Init();
void V_Tick();
void V_Quit();


#endif // V_MAIN_H
