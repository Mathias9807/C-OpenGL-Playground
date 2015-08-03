#version 330 core

layout(location = 0)out vec4 color_out;

in vec3 uv;
in vec4 vertex;

uniform samplerCube tex;

void main() {
	color_out = vec4(texture(tex, vertex.xyz).rgb, 1);
}
