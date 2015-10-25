// cvar.h - Console and program wide variables

#ifndef CVAR_H
#define CVAR_H


#include "def.h"

#define C_CONSOLE_LENGTH 48
#define C_CONSOLE_ROWS 8
#define C_CONSOLE_DISP 6
#define C_CONSOLE_FADEMS 4000

typedef struct {
	char 	text[C_CONSOLE_LENGTH];
	list 	history;
	
	int 	lastActive;
	
	bool 	inputActive;
} console;
console C_console;

typedef struct {
	char*	name;
	float	value;
	
	bool 	modified;
} cvar;

// Prints a string to the console
void C_Print(char* s);

// Executes a command and prints the result to the console
void C_Execute(char* s);

// Global list holding all cvars
list C_cvars;

// Adds cvar v to C_cvars, does nothing if v already exists
cvar* C_Add(char* name, float value);

// Sets the value of a cvar in C_cvars
void C_Set(char* name, float value);

// Returns a cvar from C_cvars or NULL if there is none with the right name
cvar* C_Get(char* name);


#endif // CVAR_H
