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
	/*说明：
		一个Scene包含很多个子Mesh，每个Mesh有各自的材质，因此我们渲染整个场景时，
		先为每个Mesh查看其材质序号，然后我们根据序号，再材质集合（一个材质的map)中查找对应材质，
		最后调用SetMaterial设置材质，然后再调用Render进行渲染。
		关于SetMaterial函数，请跳转到对应代码出查看详细解释。
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