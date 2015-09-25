// def.h - Various typedefs and definitions

#ifndef DEF_H
#define DEF_H


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "math.h"

#define TITLE "3D"

typedef struct listEntryStruct {
	void* value;
	struct listEntryStruct* next;
	struct listEntryStruct* prev;
} listEntry;

typedef struct {
	listEntry* first;
	int size;
} list;

typedef struct {
	int w;			// Dimensions of pixel array
	int h;
	uint32_t* pix;	// Pixel array in format ARGB
} sprite;

void* ListGet(list* l, int index);
int ListSize(list* l);
void ListAdd(list* l, void* value);
void ListRemove(list* l, int index);


#endif //DEF_H
