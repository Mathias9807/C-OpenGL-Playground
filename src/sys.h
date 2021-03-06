// sys.h - Generic system abstraction header file

#ifndef SYS_H
#define SYS_H


#include "def.h"

#define SYS_CLOSE_ON_ERROR 1

extern int		SYS_argc;
extern char**		SYS_argv;

extern uint32_t SYS_deltaMillis;

void SYS_GetResourcePath(const char* name, char* dest);
void SYS_GetLevelPath(const char* name, char* dest);
void SYS_CheckErrors();
void SYS_Error(char* s);
void SYS_Warning(char* s);
void SYS_Mkdir(char* s);
bool SYS_DirExists(char* s);

// Video
int SYS_OpenWindow();
void SYS_UpdateWindow();
bool SYS_WindowClosed();
void SYS_CloseWindow();

uint32_t SYS_TimeMillis();
void SYS_Sleep(uint32_t millis);

bool SYS_HasParam(char* p);


#endif // SYS_H
