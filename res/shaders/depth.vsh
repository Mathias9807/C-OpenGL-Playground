#version 330 core

layout(location = 0)in vec3 vertex_in;

out vec4 vertex_p;

uniform mat4 matProj, matView, matModel;

void main() {
	vertex_p = matProj * matView * matModel * vec4(vertex_in, 1);
	gl_Position = vertex_p;
}
