// g_main.h - Handles game ticks and rules

#ifndef G_MAIN_H
#define G_MAIN_H


#include "def.h"
#include <linmath.h>
#include "g_physics.h"
#include "cvar.h"

extern float G_moveSpeed;
extern float G_rotSpeed;
extern vec3 G_camPos;
extern vec3 G_camRot;

typedef struct {
	vec3 pos, col;
	bool directional;
} light;
extern list lights;

typedef struct {
	point p;
	float radius;
	int timeLeft;
} smoke;
extern list smokeParts;

typedef struct {
	vec3 pos;
	vec3 vel;
	vec3 acc;
	double interval;
	double lastSpawn;
} smokeGen;
extern list smokeGens;

extern mat4x4 G_gunMat;

void G_Init();
void G_Tick();
void G_Quit();

void G_AddSmoke(vec3 pos, vec3 vel, float radius, int timeLeft);


#endif // G_MAIN_H
