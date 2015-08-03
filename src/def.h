// def.h - Various typedefs and definitions

#ifndef DEF_H
#define DEF_H


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "math.h"

#define TITLE "3D"

typedef struct sprite_s {
	int w;			// Dimensions of pixel array
	int h;
	uint32_t* pix;	// Pixel array in format ARGB
} sprite_t;


#endif //DEF_H
