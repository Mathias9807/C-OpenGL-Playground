#version 330 core

layout(location = 0)in vec3 vertex_in;
layout(location = 1)in vec3 uv_in;
layout(location = 2)in vec3 normal_in;
layout(location = 3)in vec2 weights_in;

out vec3 uv, normal;
out vec4 vertex_w, vertex_c, vertex_p, vertex_shadow;

uniform mat4 matProj, matView, matModel, matShadow;
uniform mat4 bindPose[2];
uniform mat4 bones[2];

void main() {
	uv = uv_in;
	normal = (matModel * vec4(normal_in, 0)).xyz;
//	vec4 vert = vec4(vertex_in.x, vertex_in.y, vertex_in.z, 1);
//	vec4 vertex_bone = ((bindPose[0]) * bones[0] * vert) * weights_in.x;
//	vertex_bone += ((bindPose[1]) * bones[1] * vert) * weights_in.y;
	vertex_w = matModel * vec4(vertex_in, 1);
	vertex_c = matView * vertex_w;
	vertex_p = matProj * vertex_c;
	vertex_shadow = matShadow * vertex_w;
	gl_Position = vertex_p;
}
