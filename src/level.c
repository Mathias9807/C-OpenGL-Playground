// level.c - Loads and writes levels

#include "level.h"
#include "sys.h"
#include <string.h>

level L_current = {
	"Default", 
	(list) {NULL, 0}, 
	(list) {NULL, 0}
};

char* buffer;
int index = 0;

unsigned ReadUInt32B();
float ReadFloat32B();
unsigned ReadUInt32(FILE* file);
float ReadFloat32(FILE* file);
void WriteUInt32(FILE* file, unsigned value);
void WriteFloat32(FILE* file, float value);

// Loads the level 'name' in the levels folder into L_current
// Throws a fatal error if 'name' can't be found
void L_LoadLevel(char* name) {
	// Start reading the file
	char* path = malloc(32);
	SYS_GetLevelPath(name, path);
	strcat(path, "/level");
	
	FILE* file = fopen(path, "rb");

	if (file == NULL) SYS_Error("Couldn't load level");

	{
		// Set every char to NULL
		int i = 0;
		for (; i < L_NAME_LENGTH; i++) 
			L_current.name[i] = 0;
		
		// Write 'name' to L_current.name
		i = -1;
		while (name[++i] != 0)
			L_current.name[i] = name[i];
	}
	
	// Reset lists
	L_current.res = (list) {NULL, 0};
	L_current.props = (list) {NULL, 0};
	
	// Skip header
	fgetc(file);
	fgetc(file);
	
	int fileSize = ReadUInt32(file); // Read size of file
	
	// Allocate file buffer
	buffer = malloc(fileSize + 6);
	index = 6;
	
	// Read file into memory
	for (int i = index; i < fileSize; i++) 
		buffer[i] = fgetc(file);
	
	int numRes = ReadUInt32B();
	int numObjects = ReadUInt32B();
	
	// Load resources
	for (int i = 0; i < numRes; i++) {
		resource* r = malloc(sizeof(resource));
		
		for (int j = 0; j < 32; j++) 
			r->name[j] = buffer[index++];
		
		ListAdd(&L_current.res, (void*) r);
	}
	
	// Load props
	for (int i = 0; i < numObjects; i++) {
		int type = buffer[index++];
		int start = ReadUInt32B();
		/*int length = */ ReadUInt32B();
		
		int lastIndex = index;
		
		switch (type) {
		case 0: {
				prop* p = malloc(sizeof(prop));
				
				index = start;
				p->res = ListGet(&L_current.res, ReadUInt32B());
				for (int i = 0; i < 3; i++) p->pos[i] = ReadFloat32B();
				for (int i = 0; i < 3; i++) p->rot[i] = ReadFloat32B();
				
				ListAdd(&L_current.props, (void*) p);
				
				break;
			}
		}
		
		index = lastIndex;
	}
}

// Writes L_current to the file 'L_current.name' in the levels folder
void L_WriteLevel() {
	char* path = malloc(32);
	SYS_GetLevelPath(L_current.name, path);
	SYS_Mkdir(path);
	strcat(path, "/level");
	
	FILE* file = fopen(path, "wb");
	
	// Print header
	fputs("LV", file);
	
	// Print size of file
	WriteUInt32(file, 14 + L_current.res.size * 32 + L_current.props.size * 9 + ListSize(&L_current.props) * 28);
	
	// Number of resources
	WriteUInt32(file, L_current.res.size);
	
	// Number of objects
	WriteUInt32(file, L_current.props.size);
	
	// Print the name of the resource
	char* resName = "suzanne";
	int i;
	for (i = 0; resName[i]; i++) fputc(resName[i], file);
	while (i++ < L_RES_LENGTH) 
		fputc(0, file);
	
	// Print object index table
	
	int objectIndex = L_HEADER_SIZE + L_RES_LENGTH * L_current.res.size + L_INDEX_SIZE * L_current.props.size;
	for (i = 0; i < L_current.props.size; i++) {
		// Type of object
		fputc(0, file);
		
		// Starting index of object data
		WriteUInt32(file, objectIndex);
		objectIndex += 28;
		
		// Length of object data
		WriteUInt32(file, 28);
	}
	
	// Print prop data
	
	for (i = 0; i < L_current.props.size; i++) {
		// Index of resource
		WriteUInt32(file, 0);
		
		prop* p = (prop*) ListGet(&L_current.props, i);
		
		// Write position and rotation of prop
		WriteFloat32(file, p->pos[0]);
		WriteFloat32(file, p->pos[1]);
		WriteFloat32(file, p->pos[2]);
		
		WriteFloat32(file, p->rot[0]);
		WriteFloat32(file, p->rot[1]);
		WriteFloat32(file, p->rot[2]);
	}
}

unsigned ReadUInt32(FILE* file) {
	char array[4];
	for (int i = 3; i >= 0; i--) 
		array[i] = fgetc(file);
	
	return *((unsigned*) array);
}

unsigned ReadUInt32B() {
	char array[4];
	for (int i = 3; i >= 0; i--) 
		array[i] = buffer[index++];
	
	return *((unsigned*) array);
}

float ReadFloat32B() {
	char bytes[4];
	
	for (int i = 3; i >= 0; i--) 
		bytes[i] = buffer[index++];
	
	return *((float*) bytes);
}

void WriteUInt32(FILE* file, unsigned value) {
	for (int i = 3; i >= 0; i--) 
		fputc((char) (value >> 8 * i), file);
}

void WriteFloat32(FILE* file, float value) {
	char* bytes = (char*) &value;
	
	for (int i = 3; i >= 0; i--) 
		fputc(bytes[i], file);
}

