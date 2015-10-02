// g_animation.c - Animation utilities

#include "g_animation.h"
#include <math.h>

float G_Valuel(linearf f, float x) {
	if (x < f.start)	return x = f.start;
	if (x > f.stop)		return x = f.stop;

	return f.k * x + f.m;
}

float G_Valuef(function f, float x) {
	if (x < f.start)	return x = f.start;
	if (x > f.stop)		return x = f.stop;

	float sum = 0;
	for (int i = 0; i < f.numTerms; i++) {
		sum += f.term[i].coeff * pow(x, f.term[i].exp);
	}

	return sum;
}
