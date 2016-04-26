// v_main.c - Main file for rendering

#include "v_main.h"
#include "sys.h"
#include "g_main.h"
#include "g_animation.h"
#include "v_opengl.h"
#include "g_physics.h"
#include "cvar.h"
#include "level.h"

model_t plane, heightMap, cube, weapon, collBox;

mat4x4 matProj, matView, matModel, matShadow, identity;

GLuint shader, planeShader, depthShader, skyShader, smokeShader, guiShader;

struct fbo post0, post1, depth, shadow;

unsigned grassDTexture, grassSTexture, grassNTexture, roughTexture, V_skyMap, weaponTexture, specTexture, normalTexture, blackTexture, whiteTexture, grayTexture, flatNormal, scareTexture, smokeTexture, fontTexture, cursorTexture, cliffTexture, heightTexture;
enum {
	texFBO0, texFBO1, texDepth, texSky, texShadow, texDiff0, texDiff1, texSpec, texNormal, texGUI
};
float* V_skyColor = NULL;

float V_vertFov;
bool V_reloadShaders = true;

float V_near = 0.02, V_far = 800;

extern AABB dummyBox;
extern float dummyXRot;

void BindTextures();
void LoadShaders();

cvar* shadowDim, * shadowSize;
vec3 V_skyDir = {-1, -1, -1};

void V_Init() {
	shadowDim = C_Get("shadowDim");
	shadowSize = C_Get("shadowSize");
	
	V_CreateFBO(&post0, V_WIDTH, V_HEIGHT, 2);
	V_CreateFBO(&post1, V_WIDTH, V_HEIGHT, 2);
	V_CreateDepthFBO(&depth, V_WIDTH, V_HEIGHT);
	V_CreateDepthFBO(&shadow, (int)shadowDim->value, (int)shadowDim->value);

	int fails = 0;
	fails += V_LoadAssimp("plane.obj", &plane);
	fails += V_LoadAssimp("SKS.dae", &weapon);
	fails += V_LoadAssimp("Cube.obj", &cube);
	fails += V_LoadAssimp("CollisionBox.obj", &collBox);
	sprite heightSprite;
	V_LoadSprite("Terrain.png", &heightSprite);
	//V_CreateHeightMap(&heightMap, &heightSprite, 5);
	if (fails < 0) SYS_Error("Failed to load a model");

	grassDTexture = V_LoadTexture("Grass0138_35_S.jpg");
	grassSTexture = V_LoadTexture("Grass0138_35_S_spec.jpg");
	grassNTexture = V_LoadTexture("Grass0138_35_S_normal.jpg");
	cliffTexture = V_LoadTexture("Cliffside.png");
	heightTexture = V_LoadTexture("Terrain.png");
	roughTexture = V_LoadTexture("Fabric.png");
	weaponTexture = V_LoadTexture("SKS.png");
	specTexture = V_LoadTexture("SpecularGrain.png");
	normalTexture = V_LoadTexture("NormalMap.png");
	blackTexture = V_LoadTexture("Black.png");
	whiteTexture = V_LoadTexture("White.png");
	grayTexture = V_LoadTexture("Gray.png");
	flatNormal = V_LoadTexture("FlatNormal.png");
	scareTexture = V_LoadTexture("Scarecrow.png");
	smokeTexture = V_LoadTexture("Smoke.png");
	fontTexture = V_LoadTexture("font.png");
	cursorTexture = V_LoadTexture("cursor.png");
	
	BindTextures();
	
	LoadShaders();
	
	V_SetDepthTesting(true);
}

void V_RenderScene() {
	for (int i = 0; i < ListSize(&L_current.props); i++) {
		prop* p = ListGet(&L_current.props, i);

		for (int j = 0; j < ListSize(&p->res->models); j++) {
			V_BindTexture(whiteTexture, texDiff0);
			V_BindTexture(whiteTexture, texDiff1);
			V_BindTexture(whiteTexture, texSpec);
			V_BindTexture(flatNormal, texNormal);
			mat4x4_translate(matModel, p->pos[0], p->pos[1], p->pos[2]);
			mat4x4_rotate_Y(matModel, matModel, p->rot[1] * M_PI / 180);
			mat4x4_rotate_Z(matModel, matModel, p->rot[2] * M_PI / 180);
			mat4x4_rotate_X(matModel, matModel, p->rot[0] * M_PI / 180);
			V_SetParam4m("matModel", matModel);
			V_RenderModel(ListGet(&p->res->models, j));
		}
	}
}

void V_RenderSmoke() {
	V_SetParam4m("matProj", matProj);
	V_SetParam4m("matView", matView);
	V_SetParam1f("farPlane", 1);
	
	listEntry* cur = smokeParts.first;
	while (cur) {
		mat4x4_identity(matModel);
		vec3 pos = {
			((smoke*) cur->value)->p.p[0], 
			((smoke*) cur->value)->p.p[1], 
			((smoke*) cur->value)->p.p[2], 
		};
		float scale = ((smoke*) cur->value)->radius;
		mat4x4_translate(matModel, pos[0], pos[1], pos[2]);
		mat4x4_scale_aniso(matModel, matModel, scale, scale, scale);
		V_SetParam4m("matModel", matModel);
		V_BindTexture(smokeTexture, texDiff0);
		V_RenderModel(&plane);
		
		cur = (listEntry*) cur->next;
	}
}

void V_RenderNearScene() {
	V_SetParam4m("matModel", G_gunMat);
	V_SetParam1f("uvScale", 1);
	V_SetParam1f("materialWeight", 0.5);
	V_SetParam1f("materialGloss", 40);
	V_BindTexture(flatNormal, texNormal);
	V_BindTexture(weaponTexture, texDiff0);
	V_BindTexture(grayTexture, texSpec);
	V_RenderModel(&weapon);
}

void V_Tick() {
	if (shadowDim->modified || shadowSize->modified) {
		V_DeleteFBO(&shadow);
		V_CreateDepthFBO(&shadow, (int)shadowDim->value, (int)shadowDim->value);
		shadowDim->modified = false;
		shadowSize->modified = false;
		BindTextures();
		V_reloadShaders = true;
	}
	
	if (V_reloadShaders) 
		LoadShaders();
	
	mat4x4_identity(matView);
	mat4x4_rotate_X(matView, matView, -G_camRot[0]);
	mat4x4_rotate_Y(matView, matView, -G_camRot[1]);
	mat4x4_translate_in_place(matView, -G_camPos[0], -G_camPos[1], -G_camPos[2]);
	
	V_SetFBO(shadow);
	V_SetFaceCullingBack(false);
	V_ClearDepth();
	V_SetShader(depthShader);
	V_SetDepthTesting(true);
	V_SetParam3f("shadowOffs", -G_camPos[0], -G_camPos[1], -G_camPos[2]);
	V_SetParam4m("matProj", matShadow);
	V_RenderScene();
	
	V_SetFBO(post0);
	V_ClearDepth();
	
	if (V_skyColor == NULL) {
		V_SetShader(skyShader);
		V_SetDepthTesting(false);
		V_SetParam4m("matView", matView);
		V_RenderModel(&cube);
		V_SetFaceCullingBack(true);
		V_SetDepthTesting(true);
	}else 
		V_ClearColor(V_skyColor[0], V_skyColor[1], V_skyColor[2], 0);

	V_SetFaceCullingBack(true);
	V_SetDepthTesting(true);
	
	V_SetShader(shader);

	V_UpdateLighting();
	
	V_SetParam4m("matShadow", matShadow);
	V_SetParam3f("camPos", G_camPos[0], G_camPos[1], G_camPos[2]);
	V_SetParam4m("matView", matView);
	V_SetParam3f("shadowOffs", -G_camPos[0], -G_camPos[1], -G_camPos[2]);
	
	V_RenderScene();
	V_RenderNearScene();
	
	V_SetShader(smokeShader);
	V_SetAlphaBlending(true);
	V_SetDepthWriting(false);
	V_SetParam1f("farPlane", 1);
	V_RenderSmoke();
	V_SetAlphaBlending(false);
	V_SetDepthWriting(true);
	
	V_SetFBO(post1);
	V_SetDepthTesting(false);
	V_SetShader(planeShader);
	V_SetParam2f("dir", 1, 0);
	V_BindTexture(post0.att[0], texFBO0);
	V_BindTexture(post0.att[1], texFBO1);
	
	V_RenderModel(&plane);
	
	V_SetFBO(V_WINDOW_FBO);
	V_SetParam2f("dir", 0, 1);
	V_BindTexture(post1.att[0], texFBO0);
	V_BindTexture(post1.att[1], texFBO1);
	
	V_RenderModel(&plane);
	
	V_SetShader(guiShader);
	V_BindTexture(fontTexture, texGUI);
	V_SetAlphaBlending(true);
	
	float alpha = 1;
	if (C_console.lastActive + C_CONSOLE_FADEMS < SYS_TimeMillis()
		&& !C_console.inputActive) {
		termf terms[2] = { (termf) {1, 0}, (termf) {-1, 2} };
		alpha = G_Valuef((function) {0, 1, 2, terms}, 
			(SYS_TimeMillis() - C_console.lastActive - C_CONSOLE_FADEMS) / 1000.0);
	}
	V_SetParam1f("alpha", alpha);
	
	int lines = ListSize(&C_console.history);
	for (int i = 0; i < (lines > C_CONSOLE_DISP ? C_CONSOLE_DISP : lines); i++) {
		int line = i + (lines - C_CONSOLE_DISP > 0 ? lines - C_CONSOLE_DISP : 0);
		char* text = (char*) ListGet(&C_console.history, line);
		
		for (int j = 0; text[j]; j++) {
			V_SetParam2f("subSize", 8, 16);
			V_SetParam2f("pos", 16 + 8 * j, 8 + i * 16);
			V_SetParam2f("subPos", ((int) text[j] % 16) / 16.0, 
						 ((int) text[j] / 16) / 16.0);
			V_RenderModel(&plane);
		}
	}
	
	if (C_console.inputActive) {
		char* text = C_console.text;
		for (int i = 0; i < C_CONSOLE_LENGTH; i++) {
			int character = (int) text[i];
			if (!character) {
				V_BindTexture(cursorTexture, texGUI);
				V_SetParam2f("subSize", 8, 16);
				V_SetParam2f("pos", 16 + 8 * i, 16 + 
						 16 * (C_CONSOLE_DISP > lines ? lines : C_CONSOLE_DISP));
				
				if (SYS_TimeMillis() - C_CONSOLE_BLINKMS > C_cursorBlinkTimer) 
					C_cursorBlinkTimer = SYS_TimeMillis() + C_CONSOLE_BLINKMS;
				if (SYS_TimeMillis() < C_cursorBlinkTimer) 
					V_SetParam2f("subPos", 0, 0);
				else 
					V_SetParam2f("subPos", 1, 0);
				V_RenderModel(&plane);
				
				break;
			}
			
			V_SetParam2f("subSize", 8, 16);
			V_SetParam2f("pos", 16 + 8 * i, 16 + 
						 16 * (C_CONSOLE_DISP > lines ? lines : C_CONSOLE_DISP));
			V_SetParam2f("subPos", (character % 16) / 16.0, 
						 (character / 16) / 16.0);
			V_RenderModel(&plane);
		}
	}
	
	V_SetAlphaBlending(false);
	
	SYS_CheckErrors();
}

void BindTextures() {
	V_BindTexture(depth.attD, texDepth);
	V_BindTexture(shadow.attD, texShadow);
	V_SetTexInterLinear(true);
	V_BindCubeMap(V_skyMap, texSky);
	V_BindTexture(grassDTexture, texDiff0);
	V_BindTexture(grassSTexture, texSpec);
	V_BindTexture(grassNTexture, texNormal);
	V_BindTexture(roughTexture, texSpec);
	V_BindTexture(normalTexture, texNormal);
	V_BindTexture(cliffTexture, texDiff1);
	V_BindTexture(cursorTexture, texGUI);
	V_SetTexRepeating(false);
	V_BindTexture(fontTexture, texGUI);
	V_SetTexRepeating(false);
}

void LoadShaders() {
	V_SetShader(0);
	V_DeleteShader(shader);				shader = V_LoadShader("basic");
	V_DeleteShader(planeShader);		planeShader = V_LoadShader("plane");
	V_DeleteShader(depthShader);		depthShader = V_LoadShader("depth");
	V_DeleteShader(skyShader);			skyShader = V_LoadShader("sky");
	V_DeleteShader(smokeShader);		smokeShader = V_LoadShader("smoke");
	V_DeleteShader(guiShader);			guiShader = V_LoadShader("gui");

	V_SetProj(65);
	mat4x4_identity(matShadow);
	vec3_scale(V_skyDir, V_skyDir, 1 / vec3_len(V_skyDir));
	mat4x4_ortho(matShadow, (int) -shadowSize->value, (int) shadowSize->value, 
		(int) -shadowSize->value, (int) shadowSize->value, 
		(int) -shadowSize->value * 3, (int) shadowSize->value);
	mat4x4 matShadowView;
	mat4x4_identity(matShadowView);
	mat4x4_look_at(matShadowView, (vec3){0, 0, 0}, V_skyDir, (vec3){0, 1, 0});
	mat4x4_mul(matShadow, matShadow, matShadowView);
	mat4x4_identity(identity);
	
	V_SetShader(shader);
	V_SetParam1i("tex0", texDiff0);
	V_SetParam1i("tex3", texDiff1);
	V_SetParam1i("tex1", texSpec);
	V_SetParam1i("tex2", texNormal);
	V_SetParam1i("texShadow", texShadow);
	V_SetParam1i("texShadowD", texShadow);

	V_SetParam1i("texSky", texSky);
	V_SetParam1f("materialWeight", 0.9);
	V_SetParam1f("materialGloss", 80);
	V_SetParam4m("matProj", matProj);
	V_SetParam4m("matShadow", matShadow);
	V_SetParam3f("ambient", 0, 0, 0.1f);
	V_SetParam1f("farPlane", V_far);
	V_SetParam1f("shadowDim", shadowDim->value);
	V_SetParam1f("uvScale", 1);
	V_SetParam3f("V_skyDir", -V_skyDir[0], -V_skyDir[1], -V_skyDir[2]);
	V_SetParam1i("terrain", 0);
	V_SetParam1i("lightNum", 0);

	V_SetShader(skyShader);
	V_SetParam1i("tex", texSky);
	V_SetParam4m("matProj", matProj);
	
	V_SetShader(planeShader);
	V_SetParam1i("w", post0.w);
	V_SetParam1i("h", post0.h);
	V_SetParam1i("tex0", texFBO0);
	V_SetParam1i("tex1", texFBO1);
	V_SetParam1i("shadow", texShadow);
	V_SetParam1i("depth", 1);
	V_SetParam1f("farPlane", V_far);
	V_SetParam3f("modColor", 1, 0.96f, 0.92f);
	
	V_SetShader(depthShader);
	V_SetParam4m("matProj", matShadow);
	V_SetParam1f("farPlane", 1);
	V_SetParam1f("shadowDim", C_Get("shadowDim")->value);
	
	V_SetShader(smokeShader);
	V_SetParam1i("tex0", texDiff0);
	V_SetParam1i("texSky", texSky);
	V_SetParam4m("matProj", matProj);
	V_SetParam3f("bgColor", 0, 0, 0.3f);
	V_SetParam1f("farPlane", V_far);
	V_SetParam3f("lightDir", V_skyDir[0], V_skyDir[1], V_skyDir[2]);
	
	V_SetShader(guiShader);
	V_SetParam2f("screenSize", V_WIDTH, V_HEIGHT);
	V_SetParam1i("tex0", texGUI);
	
	V_reloadShaders = false;
}

void V_UpdateLighting() {
	int lightNum = lights.size > 8 ? 8 : lights.size;
	V_SetParam1i("lightNum", lightNum);

	for (int i = 0; i < lightNum; i++) {
		V_SetParamLight(i, *((light*)ListGet(&lights, i)));
	}
}

void V_SetProj(float fov) {
	if (fov == V_vertFov) return;
	V_vertFov = fov;
	V_MakeProjection(matProj, fov / 180 * M_PI, (float) V_WIDTH / V_HEIGHT, V_near, V_far);

	V_SetShader(shader);
	V_SetParam4m("matProj", matProj);
	V_SetShader(skyShader);
	V_SetParam4m("matProj", matProj);
	V_SetShader(smokeShader);
	V_SetParam4m("matProj", matProj);
}

void V_Quit() {
	V_QuitOpenGL();
	SYS_CloseWindow();
}
