#ifndef _SKYBOX_H_
#define _SKYBOX_H_
#include "inc.h"
#include "compiler.h"
#include "camera.h"
#include <string>
using namespace std;
class Skybox
{
private:
	GLuint skybox;
	int width[6], height[6], depth[6];
	unsigned char *pixels[6];
	GLuint VAO, VBO[2];
	GLuint skyboxProgram;
public:
	Skybox(const char *filename[], int size)
	{
		initBox();

		glGenTextures(1, &skybox);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
		glTexStorage2D(GL_TEXTURE_CUBE_MAP, (int)log2((double)size), GL_RGB8, size, size);

		for (int face = 0; face < 6; face++)
		{
			LoadSkyboxImage(filename[face], face);
			GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
			if (depth[face] == 3)
			{
				glTexSubImage2D(target, 0, 0, 0, width[face], height[face], GL_BGR, GL_UNSIGNED_BYTE, pixels[face]);
			}
			else
			{

				glTexSubImage2D(target, 0, 0, 0, width[face], height[face], GL_BGRA, GL_UNSIGNED_BYTE, pixels[face]);
			}
		}
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	void Render(Camera *camera)
	{
		mat4 gViewProjectMatrix = camera->GetPerspectiveMatrix()*mat4(mat3(camera->GetViewMatrix()));
		glUseProgram(skyboxProgram);
		glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "gViewProjectMatrix"), 1, GL_FALSE, &gViewProjectMatrix[0][0]);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
		glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 3);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_INT, (void*)(8 * sizeof(GLint)));
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	GLuint getSkybox()
	{
		return skybox;
	}
private:
	void initBox()
	{
		Compiler::glGenProgramFromFile("skybox", "shader/skybox_vs.glsl", NULL, NULL, NULL, "shader/skybox_fs.glsl");
		skyboxProgram = Compiler::container["skybox"];

		GLfloat vertices[][3] = {
			{ -1, -1, -1 },
			{ 1, -1, -1 },
			{ 1, 1, -1 },
			{ -1, 1, -1 },
			{ -1, -1, 1 },
			{ 1, -1, 1 },
			{ 1, 1, 1 },
			{ -1, 1, 1 }
		};
		GLint index[] = {
			0, 1, 3, 2, 7, 6, 4, 5,
			2, 6, 1, 5, 0, 4, 3, 7
		};

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(2, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), (const void*)vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), (const void*)index, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}
	bool LoadSkyboxImage(const char *filename, int face)
	{
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		FREE_IMAGE_TYPE fit = FIT_UNKNOWN;
		BITMAPINFOHEADER *bih = NULL;
		FIBITMAP *dib(0), *dib_(0);
		BYTE* bits(0);
		fif = FreeImage_GetFileType(filename, 0);
		if (fif == FIF_UNKNOWN)
			fif = FreeImage_GetFIFFromFilename(filename);
		if (fif == FIF_UNKNOWN)
			return false;
		if (FreeImage_FIFSupportsReading(fif))
			dib_ = FreeImage_Load(fif, filename);
		if (!dib_)
			return false;
		int width_ = FreeImage_GetWidth(dib_);
		int height_ = FreeImage_GetHeight(dib_);
		int width2 = 1, height2 = 1;
		while (width2 < width_) width2 *= 2;
		while (height2 < height_) height2 *= 2;
		dib = FreeImage_Rescale(dib_, width2, height2);
		FreeImage_Unload(dib_);
		//rotate
		dib = FreeImage_Rotate(dib, 180);

		width[face] = FreeImage_GetWidth(dib);
		height[face] = FreeImage_GetHeight(dib);
		bits = FreeImage_GetBits(dib);
		fit = FreeImage_GetImageType(dib);
		bih = FreeImage_GetInfoHeader(dib);
		if ((bits == 0) || (width == 0) || (height == 0))
		{
			FreeImage_Unload(dib);
			return false;
		}

		switch (fit)
		{
		case FIT_BITMAP:
		{
			depth[face] = bih->biBitCount / 8;
			pixels[face] = new unsigned char[width[face] * height[face] * depth[face]];
			memcpy(pixels[face], bits, sizeof(unsigned char)*(width[face] * height[face] * depth[face]));
			break;
		}
		default:
			FreeImage_Unload(dib);
			return false;
			break;
		};

		FreeImage_Unload(dib);

		cout << "Image: " << filename << " loading succeeded" << endl;

		return true;
	}
};

#endif