// g_animation.h - Animation utilities

#ifndef G_ANIMATION_H
#define G_ANIMATION_H


typedef struct {
	float start;
	float stop;
	float k;
	float m;
} linearf;

float G_Value(linearf f, float x);


#endif // G_ANIMATION_H