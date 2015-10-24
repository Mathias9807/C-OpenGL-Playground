// cvar.c - Console and program wide variables

#include "cvar.h"
#include "sys.h"

list C_cvars = {NULL, 0};

void C_Print(char* s) {
	int strLength = 1;
	while (s[strLength - 1]) strLength++;
	if (strLength == 0) return;
	
	char* copiedStr = malloc(strLength);
	for (int i = 0; i < strLength; i++) 
		copiedStr[i] = s[i];
	copiedStr[strLength - 1] = 0;
	
	ListAdd(&C_console.history, copiedStr);
	C_console.lastActive = SYS_TimeMillis();
}

cvar* C_Add(cvar v) {
	cvar* existing = C_Get(v.name);
	if (existing) return existing;
	
	cvar* newVar = malloc(sizeof(cvar));
	
	int strLength = 0;	// Calculate length of name string
	while (v.name[strLength++]);
	
	newVar->name = malloc(strLength); // Copy name string
	for (int i = 0; i < strLength; i++) 
		newVar->name[i] = v.name[i];
	
	newVar->value = v.value; // Copy float value
	
	ListAdd(&C_cvars, newVar);
	return newVar;
}

void C_Set(char* name, float value) {
	cvar* v = C_Get(name);
	
	if (v) v->value = value;
}

cvar* C_Get(char* name) {
	listEntry* cur = C_cvars.first;
	
	while (cur) {
		cvar* v = (cvar*) cur->value;
		
		int i = 0;
		while (name[i]) {
			if (name[i] != v->name[i]) break;
			if ((int) name[i] == 0) return v;
			i++;
		}
		
		cur = cur->next;
	}
	
	return NULL;
}
