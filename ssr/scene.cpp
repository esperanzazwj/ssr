#include <iostream>
#include "inc.h"
#include "scene.h"

bool Mesh::GenOGLMesh()
{
	GLuint VSize = Vertices.size() * 3 * sizeof(GLfloat);
	GLuint NSize = Normals.size() * 3 * sizeof(GLfloat);
	GLuint TSize = TextureCoords.size() * 2 * sizeof(GLfloat);
	GLuint ISize = Indices.size() * 3 * sizeof(GLuint);
	GLuint TGSize = Tangents.size() * 3 * sizeof(GLfloat);
	GLuint BTGSize = Bitangents.size() * 3 * sizeof(GLfloat);
	GLuint DSize = Distance.size() * sizeof(GLfloat);
	count = Indices.size() * 3;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(2, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, VSize + NSize + TSize + DSize, NULL, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER,VSize+NSize+TSize+TGSize+BTGSize,NULL,GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, VSize, (const void*)&Vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, VSize, NSize, (const void*)&Normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, VSize + NSize, TSize, (const void*)&TextureCoords[0]);
	glBufferSubData(GL_ARRAY_BUFFER, VSize + NSize + TSize, DSize, (const void*)&Distance[0]);
	//glBufferSubData(GL_ARRAY_BUFFER,VSize+NSize+TSize,TGSize,(const void*)&Mesh.Tangents[0]);
	//glBufferSubData(GL_ARRAY_BUFFER,VSize+NSize+TSize+TGSize,BTGSize,(const void*)&Mesh.Bitangents[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)VSize);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(VSize + NSize));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(VSize + NSize + TSize));
	//glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)(VSize+NSize+TSize));
	//glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)(VSize+NSize+TSize+TGSize));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	//glEnableVertexAttribArray(4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ISize, (const void*)&Indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	return true;
}

bool Mesh::GenOGLStructuredData()
{
	vector<vec4> verts;
	vector<vec4> norms;
	vector<uvec4> idxs;
	for (int i = 0; i < Vertices.size(); i++)
		verts.push_back(vec4(Vertices[i], 0.0));
	for (int i = 0; i < Normals.size(); i++)
		norms.push_back(vec4(Normals[i], 0.0));
	for (int i = 0; i < Indices.size(); i++)
		idxs.push_back(vec4(Indices[i], 0.0));
	GLuint VSize = verts.size() * 4 * sizeof(GLfloat);
	GLuint NSize = norms.size() * 4 * sizeof(GLfloat);
	GLuint ISize = idxs.size() * 4 * sizeof(GLuint);
	cout << "size: " << verts.size() << "," << norms.size() << "," << idxs.size() << endl;
	//cout << "size: " << VSize << "," << NSize << "," << ISize << endl;
	glGenBuffers(3, GPUVBO);
	//ver
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GPUVBO[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, VSize, (const GLvoid*)&verts[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, GPUVBO[0]);
	//normal
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GPUVBO[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NSize, (const GLvoid*)&norms[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, GPUVBO[1]);
	//indices
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GPUVBO[2]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ISize, (const GLvoid*)&idxs[0], GL_STATIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, GPUVBO[2]);
	//bind
	
	return true;
}

void Mesh::Render()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (const void*)0);
	glBindVertexArray(0);
}

/*
	该部分非常重要，请保证shader中材质变量名与该部分完全一致
	否则，请修改这部分相应的代码。
*/
void Material::SetMaterial(GLuint program, map<string, Texture2D*> &m_Textures)
{
	glUniform3fv(glGetUniformLocation(program, "Ka"), 1, Ka);
	glUniform3fv(glGetUniformLocation(program, "Kd"), 1, Kd);
	glUniform3fv(glGetUniformLocation(program, "Ks"), 1, Ks);
	glUniform1f(glGetUniformLocation(program, "Shininess"), Shininess);
	//glUniform1f(glGetUniformLocation(program, "Opacity"), Opacity)
	
	if (diffuseTex.size() > 0 && m_Textures.find(diffuseTex) != m_Textures.end())
	{
		glUniform1i(glGetUniformLocation(program, "HasDiffuseTexture"), 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Textures[diffuseTex]->OGLTexture);
		glUniform1i(glGetUniformLocation(program, "DiffuseTexture"), 0);
	}
	else 
		glUniform1i(glGetUniformLocation(program, "HasDiffuseTexture"), 0);

	if (specularTex.size() > 0 && m_Textures.find(specularTex) != m_Textures.end())
	{
		glUniform1i(glGetUniformLocation(program, "HasSpecularTexture"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_Textures[specularTex]->OGLTexture);
		glUniform1i(glGetUniformLocation(program, "SpecularTexture"), 1);
	}
	else
		glUniform1i(glGetUniformLocation(program, "HasSpecularTexture"), 0);
}
bool Scene::LoadFromFile(string &Filename)
{
	Assimp::Importer importer;
	const aiScene *pScene=importer.ReadFile(Filename,
		aiProcessPreset_TargetRealtime_Quality 
		| aiProcess_PreTransformVertices
		);

	if (!pScene)
	{
		cerr << "Assimp: cannot open file!" << endl;
		return false;
	}
	m_Meshes.resize(pScene->mNumMeshes);
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh);
	}
	return InitMaterial(pScene);

};
void Scene::InitMesh(int Index, const aiMesh *paiMesh)
{
	m_Meshes[Index].MaterialIndex = paiMesh->mMaterialIndex;
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = paiMesh->HasNormals() ? &(paiMesh->mNormals[i]) : &Zero3D;
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		const aiVector3D* pTangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mTangents[i]) : &Zero3D;
		const aiVector3D* pBitangent = paiMesh->HasTangentsAndBitangents() ? &(paiMesh->mBitangents[i]) : &Zero3D;
		m_Meshes[Index].Vertices.push_back(vec3(pPos->x, pPos->y, pPos->z));
		m_Meshes[Index].Normals.push_back(vec3(pNormal->x, pNormal->y, pNormal->z));
		m_Meshes[Index].TextureCoords.push_back(vec2(pTexCoord->x, pTexCoord->y));
		m_Meshes[Index].Tangents.push_back(vec3(pTangent->x, pTangent->y, pTangent->z));
		m_Meshes[Index].Bitangents.push_back(vec3(pBitangent->x, pBitangent->y, pBitangent->z));
	}

	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
	{
		const aiFace Face = paiMesh->mFaces[i];
		if (Face.mNumIndices != 3) continue;
		//assert(Face.mNumIndices == 3);
		m_Meshes[Index].Indices.push_back(vec3(Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]));
	}
}

bool Scene::InitMaterial(const aiScene* pScene)
{
	m_Materials.resize(pScene->mNumMaterials);
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		for (unsigned int j = 0; j < pMaterial->mNumProperties; j++)
		{
			const aiMaterialProperty* pMaterialProperty = pMaterial->mProperties[j];
			if (pMaterialProperty->mKey == aiString(string("$clr.ambient")))
				memcpy(m_Materials[i].Ka, pMaterialProperty->mData, sizeof(float)* 3);
			else if (pMaterialProperty->mKey == aiString(string("$clr.diffuse")))
				memcpy(m_Materials[i].Kd, pMaterialProperty->mData, sizeof(float)* 3);
			else if (pMaterialProperty->mKey == aiString(string("$clr.specular")))
				memcpy(m_Materials[i].Ks, pMaterialProperty->mData, sizeof(float)* 3);
			else if (pMaterialProperty->mKey == aiString(string("$mat.shininess")))
				memcpy(&(m_Materials[i].Shininess), pMaterialProperty->mData, sizeof(float));
			else if (pMaterialProperty->mKey == aiString(string("$mat.opacity")))
			{
				memcpy(&(m_Materials[i].Opacity), pMaterialProperty->mData, sizeof(float));
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				string path = Path.C_Str();
				m_Materials[i].diffuseTex = path;
				auto it = m_Textures.find(path);
				if (it == m_Textures.end())
					m_Textures[path] = new Texture2D(Path.C_Str());
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				string path = Path.C_Str();
				m_Materials[i].specularTex = path;
				auto it = m_Textures.find(path);
				if (it == m_Textures.end())
					m_Textures[path] = new Texture2D(Path.C_Str());
			}
		}
	}
	return true;
}

bool Scene::GenOGLMeshes()
{
	bool res = true;
	for (int idx = 0; idx < m_Meshes.size(); idx++)
	{
		res &= m_Meshes[idx].GenOGLMesh();
		res &= m_Meshes[idx].GenOGLStructuredData();
	}
	return res;
}

void Scene::calcNormal()
{
	for (int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].Distance.resize(m_Meshes[i].Vertices.size());
		for (int j = 0; j < m_Meshes[i].Vertices.size(); j++)
		{
			vec3 org = m_Meshes[i].Vertices[j];
			vec3 dir = -m_Meshes[i].Normals[j];
			float delta = 1e-3;
			m_Meshes[i].Distance[j] = 0;
			for (int k = 0; k < m_Meshes[i].Indices.size(); k++)
			{
				vec3 index = m_Meshes[i].Indices[k];
				vec3 d1 = m_Meshes[i].Vertices[index.x];
				vec3 d2 = m_Meshes[i].Vertices[index.y];
				vec3 d3 = m_Meshes[i].Vertices[index.z];
				mat3 A = mat3(d1.x - d2.x, d1.y - d2.y, d1.z - d2.z, d1.x - d3.x, d1.y - d3.y, d1.z - d3.z, dir.x, dir.y, dir.z);
				vec3 b = vec3(d1.x - org.x, d1.y - org.y, d1.z - org.z);
				if (abs(determinant(A)) < 1e-08)
					continue;
				mat3 Ainv = inverse(A);
				vec3 v = Ainv*b;
				if (0 <= v.x && 0 <= v.y && v.x + v.y <= 1 && v.z >= 1e-3)
				{
					if (v.z > m_Meshes[i].Distance[j])
						m_Meshes[i].Distance[j] = v.z;
				}
			}
			//cout << "v: " << org.x << "," << org.y << "," << org.z << "   n:" << dir.x << "," << dir.y << "," << dir.z << "   distance[" << j << "]: " << m_Meshes[i].Distance[j] << endl;
		}
	}
}