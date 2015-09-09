// input.h - Various input declarations

#ifndef INPUT_H
#define INPUT_H


enum {
	IN_W, IN_A, IN_S, IN_D, IN_LEFT, IN_RIGHT, 
	IN_PITCH_UP, IN_PITCH_DOWN, IN_UP, IN_DOWN, IN_ACTION
};

bool In_IsKeyPressed(int key);


#endif // INPUT_H
