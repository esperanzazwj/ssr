#version 400 core
//输入：
in vec3 Position_FS_in;//世界坐标
in vec3 Normal_FS_in;//法线
in vec2 Texcoord_FS_in;//纹理坐标
//输出：
layout(location = 0) out vec4 FragColor;//颜色
layout(location = 1) out vec4 norm;
void main(void)
{
	FragColor = vec4(Position_FS_in, 1.0f);
	norm = vec4(Normal_FS_in,1.0f);
}