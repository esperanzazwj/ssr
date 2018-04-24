#version 400 core
//���룺
layout(location = 0) in vec3 in_position;//�ֲ�����
layout(location = 1) in vec3 in_normal;//����
layout(location = 2) in vec2 in_texcoord;//��������
layout(location = 3) in float in_distance;//���߾���
//�����
out vec3 Position_FS_in;//��������
out vec3 Normal_FS_in;//����
out vec2 Texcoord_FS_in;//��������
out float Distance_FS_in;//���߾���
//������
uniform mat4 gWorld;//����任����
uniform mat4 gViewProjectMatrix;//�ӽ�ͶӰ�任����
void main(void)
{
	Position_FS_in = (gWorld * vec4(in_position, 1.0)).xyz;//�Ӿֲ�����任Ϊ��������
	Normal_FS_in = (gWorld * vec4(in_normal, 0.0)).xyz;//���߽�����ͬ�任
	Normal_FS_in = normalize(Normal_FS_in);//���߹�һ��
	Texcoord_FS_in = in_texcoord;//�����������
	Distance_FS_in = in_distance;
	gl_Position = gViewProjectMatrix * gWorld * vec4(in_position, 1.0);//ִ��ͶӰ�任��
	//gl_Position��OpenGL shader��һ���ڽ����������ڱ���ͶӰ�任�������
}