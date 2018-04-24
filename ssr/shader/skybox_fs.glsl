#version 400 core
in vec3 tex_coord;
layout(location = 0) out vec4 color;
uniform samplerCube skybox;
void main(void)
{
	color = texture(skybox, tex_coord);
}