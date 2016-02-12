// g_lua.c - Defines functions used in the lua scripts

#include "g_lua.h"
#include "sys.h"


prop* G_currentProp = NULL;

double ReadNum(int i);

int addSmokeGenerator(lua_State* l) {
	if (!G_currentProp) return 0;

	vec4 local;
	for (int i = 0; i < 3; i++) 
		local[i] = ReadNum(-3 + i);
	local[3] = 1;

	double angX = G_currentProp->rot[0] * M_PI / 180;
	double angY = G_currentProp->rot[1] * M_PI / 180;
	double angZ = G_currentProp->rot[2] * M_PI / 180;

	vec4 new0 = {local[0], local[1], local[2], 1};
	vec4 new1;
	mat4x4 rot;
	mat4x4_identity(rot);
	mat4x4_rotate_Y(rot, rot, angY);
	mat4x4_rotate_Z(rot, rot, angZ);
	mat4x4_rotate_X(rot, rot, angX);
	mat4x4_mul_vec4(new1, rot, new0);
	
	smokeGen* gen = malloc(sizeof(smokeGen));
	ListAdd(&smokeGens, gen);

	memset(gen, 0, sizeof(smokeGen));
	gen->pos[0] = new1[0];
	gen->pos[1] = new1[1];
	gen->pos[2] = new1[2];
	gen->interval = 200;
	gen->lastSpawn = SYS_TimeMillis();

	return 0;
}

int setPlayerPos(lua_State* l) {
	for (int i = 0; i < 3; i++) 
		G_camPos[i] = ReadNum(-3 + i);
	
	return 0;
}

void G_LoadLuaFunctions() {
	lua_pushcfunction(G_luaState, addSmokeGenerator);
	lua_setglobal(G_luaState, "addSmokeGenerator");
	
	lua_pushcfunction(G_luaState, setPlayerPos);
	lua_setglobal(G_luaState, "setPlayerPos");
}

double ReadNum(int i) {
	if (!lua_isnumber(G_luaState, i)) SYS_Warning("Incorrect argument for function. ");
	return lua_tonumber(G_luaState, i);
}

