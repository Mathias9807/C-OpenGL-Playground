// level.h - Loads and writes levels

#ifndef LEVEL_H
#define LEVEL_H

#include "def.h"

#define L_NAME_LENGTH 32
#define L_RES_LENGTH 32

#define L_HEADER_SIZE 14
#define L_INDEX_SIZE 9
#define L_PROP_SIZE 28

typedef struct {
	char name[L_NAME_LENGTH];
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

level L_current;


void L_LoadLevel(char* name);
void L_WriteLevel();


#endif // LEVEL_H
