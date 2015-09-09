// g_collision.h - Geometry functions and types

#ifndef G_COLLISION_H
#define G_COLLISION_H


#include "def.h"
#include <linmath.h>

// Geometry type definitions
typedef struct {
	double x, y, z, w, h, d;
} AABB;

typedef struct {
	double x, z, r;
} cylinder;

typedef struct {
	void* points;
	void* indices;
	int numPoints;
} mesh;

// Geometry collision functions
bool G_ContainsAABB(AABB t, vec3 v);
bool G_ContainsCylinder(cylinder t, vec3 v);
bool G_ContainsMesh(mesh t, vec3 v);
bool G_CollidingAABB(AABB a, AABB b);
bool G_CollidingCylinder(cylinder a, cylinder b);
bool G_CollidingMesh(mesh a, mesh b);
bool G_RayHitsAABB(AABB t, vec3 p, vec3 d);
bool G_RayHitsCylinder(cylinder t, vec3 p, vec3 d);
bool G_RayHitsMesh(mesh t, vec3 p, vec3 d);

float* G_GetVertex(int i);


#endif // G_COLLISION_H
