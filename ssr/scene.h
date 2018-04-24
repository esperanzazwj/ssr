#ifndef _SCENE_H_
#define _SCENE_H_
#include "inc.h"
#include "Texture.h"
#include <vector>
#include <map>

using namespace std;
using namespace glm;

class Timer;
class Mesh
{
public:
	string Name;
	int MaterialIndex;
	vector<vec3> Vertices;
	vector<vec3> Normals;
	vector<vec2> TextureCoords;
	vector<vec3> Tangents;
	vector<vec3> Bitangents;
	vector<uvec3> Indices;
	vector<float> Distance;
private:
	GLuint VAO;
	GLuint Buffers[2];
	unsigned int count;
public:
	GLuint GPUVBO[3];
public:
	bool GenOGLMesh();
	bool GenOGLStructuredData();
	void Render();
};

class Material
{
public:
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Shininess;
	float Opacity;
	string diffuseTex;
	string specularTex;
public:
	Material(){}
	void SetMaterial(GLuint program, map<string, Texture2D*> &m_Textures);
};

class Scene
{
public:
	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
	map<string, Texture2D*> m_Textures;
private:
	Scene(){}
public:
	Scene(string Filename)
	{
		LoadFromFile(Filename);
		calcNormal();
		GenOGLMeshes();
	}
	~Scene()
	{
		//garbage collection
	}
	/*˵����
		һ��Scene�����ܶ����Mesh��ÿ��Mesh�и��ԵĲ��ʣ����������Ⱦ��������ʱ��
		��Ϊÿ��Mesh�鿴�������ţ�Ȼ�����Ǹ�����ţ��ٲ��ʼ��ϣ�һ�����ʵ�map)�в��Ҷ�Ӧ���ʣ�
		������SetMaterial���ò��ʣ�Ȼ���ٵ���Render������Ⱦ��
		����SetMaterial����������ת����Ӧ������鿴��ϸ���͡�
	*/
	void RenderScene(GLuint program)
	{
		int meshesNum = m_Meshes.size();
		for (int idx = 0; idx < meshesNum; idx++)
		{
			m_Materials[m_Meshes[idx].MaterialIndex].SetMaterial(program, m_Textures);
			m_Meshes[idx].Render();

		}
	}
	int GetMeshesNum()
	{
		return m_Meshes.size();
	}
	int GetMaterialsNum()
	{
		return m_Materials.size();
	}
	Mesh* GetMesh(int index)
	{
		return &m_Meshes[index];
	}
	Material* GetMaterial(int index)
	{
		return &m_Materials[index];
	}
	void SetMaterial(GLuint program, int index)
	{
		m_Materials[index].SetMaterial(program, m_Textures);
	}
private:
	bool LoadFromFile(string &Filename);
	void InitMesh(int, const aiMesh*);
	bool InitMaterial(const aiScene*);
	void calcNormal();
	bool GenOGLMeshes();
};

#endif