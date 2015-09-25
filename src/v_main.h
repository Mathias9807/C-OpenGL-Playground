// v_main.c - Main file for rendering

#ifndef V_MAIN_H
#define V_MAIN_H


#include "def.h"
#include <linmath.h>

#define V_WIDTH 800
#define V_HEIGHT 600

typedef struct {
	vec3 pos;
	float radius;
} smoke;

bool V_reloadShaders;

void V_Init();
void V_Tick();
void V_Quit();

void V_AddSmoke(vec3 pos, float radius);


#endif // V_MAIN_H
