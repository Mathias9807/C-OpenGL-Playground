// cvar.h - Program wide variables

#ifndef CVAR_H
#define CVAR_H


#include "def.h"

typedef struct {
	char*	name;
	float	value;
} cvar;

// Global list holding all cvars.
list C_cvars;

// Adds cvar v to C_cvars, does nothing if v already exists.
cvar* C_Add(cvar v);

// Sets the value of a cvar in C_cvars.
void C_Set(char* name, float value);

// Returns a cvar from C_cvars or NULL if there is none with the right name. 
cvar* C_Get(char* name);


#endif // CVAR_H