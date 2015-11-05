#version 330 core

layout(location = 0)out vec4 color_out;
layout(location = 1)out vec4 depth_out;

in vec3 uv, normal;
in vec4 vertex_w, vertex_c, vertex_p, vertex_shadow;
in mat3 matNormal;

uniform mat4				matProj, matView, matModel, matShadow;
uniform sampler2D			tex0, tex1, tex2, tex3;
uniform sampler2DShadow		texShadow;
uniform samplerCube			texSky;
uniform float				materialWeight, materialGloss;
uniform float				time;
uniform float				farPlane;
uniform vec3				bgColor, camPos, lightDir;
uniform int					terrain;
uniform int					lightNum;
uniform struct light_t {
	vec3 pos, col;
} lights[1];

void main() {
	vec3 normal_i = normalize(normal);
	vec3 reflectDir = -reflect(normalize(camPos - vertex_w.xyz), normal_i);
	float slope = 1 - normal_i.y;
	vec3 texDiff = mix(texture(tex0, uv.st).rgb, texture(tex3, uv.st).rgb, terrain * pow(slope * 2, 1.5));
	vec3 texSpec = texture(tex1, uv.st).rgb;

	float shadow = 0;
	vec2 shadowUV = vertex_shadow.st / 2 + 0.5;
	
	if (abs(shadowUV.s - 0.5) > 0.5 
		|| abs(shadowUV.t - 0.5) > 0.5 
		|| abs(vertex_shadow.z - 0.5) > 0.5) 
		shadow = 1;
	else {
		vec2 offs = 1.0 / textureSize(texShadow, 0);
		float bias = 0.002;
		shadow += texture(texShadow, vec3(shadowUV+vec2(offs.x, 0), vertex_shadow.z + bias));
		shadow += texture(texShadow, vec3(shadowUV+vec2(0, offs.y), vertex_shadow.z + bias));
		shadow += texture(texShadow, vec3(shadowUV+vec2(-offs.x, 0), vertex_shadow.z + bias));
		shadow += texture(texShadow, vec3(shadowUV+vec2(0, -offs.y), vertex_shadow.z + bias));
		shadow += texture(texShadow, vec3(shadowUV, vertex_shadow.z + bias));
		shadow /= 5;
	}
	
	vec3 intensity = vec3(0.1, 0.1, 0.15) * 3;
	vec3 light = 0.1 * intensity;
	
	float diffuse = clamp(dot(normal_i, lightDir), 0, 1);
	float indirectSpec = 0.25;
	float specular = (1 - indirectSpec) * pow(
		clamp(dot(reflectDir, lightDir), 0, 1), materialGloss
	);
	
	float weight = materialWeight;
	
	light += weight * (diffuse * texDiff * intensity * shadow);
	light += (1 - weight) * (specular * texSpec * intensity * shadow);
	light += (1 - weight) * (indirectSpec * texture(texSky, 
		reflectDir).rgb * texSpec * intensity * shadow);
	
	for (int i = 0; i < 1; i++) {
		float falloff = 1 / (1 + pow(length(lights[i].pos - vertex_w.xyz), 2));
		vec3 lDir = normalize(lights[i].pos - vertex_w.xyz);
		
		light += weight * falloff * lights[i].col * texDiff * clamp(
			dot(normal_i, lDir), 0, 1);
		
		light += (1 - weight) * falloff * texSpec * lights[i].col * pow(
			clamp(dot(lDir, reflectDir), 0, 1), materialGloss);
	}
	
	light = mix(light, texture(texSky, vertex_w.xyz - camPos).rgb, 
		clamp(-vertex_c.z / farPlane - 0.5, 0, 0.5) * 2);
	
	light /= light + 1;
	
	color_out = vec4(light, texture(tex0, uv.st).a);
	depth_out = vec4(vertex_p.z / farPlane);
}
