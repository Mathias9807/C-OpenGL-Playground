#version 330 core

layout(location = 0)in vec3 vertex_in;
layout(location = 1)in vec3 uv_in;

out vec2 uv;

uniform vec2 screenSize;
uniform sampler2D tex0;
uniform vec2 pos, subPos, scale, subSize;

void main() {
	vec2 texSize = textureSize(tex0, 0);
	vec2 vertex = (vec2(vertex_in.x, -vertex_in.y) / 2 + 0.5) * subSize / screenSize;
	
	uv = (uv_in.st) / texSize * subSize + subPos;
	vertex = vertex + pos / screenSize;
	
	vertex = vertex * 2 - 1;
	gl_Position = vec4(vertex.x, -vertex.y, 0, 1);
}
