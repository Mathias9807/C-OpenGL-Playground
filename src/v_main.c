// v_main.c - Main file for rendering

#include "v_main.h"
#include "sys.h"
#include "g_main.h"
#include "v_opengl.h"

model_t model, plane, pillar, cube, sphere, walther;
mat4x4 matProj, matView, matModel, matShadow, identity;
GLuint shader, planeShader, depthShader, skyShader;
struct fbo post0, post1, depth, shadow;
GLuint grassTexture, roughTexture, skyMap, waltherTexture, specTexture, normalTexture, blackTexture, whiteTexture, flatNormal;
const int texFBO = 0, texDepth = 1, texSky = 2, texShadow = 3, texDiff = 8, texSpec = 9, texNormal = 10;

void V_Init() {
	V_InitOpenGL();
	
	V_CreateFBO(&post0, V_WIDTH, V_HEIGHT, 1);
	V_CreateFBO(&post1, V_WIDTH, V_HEIGHT, 1);
	V_CreateDepthFBO(&depth, V_WIDTH, V_HEIGHT);
	V_CreateDepthFBO(&shadow, 1024, 1024);
	shader = V_LoadShaders("basic");
	planeShader = V_LoadShaders("plane");
	depthShader = V_LoadShaders("depth");
	skyShader = V_LoadShaders("sky");
	V_LoadAssimp("Hills.obj", &model);
	V_LoadAssimp("SmoothPillars.obj", &pillar);
	V_LoadAssimp("plane.obj", &plane);
	V_LoadAssimp("Cube.obj", &cube);
	V_LoadAssimp("Disco.obj", &sphere);
	V_LoadAssimp("Walther.obj", &walther);
	grassTexture = V_LoadTexture("Grass0138_35_S.jpg");
	roughTexture = V_LoadTexture("Fabric.png");
	skyMap = V_LoadCubeMap("Sunny sky");
	waltherTexture = V_LoadTexture("Walther.png");
	specTexture = V_LoadTexture("SpecularGrain.png");
	normalTexture = V_LoadTexture("NormalMap.png");
	blackTexture = V_LoadTexture("Black.png");
	whiteTexture = V_LoadTexture("White.png");
	flatNormal = V_LoadTexture("FlatNormal.png");
	
	V_MakeProjection(matProj, V_FOV, (float) V_WIDTH / V_HEIGHT, V_NEAR, V_FAR / V_NEAR);
	mat4x4_identity(matShadow);
	mat4x4_rotate_X(matShadow, matShadow, -M_PI / 16);
	mat4x4_scale_aniso(matShadow, matShadow, 0.1, 0.1, 0.1);
	mat4x4_identity(identity);
	V_BindTexture(depth.attD, texDepth);
	V_BindTexture(shadow.attD, texShadow);
	V_BindCubeMap(skyMap, texSky);
	V_BindTexture(grassTexture, texDiff);
	V_BindTexture(roughTexture, texSpec);
	V_BindTexture(normalTexture, texNormal);
	
	V_SetShader(shader);
	V_SetParam1i("tex0", texDiff);
	V_SetParam1i("tex1", texSpec);
	V_SetParam1i("tex2", texNormal);
	V_SetParam1i("texShadow", texShadow);
	V_SetParam1i("texSky", texSky);
	V_SetParam4m("matProj", matProj);
	V_SetParam4m("matShadow", matShadow);
	V_SetParam3f("bgColor", 0, 0, 0.3f);
	V_SetParam1f("farPlane", V_FAR);
	V_SetParam1f("uvScale", 1);
	
	V_SetShader(skyShader);
	V_SetParam1i("tex", texSky);
	V_SetParam4m("matProj", matProj);
	
	V_SetShader(planeShader);
	V_SetParam1i("w", post0.w);
	V_SetParam1i("h", post0.h);
	V_SetParam1i("tex0", 0);
	V_SetParam1i("shadow", texShadow);
	V_SetParam1i("depth", 1);
	V_SetParam1f("farPlane", V_FAR);
	V_SetParam3f("modColor", 1, 0.96f, 0.92f);
	
	V_SetDepthTesting(true);
}

void V_RenderScene() {
	V_BindTexture(normalTexture, texNormal);
	mat4x4_translate(matModel, 0, -1, 0);
	mat4x4_rotate_X(matModel, matModel, -M_PI / 2);
	mat4x4_scale_aniso(matModel, matModel, 10, 10, 10);
	V_SetParam4m("matModel", matModel);
	V_SetParam1f("uvScale", 10);
	V_BindTexture(whiteTexture, texDiff);
	V_BindTexture(specTexture, texSpec);
	V_RenderModel(&plane);
	
	V_SetParam4m("matModel", G_gunMat);
	V_SetParam1f("uvScale", 1);
	V_BindTexture(waltherTexture, texDiff);
	V_BindTexture(waltherTexture, texSpec);
	V_RenderModel(&walther);
	
	V_BindTexture(flatNormal, texNormal);
	mat4x4_translate(matModel, 0, 5, -5);
	V_SetParam4m("matModel", matModel);
	V_BindTexture(whiteTexture, texDiff);
	V_BindTexture(whiteTexture, texSpec);
	V_RenderModel(&sphere);
	V_BindTexture(normalTexture, texNormal);
	for (int x = -2; x <= 2; x++) 
		for (int y = -2; y <= 2; y++) {
			V_BindTexture(grassTexture, texDiff);
			V_BindTexture(blackTexture, texSpec);
			mat4x4_translate(matModel, x * 128, 0, y * 128);
			V_SetParam4m("matModel", matModel);
			V_RenderModel(&model);
			V_BindTexture(whiteTexture, texDiff);
			V_BindTexture(specTexture, texSpec);
			mat4x4_translate_in_place(matModel, 0, -4, 0);
			V_SetParam4m("matModel", matModel);
			V_RenderModel(&pillar);
		}
}

void V_Tick() {
	mat4x4_identity(matView);
	mat4x4_rotate_X(matView, matView, -G_camRot[0]);
	mat4x4_rotate_Y(matView, matView, -G_camRot[1]);
	mat4x4_translate_in_place(matView, -G_camPos[0], -G_camPos[1], -G_camPos[2]);
	
	/*V_SetFBO(shadow);
	V_ClearDepth();
	V_SetShader(depthShader);
	V_SetDepthTesting(1);
	V_SetParam4m("matProj", matShadow);
	V_SetParam4m("matView", identity);
	V_SetParam1f("farPlane", 1);
	V_RenderScene();*/
	
	V_SetFBO(post0);
	V_ClearDepth();
	
	V_SetShader(skyShader);
	V_SetDepthTesting(0);
	V_SetParam4m("matView", matView);
	V_RenderModel(&cube);
	V_SetDepthTesting(1);
	
	V_SetShader(shader);
	
	V_SetParam3f("camPos", G_camPos[0], G_camPos[1], G_camPos[2]);
	V_SetParam4m("matView", matView);
	
	V_RenderScene();
	V_SetDepthTesting(0);
	
	V_SetFBO(post1);
	V_SetShader(planeShader);
	V_SetParam2f("dir", 1, 0);
	V_BindTexture(post0.att[0], texFBO);
	
	V_RenderModel(&plane);
	
	V_SetFBO(V_WINDOW_FBO);
	V_SetParam2f("dir", 0, 1);
	V_BindTexture(post1.att[0], texFBO);
	
	V_RenderModel(&plane);
	
	Sys_CheckErrors();
}

void V_Quit() {
	V_QuitOpenGL();
	Sys_CloseWindow();
}
