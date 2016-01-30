// sys_sdl.c - Platform independent functions

#include "sys.h"
#include "g_main.h"
#include "v_main.h"
#include "input.h"
#include "level.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

#define RES_DIR "./res/"
#define LVL_DIR "./levels/"

// Program arguments
int	SYS_argc = 0;
char** SYS_argv = NULL;

// 'Delta' time value in milliseconds
uint32_t SYS_deltaMillis;

// Window and GL context
SDL_Window* SYS_window;
SDL_GLContext SYS_glContext;

// If the program is listening for text input or key presses
bool IN_readingText = false;

// Returns a file path to the resource directory
void SYS_GetResourcePath(char* name, char* dest) {
	// Set string to be empty
	dest[0] = 0;
	
	// Check if argv[0] might contain the path
	if (SYS_argv[0][0] == '/' || SYS_argv[0][0] == '.') {
		// Add executable path to string, gotten from argv[0]
		strcat(dest, SYS_argv[0]);
		
		// Cut off string after last '/'
		int lastSlash = 0;
		for (int i = 0; dest[i]; i++) 
			if (dest[i] == '/') lastSlash = i;
		dest[lastSlash + 1] = 0;
	}
	
	strcat(dest, RES_DIR);
	strcat(dest, name);
}

// Get a file path to the level directory
void SYS_GetLevelPath(char* name, char* dest) {
	// Set string to be empty
	dest[0] = 0;
	
	// Check if argv[0] might contain the path
	if (SYS_argv[0][0] == '/' || SYS_argv[0][0] == '.') {
		// Add executable path to string, gotten from argv[0]
		strcat(dest, SYS_argv[0]);
		
		// Cut off string after last '/'
		int lastSlash = 0;
		for (int i = 0; dest[i]; i++) 
			if (dest[i] == '/') lastSlash = i;
		dest[lastSlash + 1] = 0;
	}
	
	strcat(dest, LVL_DIR);
	strcat(dest, name);
}

// Check if there are any errors
void SYS_CheckErrors() {
//	const char* error = SDL_GetError();
//	if (*error) printf("SDL Error: %s\n", SDL_GetError());
	
	// Decode OpenGL error codes
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

// Print fatal error
void SYS_Error(char* s) {
	printf("Runtime error: %s\n", s);

	if (SYS_CLOSE_ON_ERROR) 
		exit(0);
}

// Print non-fatal warning
void SYS_Warning(char* s) {
	printf("Runtime warning: %s\n", s);
}

// Create a directory
void SYS_Mkdir(char* s) {
#ifdef _WIN32
	CreateDirectory(s, NULL);
#else
	mkdir(s, 0711);
#endif
}

// Open and prepare a window for 3D accelerated rendering
int SYS_OpenWindow() {
	// Init the SDL video sub-system
	if (SDL_Init(SDL_INIT_VIDEO)) return false;

	// Request OpenGL version 3.3
	int attrResult = 0;
	attrResult += SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	attrResult += SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	attrResult += SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	if (attrResult < 0) return false;
	
	// Open window
	SYS_window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, V_WIDTH, V_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!SYS_window) return false;
	
	// Create an OpenGL context
	SYS_glContext = SDL_GL_CreateContext(SYS_window);
	if (!SYS_glContext) return false;

	V_SetVSync(true);
	
	// Check if anything broke
	SYS_CheckErrors();
	return true;
}

// Set the vertical sync
void V_SetVSync(bool vsync) {
	SDL_GL_SetSwapInterval(vsync);
}

// Swap video buffers
void SYS_UpdateWindow() {
	SDL_GL_SwapWindow(SYS_window);
}

// Check if window should close
bool SYS_WindowClosed() {
	return SDL_QuitRequested();
}

// Close window
void SYS_CloseWindow() {
	SDL_Quit();
}

// Get the current value of the millisecond clock
uint32_t SYS_TimeMillis() {
	return SDL_GetTicks();
}

// Put the thread to sleep for some amount of milliseconds
void SYS_Sleep(uint32_t millis) {
	SDL_Delay(millis);
}

// Check if key is pressed
bool IN_IsKeyPressed(int key) {
	if (IN_readingText) return false;
	
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
		case IN_CHAT: return state[SDL_SCANCODE_T];
	}
	return false;
}

// Start text input mode, disables key input
void IN_StartTextInput() {
	if (IN_readingText) return;
	
	SDL_StartTextInput();
	IN_readingText = true;
}

// Stop text input mode, enables key input
void IN_StopTextInput() {
	if (!IN_readingText) return;
	
	SDL_StopTextInput();
	IN_readingText = false;
}

// Read input into 'text'
int IN_ReadTextInput(char* text, int length) {
	SDL_Event event;
	if (!IN_readingText) {
		while (SDL_PollEvent(&event));
		return 0;
	}

	int curLength = 0;
	for (int i = 0; text[i];) 
		curLength = ++i;
	
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_TEXTINPUT: {
				char* input = event.text.text;
				for (int i = 0; input[i] && curLength + i < length - 1; i++) {
					text[curLength++ + i] = input[i];
					text[curLength + i] = 0;
				}
				
				// Make cursor blinking stop when typing
				C_cursorBlinkTimer = SYS_TimeMillis() + C_CONSOLE_BLINKMS;
				
				break;
			}
			case SDL_KEYDOWN: 
				if (event.key.keysym.sym == SDLK_BACKSPACE && curLength >= 0) 
					text[--curLength] = 0;
				if (event.key.keysym.sym == SDLK_RETURN && curLength < length) 
					text[curLength++] = '\n';
				if (event.key.keysym.sym == SDLK_TAB && curLength < length) 
					text[curLength++] = '\t';
				if (event.key.keysym.sym == SDLK_ESCAPE && curLength < length) 
					return -1;
				if (event.key.keysym.sym == SDLK_UP) {
					int listSize = ListSize(&C_console.commandHistory);
					if (C_console.selectedRow < listSize - 1) {
						C_console.selectedRow++;
						
						char* line = ListGet(&C_console.commandHistory, 
							listSize - C_console.selectedRow - 1);
						
						C_console.text[0] = 0;
						for (int i = 0; line[i]; i++) {
							C_console.text[i] = line[i];
							C_console.text[i + 1] = 0;
						}
					}
				}
				if (event.key.keysym.sym == SDLK_DOWN) {
					int listSize = ListSize(&C_console.commandHistory);
					if (C_console.selectedRow > -1) {
						C_console.selectedRow--;
						
						C_console.text[0] = 0;
						if (C_console.selectedRow > -1) {
							char* line = ListGet(&C_console.commandHistory, 
								listSize - C_console.selectedRow - 1);
							
							for (int i = 0; line[i]; i++) {
								C_console.text[i] = line[i];
								C_console.text[i + 1] = 0;
							}
						}
					}
				}
				
				C_cursorBlinkTimer = SYS_TimeMillis() + C_CONSOLE_BLINKMS;
				
				break;
		}
	}
	
	return 0;
}

// Print program usage
void PrintUsage() {
	printf("Usage: program [-l <level>]\n\nFlags: \n  -l <level>\tPrint level data\n");
}

// Check if program was called with a certain argument
bool SYS_HasParam(char* p) {
	for (int i = 0; i < SYS_argc; i++) {
		int j = 0;
		bool areEqual = true;
		while (p[j] && SYS_argv[i][j] && areEqual) {
			if (p[j] != SYS_argv[i][j])
				areEqual = false;
			j++;
		}
		if (areEqual) return true;
	}
	return false;
}

int main(int argc, char** argv) {
	SYS_argc = argc;
	SYS_argv = argv;

	// Check command line options
	if (SYS_HasParam("-l")) {
		if (argc != 3) {
			PrintUsage();
			return 0;
		}

		// Load the level and print its data
		L_LoadLevel(argv[argc - 1]);
		printf("Name: %s\n", L_current.name);
		printf("Contains %d resources and %d props\n", 
		L_current.res.size, L_current.props.size);
		
		// Print name of every resource
		printf("\nResources\n");
		for (int i = 0; i < L_current.res.size; i++) 
			printf("\t%d:\t%s\n", i, ((resource*)ListGet(&L_current.res, i))->name);
		
		// Print data of every prop
		printf("\nProps\n");
		for (int i = 0; i < L_current.props.size; i++) {
			prop* p = ListGet(&L_current.props, i);
			printf(
				"\t%d:\tResource: %s\n\t\tPos: %f, %f, %f\n\t\tRot: %f, %f, %f\n", 
				i, p->res->name, p->pos[0], p->pos[1], p->pos[2], 
				p->rot[0], p->rot[1], p->rot[2]
			);
		}

		return 0;
	}
	
	// Initialize console variables
	C_Init();
	
	// Try to open a window with rendering capabilities
	if (!SYS_OpenWindow()) SYS_Error("Failed to open a window with OpenGL Core 3.3");

	// Initialize OpenGL
	V_InitOpenGL();

	// Initialize main engine
	G_Init();
	
	// Initialize rendering system
	V_Init();
	
	// Main game loop
	uint32_t current = SYS_TimeMillis();
	uint32_t last = current;
	uint32_t secondTimer = 0, frames = 0;
	while (!SYS_WindowClosed()) {
		last = current;
		current = SYS_TimeMillis();
		SYS_deltaMillis = current - last;
		secondTimer += SYS_deltaMillis;
		frames++;
		
		if (secondTimer >= 1000) {
			secondTimer -= 1000;
			
			printf("\rFPS: %d", frames);
			fflush(stdout);
			frames = 0;
		}
		
		// Update game, then render to screen
		G_Tick();
		V_Tick();
		SYS_UpdateWindow();
	}
	printf("\n");
	
	G_Quit();
	V_Quit();
	return 0;
}
