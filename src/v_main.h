// v_main.c - Main file for rendering

#ifndef V_MAIN_H
#define V_MAIN_H


#include "def.h"
#include <linmath.h>

#define V_WIDTH 800
#define V_HEIGHT 600

float V_vertFov;
bool V_reloadShaders;

void V_Init();
void V_Tick();
void V_Quit();


#endif // V_MAIN_H
