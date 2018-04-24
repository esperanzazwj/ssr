#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_
#include "inc.h"
#include "compiler.h"
class Raytracer
{
private:
	GLuint VAO, VBO;
	GLuint program;
public:
	Raytracer()
	{
		createQuad();
		createProgram();
	}
	void render()
	{
		//glUseProgram(program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		//glUseProgram(0);
	}
	GLuint getProgram()
	{
		return program;
	}
private:
	void createQuad()
	{
		GLfloat Vertices[4][2] = {
			{ -1.0f, -1.0f },
			{ -1.0f, 1.0f },
			{ 1.0f, -1.0f },
			{ 1.0f, 1.0f }
		};
		GLfloat TexCoord[4][2] = {
			{ 0.0, 0.0 },
			{ 0.0, 1.0 },
			{ 1.0, 0.0 },
			{ 1.0, 1.0 }
		};
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 16, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)* 8, Vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* 8, sizeof(GLfloat)* 8, TexCoord);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(Vertices));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
		glBindVertexArray(0);
	}
	void createProgram()
	{
		Compiler::glGenProgramFromFile("raytracer", "shader/raytracer_vs.glsl", NULL, NULL, NULL, "shader/raytracer_fs.glsl");
		program = Compiler::container["raytracer"];
	}
};
#endif