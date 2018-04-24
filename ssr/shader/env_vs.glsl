#version 430 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
centroid out vec3 Position_FS_in;
centroid out vec3 Normal_FS_in;
centroid out vec2 Texcoord_FS_in;
uniform mat4 gWorld;
uniform mat4 gViewProjectMatrix;
void main(void)
{
	Position_FS_in = (gWorld * vec4(in_position, 1.0)).xyz;
	Normal_FS_in = (gWorld * vec4(in_normal, 0.0)).xyz;
	Normal_FS_in = normalize(Normal_FS_in);
	Texcoord_FS_in = in_texcoord;
	gl_Position = gViewProjectMatrix * gWorld * vec4(in_position, 1.0);
}