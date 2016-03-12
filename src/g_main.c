// g_main.c - Handles game ticks and rules

#include "g_main.h"
#include "v_main.h"
#include "sys.h"
#include "input.h"
#include "g_animation.h"
#include "level.h"
#include <linmath.h>
#include <math.h>
#include <string.h>
#include "g_lua.h"

#define PITCH_LIMIT M_PI/2

float G_moveSpeed = 8, G_rotSpeed = 3;
vec3 G_camPos, G_camRot;
list smokeParts, smokeGens, lights;
cvar* fov, * adsFov;
mat4x4 G_gunMat;
vec3 defGunPos = {0, 0, 0.2}, hipGunPos = {-0.2, -0.15, 0.5},
fireGunPos = {-0.004, 0.0125, -0.03}, hipFireGunPos = {0, 0.1, -0.4};
float curGunXRot = 0, curGunYRot = 0, curGunZRot = 0;
bool ads = false;
double lastAdsTime = -1, lastShootTime = -1, dummyHitTime = 0;
bool actionHeld = false, toggleHeld = false, chatHeld = false;
vec3 smokeAcc = {0, 1, 0};
int lastSmokeSpawn = 0, smokeSpawnInterval = 250;

AABB dummyBox = {-1, -1, -1, 2, 2, 2};
float dummyXRot = 0;

lua_State* G_luaState = NULL;

void Shoot();
void LoadScripting();
void ReloadLevel();

void G_Init() {
	L_InitLevel("writing");

	resource* r0 = L_AddResource("house");
	L_AddProp(r0, (vec3){0, 0, -5}, (vec3){0, 0, 0});

	resource* r1 = L_AddResource("street");
	L_AddProp(r1, (vec3){5, 0, -15}, (vec3){0, 45, 0});
	
	resource* r3 = L_AddResource("heightmap");
	L_AddProp(r3, (vec3){-50, -10, -50}, (vec3){0, 0, 0});

	L_WriteLevel();
	
	C_console.selectedRow = -1;
	
	fov = C_Get("fov");
	adsFov = C_Get("adsFov");
	
	smokeParts = (list) {NULL, 0};
	smokeGens = (list) {NULL, 0};
	lights = (list) {NULL, 0};

	LoadScripting();
}

void LoadScripting() {
	// Initialize the interpreter
	G_luaState = luaL_newstate();
	luaL_openlibs(G_luaState);

	// Get the path
	char* path = malloc(PATH_LENGTH);
	memset(path, 0, PATH_LENGTH);
	SYS_GetResourcePath("/scripts/main.lua", path);

	// Run the file
	luaL_dofile(G_luaState, path);

	// Load the engines interface
	G_LoadLuaFunctions();

	// Call the main script's 'init' function
	lua_getglobal(G_luaState, "init");
	char levelPath[PATH_LENGTH];
	SYS_GetLevelPath(L_current.name, levelPath);
	lua_pushstring(G_luaState, levelPath);
	int err = lua_pcall(G_luaState, 1, 0, 0);

	// Add all prop scripts
	for (int i = 0; i < L_current.props.size; i++) {
		prop* p = ListGet(&L_current.props, i);
		G_currentProp = p;

		lua_getglobal(G_luaState, "addProp");
		char propPath[PATH_LENGTH];
		SYS_GetLevelPath(L_current.name, propPath);
		strcat(propPath, "/resources/");
		strcat(propPath, p->res->name);
		strcat(propPath, "/script.lua");
		lua_pushstring(G_luaState, propPath);
		lua_pcall(G_luaState, 1, 0, 0);
	}

	// Check for errors
	if (err != 0) SYS_Warning("A lua error occured!");
}

void G_Tick() {
	int stopReading = IN_ReadTextInput(C_console.text, C_CONSOLE_LENGTH);
	if (stopReading) {
		IN_StopTextInput();
		C_console.inputActive = false;
	}
	for (int i = 0; C_console.text[i]; i++) {
		if (C_console.text[i] == '\n') {
			C_console.text[i] = 0;
			C_PrintCommand(C_console.text);
			C_Execute(C_console.text);
			for (int j = 0; j < C_CONSOLE_LENGTH; j++)
				C_console.text[j] = 0;
			IN_StopTextInput();
			C_console.inputActive = false;
			actionHeld = true;
			C_console.selectedRow = -1;
			
			break;
		}
		if (C_console.text[i] == '\t') {
			C_console.text[i] = 0;
			break;
		}
	}
	
	vec3 dir = {0, 0, 0};
	float moveSpeed = G_moveSpeed * (SYS_deltaMillis / 1000.0);
	float rotSpeed = G_rotSpeed * (SYS_deltaMillis / 1000.0);
	if (IN_IsKeyPressed(IN_W))		dir[2]		-= moveSpeed;
	if (IN_IsKeyPressed(IN_A))		dir[0]		-= moveSpeed;
	if (IN_IsKeyPressed(IN_S))		dir[2]		+= moveSpeed;
	if (IN_IsKeyPressed(IN_D))		dir[0]		+= moveSpeed;
	if (IN_IsKeyPressed(IN_UP))		dir[1]		+= moveSpeed;
	if (IN_IsKeyPressed(IN_DOWN))	dir[1]		-= moveSpeed;
	if (IN_IsKeyPressed(IN_RIGHT))	G_camRot[1]	+= rotSpeed;
	if (IN_IsKeyPressed(IN_LEFT))	G_camRot[1]	-= rotSpeed;
	if (IN_IsKeyPressed(IN_PITCH_UP))	G_camRot[0]	+= rotSpeed;
	if (IN_IsKeyPressed(IN_PITCH_DOWN))	G_camRot[0]	-= rotSpeed;
	if (IN_IsKeyPressed(IN_ACTION)) {
		if (!actionHeld) {
			Shoot();
			actionHeld = true;
		}
	}else actionHeld = false;
	if (IN_IsKeyPressed(IN_TOGGLE)) {
		if (!toggleHeld) {
			ads = ads ? false : true;
			lastAdsTime = SYS_TimeMillis() / 1000.0;
			toggleHeld = true;
		}
	}else toggleHeld = false;
	if (IN_IsKeyPressed(IN_CHAT)) {
		if (!chatHeld) {
			IN_StartTextInput();
			C_console.inputActive = true;
			chatHeld = true;
		}
	}else chatHeld = false;
	
	if (IN_IsKeyPressed(IN_RELOAD)) {
		ReloadLevel();
	}
	
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

	double d, f;
	{
		termf terms[2] = {{2, 2}, {-1, 4}};
		d = G_Valuef((function) {0, 1, 2, terms},
							(SYS_TimeMillis() / 1000.0 - lastAdsTime) * 5);
		if (ads) d = 1 - d;
	}
	{
		termf terms[2] = {{1, 0.3}, {-1, 1}};
		f = G_Valuef((function) {0, 1, 2, terms},
							(SYS_TimeMillis() / 1000.0 - lastShootTime) * 5);
	}
	V_SetProj(fov->value * d + adsFov->value * (1 - d));
	vec3 resultant = {0, 0, 0};
	for (int i = 0; i < 3; i++) {
		resultant[i] += defGunPos[i];
		resultant[i] += hipGunPos[i] * d;
		resultant[i] += fireGunPos[i] * f;
		resultant[i] += hipFireGunPos[i] * d * f;
	}
	resultant[0] += d * 0.01 * cos(SYS_TimeMillis() / 2000.0);
	resultant[1] += d * 0.005 * cos(SYS_TimeMillis() / 1500.0);
	mat4x4_translate_in_place(G_gunMat, resultant[0], resultant[1], resultant[2]);

	for (int i = 0; i < ListSize(&smokeGens); i++) {
		smokeGen* gen = ListGet(&smokeGens, i);

		while (SYS_TimeMillis() - gen->lastSpawn > gen->interval) {
			gen->lastSpawn = SYS_TimeMillis();
			G_AddSmoke(gen->pos, (vec3) {0, 0, 0}, 0.4, 5000);
		}
	}

	for (int i = 0; i < ListSize(&smokeParts); i++) {
		smoke* s = (smoke*) ListGet(&smokeParts, i);
		
		s->timeLeft -= SYS_deltaMillis;
		if (s->timeLeft <= 0) {
			ListRemove(&smokeParts, i);
			i--;
			continue;
		}
		
		G_TickPointPhysics(&s->p, smokeAcc);
		s->radius *= pow(1.2, SYS_deltaMillis / 1000.0);
	}
	
	if (IN_IsKeyPressed(IN_RELOAD)) V_reloadShaders = true;
}

void Shoot() {
	vec3 rayDir = {
		sin(G_camRot[1]) * cos(G_camRot[0]), 
		sin(G_camRot[0]), 
		-cos(G_camRot[0]) * cos(G_camRot[1]), 
	};
	if (!dummyHitTime && G_RayHitsAABB(dummyBox, G_camPos, rayDir)) {
		dummyXRot = -M_PI / 8;
		dummyHitTime = SYS_TimeMillis() / 1000.0;
	}
	lastShootTime = SYS_TimeMillis() / 1000.0;
	fireGunPos[0] = cos(SYS_TimeMillis() / 10.0) * 0.002;
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

void ReloadLevel() {
	lua_close(G_luaState);
	
	LoadScripting();
}

void G_Quit() {
	lua_close(G_luaState);
}
