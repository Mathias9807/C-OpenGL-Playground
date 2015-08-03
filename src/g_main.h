// g_main.h - Handles game ticks and rules

#ifndef G_MAIN_H
#define G_MAIN_H


#include "def.h"
#include <linmath.h>

float G_moveSpeed;
float G_rotSpeed;
vec3 G_camPos;
float G_camRot;

void G_Init();
void G_Tick();
void G_Quit();


#endif // G_MAIN_H
