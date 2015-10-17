// sys_sdl.c - Platform independent functions

#include "sys.h"
#include "g_main.h"
#include "v_main.h"
#include "input.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <string.h>

#define RES_DIR "./res/"

int	Sys_argc = 0;
char** Sys_argv = NULL;

uint32_t Sys_deltaMillis;

SDL_Window* Sys_window;
SDL_GLContext Sys_glContext;

bool In_readingText = false;

void Sys_GetResourcePath(char* name, char* dest) {
	strcpy(dest, RES_DIR);
	strcat(dest, name);
}

void Sys_CheckErrors() {
//	const char* error = SDL_GetError();
//	if (*error) printf("SDL Error: %s\n", SDL_GetError());
	
	switch (glGetError()) {
		case GL_NO_ERROR: break;
		case GL_INVALID_ENUM: printf("OpenGL Error: GL_INVALID_ENUM\n"); break;
		case GL_INVALID_VALUE: printf("OpenGL Error: GL_INVALID_VALUE\n"); break;
		case GL_INVALID_OPERATION: printf("OpenGL Error: GL_INVALID_OPERATION\n"); break;
		case GL_STACK_OVERFLOW: printf("OpenGL Error: GL_STACK_OVERFLOW\n"); break;
		case GL_STACK_UNDERFLOW: printf("OpenGL Error: GL_STACK_UNDERFLOW\n"); break;
		case GL_OUT_OF_MEMORY: printf("OpenGL Error: GL_OUT_OF_MEMORY\n"); break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: printf("OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION\n"); break;
		case GL_CONTEXT_LOST: printf("OpenGL Error: GL_CONTEXT_LOST\n"); break;
		case GL_TABLE_TOO_LARGE: printf("OpenGL Error: GL_TABLE_TOO_LARGE\n"); break;
	}
}

void Sys_Error(char* s) {
	printf("Runtime error: %s\n", s);

	Sys_CloseWindow();
}

void Sys_Warning(char* s) {
	printf("Runtime warning: %s", s);
}

int Sys_OpenWindow() {
	if (SDL_Init(SDL_INIT_VIDEO)) return false;

	int attrResult = 0;
	attrResult += SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	attrResult += SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	attrResult += SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	if (attrResult < 0) return false;
	
	Sys_window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, V_WIDTH, V_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!Sys_window) return false;
	
	Sys_glContext = SDL_GL_CreateContext(Sys_window);
	if (!Sys_glContext) return false;

	SDL_GL_SetSwapInterval(1);
	
	Sys_CheckErrors();
	return true;
}

void Sys_UpdateWindow() {
	SDL_GL_SwapWindow(Sys_window);
}

bool Sys_WindowClosed() {
	return SDL_QuitRequested();
}

void Sys_CloseWindow() {
	SDL_Quit();
}

uint32_t Sys_TimeMillis() {
	return SDL_GetTicks();
}

void Sys_Sleep(uint32_t millis) {
	SDL_Delay(millis);
}

bool In_IsKeyPressed(int key) {
	if (In_readingText) return false;
	
	const Uint8* state = SDL_GetKeyboardState(NULL);
	switch (key) {
		case IN_W: return state[SDL_SCANCODE_W];
		case IN_A: return state[SDL_SCANCODE_A];
		case IN_S: return state[SDL_SCANCODE_S];
		case IN_D: return state[SDL_SCANCODE_D];
		case IN_LEFT: return state[SDL_SCANCODE_LEFT];
		case IN_RIGHT: return state[SDL_SCANCODE_RIGHT];
		case IN_PITCH_UP: return state[SDL_SCANCODE_UP];
		case IN_PITCH_DOWN: return state[SDL_SCANCODE_DOWN];
		case IN_UP: return state[SDL_SCANCODE_SPACE];
		case IN_DOWN: return state[SDL_SCANCODE_LSHIFT];
		case IN_ACTION: return state[SDL_SCANCODE_RETURN];
		case IN_RELOAD: return state[SDL_SCANCODE_R];
		case IN_TOGGLE: return state[SDL_SCANCODE_Q];
	}
	return false;
}


void In_StartTextInput() {
	if (In_readingText) return;
	
	SDL_StartTextInput();
	In_readingText = true;
}

void In_StopTextInput() {
	if (!In_readingText) return;
	
	SDL_StopTextInput();
	In_readingText = false;
}

void In_ReadTextInput(char* text, int length) {
	int curLength = 0;
	for (int i = 0; text[i];) 
		curLength = ++i;
	
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_TEXTINPUT: {
				char* input = event.text.text;
				for (int i = 0; input[i]; i++) {
					text[curLength + i] = input[i];
					text[curLength + i + 1] = 0;
				}
				
				break;
			}
			case SDL_KEYDOWN: 
				if (event.key.keysym.sym == SDLK_BACKSPACE && curLength >= 0) 
					text[--curLength] = 0;
				break;
		}
	}
}

bool Sys_HasParam(char* p) {
	for (int i = 0; i < Sys_argc; i++) {
		int j = 0;
		bool areEqual = true;
		while (p[j] && Sys_argv[i][j] && areEqual) {
			if (p[j] != Sys_argv[i][j])
				areEqual = false;
			j++;
		}
		if (areEqual) return true;
	}
	return false;
}

int main(int argc, char** argv) {
	Sys_argc = argc;
	Sys_argv = argv;
	
	if (!Sys_OpenWindow()) Sys_Error("Failed to open a window with OpenGL Core 3.3");

	G_Init();
	V_Init();
	
	uint32_t current = Sys_TimeMillis();
	uint32_t last = current;
	uint32_t secondTimer = 0, frames = 0;
	while (!Sys_WindowClosed()) {
		last = current;
		current = Sys_TimeMillis();
		Sys_deltaMillis = current - last;
		secondTimer += Sys_deltaMillis;
		frames++;
		
		if (secondTimer >= 1000) {
			secondTimer -= 1000;
			
			printf("\rFPS: %d", frames);
			fflush(stdout);
			frames = 0;
		}
		
		G_Tick();
		V_Tick();
		Sys_UpdateWindow();
	}
	printf("\n");
	
	G_Quit();
	V_Quit();
	return 0;
}
