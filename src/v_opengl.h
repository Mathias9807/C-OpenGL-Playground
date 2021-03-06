// v_opengl.h

#ifndef V_OPENGL_H
#define V_OPENGL_H


#include "def.h"
#include "v_main.h"
#include "g_main.h"
#include "v_model.h"
#include <GL/glew.h>
#include <linmath.h>

#define V_WINDOW_FBO ((struct fbo){0,V_WIDTH,V_HEIGHT})
#define LIGHT_DEFAULT {{0, 0, 0}, {1, 1, 1}, false}

extern GLuint curShader;
extern float V_vertFov;

int V_LoadAssimp(char* path, model_t* m);
void V_InitOpenGL();
void V_QuitOpenGL();
void V_RenderModel(model_t* m);
void V_CreateHeightMap(model_t* m, sprite* s, double size, double height);
void V_ClearColor(float r, float g, float b, float a);
void V_ClearDepth();
void V_SetDepthTesting(bool b);
void V_SetDepthWriting(bool b);
void V_SetAlphaBlending(bool b);
void V_CreateFBO(struct fbo* fbo, int w, int h, int attachments);
void V_CreateDepthFBO(struct fbo* fbo, int w, int h);
void V_DeleteFBO(struct fbo* fbo);
void V_SetFBO(struct fbo fbo);
void V_BindTexture(unsigned id, int pos);
void V_BindCubeMap(unsigned id, int pos);
void V_SetTexRepeating(bool b);
void V_SetTexInterLinear(bool b);
void V_SetTexMipmapLinear(bool b);
void V_MakeProjection(mat4x4 m, float fov, float aspect, float near, float far);
void V_SetFaceCullingBack(bool back);

GLuint V_LoadShader(const char* name);
void V_DeleteShader(int programId);
GLuint V_LoadTexture(char* name);
GLuint V_LoadCubeMap(char* name);
void V_SetAnimMatrix(model_t* m, unsigned int time, mat4x4 r);
void V_GetBoneMatrix(bone_t* bone, unsigned int time, mat4x4 r);
void V_SetShader(GLuint id);
void V_SetParam1f(const char* var, float f);
void V_SetParam2f(const char* var, float x, float y);
void V_SetParam3f(const char* var, float x, float y, float z);
void V_SetParam4f(const char* var, float x, float y, float z, float a);
void V_SetParam1i(const char* var, int i);
void V_SetParam4m(const char* var, mat4x4 mat);
void V_SetParamLight(int i, light l);
void V_PrintMat(mat4x4 mat);


#endif // V_OPENGL_H
