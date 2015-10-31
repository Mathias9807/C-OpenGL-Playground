// cvar.h - Console and program wide variables

#ifndef CVAR_H
#define CVAR_H


#include "def.h"

#define C_CONSOLE_LENGTH 48
#define C_CONSOLE_ROWS 8
#define C_CONSOLE_DISP 6
#define C_CONSOLE_FADEMS 4000
#define C_CONSOLE_BLINKMS 400

typedef struct {
	char 	text[C_CONSOLE_LENGTH]; // Input row
	list 	history; // Previously printed lines, newest last
	list	commandHistory; // Commands only
	
	int 	lastActive; // Time in ms when something was last printed
	
	bool 	inputActive; // If console is waiting for input
	
	int		selectedRow;	// What row from history is copied to input line
							// -1 is empty, 0 and up is copied from history
} console;
console C_console;
int C_cursorBlinkTimer;

typedef struct {
	char*	name;
	float	value;
	
	bool 	modified;
} cvar;

// Prints a string to the console
void C_Print(char* s);

// Prints a command to the console
void C_PrintCommand(char* s);

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
