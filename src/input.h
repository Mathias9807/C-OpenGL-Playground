// input.h - Various input declarations

#ifndef INPUT_H
#define INPUT_H


enum {
	IN_W, IN_A, IN_S, IN_D, IN_LEFT, IN_RIGHT, 
	IN_PITCH_UP, IN_PITCH_DOWN, IN_UP, IN_DOWN, IN_ACTION, IN_RELOAD, 
	IN_TOGGLE, IN_CHAT
};
bool IN_readingText;

bool IN_IsKeyPressed(int key);

void IN_StartTextInput();
void IN_StopTextInput();
int IN_ReadTextInput(char* text, int length);


#endif // INPUT_H
