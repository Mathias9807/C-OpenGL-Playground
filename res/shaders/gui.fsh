#version 330 core

layout(location = 0)out vec4 color_out;

in vec2 uv;

uniform sampler2D tex0;
uniform float alpha;

void main() {
	vec4 guiTex = texture(tex0, uv);
	color_out = vec4(mix(vec3(0), guiTex.rgb, guiTex.a), 0.5 * alpha);
}
