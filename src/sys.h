// sys.h - Generic system abstraction header file

#ifndef SYS_H
#define SYS_H


#include "def.h"

int		Sys_argc;
char**	Sys_argv;

uint32_t Sys_deltaMillis;

void Sys_GetResourcePath(char* name, char* dest);
void Sys_CheckErrors();

// Video
void Sys_OpenWindow();
void Sys_UpdateWindow();
bool Sys_WindowClosed();
void Sys_CloseWindow();

uint32_t Sys_TimeMillis();
void Sys_Sleep(uint32_t millis);

bool Sys_HasParam(char* p);


#endif // SYS_H
