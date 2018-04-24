#version 430 core
layout(location = 0) in vec2 Position_VS_in;
layout(location = 1) in vec2 TexCoord_VS_in;

out vec2 TexCoord_FS_in;

void main()
{
	TexCoord_FS_in = TexCoord_VS_in;
	gl_Position = vec4(Position_VS_in, 0.0, 1.0);
}