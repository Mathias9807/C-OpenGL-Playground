#version 330 core

layout(location = 0)in vec3 vertex_in;

out vec4 vertex_p;

uniform mat4 matProj, matModel;
uniform vec3 shadowOffs;
uniform float shadowDim;

void main() {
	vertex_p = matProj * matModel * vec4(vertex_in, 1);
	vec4 vertex_offs = matProj * vec4(shadowOffs, 0);
	vertex_p.xyz += floor(vertex_offs.xyz * shadowDim / 2) / shadowDim * 2;
	gl_Position = vertex_p;
}
