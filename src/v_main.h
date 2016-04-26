// v_main.c - Main file for rendering

#ifndef V_MAIN_H
#define V_MAIN_H


#include "def.h"
#include <linmath.h>

#define V_WIDTH 800
#define V_HEIGHT 600

struct fbo {
	int id;
	int w;
	int h;
	int numAtt;
	unsigned* att;
	unsigned attD;
};

typedef struct {
	vec3 dir;
	mat4x4 trans;
	struct fbo map;
} shadowMap;

extern bool V_reloadShaders;
extern bool V_rendererUp;
extern float V_near, V_far; // V_wherever you are

extern unsigned V_skyMap;
extern float* 	V_skyColor;
extern vec3	V_skyDir;

void V_SetProj(float fov);
void V_SetVSync(bool vsync);
void V_UpdateLighting();
void V_LoadSprite(const char* name, sprite* s);
void V_CreateShadowMap(shadowMap* sMap, vec3 dir);

void V_Init();
void V_Tick();
void V_Quit();


#endif // V_MAIN_H
