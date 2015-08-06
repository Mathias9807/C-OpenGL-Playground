#version 330 core

layout(location = 0)out vec4 color_out;

in vec3 uv;

uniform int w, h;
uniform sampler2D tex0, shadow;
uniform vec2 dir;
uniform float farPlane;
uniform vec3 modColor;

int maxRadius = 3;
float blurScale = 2, blurStartDist = farPlane / 4 * 3, 
	blurLength = farPlane - blurStartDist, 
	nearBlurEnd = 0.3;

void main() {
	float rawDepth = texture(tex0, uv.st).a;
	float fragDepth = rawDepth * farPlane;
	vec2 deltaDir = dir / vec2(w, h) * blurScale;
	
	vec4 blur = vec4(texture(tex0, uv.st));
	float strength = 1;
	vec2 uvOffs = vec2(0);
	float radius = max(fragDepth - blurStartDist, 0) / blurLength * maxRadius;
	for (float i = -radius; i <= radius; i++) {
		uvOffs = uv.st + i * deltaDir;
		float depthSample = texture(tex0, uvOffs).a * farPlane;
		if (depthSample < blurStartDist) continue;
		float localStrength = 1 / (1 + i * i);
		blur += vec4(texture(tex0, uvOffs)) * localStrength;
		strength += localStrength;
	}
	/*float nearRadius = max(1 - fragDepth / nearBlurEnd, 0) * maxRadius;
	for (float i = -nearRadius; i <= nearRadius; i++) {
		uvOffs = uv.st + i * deltaDir;
		float depthSample = texture(depth, uvOffs).r * farPlane;
		if (depthSample > nearBlurEnd) continue;
		float localStrength = 1 / (1 + i * i);
		blur += vec4(texture(tex0, uvOffs)) * localStrength;
		strength += localStrength;
	}*/
	blur /= strength;
	blur *= vec4(modColor, 1);
	
	color_out = vec4(blur.rgb, rawDepth);
}
