// g_animation.h - Animation utilities

#ifndef G_ANIMATION_H
#define G_ANIMATION_H


typedef struct {
	float start;
	float stop;
	float k;
	float m;
} linearf;

typedef struct {
	float coeff;
	float exp;
} termf;

typedef struct {
	float start;
	float stop;
	int numTerms;
	termf* term;
} function;

float G_Valuel(linearf f, float x);
float G_Valuef(function f, float x);


#endif // G_ANIMATION_H