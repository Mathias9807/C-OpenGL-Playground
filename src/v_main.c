// v_main.c - Main file for rendering

#include "v_main.h"
#include "sys.h"
#include "g_main.h"
#include "v_opengl.h"
#include "g_physics.h"

model_t model, plane, heightMap, pillar, cube, sphere, weapon, scarecrow, collBox;

mat4x4 matProj, matView, matModel, matShadow, identity;

GLuint shader, planeShader, depthShader, skyShader, smokeShader;

struct fbo post0, post1, depth, shadow;

GLuint grassTexture, roughTexture, skyMap, weaponTexture, specTexture, normalTexture, blackTexture, whiteTexture, flatNormal, scareTexture, smokeTexture, cliffTexture, heightTexture;
enum {
	texFBO0, texFBO1, texDepth, texSky, texShadow, texDiff0, texDiff1, texSpec, texNormal
};

light l = LIGHT_DEFAULT;
float V_vertFov = 45;
bool V_reloadShaders = true;

extern AABB dummyBox;
extern float dummyXRot;

void LoadShaders();

void V_Init() {
	V_InitOpenGL();
	
	V_CreateFBO(&post0, V_WIDTH, V_HEIGHT, 2);
	V_CreateFBO(&post1, V_WIDTH, V_HEIGHT, 2);
	V_CreateDepthFBO(&depth, V_WIDTH, V_HEIGHT);
	V_CreateDepthFBO(&shadow, 512, 512);

	V_LoadAssimp("Hills.obj", &model);
	V_LoadAssimp("SmoothPillars.obj", &pillar);
	V_LoadAssimp("plane.obj", &plane);
	V_LoadAssimp("Cube.obj", &cube);
	V_LoadAssimp("Disco.obj", &sphere);
	V_LoadAssimp("SKS.dae", &weapon);
	V_LoadAssimp("Scarecrow.dae", &scarecrow);
	V_LoadAssimp("CollisionBox.obj", &collBox);
	sprite heightSprite;
	V_LoadSprite("Terrain.png", &heightSprite);
	V_CreateHeightMap(&heightMap, &heightSprite, 5);

	grassTexture = V_LoadTexture("Grass0138_35_S.jpg");
	cliffTexture = V_LoadTexture("Cliffside.png");
	heightTexture = V_LoadTexture("Terrain.png");
	roughTexture = V_LoadTexture("Fabric.png");
	skyMap = V_LoadCubeMap("Sunny sky");
	weaponTexture = V_LoadTexture("SKS.png");
	specTexture = V_LoadTexture("SpecularGrain.png");
	normalTexture = V_LoadTexture("NormalMap.png");
	blackTexture = V_LoadTexture("Black.png");
	whiteTexture = V_LoadTexture("White.png");
	flatNormal = V_LoadTexture("FlatNormal.png");
	scareTexture = V_LoadTexture("Scarecrow.png");
	smokeTexture = V_LoadTexture("Smoke.png");
	
	V_BindTexture(depth.attD, texDepth);
	V_BindTexture(shadow.attD, texShadow);
	V_SetTexInterLinear(true);
	V_BindCubeMap(skyMap, texSky);
	V_BindTexture(grassTexture, texDiff0);
	V_BindTexture(roughTexture, texSpec);
	V_BindTexture(normalTexture, texNormal);
	V_BindTexture(cliffTexture, texDiff1);
	
	LoadShaders();
	
	V_SetDepthTesting(true);
}

void V_RenderScene() {
	/*V_BindTexture(normalTexture, texNormal);
	mat4x4_translate(matModel, 0, -1, 0);
	mat4x4_rotate_X(matModel, matModel, -M_PI / 2);
	mat4x4_scale_aniso(matModel, matModel, 10, 10, 10);
	V_SetParam4m("matModel", matModel);
	V_SetParam1f("uvScale", 10);
	V_BindTexture(whiteTexture, texDiff0);
	V_BindTexture(specTexture, texSpec);
	V_RenderModel(&plane);
	
	mat4x4_translate(matModel, 0, 5, -5);
	V_SetParam4m("matModel", matModel);
	V_BindTexture(whiteTexture, texDiff0);
	V_BindTexture(whiteTexture, texSpec);
	V_RenderModel(&sphere);
	V_BindTexture(flatNormal, texNormal);
	V_SetParam1f("uvScale", 1);
	for (int x = -2; x <= 2; x++) 
		for (int y = -2; y <= 2; y++) {
			V_BindTexture(grassTexture, texDiff0);
			V_BindTexture(blackTexture, texSpec);
			mat4x4_translate(matModel, x * 128, 0, y * 128);
			V_SetParam4m("matModel", matModel);
			V_RenderModel(&model);
			V_BindTexture(whiteTexture, texDiff0);
			V_BindTexture(specTexture, texSpec);
			mat4x4_translate_in_place(matModel, 0, -4, 0);
			V_SetParam4m("matModel", matModel);
			V_RenderModel(&pillar);
		}

	mat4x4_translate(matModel, dummyBox.x + dummyBox.w / 2, dummyBox.y, dummyBox.z + dummyBox.d / 2);
	mat4x4_rotate_X(matModel, matModel, -M_PI / 2 + dummyXRot);
	V_SetParam4m("matModel", matModel);
	V_SetParam1f("uvScale", 1);
	V_BindTexture(scareTexture, texDiff0);
	V_BindTexture(blackTexture, texSpec);
	V_BindTexture(flatNormal, texNormal);
	V_RenderModel(&scarecrow);*/

	mat4x4_translate(matModel, -120, -1, -120);
	mat4x4_scale_aniso(matModel, matModel, 10, 10, 10);
	V_SetParam4m("matModel", matModel);
	V_SetParam1f("uvScale", 1);
	V_SetParam1i("terrain", 1);
	V_BindTexture(grassTexture, texDiff0);
	V_BindTexture(blackTexture, texSpec);
	V_BindTexture(flatNormal, texNormal);
	V_RenderModel(&heightMap);
	V_SetParam1i("terrain", 0);
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
	V_BindTexture(flatNormal, texNormal);
	V_BindTexture(weaponTexture, texDiff0);
	V_BindTexture(weaponTexture, texSpec);
	//V_RenderModel(&weapon);
}

void V_Tick() {
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
	V_SetParam4m("matProj", matShadow);
	V_SetParam4m("matView", identity);
	V_SetParam1f("farPlane", 1);
	V_RenderScene();
	
	V_SetFBO(post0);
	V_ClearDepth();
	
	V_SetShader(skyShader);
	V_SetDepthTesting(false);
	V_SetParam4m("matView", matView);
	V_RenderModel(&cube);
	V_SetFaceCullingBack(true);
	V_SetDepthTesting(true);
	
	V_SetShader(shader);
	
	V_SetParam3f("camPos", G_camPos[0], G_camPos[1], G_camPos[2]);
	V_SetParam4m("matView", matView);
	
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
	
	Sys_CheckErrors();
}

void LoadShaders() {
	V_SetShader(0);
	V_DeleteShader(shader);				shader = V_LoadShader("basic");
	V_DeleteShader(planeShader);		planeShader = V_LoadShader("plane");
	V_DeleteShader(depthShader);		depthShader = V_LoadShader("depth");
	V_DeleteShader(skyShader);			skyShader = V_LoadShader("sky");
	V_DeleteShader(smokeShader);		smokeShader = V_LoadShader("smoke");

	V_SetProj(65);
	mat4x4_identity(matShadow);
	vec4 lightDir = {1, 1, 1, 0};
	vec3_scale(lightDir, lightDir, 1 / vec3_len(lightDir));
	mat4x4_look_at(matShadow, lightDir, (vec3){0, 0, 0}, (vec3){0, 1, 0});
	mat4x4 ortho;
	mat4x4_ortho(ortho, -20, 20, -20, 20, -40, 40);
	mat4x4_mul(matShadow, ortho, matShadow);
	mat4x4_identity(identity);
	
	V_SetShader(shader);
	V_SetParam1i("tex0", texDiff0);
	V_SetParam1i("tex3", texDiff1);
	V_SetParam1i("tex1", texSpec);
	V_SetParam1i("tex2", texNormal);
	V_SetParam1i("texShadow", texShadow);
	V_SetParam1i("texSky", texSky);
	V_SetParam4m("matProj", matProj);
	V_SetParam4m("matShadow", matShadow);
	V_SetParam3f("bgColor", 0, 0, 0.3f);
	V_SetParam1f("farPlane", V_FAR);
	V_SetParam1f("uvScale", 1);
	V_SetParam3f("lightDir", lightDir[0], lightDir[1], lightDir[2]);
	V_SetParam1i("terrain", 0);
	
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
	V_SetParam1f("farPlane", V_FAR);
	V_SetParam3f("modColor", 1, 0.96f, 0.92f);
	
	V_SetShader(smokeShader);
	V_SetParam1i("tex0", texDiff0);
	V_SetParam1i("texSky", texSky);
	V_SetParam4m("matProj", matProj);
	V_SetParam3f("bgColor", 0, 0, 0.3f);
	V_SetParam1f("farPlane", V_FAR);
	V_SetParam3f("lightDir", lightDir[0], lightDir[1], lightDir[2]);
	
	V_reloadShaders = false;
}

void V_SetProj(float fov) {
	if (fov == V_vertFov) return;
	V_vertFov = fov;
	V_MakeProjection(matProj, fov / 180 * M_PI, (float) V_WIDTH / V_HEIGHT, V_NEAR, V_FAR);

	V_SetShader(shader);
	V_SetParam4m("matProj", matProj);
	V_SetShader(skyShader);
	V_SetParam4m("matProj", matProj);
	V_SetShader(smokeShader);
	V_SetParam4m("matProj", matProj);
}

void V_Quit() {
	V_QuitOpenGL();
	Sys_CloseWindow();
}
