// level.h - Loads and writes levels

#ifndef LEVEL_H
#define LEVEL_H

#include "def.h"
#include "v_main.h"
#include "v_opengl.h"

#define L_NAME_LENGTH 32
#define L_RES_LENGTH 32

#define L_HEADER_SIZE 14
#define L_INDEX_SIZE 9
#define L_PROP_SIZE 28

typedef struct {
	char name[L_NAME_LENGTH];
	
	model_t model;
} resource;

typedef struct {
	resource* res;
	float pos[3];
	float rot[3];
} prop;

typedef struct {
	char name[L_NAME_LENGTH];
	
	list res;
	list props;
} level;

extern level L_current;


void L_InitLevel(char* name);
void L_LoadLevel(char* name);
void L_WriteLevel();
resource* L_AddResource(char* name);
prop* L_AddProp(resource* r, float* pos, float* rot);


#endif // LEVEL_H
