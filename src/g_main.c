// g_main.c - Handles game ticks and rules

#include "g_main.h"
#include "v_main.h"
#include "sys.h"
#include "input.h"
#include "g_animation.h"
#include <linmath.h>
#include <math.h>

#define PITCH_LIMIT M_PI/2

float G_moveSpeed = 8;
float G_rotSpeed = 3;
vec3 G_camPos, G_camRot;
mat4x4 G_gunMat;
vec3 defGunPos = {0, 0, 0.3}, hipGunPos = {-0.2, -0.15, 0.4};
float curGunXRot = 0, curGunYRot = 0, curGunZRot = 0;
bool ads = false;
double lastAdsTime = -1;
bool actionHeld = false, toggleHeld = false;
vec3 smokeAcc = {0, 1, 0};
int lastSmokeSpawn = 0, smokeSpawnInterval = 250;

AABB dummyBox = {-1, -1, -1, 2, 2, 2};
float dummyXRot = 0;

void Shoot();

void G_Init() {
	G_camPos[2] = 2.5;
	
	smokeParts = (list) {NULL, 0};
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
	if (In_IsKeyPressed(IN_ACTION)) {
		if (!actionHeld) {
			Shoot();
			actionHeld = true;
		}
	}else actionHeld = false;
	if (In_IsKeyPressed(IN_TOGGLE)) {
		if (!toggleHeld) {
			ads = ads ? false : true;
			lastAdsTime = Sys_TimeMillis() / 1000.0;
			toggleHeld = true;
		}
	}else toggleHeld = false;
	
	
	if (G_camRot[0] > PITCH_LIMIT) G_camRot[0] = PITCH_LIMIT;
	else if (G_camRot[0] < -PITCH_LIMIT) G_camRot[0] = -PITCH_LIMIT;
	
	float c = cos(G_camRot[1]);
	float s = sin(G_camRot[1]);
	G_camPos[0] += -dir[2] * s + dir[0] * c;
	G_camPos[2] += dir[2] * c + dir[0] * s;
	G_camPos[1] += dir[1];
	
	mat4x4_translate(G_gunMat, G_camPos[0], G_camPos[1], G_camPos[2]);
	mat4x4_rotate_Y(G_gunMat, G_gunMat, G_camRot[1] + M_PI);
	mat4x4_rotate_X(G_gunMat, G_gunMat, -G_camRot[0]);
	mat4x4_translate_in_place(G_gunMat, defGunPos[0], defGunPos[1], defGunPos[2]);
	double d = G_Value((linearf) {0, 0.15, 1 / 0.15, 0},
		Sys_TimeMillis() / 1000.0 - lastAdsTime);
	if (ads) d = 1 - d;
	mat4x4_translate_in_place(G_gunMat,
		d * hipGunPos[0], d * hipGunPos[1],	d * hipGunPos[2]);

	
	dummyXRot *= pow(0.0001, Sys_deltaMillis / 1000.0);
	
	if (lastSmokeSpawn - Sys_TimeMillis() > smokeSpawnInterval) {
		G_AddSmoke((vec3) {2, 0, 0}, (vec3) {0, 0, 0}, 0.5, 5000);
		lastSmokeSpawn += smokeSpawnInterval;
	}
	for (int i = 0; i < ListSize(&smokeParts); i++) {
		smoke* s = (smoke*) ListGet(&smokeParts, i);
		
		s->timeLeft -= Sys_deltaMillis;
		if (s->timeLeft <= 0) {
			ListRemove(&smokeParts, i);
			i--;
			continue;
		}
		
		G_TickPointPhysics(&s->p, smokeAcc);
		s->radius *= pow(1.2, Sys_deltaMillis / 1000.0);
	}
	
	if (In_IsKeyPressed(IN_RELOAD)) V_reloadShaders = true;
}

void Shoot() {
	vec3 rayDir = {
		sin(G_camRot[1]) * cos(G_camRot[0]), 
		sin(G_camRot[0]), 
		-cos(G_camRot[0]) * cos(G_camRot[1]), 
	};
	if (G_RayHitsAABB(dummyBox, G_camPos, rayDir)) dummyXRot = -M_PI / 8;
}

void G_AddSmoke(vec3 pos, vec3 vel, float radius, int timeLeft) {
	smoke* s = malloc(sizeof(smoke));
	s->p.p[0]	= pos[0];
	s->p.p[1]	= pos[1];
	s->p.p[2]	= pos[2];
	s->p.v[0]	= vel[0];
	s->p.v[1]	= vel[1];
	s->p.v[2]	= vel[2];
	s->radius	= radius;
	s->timeLeft	= timeLeft;
	
	ListAdd(&smokeParts, s);
}

void G_Quit() {
}
