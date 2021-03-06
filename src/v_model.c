// v_model.c - Functions for loading 3D model files

#include "def.h"
#include "sys.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string.h>
#include "v_opengl.h"

void V_InitVBO(VBO_t* vbo, int index, int dim, GLenum type);
void V_AiToLinMat(struct aiMatrix4x4* ai, mat4x4 lin);

int V_LoadAssimp(char* path, model_t* m) {
	char* fullPath = calloc(PATH_LENGTH, sizeof(char));
	SYS_GetResourcePath(path, fullPath);
	
	const struct aiScene* scene = aiImportFile(fullPath, 
		aiProcess_Triangulate 
		| aiProcess_JoinIdenticalVertices 
		| aiProcess_FlipUVs 
		| aiProcess_CalcTangentSpace);
	if (scene == NULL) {
		return -1;
	}
	
	struct aiMesh* mesh = scene->mMeshes[0];
	int vertCount = mesh->mNumVertices;
	int indexCount = mesh->mNumFaces * 3;
	
	/*for (int i = 0; i < scene->mNumAnimations; i++) {
		for (int j = 0; j < scene->mAnimations[i]->mNumChannels; j++) {
			struct aiNodeAnim* anim = scene->mAnimations[i]->mChannels[j];
			printf("%s\n", anim->mNodeName.data);
			for (int a = 0; a < anim->mNumPositionKeys; a++)
				printf("\t%f, %f, %f\n", anim->mPositionKeys[a].mValue.x, anim->mPositionKeys[a].mValue.y, anim->mPositionKeys[a].mValue.z);
			for (int a = 0; a < anim->mNumRotationKeys; a++)
				printf("\t%f, %f, %f, %f\n", anim->mRotationKeys[a].mValue.x, anim->mRotationKeys[a].mValue.y, anim->mRotationKeys[a].mValue.z, anim->mRotationKeys[a].mValue.w);
			for (int a = 0; a < anim->mNumScalingKeys; a++)
				printf("\t%f, %f, %f\n", anim->mScalingKeys[a].mValue.x, anim->mScalingKeys[a].mValue.y, anim->mScalingKeys[a].mValue.z);
		}
	}*/
	
	if (scene->mNumAnimations > 0) {
		m->keyCount = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
		m->posKeys = malloc(m->keyCount * sizeof(vectorKey_t));
		for (int i = 0; i < m->keyCount; i++) {
			struct aiVectorKey* key = &scene->mAnimations[0]->mChannels[0]->mPositionKeys[i];
			m->posKeys[i].time = key->mTime * 1000;
			m->posKeys[i].value[0] = key->mValue.x;
			m->posKeys[i].value[1] = key->mValue.y;
			m->posKeys[i].value[2] = key->mValue.z;
		}
	}else {
		m->keyCount = 0;
	}
	
	m->boneCount = mesh->mNumBones;
	if (mesh->mNumBones > 0) {
		m->bones = malloc(m->boneCount * sizeof(bone_t));
		for (int i = 0; i < mesh->mNumBones; i++) {
			bone_t* bone = &m->bones[i];
			bone->name = malloc(mesh->mBones[i]->mName.length * sizeof(char));
			for (int j = 0; j <= mesh->mBones[i]->mName.length; j++) // j <= ... to keep null-terminator
				bone->name[j] = mesh->mBones[i]->mName.data[j];
			V_AiToLinMat(&mesh->mBones[i]->mOffsetMatrix, bone->bindPose);
		}
	}else m->bones = NULL;
	
	if (scene->mNumAnimations > 0) {
		for (int i = 0; i < scene->mAnimations[0]->mNumChannels; i++) {
			bone_t* bone = NULL;
			struct aiNodeAnim* anim = scene->mAnimations[0]->mChannels[i];
			for (int j = 0; j < m->boneCount; j++) {
				if (!strcmp(m->bones[j].name, anim->mNodeName.data)) bone = &m->bones[j];
			}
			if (!bone) continue;
			
			bone->keyCount = anim->mNumRotationKeys;
			bone->keys = malloc(bone->keyCount * sizeof(quatKey_t));
			for (int j = 0; j < bone->keyCount; j++) {
				struct aiQuatKey* key = &anim->mRotationKeys[j];
				bone->keys[j].value[0] = key->mValue.x;
				bone->keys[j].value[1] = key->mValue.y;
				bone->keys[j].value[2] = key->mValue.z;
				bone->keys[j].value[3] = key->mValue.w;
				bone->keys[j].time = key->mTime * 1000;
			}
		}
	}else {
		for (int i = 0; i < m->boneCount; i++) m->bones[i].keyCount = 0;
	}
	
	VAO_t* vao = &m->vao;
	glGenVertexArrays(1, &vao->id);
	glBindVertexArray(vao->id);
	vao->vertCount = vertCount;
	vao->indexCount = indexCount;
	
	vao->vert.bufferSize = 3 * vertCount * sizeof(float);
	vao->vert.buffer = malloc(vao->vert.bufferSize);
	for (int i = 0; i < vertCount; i++) {
		struct aiVector3D v = mesh->mVertices[i];
		((float*)vao->vert.buffer)[i * 3 + 0] = v.x;
		((float*)vao->vert.buffer)[i * 3 + 1] = v.y;
		((float*)vao->vert.buffer)[i * 3 + 2] = v.z;
	}
	V_InitVBO(&vao->vert, 0, 3, GL_FLOAT);
	
	if (*mesh->mTextureCoords) {
		vao->uv.bufferSize = 3 * vertCount * sizeof(float);
		vao->uv.buffer = malloc(vao->uv.bufferSize);
		for (int i = 0; i < vertCount; i++) {
			struct aiVector3D v = mesh->mTextureCoords[0][i];
			((float*)vao->uv.buffer)[i * 3 + 0] = v.x;
			((float*)vao->uv.buffer)[i * 3 + 1] = v.y;
			((float*)vao->uv.buffer)[i * 3 + 2] = v.z;
		}
		V_InitVBO(&vao->uv, 1, 3, GL_FLOAT);
	}else vao->uv.buffer = NULL;
	
	vao->normal.bufferSize = 3 * vertCount * sizeof(float);
	vao->normal.buffer = malloc(vao->normal.bufferSize);
	for (int i = 0; i < vertCount; i++) {
		struct aiVector3D v = mesh->mNormals[i];
		((float*)vao->normal.buffer)[i * 3 + 0] = v.x;
		((float*)vao->normal.buffer)[i * 3 + 1] = v.y;
		((float*)vao->normal.buffer)[i * 3 + 2] = v.z;
	}
	V_InitVBO(&vao->normal, 2, 3, GL_FLOAT);
	
	vao->tangents.bufferSize = 3 * vertCount * sizeof(float);
	vao->tangents.buffer = malloc(vao->tangents.bufferSize);
	if (mesh->mTangents) 
		for (int i = 0; i < vertCount; i++) {
			struct aiVector3D v = mesh->mTangents[i];
			((float*)vao->tangents.buffer)[i * 3 + 0] = v.x;
			((float*)vao->tangents.buffer)[i * 3 + 1] = v.y;
			((float*)vao->tangents.buffer)[i * 3 + 2] = v.z;
		}
	else memset(vao->tangents.buffer, 0, vao->tangents.bufferSize);
	V_InitVBO(&vao->tangents, 4, 3, GL_FLOAT);
	
	glGenBuffers(1, &vao->index.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->index.id);
	vao->index.type = GL_UNSIGNED_INT;
	vao->index.dim = 1;
	vao->index.buffer = malloc(indexCount * sizeof(int));
	for (int i = 0; i < mesh->mNumFaces; i++) {
		struct aiFace f = mesh->mFaces[i];
		((int*)vao->index.buffer)[i * 3 + 0] = f.mIndices[0];
		((int*)vao->index.buffer)[i * 3 + 1] = f.mIndices[1];
		((int*)vao->index.buffer)[i * 3 + 2] = f.mIndices[2];
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), vao->index.buffer, GL_STATIC_DRAW);
	
	if (mesh->mNumBones > 0) {
		vao->weights.bufferSize = 2 * vertCount * sizeof(float);
		vao->weights.buffer = malloc(vao->weights.bufferSize);
		for (int i = 0; i < vertCount; i++) {
			((float*)vao->weights.buffer)[i*2+0] = 0;
			((float*)vao->weights.buffer)[i*2+1] = 0;
			for (int j = 0; j < mesh->mBones[0]->mNumWeights; j++) {
				if (mesh->mBones[0]->mWeights[j].mVertexId == i) ((float*)vao->weights.buffer)[i*2+0] = mesh->mBones[0]->mWeights[j].mWeight;
			}
			for (int j = 0; j < mesh->mBones[1]->mNumWeights; j++) {
				if (mesh->mBones[1]->mWeights[j].mVertexId == i) ((float*)vao->weights.buffer)[i*2+1] = mesh->mBones[1]->mWeights[j].mWeight;
			}
		}
		V_InitVBO(&vao->weights, 3, 2, GL_FLOAT);
	}else
		vao->weights.buffer = NULL;
	
	aiReleaseImport(scene);
	
	printf("Loaded model: %s\n", path);
	return 0;
}

void V_CreateHeightMap(model_t* m, sprite* s, double size, double height) {
	int w = s->w, h = s->h;
	if (w < 1 || h < 1)
		SYS_Error("Heightmap has an invalid width or height. ");

	int vertCount = w * h;
	int indexCount = (w - 1) * (h - 1) * 6;

	memset(m, 0, sizeof(model_t));

	for (int i = 0; i < m->boneCount; i++) m->bones[i].keyCount = 0;

	VAO_t* vao = &m->vao;
	glGenVertexArrays(1, &vao->id);
	glBindVertexArray(vao->id);
	vao->vertCount = vertCount;
	vao->indexCount = indexCount;

	vao->vert.bufferSize = 3 * vertCount * sizeof(float);
	vao->vert.buffer = malloc(vao->vert.bufferSize);
	double hScale = size / w, vScale = size / h;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			double sampleHeight = ((s->pix[y * w + x] & 0xFF000000) >> 24) / 256.0;
			((float*)vao->vert.buffer)[y * w * 3 + x * 3 + 0] = x * hScale;
			((float*)vao->vert.buffer)[y * w * 3 + x * 3 + 1] = sampleHeight * height;
			((float*)vao->vert.buffer)[y * w * 3 + x * 3 + 2] = y * vScale;
		}
	}
	V_InitVBO(&vao->vert, 0, 3, GL_FLOAT);

	vao->uv.bufferSize = 3 * vertCount * sizeof(float);
	vao->uv.buffer = malloc(vao->uv.bufferSize);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			((float*)vao->uv.buffer)[y * w * 3 + x * 3 + 0] = x;
			((float*)vao->uv.buffer)[y * w * 3 + x * 3 + 1] = y;
			((float*)vao->uv.buffer)[y * w * 3 + x * 3 + 2] = 0;
		}
	}
	V_InitVBO(&vao->uv, 1, 3, GL_FLOAT);

	vao->normal.bufferSize = 3 * vertCount * sizeof(float);
	vao->normal.buffer = malloc(vao->normal.bufferSize);
	for (int i = 0; i < vertCount; i++) {
		vec3 normal = {0, 0, 0};
		int x = i % w, y = i / w;

		int xMin = x == 0 ? x : x - 1;
		int xMax = x >= w - 1 ? x : x + 1;
		float yDelta = ((float*)vao->vert.buffer)[y * w * 3 + xMax * 3 + 1]
			- ((float*)vao->vert.buffer)[y * w * 3 + xMin * 3 + 1];
		float xDelta = xMax - xMin;
		float zAngle = atan2(yDelta, xDelta);
		normal[1] = cos(zAngle);

		int yMin = y == 0 ? y : y - 1;
		int yMax = y >= h - 1 ? y : y + 1;
		yDelta = ((float*)vao->vert.buffer)[yMax * w * 3 + x * 3 + 1]
			- ((float*)vao->vert.buffer)[yMin * w * 3 + x * 3 + 1];
		float zDelta = yMax - yMin;
		float xAngle = atan2(yDelta, zDelta);
		normal[1] *= cos(xAngle);

		normal[0] = -sin(zAngle);
		normal[2] = -sin(xAngle);

		((float*)vao->normal.buffer)[i * 3 + 0] = normal[0];
		((float*)vao->normal.buffer)[i * 3 + 1] = normal[1];
		((float*)vao->normal.buffer)[i * 3 + 2] = normal[2];
	}
	V_InitVBO(&vao->normal, 2, 3, GL_FLOAT);

	vao->tangents.bufferSize = 3 * vertCount * sizeof(float);
	vao->tangents.buffer = malloc(vao->tangents.bufferSize);
	for (int i = 0; i < vertCount; i++) {
		vec3_mul_cross(&((float*)vao->tangents.buffer)[i * 3],
			&((float*)vao->normal.buffer)[i * 3],
			(vec3) {0, 0, 1});
	}
	V_InitVBO(&vao->tangents, 4, 3, GL_FLOAT);

	glGenBuffers(1, &vao->index.id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao->index.id);
	vao->index.type = GL_UNSIGNED_INT;
	vao->index.dim = 1;
	vao->index.buffer = malloc(indexCount * sizeof(int));
	for (int x = 0; x < w - 1; x++) {
		for (int y = 0; y < h - 1; y++) {
			((int*)vao->index.buffer)[y * (w - 1) * 6 + x * 6 + 0] = y * w + x;
			((int*)vao->index.buffer)[y * (w - 1) * 6 + x * 6 + 1] = (y + 1) * w + x;
			((int*)vao->index.buffer)[y * (w - 1) * 6 + x * 6 + 2] = y * w + x + 1;
			((int*)vao->index.buffer)[y * (w - 1) * 6 + x * 6 + 3] = (y + 1) * w + x;
			((int*)vao->index.buffer)[y * (w - 1) * 6 + x * 6 + 4] = (y + 1) * w + x + 1;
			((int*)vao->index.buffer)[y * (w - 1) * 6 + x * 6 + 5] = y * w + x + 1;
		}
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), vao->index.buffer, GL_STATIC_DRAW);

	vao->weights.buffer = NULL;
}

void V_InitVBO(VBO_t* vbo, int index, int dim, GLenum type) {
	glGenBuffers(1, &vbo->id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
	vbo->type = type;
	vbo->dim = dim;
	glBufferData(GL_ARRAY_BUFFER, vbo->bufferSize, vbo->buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(index, vbo->dim, vbo->type, GL_FALSE, 0, (void*) 0);
}

void V_RenderModel(model_t* m) {
	glBindVertexArray(m->vao.id);
	glEnableVertexAttribArray(0);
	if (m->vao.uv.buffer) glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	if (m->vao.weights.buffer) glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	if (!m->vao.index.buffer) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->vao.index.id);
	
	glDrawElements(GL_TRIANGLES, m->vao.indexCount, m->vao.index.type, (void*) 0);
	
	glDisableVertexAttribArray(0);
	if (m->vao.uv.buffer) glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	if (m->vao.weights.buffer) glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}

void V_AiToLinMat(struct aiMatrix4x4* ai, mat4x4 lin) {
	lin[0][0] = ai->a1; lin[0][1] = ai->a2; lin[0][2] = ai->a3; lin[0][3] = ai->a4;
	lin[1][0] = ai->b1; lin[1][1] = ai->b2; lin[1][2] = ai->b3; lin[1][3] = ai->b4;
	lin[2][0] = ai->c1; lin[2][1] = ai->c2; lin[2][2] = ai->c3; lin[2][3] = ai->c4;
	lin[3][0] = ai->d1; lin[3][1] = ai->d2; lin[3][2] = ai->d3; lin[3][3] = ai->d4;
}
