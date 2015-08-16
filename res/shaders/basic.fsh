#version 330 core

layout(location = 0)out vec4 color_out;

in vec3 uv, normal;
in vec4 vertex_w, vertex_c, vertex_p, vertex_shadow;
in mat3 matNormal;

uniform mat4 matProj, matView, matModel, matShadow;
uniform sampler2D	tex0, tex1, tex2, texShadow;
uniform samplerCube texSky;
uniform float		time;
uniform float		farPlane;
uniform vec3		bgColor, camPos, lightDir;

void main() {
	vec3 normal_i = normalize((matModel * vec4(matNormal * (texture(tex2, uv.st).xyz * 2 - 1), 0)).xyz);
	
	float shadow = 1;
	vec2 shadowUV = vertex_shadow.st / 2 + 0.5;
	if (vertex_shadow.z - 0.001 > texture(texShadow, shadowUV).r) 
		shadow = 0;
	if (abs(shadowUV.s - 0.5) > 0.5 
		|| abs(shadowUV.t - 0.5) > 0.5 
		|| abs(vertex_shadow.z - 0.5) > 0.5) 
		shadow = 1;
	
	vec3 light = vec3(0.2) * texture(tex0, uv.st).rgb;
	
	vec3 diffuse = vec3(1) * max(dot(normal_i, lightDir), 0) * shadow;
	
	vec3 specular = vec3(3) * pow(
		max(dot(
			reflect(
				normalize(camPos - vertex_w.xyz), 
				normal_i), 
			-lightDir), 
		0), 
	60) * shadow;
	specular += texture(texSky, 
		-reflect(normalize(camPos - vertex_w.xyz), normal_i)).xyz;
	
	light += diffuse * texture(tex0, uv.st).rgb 
		+ specular * texture(tex1, uv.st).rgb;
	
	light /= 1.5;
	light = mix(light, texture(texSky, vertex_w.xyz - camPos).rgb, 
				clamp(-vertex_c.z / farPlane - 0.5, 0, 0.5) * 2);
	
	color_out = vec4(light, vertex_p.z / farPlane);
}
