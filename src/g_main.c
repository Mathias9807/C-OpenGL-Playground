// g_main.c - Handles game ticks and rules

#include "g_main.h"
#include "v_main.h"
#include "sys.h"
#include "input.h"
#include <linmath.h>
#include <math.h>

#define PITCH_LIMIT M_PI/2

float G_moveSpeed = 8;
float G_rotSpeed = 3;
vec3 G_camPos;
vec3 G_camRot;

void G_Init() {
	G_camPos[2] = 2.5;
}

void G_Tick() {
	vec3 dir = {0, 0, 0};
	float moveSpeed = G_moveSpeed * (Sys_deltaMillis / 1000.0);
	float rotSpeed = G_rotSpeed * (Sys_deltaMillis / 1000.0);
	if (In_IsKeyPressed(IN_W))		dir[2]		-= moveSpeed;
	if (In_IsKeyPressed(IN_A))		dir[0]		-= moveSpeed;
	if (In_IsKeyPressed(IN_S))		dir[2]		+= moveSpeed;
	if (In_IsKeyPressed(IN_D))		dir[0]		+= moveSpeed;
	if (In_IsKeyPressed(IN_UP))		dir[1]		+= moveSpeed;
	if (In_IsKeyPressed(IN_DOWN))	dir[1]		-= moveSpeed;
	if (In_IsKeyPressed(IN_RIGHT))	G_camRot[1]	+= rotSpeed;
	if (In_IsKeyPressed(IN_LEFT))	G_camRot[1]	-= rotSpeed;
	if (In_IsKeyPressed(IN_PITCH_UP))	G_camRot[0]	+= rotSpeed;
	if (In_IsKeyPressed(IN_PITCH_DOWN))	G_camRot[0]	-= rotSpeed;
	
	if (G_camRot[0] > PITCH_LIMIT) G_camRot[0] = PITCH_LIMIT;
	else if (G_camRot[0] < -PITCH_LIMIT) G_camRot[0] = -PITCH_LIMIT;
	
	float c = cos(G_camRot[1]);
	float s = sin(G_camRot[1]);
	G_camPos[0] += -dir[2] * s + dir[0] * c;
	G_camPos[2] += dir[2] * c + dir[0] * s;
	G_camPos[1] += dir[1];
}

void G_Quit() {
}
