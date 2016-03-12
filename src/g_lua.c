// g_lua.c - Defines functions used in the lua scripts

#include "g_lua.h"
#include "v_main.h"
#include "v_opengl.h"
#include "sys.h"


prop* G_currentProp = NULL;

double ReadNum(int i);
bool ReadBool(int i);
void LocalToGlobal(vec3 p);

// addHeightMap("HeightMap.png", widthScale, heightScale)
int addHeightMap(lua_State* l) {
	if (!G_currentProp) return 0;

	sprite s;
	memset(&s, 0, sizeof(sprite));
	const char* name = lua_tostring(G_luaState, -3);
	char path[PATH_LENGTH];
	memset(path, 0, PATH_LENGTH);
	strcat(path, "../levels/");
	strcat(path, L_current.name);
	strcat(path, "/resources/");
	strcat(path, G_currentProp->res->name);
	strcat(path, "/");
	strcat(path, name);
	V_LoadSprite(path, &s);

	double h = lua_tonumber(G_luaState, -2);
	double size = lua_tonumber(G_luaState, -1);

	model_t* m = calloc(1, sizeof(model_t));
	V_CreateHeightMap(m, &s, size, h);

	ListAdd(&G_currentProp->res->models, m);

	return 0;
}

int addSmokeGenerator(lua_State* l) {
	if (!G_currentProp) return 0;

	vec4 local;
	for (int i = 0; i < 3; i++) 
		local[i] = ReadNum(-3 + i);
	local[3] = 1;

	LocalToGlobal(local);
	
	smokeGen* gen = malloc(sizeof(smokeGen));
	ListAdd(&smokeGens, gen);

	memset(gen, 0, sizeof(smokeGen));
	gen->pos[0] = local[0];
	gen->pos[1] = local[1];
	gen->pos[2] = local[2];
	gen->interval = 200;
	gen->lastSpawn = SYS_TimeMillis();

	return 0;
}

int setPlayerPos(lua_State* l) {
	for (int i = 0; i < 3; i++) 
		G_camPos[i] = ReadNum(-3 + i);
	
	return 0;
}

int setSky() {
	if (!lua_isnumber(G_luaState, -1)) {
		char path[PATH_LENGTH];
		memset(path, 0, PATH_LENGTH);
		strcat(path, "../levels/");
		strcat(path, L_current.name);
		strcat(path, "/resources/");
		strcat(path, lua_tostring(G_luaState, -1));
		V_skyMap = V_LoadCubeMap(path);
		free(V_skyColor);
		V_skyColor = NULL;
	}else {
		if (!V_skyColor) V_skyColor = malloc(sizeof(vec3));

		for (int i = 0; i < 3; i++) 
			V_skyColor[i] = ReadNum(-3 + i);
	}

	return 0;
}

// addModel("modelName.dae")
int addModel() {
	char* path = calloc(PATH_LENGTH, sizeof(char));
	const char* name = lua_tostring(G_luaState, -1);

	strcat(path, "../levels/");
	strcat(path, L_current.name);
	strcat(path, "/resources/");
	strcat(path, G_currentProp->res->name);
	strcat(path, "/");
	strcat(path, name);
	
	model_t* m = calloc(1, sizeof(model_t));
	V_LoadAssimp(path, m);
	ListAdd(&G_currentProp->res->models, m);

	return 0;
}

int addLight() {
	light* l = calloc(1, sizeof(light));

	for (int i = 0; i < 3; i++) 
		l->pos[i] = ReadNum(-6 + i);
	LocalToGlobal(l->pos);

	for (int i = 0; i < 3; i++) 
		l->col[i] = ReadNum(-3 + i);

	l->directional = false;
	l->shadowed = false;

	ListAdd(&lights, l);

	return 0;
}

int addSun() {
	light* l = calloc(1, sizeof(light));

	for (int i = 0; i < 3; i++) 
		l->pos[i] = ReadNum(-6 + i);

	for (int i = 0; i < 3; i++) 
		l->col[i] = ReadNum(-3 + i);

	l->directional = true;

	// Only the first light should be affected by shadows
	static bool shadowed = true;
	l->shadowed = shadowed;
	shadowed = false;

	ListAdd(&lights, l);

	return 0;
}

void G_LoadLuaFunctions() {
	lua_pushcfunction(G_luaState, addSmokeGenerator);
	lua_setglobal(G_luaState, "addSmokeGenerator");
	
	lua_pushcfunction(G_luaState, setPlayerPos);
	lua_setglobal(G_luaState, "setPlayerPos");

	lua_pushcfunction(G_luaState, setSky);
	lua_setglobal(G_luaState, "setSky");
	
	lua_pushcfunction(G_luaState, addLight);
	lua_setglobal(G_luaState, "addLight");

	lua_pushcfunction(G_luaState, addSun);
	lua_setglobal(G_luaState, "addSun");

	lua_pushcfunction(G_luaState, addModel);
	lua_setglobal(G_luaState, "addModel");

	lua_pushcfunction(G_luaState, addHeightMap);
	lua_setglobal(G_luaState, "addHeightMap");
}

double ReadNum(int i) {
	if (!lua_isnumber(G_luaState, i)) SYS_Warning("Incorrect argument for function. ");
	return lua_tonumber(G_luaState, i);
}

bool ReadBool(int i) {
	if (!lua_isboolean(G_luaState, i)) SYS_Warning("Incorrect argument for function. ");
	return lua_toboolean(G_luaState, i);
}

// Convert local coordinates to global
void LocalToGlobal(vec4 p) {
	if (G_currentProp == NULL) return;

	double angX = G_currentProp->rot[0] * M_PI / 180;
	double angY = G_currentProp->rot[1] * M_PI / 180;
	double angZ = G_currentProp->rot[2] * M_PI / 180;

	vec4 new0 = {p[0], p[1], p[2], 1};
	vec4 new1;
	mat4x4 rot;
	mat4x4_identity(rot);
	mat4x4_translate_in_place(rot, G_currentProp->pos[0], G_currentProp->pos[1], G_currentProp->pos[2]);
	mat4x4_rotate_Y(rot, rot, angY);
	mat4x4_rotate_Z(rot, rot, angZ);
	mat4x4_rotate_X(rot, rot, angX);
	mat4x4_mul_vec4(new1, rot, new0);

	p[0] = new1[0];
	p[1] = new1[1];
	p[2] = new1[2];
	p[3] = new1[3];
}

