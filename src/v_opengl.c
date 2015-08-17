// v_opengl.h

#include "v_opengl.h"
#include "sys.h"
#include "v_main.h"
#include "g_main.h"
#include <string.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint curShader;
int animStartTime;

void V_InitOpenGL() {
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // glewInit() may throw a GL_INVALID_ENUM error
	
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("OpenGL implementation provided by %s\n", glGetString(GL_VENDOR));
	
	glEnable(GL_CULL_FACE);
	V_SetFaceCullingBack(true);
	
	animStartTime = Sys_TimeMillis();
}

void V_QuitOpenGL() {
}

void V_ClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void V_ClearDepth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

void V_SetDepthTesting(bool b) {
	if (b) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void V_SetFBO(struct fbo fbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
	glViewport(0, 0, fbo.w, fbo.h);
}

void V_CreateFBO(struct fbo* fbo, int w, int h, int attachments) {
	glGenFramebuffers(1, (GLuint*) &fbo->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	fbo->w = w;
	fbo->h = h;
	fbo->numAtt = attachments;
	fbo->att = malloc(attachments * sizeof(int));
	glGenTextures(attachments, fbo->att);
	for (int i = 0; i < attachments; i++) {
		glBindTexture(GL_TEXTURE_2D, fbo->att[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, fbo->att[i], 0);
	}
	glGenRenderbuffers(1, &fbo->attD);
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->attD);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->attD);
	GLenum drawBuffers[attachments];
	for (int i = 0; i < attachments; i++) drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	glDrawBuffers(attachments, drawBuffers);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Failed to create framebuffer\n");
}

void V_CreateDepthFBO(struct fbo* fbo, int w, int h) {
	glGenFramebuffers(1, (GLuint*) &fbo->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
	fbo->w = w;
	fbo->h = h;
	fbo->numAtt = 0;
	glGenTextures(1, &fbo->attD);
	glBindTexture(GL_TEXTURE_2D, fbo->attD);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo->attD, 0);
	Sys_CheckErrors();
	glDrawBuffer(GL_NONE);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Failed to create framebuffer\n");
}

void V_BindTexture(unsigned id, int pos) {
	glActiveTexture(GL_TEXTURE0 + pos);
	glBindTexture(GL_TEXTURE_2D, id);
}

void V_BindCubeMap(unsigned id, int pos) {
	glActiveTexture(GL_TEXTURE0 + pos);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void V_SetTexInterLinear(bool b) {
	GLuint inter = b ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, inter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, inter);
}

void V_SetTexMipmapLinear(bool b) {
	GLuint interMag = b ? GL_LINEAR : GL_NEAREST;
	GLuint interMin = b ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interMag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interMin);
}

void V_MakeProjection(mat4x4 m, float fov, float aspect, float near, float far) {
	mat4x4_perspective(m, fov, aspect, near, far);
}

void V_SetFaceCullingBack(bool back) {
	glCullFace(back ? GL_BACK : GL_FRONT);
}

void V_SetAnimMatrix(model_t* m, unsigned int time, mat4x4 r) {
	mat4x4_translate(r, m->posKeys[0].value[0], m->posKeys[0].value[1], m->posKeys[0].value[2]);
	
	for (int i = 1; i < m->keyCount; i++) {
		if (m->posKeys[i].time > time && m->posKeys[i-1].time < time) {
			float delta = (float) (time - m->posKeys[i-1].time) / (m->posKeys[i].time - m->posKeys[i-1].time);
			float iDelta = 1 - delta;
			float transl[3] = {
				iDelta * m->posKeys[i-1].value[0] + delta * m->posKeys[i].value[0], 
				iDelta * m->posKeys[i-1].value[1] + delta * m->posKeys[i].value[1], 
				iDelta * m->posKeys[i-1].value[2] + delta * m->posKeys[i].value[2]
			};
			mat4x4_translate(r, transl[0], transl[1], transl[2]);
			return;
		}
	}
	
	if (m->posKeys[m->keyCount-1].time < time)
		mat4x4_translate(r, m->posKeys[m->keyCount-1].value[0], m->posKeys[m->keyCount-1].value[1], m->posKeys[m->keyCount-1].value[2]);
}

void V_GetBoneMatrix(bone_t* bone, unsigned int time, mat4x4 r) {
	mat4x4_from_quat(r, bone->keys[0].value);
	
	for (int i = 1; i < bone->keyCount; i++) {
		if (bone->keys[i].time > time && bone->keys[i-1].time < time) {
			float delta = (float) (time - bone->keys[i-1].time) / (bone->keys[i].time - bone->keys[i-1].time);
			float iDelta = 1 - delta;
			quat inter;
			inter[0] = iDelta * bone->keys[i-1].value[0] + delta * bone->keys[i].value[0]; 
			inter[1] = iDelta * bone->keys[i-1].value[1] + delta * bone->keys[i].value[1]; 
			inter[2] = iDelta * bone->keys[i-1].value[2] + delta * bone->keys[i].value[2]; 
			inter[3] = iDelta * bone->keys[i-1].value[3] + delta * bone->keys[i].value[3];
			mat4x4_from_quat(r, inter);
			return;
		}
	}
	
	if (bone->keys[bone->keyCount-1].time < time)
		mat4x4_from_quat(r, bone->keys[bone->keyCount-1].value);
}

void V_SetShader(GLuint id) {
	curShader = id;
	glUseProgram(id);
}

void V_CheckShader(GLuint id) {
	GLint result = -1, logLength = -1;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_TRUE) return;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
	GLchar log[logLength];
	glGetShaderInfoLog(id, logLength, &logLength, log);
	printf("%s\n", log);
}

void V_CheckProgram(GLuint id) {
	GLint result = -1, logLength = -1;
	glGetProgramiv(id, GL_LINK_STATUS, &result);
	if (result == GL_TRUE) return;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
	GLchar log[logLength];
	glGetProgramInfoLog(id, logLength, &logLength, log);
	printf("%s\n", log);
}

GLuint V_LoadShaders(char* name) {
	char fragPath[64]; Sys_GetResourcePath("shaders/", fragPath); strcat(fragPath, name); strcat(fragPath, ".fsh");
	char vertPath[64]; Sys_GetResourcePath("shaders/", vertPath); strcat(vertPath, name); strcat(vertPath, ".vsh");
	
	GLuint fragId = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vertId = glCreateShader(GL_VERTEX_SHADER);
	
	FILE* fragFile = fopen(fragPath, "r");
	FILE* vertFile = fopen(vertPath, "r");
	
	int fragLength = 0;
	while (fgetc(fragFile) != EOF) fragLength++;
	rewind(fragFile);
	int vertLength = 0;
	while (fgetc(vertFile) != EOF) vertLength++;
	rewind(vertFile);
	GLchar fragText[fragLength];
	GLchar vertText[vertLength];
	for (int i = 0; i < fragLength; i++) fragText[i] = fgetc(fragFile);
	for (int i = 0; i < vertLength; i++) vertText[i] = fgetc(vertFile);
	
	fclose(fragFile);
	fclose(vertFile);
	
	const char* const fragPtr = fragText;
	const char* const vertPtr = vertText;
	
	glShaderSource(fragId, 1, &fragPtr, &fragLength);
	glShaderSource(vertId, 1, &vertPtr, &vertLength);
	glCompileShader(fragId);
	glCompileShader(vertId);
	V_CheckShader(fragId);
	V_CheckShader(vertId);
	
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertId);
	glAttachShader(programId, fragId);
	glLinkProgram(programId);
	
	glDeleteShader(fragId);
	glDeleteShader(vertId);
	
	return programId;
}

GLuint V_LoadTexture(char* name) {
	char path[64]; Sys_GetResourcePath(name, path);
	unsigned char* data;
	int w, h, n;
	data = stbi_load(path, &w, &h, &n, 4);
	
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	stbi_image_free(data);
	return tex;
}

GLuint V_LoadCubeMap(char* name) {
	char* faces[6] = {
		"/right.png", "/left.png", "/top.png", 
		"/bottom.png", "/front.png", "/back.png"
	};
	
	GLuint cubeTex;
	glGenTextures(1, &cubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
	for (int i = 0; i < 6; i++) {
		char path[64]; Sys_GetResourcePath(name, path); strcat(path, faces[i]);
		unsigned char* data;
		int w, h, n;
		data = stbi_load(path, &w, &h, &n, 4);
		
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		stbi_image_free(data);
	}
	
	return cubeTex;
}

void V_SetParam1f(const char* var, float f) {
	GLuint id = glGetUniformLocation(curShader, var);
	glUniform1f(id, f);
}

void V_SetParam2f(const char* var, float x, float y) {
	GLuint id = glGetUniformLocation(curShader, var);
	glUniform2f(id, x, y);
}

void V_SetParam3f(const char* var, float x, float y, float z) {
	GLuint id = glGetUniformLocation(curShader, var);
	glUniform3f(id, x, y, z);
}

void V_SetParam4m(const char* var, mat4x4 mat) {
	GLuint id = glGetUniformLocation(curShader, var);
	float fArr[16];
	for (int i = 0; i < 16; i++) fArr[i] = mat[i / 4][i % 4];
	glUniformMatrix4fv(id, 1, GL_FALSE, fArr);
}

void V_SetParam1i(const char* var, int i) {
	GLuint id = glGetUniformLocation(curShader, var);
	glUniform1i(id, i);
}

void V_PrintMat(mat4x4 mat) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			printf("%f ", mat[i][j]);
		printf("\n");
	}
}
