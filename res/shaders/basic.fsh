#version 330 core

layout(location = 0)out vec4 color_out;

in vec3 uv, normal;
in vec4 vertex_w, vertex_c, vertex_p, vertex_shadow;

uniform sampler2D	tex0, tex1, texShadow;
uniform samplerCube texSky;
uniform float		time;
uniform float		farPlane;
uniform vec3		bgColor, camPos;

void main() {
	vec3 normal_i = normalize(normal);
	vec3 lightDir = normalize(vec3(1, 1, 1));
	vec3 spec = texture(tex1, uv.st).rgb;
	
	float shadow = 1;
	//if (abs(vertex_shadow.z) < texture(texShadow, vertex_shadow.st).r) 
	//	shadow /= 2;
	
	vec3 light = vec3(0.2);
	light += texture(tex0, uv.st).rgb * max(dot(normal_i, lightDir), 0);
	light += spec * 3 * pow(
		max(dot(
			reflect(
				normalize(camPos - vertex_w.xyz), 
				normal_i), 
			-lightDir), 
		0), 
	60);
	light += spec * texture(texSky, 
		-reflect(normalize(camPos - vertex_w.xyz), normal_i)).xyz;
	
	light /= 1.5;
	light = mix(light, texture(texSky, vertex_w.xyz - camPos).rgb, 
				clamp(-vertex_c.z / farPlane - 0.5, 0, 0.5) * 2);
	
	color_out = vec4(light * shadow, vertex_p.z / farPlane);
}
