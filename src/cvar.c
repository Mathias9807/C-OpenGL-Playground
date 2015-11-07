// cvar.c - Console and program wide variables

#include "cvar.h"
#include "sys.h"

int C_cursorBlinkTimer = 0;
list C_cvars = {NULL, 0};

console C_console;

void C_Init() {
	// Initialize to default values
	C_Add("shadowDim",		512);
	C_Add("shadowSize",		40);
	C_Add("fov",			65);
	C_Add("adsFov",			40);
	
	// Apply values from program arguments
}

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

void C_PrintCommand(char* s) {
	int strLength = 1;
	while (s[strLength - 1]) strLength++;
	if (strLength == 0) return;
	
	char* copiedStr = malloc(strLength);
	for (int i = 0; i < strLength; i++) 
		copiedStr[i] = s[i];
	copiedStr[strLength - 1] = 0;
	
	ListAdd(&C_console.commandHistory, copiedStr);
	ListAdd(&C_console.history, copiedStr);
	C_console.lastActive = SYS_TimeMillis();
}

void C_Execute(char* s) {
	int tokens = (s[0] == ' ' && s[0]) ? 0 : 1;
	int tokenIndices[5]; // Start index of every token
						 // Max 5 tokens
	for (int i = 0; i < 5; i++) tokenIndices[i] = 0;
	
	// Count how many whitespace separated tokens exists
	for (int i = 0; s[i]; i++) {
		if (s[i] == ' ') {
			while (s[i] == ' ') i++;
			if (s[i]) {
				tokenIndices[tokens] = i;
				tokens++;
			}
		}
	}
	
	switch (tokens) {
	case 0: return;
	case 1: {
		char t[32];
		for (int i = tokenIndices[0]; s[i] != ' '; i++) {
			if (i - tokenIndices[0] + 1 >= 31) break;
			t[i - tokenIndices[0]] = s[i];
			t[i - tokenIndices[0] + 1] = 0;
		}
		
		cvar* v = C_Get(t);
		if (v == NULL) {
			C_Print("No such variable exists :(");
			return;
		}
		
		char fStr[32];
		sprintf(fStr, "%f", v->value);
		C_Print(fStr);
		
		break;
	}
	case 2: {
		char t0[32], t1[16];
		for (int i = tokenIndices[0]; s[i] != ' '; i++) {
			if (i - tokenIndices[0] + 1 >= 31) break;
			t0[i - tokenIndices[0]] = s[i];
			t0[i - tokenIndices[0] + 1] = 0;
		}
		
		for (int i = tokenIndices[1]; s[i] != ' '; i++) {
			if (i - tokenIndices[1] + 1 >= 15) break;
			t1[i - tokenIndices[1]] = s[i];
			t1[i - tokenIndices[1] + 1] = 0;
		}
		
		cvar* v = C_Get(t0);
		if (v == NULL) {
			C_Print("No such variable exists :(");
			return;
		}
		
		v->value = atof(t1);
		v->modified = true;
		
		char fStr[32];
		sprintf(fStr, "%s = %f", v->name, v->value);
		C_Print(fStr);
		
		break;
	}
	}
}

cvar* C_Add(char* name, float value) {
	cvar* existing = C_Get(name);
	if (existing) return existing;
	
	cvar* newVar = malloc(sizeof(cvar));
	
	int strLength = 0;	// Calculate length of name string
	while (name[strLength++]);
	
	newVar->name = malloc(strLength); // Copy name string
	for (int i = 0; i < strLength; i++) 
		newVar->name[i] = name[i];
	
	newVar->value = value; // Copy float value
	
	newVar->modified = false;
	
	ListAdd(&C_cvars, newVar);
	return newVar;
}

void C_Set(char* name, float value) {
	cvar* v = C_Get(name);
	
	if (v) {
		v->value = value;
		v->modified = true;
	}
}

cvar* C_Get(char* name) {
	listEntry* cur = C_cvars.first;
	
	while (cur) {
		cvar* v = (cvar*) cur->value;
		
		int i = 0;
		while (name[i]) {
			if (name[i] != v->name[i]) break;
			if ((int) name[i + 1] == 0) return v;
			i++;
		}
		
		cur = cur->next;
	}
	
	return NULL;
}
