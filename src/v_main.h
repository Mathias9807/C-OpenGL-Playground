// v_main.c - Main file for rendering

#ifndef V_MAIN_H
#define V_MAIN_H


#include "def.h"
#include <linmath.h>

#define V_WIDTH 800
#define V_HEIGHT 600

extern bool V_reloadShaders;
extern bool V_rendererUp;

extern unsigned V_skyMap;
extern float* 	V_skyColor;

void V_SetProj(float fov);
void V_SetVSync(bool vsync);
void V_UpdateLighting();

void V_Init();
void V_Tick();
void V_Quit();


#endif // V_MAIN_H
