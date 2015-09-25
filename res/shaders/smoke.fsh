#version 330 core

layout(location = 0)out vec4 color_out;

in vec3 uv, normal;
in vec4 vertex_w, vertex_c, vertex_p; // vertex_shadow;
// in mat3 matNormal;

uniform mat4 matProj, matView, matModel; // matShadow;
uniform sampler2D			tex0; // tex2;
uniform sampler2D			fbo0, fbo1;
// uniform sampler2DShadow		texShadow;
uniform samplerCube			texSky;
// uniform float				time;
uniform float				farPlane;
uniform vec3				bgColor, camPos, lightDir;
uniform struct light_t {
	vec3 pos, col;
	bool directional;
} lights;

void main() {
	// vec3 normal_i = normalize((matModel * vec4(matNormal * (texture(tex2, uv.st).xyz * 2 - 1), 0)).xyz);
	vec3 normal_i = normal;
	vec3 texDiff = texture(tex0, uv.st).rgb;
	// vec3 texSpec = texture(tex1, uv.st).rgb;
	
	/*float shadow = 0;
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
	}*/
	
	vec3 light = vec3(0.2, 0.2, 0.4) * texDiff;
	
	vec3 diffuse = vec3(2, 2, 1.5) * max(dot(normal_i, lightDir), 0); // * shadow;
	
	//diffuse += lights.col;
	
	light += diffuse * texDiff;
	
	light /= 2;
	/*light = mix(light, texture(texSky, vertex_w.xyz - camPos).rgb, 
				clamp(-vertex_c.z / farPlane - 0.5, 0, 0.5) * 2);*/
	
	color_out = vec4(light, texture(tex0, uv.st).a);
	// Shader doesn't write to the depth attachment
}
