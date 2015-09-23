#version 330 core

layout(location = 0)in vec3 vertex_in;
layout(location = 1)in vec3 uv_in;
layout(location = 2)in vec3 normal_in;

out vec3 uv, normal;
out vec4 vertex_w, vertex_c, vertex_p; // vertex_shadow;
// out mat3 matNormal;

uniform mat4 matProj, matView, matModel; // matShadow;

void main() {
	uv = uv_in;
	
	/*matNormal = mat3(
		tangent_in, 
		cross(normal_in, tangent_in), 
		normal_in
	);
	normal = (matModel * vec4(matNormal * vec3(0, 0, 1), 0)).xyz;*/
	normal = normal_in;
	
	vertex_w = matModel * vec4(0, 0, 0, 1);
	vertex_c = matView * vertex_w + vec4(vertex_in, 0);
	vertex_p = matProj * vertex_c;
//	vertex_shadow = matShadow * vertex_w;
	gl_Position = vertex_p;
}
