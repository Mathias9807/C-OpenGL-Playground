#version 330 core

layout(location = 0)in vec3 vertex_in;
layout(location = 1)in vec3 uv_in;

out vec3 uv;
out vec4 vertex;

uniform mat4 matProj, matView;

void main() {
	uv = uv_in;
	vertex = vec4(vertex_in, 0);
	gl_Position = matProj * vec4((matView * vec4(vertex_in, 0)).xyz, 1);
}
