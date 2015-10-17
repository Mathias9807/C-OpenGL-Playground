// g_physics.h - Geometry and physics functions and types

#include "g_physics.h"
#include "sys.h"

float vec3_dot(vec3 a, vec3 b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

float max(float a, float b) {
	return a > b ? a : b;
}

bool G_ContainsAABB(AABB t, vec3 v) {
	return (v[0] >= t.x && v[0] <= t.x + t.w)
			&& (v[1] >= t.y && v[1] <= t.y + t.h)
			&& (v[2] >= t.z && v[2] <= t.z + t.d);
}

bool G_CollidingAABB(AABB a, AABB b) {
	return (a.x <= b.x + b.w && a.x + a.w >= b.x)
			&& (a.y <= b.y + b.h && a.y + a.h >= b.y)
			&& (a.z <= b.z + b.d && a.z + a.d >= b.z);
}

bool G_RayHitsAABB(AABB t, vec3 p, vec3 dir) {
	vec3 d;
	vec3_norm(d, dir);
	float l = 0; // Length of ray
	
	// Get point where ray intersects AABB planes and compare it to the box
	if (d[0] != 0) {
		l = ((t.x - p[0]) / d[0]);
		vec3 test = {p[0] + l * d[0], p[1] + l * d[1], p[2] + l * d[2]};
		if (G_ContainsAABB(t, test)) return true;
	}
	if (d[0] != 0) {
		l = max((t.x + t.w - p[0]) / d[0], 0);
		vec3 test = {p[0] + l * d[0], p[1] + l * d[1], p[2] + l * d[2]};
		if (G_ContainsAABB(t, test)) return true;
	}
	if (d[1] != 0) {
		l = max((t.y - p[1]) / d[1], 0);
		vec3 test = {p[0] + l * d[0], p[1] + l * d[1], p[2] + l * d[2]};
		if (G_ContainsAABB(t, test)) return true;
	}
	if (d[1] != 0) {
		l = max((t.y + t.h - p[1]) / d[1], 0);
		vec3 test = {p[0] + l * d[0], p[1] + l * d[1], p[2] + l * d[2]};
		if (G_ContainsAABB(t, test)) return true;
	}
	if (d[2] != 0) {
		l = max((t.z - p[2]) / d[2], 0);
		vec3 test = {p[0] + l * d[0], p[1] + l * d[1], p[2] + l * d[2]};
		if (G_ContainsAABB(t, test)) return true;
	}
	if (d[2] != 0) {
		l = max((t.z + t.d - p[2]) / d[2], 0);
		vec3 test = {p[0] + l * d[0], p[1] + l * d[1], p[2] + l * d[2]};
		if (G_ContainsAABB(t, test)) return true;
	}
	
	return false;
}

void G_TickPointPhysics(point* p, vec3 a) {
	float t = SYS_deltaMillis / 1000.0;
	for (int i = 0; i < 3; i++) {
		p->p[i] += p->v[i] * t + a[i] * t * t / 2;
		p->v[i] += a[i] * t;
	}
}
