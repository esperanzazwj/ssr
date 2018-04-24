#version 400 core
layout(location = 0) in vec3 in_position;
uniform mat4 gWorld;
uniform mat4 gViewProjectMatrix;
out vec3 tex_coord;
void main(void)
{
	tex_coord = in_position;
	vec4 tmp = gViewProjectMatrix * vec4(in_position, 1.0);
	gl_Position = vec4(tmp.xy,tmp.w-0.001,tmp.w);
}