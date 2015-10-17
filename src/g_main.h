// g_main.h - Handles game ticks and rules

#ifndef G_MAIN_H
#define G_MAIN_H


#include "def.h"
#include <linmath.h>
#include "g_physics.h"

float G_moveSpeed;
float G_rotSpeed;
vec3 G_camPos;
vec3 G_camRot;
char* G_consoleString;

typedef struct {
	point p;
	float radius;
	int timeLeft;
} smoke;
list smokeParts;

mat4x4 G_gunMat;

void G_Init();
void G_Tick();
void G_Quit();

void G_AddSmoke(vec3 pos, vec3 vel, float radius, int timeLeft);


#endif // G_MAIN_H
