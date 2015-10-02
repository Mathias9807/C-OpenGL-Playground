// g_animation.c - Animation utilities

#include "g_animation.h"

float G_Value(linearf f, float x) {
	if (x < f.start) return f.k * f.start + f.m;
	if (x > f.stop) return f.k * f.stop + f.m;
	
	return f.k * x + f.m;
}
