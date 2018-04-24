#ifndef _COMPILER_H_
#define _COMPILER_H_
#include "inc.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <iterator>

using namespace std;
class Compiler
{
public:
	static map<string, GLuint> container;
	static void glGenProgramFromFile(const char* programName, const char* vShader, const char* tcShader, const char* teShader, const char* gShader, const char* fShader)
	{
		cout << "Compiling Program: " << programName << " ..."<<endl;
		if (container.find(programName) == container.end())
		{
			container[programName] = glCompileProgramFromFile(vShader, tcShader, teShader, gShader, fShader);
		}
		else
		{
			cout << "Program: " << programName << " has been compiled. " << endl;
		}
	}
	static bool glLoadProgram(const char* programName)
	{
		auto it = container.find(programName);
		if (it == container.end())
			return false;
		else
		{
			glUseProgram(it->second);
			return true;
		}
	}
private:
	static GLuint glCompileProgramFromFile(const char* vShader, const char* tcShader, const char* teShader, const char* gShader, const char* fShader)
	{
		string vShader_ = glGetShader(vShader);
		string tcShader_ = glGetShader(tcShader);
		string teShader_ = glGetShader(teShader);
		string gShader_ = glGetShader(gShader);
		string fShader_ = glGetShader(fShader);
		return glCompileProgram(vShader_.c_str(),
			tcShader_.size() != 0 ? tcShader_.c_str() : NULL,
			teShader_.size() != 0 ? teShader_.c_str() : NULL,
			gShader_.size() != 0 ? gShader_.c_str() : NULL,	fShader_.c_str());
	}
	static std::string glGetShader(const char *fileName)
	{
		if (fileName == NULL) return string();
		ifstream fs(fileName);
		stringstream ss;
		ss << fs.rdbuf();
		return ss.str();
	}
	static GLuint glCompileProgram(const char* vShader, const char* tcShader, const char* teShader, const char* gShader, const char* fShader)
	{
		GLuint program = glCreateProgram();
		GLuint shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shader, 1, &vShader, NULL);
		glCompileShader(shader);
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE)
		{
			char infolog[1000];
			glGetShaderInfoLog(shader, sizeof(infolog), NULL, infolog);
			std::cout << "Failed to compile vertex shader:" << std::endl << infolog << std::endl;;
		}
		else std::cout << "Successfully compiling vertex shader" << std::endl;
		glAttachShader(program, shader);

		if (tcShader != NULL)
		{
			shader = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(shader, 1, &tcShader, NULL);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE)
			{
				char infolog[1000];
				glGetShaderInfoLog(shader, sizeof(infolog), NULL, infolog);
				std::cout << "Failed to compile tess control shader:" << std::endl << infolog << std::endl;;
			}
			else std::cout << "Successfully compiling tess control shader" << std::endl;
			glAttachShader(program, shader);
		}

		if (teShader != NULL)
		{
			shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(shader, 1, &teShader, NULL);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE)
			{
				char infolog[1000];
				glGetShaderInfoLog(shader, sizeof(infolog), NULL, infolog);
				std::cout << "Failed to compile tess evaluate shader:" << std::endl << infolog << std::endl;;
			}
			else std::cout << "Successfully compiling tess evaluate shader" << std::endl;
			glAttachShader(program, shader);
		}

		if (gShader != NULL)
		{
			shader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(shader, 1, &gShader, NULL);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE)
			{
				char infolog[1000];
				glGetShaderInfoLog(shader, sizeof(infolog), NULL, infolog);
				std::cout << "Failed to compile geometry shader: " << std::endl << infolog << std::endl;;
			}
			else std::cout << "Successfully compiling geometry shader" << std::endl;
			glAttachShader(program, shader);
		}

		shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shader, 1, &fShader, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE)
		{
			char infolog[1000];
			glGetShaderInfoLog(shader, sizeof(infolog), NULL, infolog);
			std::cout << "Failed to compile fragment shader:" << std::endl << infolog << std::endl;
		}
		else std::cout << "Successfully compiling fragment shader" << std::endl;
		glAttachShader(program, shader);
		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE)
		{
			char infolog[1000];
			glGetProgramInfoLog(program, sizeof(infolog), NULL, infolog);
			std::cout << "Failed to link program" << std::endl << infolog << std::endl;
		}
		else std::cout << "Successfully linking program" << std::endl;
		return program;
	}
};

map<string, GLuint> Compiler::container;

#endif