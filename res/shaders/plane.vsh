#version 330 core

layout(location = 0)in vec3 vertex_in;
layout(location = 1)in vec3 uv_in;

out vec3 uv;

void main() {
	uv = uv_in;
	gl_Position = vec4(vertex_in, 1);
}
