#version 400 core
//输入：
layout(location = 0) in vec3 in_position;//局部坐标
layout(location = 1) in vec3 in_normal;//法线
layout(location = 2) in vec2 in_texcoord;//纹理坐标
layout(location = 3) in float in_distance;//法线距离
//输出：
out vec3 Position_FS_in;//世界坐标
out vec3 Normal_FS_in;//法线
out vec2 Texcoord_FS_in;//纹理坐标
out float Distance_FS_in;//法线距离
//参数：
uniform mat4 gWorld;//世界变换矩阵
uniform mat4 gViewProjectMatrix;//视角投影变换矩阵
void main(void)
{
	Position_FS_in = (gWorld * vec4(in_position, 1.0)).xyz;//从局部坐标变换为世界坐标
	Normal_FS_in = (gWorld * vec4(in_normal, 0.0)).xyz;//法线进行相同变换
	Normal_FS_in = normalize(Normal_FS_in);//法线归一化
	Texcoord_FS_in = in_texcoord;//纹理坐标输出
	Distance_FS_in = in_distance;
	gl_Position = gViewProjectMatrix * gWorld * vec4(in_position, 1.0);//执行投影变换。
	//gl_Position是OpenGL shader中一个内建变量，用于保存投影变换后的坐标
}