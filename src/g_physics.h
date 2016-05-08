// g_physics.h - Geometry and physics functions and types

#ifndef G_PHYSICS_H
#define G_PHYSICS_H


#include "def.h"
#include "v_model.h"
#include <linmath.h>

enum {
	G_AABB_TYPE,
	G_CYLINDER_TYPE, 
	G_MESH_TYPE, 
	G_POINT_TYPE, 
	G_HEIGHTMAP_TYPE
};

// Geometry type definitions
typedef struct {
	int type;
	double x, y, z, w, h, d;
} AABB;

typedef struct {
	int type;
	double x, z, r;
} cylinder;

typedef struct {
	int type;
	void* points;
	void* indices;
	int numPoints;
} mesh;

// Point physics object
typedef struct {
	int type;
	vec3 p;
	vec3 v;
} point;

typedef struct {
	int type;
	model_t* m;
	vec3 pos;
	int size, res;
} heightmap;

typedef struct {
	void* shape;
	point pos;
} object;

// Geometry collision functions
bool G_ContainsAABB(AABB t, vec3 v);
bool G_ContainsCylinder(cylinder t, vec3 v);
bool G_ContainsMesh(mesh t, vec3 v);
bool G_ContainsHeightMap(heightmap h, vec3 v);
bool G_CollidingAABB(AABB a, AABB b);
bool G_CollidingCylinder(cylinder a, cylinder b);
bool G_CollidingMesh(mesh a, mesh b);
bool G_RayHitsAABB(AABB t, vec3 p, vec3 d);
bool G_RayHitsCylinder(cylinder t, vec3 p, vec3 d);
bool G_RayHitsMesh(mesh t, vec3 p, vec3 d);

float* G_GetVertex(int i);

void G_TickPointPhysics(point* p, vec3 a);


#endif // G_PHYSICS_H
