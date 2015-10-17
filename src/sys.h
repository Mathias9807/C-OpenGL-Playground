// sys.h - Generic system abstraction header file

#ifndef SYS_H
#define SYS_H


#include "def.h"

int		SYS_argc;
char**	SYS_argv;

uint32_t SYS_deltaMillis;

void SYS_GetResourcePath(char* name, char* dest);
void SYS_CheckErrors();
void SYS_Error(char* s);
void SYS_Warning(char* s);

// Video
int SYS_OpenWindow();
void SYS_UpdateWindow();
bool SYS_WindowClosed();
void SYS_CloseWindow();

uint32_t SYS_TimeMillis();
void SYS_Sleep(uint32_t millis);

bool SYS_HasParam(char* p);


#endif // SYS_H
