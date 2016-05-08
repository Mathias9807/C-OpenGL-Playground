// v_model.h

#ifndef V_MODEL_H
#define V_MODEL_H


#include <linmath.h>

typedef struct {
	unsigned id;
	int type;
	int dim; // 2, 3 or 4
	int bufferSize;
	void* buffer;
} VBO_t;

typedef struct {
	unsigned id;
	VBO_t vert;
	VBO_t uv;
	VBO_t normal;
	VBO_t weights;
	VBO_t tangents;
	VBO_t index;
	int vertCount;
	int indexCount;
} VAO_t;

typedef struct vectorKey_s {
	vec3 value;
	unsigned int time;
} vectorKey_t;

typedef struct matrixKey_s {
	mat4x4 value;
	unsigned int time;
} matrixKey_t;

typedef struct quatKey_s {
	quat value;
	unsigned int time;
} quatKey_t;

typedef struct bone_s {
	char* name;
	int keyCount;
	quatKey_t* keys;
	mat4x4 bindPose;
} bone_t;

typedef struct model_s {
	VAO_t vao;
	int boneCount;
	bone_t* bones;
	int keyCount;
	vectorKey_t* posKeys;
} model_t;


#endif // V_MODEL_H
