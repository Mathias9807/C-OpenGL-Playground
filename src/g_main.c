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
vec3 G_camPos, G_camRot;
mat4x4 G_gunMat;
vec3 lastGunPos, prefGunPos = {-0.15, -0.1, 0.4}, curGunPos;
float curGunYRot = 0, curGunZRot = 0;

void Shoot();

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
//	if (In_IsKeyPressed(IN_UP))		dir[1]		+= moveSpeed;
//	if (In_IsKeyPressed(IN_DOWN))	dir[1]		-= moveSpeed;
	if (In_IsKeyPressed(IN_RIGHT))	G_camRot[1]	+= rotSpeed;
	if (In_IsKeyPressed(IN_LEFT))	G_camRot[1]	-= rotSpeed;
//	if (In_IsKeyPressed(IN_PITCH_UP))	G_camRot[0]	+= rotSpeed;
//	if (In_IsKeyPressed(IN_PITCH_DOWN))	G_camRot[0]	-= rotSpeed;
	
	if (G_camRot[0] > PITCH_LIMIT) G_camRot[0] = PITCH_LIMIT;
	else if (G_camRot[0] < -PITCH_LIMIT) G_camRot[0] = -PITCH_LIMIT;
	
	float c = cos(G_camRot[1]);
	float s = sin(G_camRot[1]);
	G_camPos[0] += -dir[2] * s + dir[0] * c;
	G_camPos[2] += dir[2] * c + dir[0] * s;
	G_camPos[1] += dir[1];
	
	curGunPos[0] = (curGunPos[0] - prefGunPos[0]) / 1.2 + prefGunPos[0] + dir[0] * 0.05;
	curGunPos[1] = prefGunPos[1];
	curGunPos[2] = prefGunPos[2];
	curGunYRot = 0.8 * ((curGunYRot - G_camRot[1]) / 1.05 + G_camRot[1]) + 0.2 * G_camRot[1];
	curGunZRot += dir[0] * 0.15;
	curGunZRot = curGunZRot / 1.05;
	mat4x4_translate(G_gunMat, G_camPos[0], G_camPos[1], G_camPos[2]);
	mat4x4_rotate_Y(G_gunMat, G_gunMat, curGunYRot + M_PI);
	mat4x4_rotate_X(G_gunMat, G_gunMat, -G_camRot[0]);
	mat4x4_translate_in_place(G_gunMat, curGunPos[0], curGunPos[1], curGunPos[2]);
	mat4x4_rotate_Z(G_gunMat, G_gunMat, curGunZRot);
	mat4x4_scale_aniso(G_gunMat, G_gunMat, 0.05, 0.05, 0.05);
	lastGunPos[0] = curGunPos[0];
	lastGunPos[1] = curGunPos[1];
	lastGunPos[2] = curGunPos[2];
}

void Shoot() {
	
}

void G_Quit() {
}
