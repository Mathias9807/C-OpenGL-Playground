#version 330 core

in vec4 vertex_p;

uniform float farPlane;

void main() {
	gl_FragDepth = vertex_p.z / farPlane;
}
