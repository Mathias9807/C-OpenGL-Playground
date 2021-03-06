// level.c - Loads and writes levels

#include "level.h"
#include "sys.h"
#include <string.h>

level L_current = {
	"Default", 
	{NULL, 0}, 
	{NULL, 0}
};

char* buffer;
int bufferIndex = 0;

unsigned ReadUInt32B();
float ReadFloat32B();
unsigned ReadUInt32(FILE* file);
float ReadFloat32(FILE* file);
void WriteUInt32(FILE* file, unsigned value);
void WriteFloat32(FILE* file, float value);

// Creates an empty level
void L_InitLevel(char* name) {
	// Overwrite current level with zeroes
	memset(&L_current, 0, sizeof(level));
	
	// Figure out the length of the name
	int length = 0;
	while (name[length]) length++;
	
	// Write the name
	memcpy(L_current.name, name, length);
}

// Loads the level 'name' in the levels folder into L_current
// Throws a fatal error if 'name' can't be found
void L_LoadLevel(char* name) {
	// Start reading the file
	char* path = malloc(PATH_LENGTH);
	SYS_GetLevelPath(name, path);
	char levelPath[PATH_LENGTH];
	memcpy(levelPath, path, PATH_LENGTH);
	strcat(levelPath, "/level");
	
	FILE* file = fopen(levelPath, "rb");

	if (file == NULL) SYS_Error("Couldn't load level");

	// Clear the current level
	L_InitLevel(name);
	
	// Skip header
	fgetc(file);
	fgetc(file);
	
	int fileSize;
	fileSize = ReadUInt32(file); // Read size of file
	
	// Check if file actually exists
	if (fileSize == EOF) SYS_Error("Level doesn't exist!"); // A bit hacky
	
	// Allocate file buffer
	buffer = malloc(fileSize);
	bufferIndex = 6;
	
	// Read file into memory
	for (int i = bufferIndex; i < fileSize; i++) 
		buffer[i] = fgetc(file);

	fclose(file);
	
	int numRes = ReadUInt32B();
	int numObjects = ReadUInt32B();
	
	// Load resources
	for (int i = 0; i < numRes; i++) {
		resource* r = malloc(sizeof(resource));
		memset(r, 0, sizeof(resource));
		
		for (int j = 0; j < 32; j++) 
			r->name[j] = buffer[bufferIndex++];
		
		ListAdd(&L_current.res, (void*) r);
	}
	
	// Load props
	for (int i = 0; i < numObjects; i++) {
		int type = buffer[bufferIndex++];
		int start = ReadUInt32B();
		/*int length = */ ReadUInt32B();
		
		int lastIndex = bufferIndex;
		
		switch (type) {
		case 0: {
				prop* p = malloc(sizeof(prop));
				
				bufferIndex = start;
				p->res = ListGet(&L_current.res, ReadUInt32B());
				for (int i = 0; i < 3; i++) p->pos[i] = ReadFloat32B();
				for (int i = 0; i < 3; i++) p->rot[i] = ReadFloat32B();
				
				ListAdd(&L_current.props, (void*) p);
				
				break;
			}
		}
		
		bufferIndex = lastIndex;
	}
	
	free(buffer);
	bufferIndex = 0;
}

// Writes L_current to the file 'L_current.name' in the levels folder
void L_WriteLevel() {
	char* path = malloc(PATH_LENGTH);
	memset(path, 0, PATH_LENGTH);
	SYS_GetLevelPath(L_current.name, path);
	SYS_Mkdir(path);
	strcat(path, "/level");
	
	FILE* file = fopen(path, "wb");
	
	if (file == NULL) SYS_Error("Couldn't write level");
	
	// Print header
	fputs("LV", file);
	
	// Print size of file
	WriteUInt32(file, 14 + L_current.res.size * 32 + L_current.props.size * 9 + L_current.props.size * 28);
	
	// Number of resources
	WriteUInt32(file, L_current.res.size);
	
	// Number of objects
	WriteUInt32(file, L_current.props.size);
	
	// Print the name of the resource
	for (int i = 0; i < L_current.res.size; i++) {
		resource* r = ListGet(&L_current.res, i);
		for (int j = 0; j < L_NAME_LENGTH; j++) 
			fputc(r->name[j], file);
	}
	
	// Print object index table
	
	int objectIndex = L_HEADER_SIZE + L_RES_LENGTH * L_current.res.size + L_INDEX_SIZE * L_current.props.size;
	for (int i = 0; i < L_current.props.size; i++) {
		// Type of object
		fputc(0, file);
		
		// Starting index of object data
		WriteUInt32(file, objectIndex);
		objectIndex += 28;
		
		// Length of object data
		WriteUInt32(file, 28);
	}
	
	// Print prop data
	
	for (int i = 0; i < L_current.props.size; i++) {
		prop* p = (prop*) ListGet(&L_current.props, i);

		// Index of resource
		WriteUInt32(file, ListFind(&L_current.res, p->res));
		
		// Write position and rotation of prop
		WriteFloat32(file, p->pos[0]);
		WriteFloat32(file, p->pos[1]);
		WriteFloat32(file, p->pos[2]);
		
		WriteFloat32(file, p->rot[0]);
		WriteFloat32(file, p->rot[1]);
		WriteFloat32(file, p->rot[2]);
	}

	fclose(file);
}

// Add a resource to the level, not written until next L_WriteLevel call
resource* L_AddResource(char* name) {
	// Allocate space for the resource and set every byte to 0
	resource* r = malloc(sizeof(resource));
	memset(r, 0, sizeof(resource));
	
	// Figure out the length of the name
	int length = 0;
	while (name[length]) length++;
	
	// Copy name to the resource
	memcpy(r->name, name, length);
	
	// Add resource to the level
	ListAdd(&L_current.res, r);
	
	// Return the new resource
	return r;
}

// Add a prop to the level, not written until next L_WriteLevel call
prop* L_AddProp(resource* r, float* pos, float* rot) {
	// Allocate space for a prop
	prop* p = malloc(sizeof(prop));
	
	// Fill prop struct with zeroes
	memset(p, 0, sizeof(prop));
	
	// Add data to prop
	p->res = r;
	memcpy(p->pos, pos, 3 * sizeof(float));
	memcpy(p->rot, rot, 3 * sizeof(float));
	
	// Add prop to list
	ListAdd(&L_current.props, p);
	
	// Return pointer to new prop
	return p;
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
		array[i] = buffer[bufferIndex++];
	
	return *((unsigned*) array);
}

float ReadFloat32B() {
	char bytes[4];
	
	for (int i = 3; i >= 0; i--) 
		bytes[i] = buffer[bufferIndex++];
	
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

