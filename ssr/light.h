#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "inc.h"
using namespace glm;
class Light
{
private:
	static const int maxLights = 10;
private:
	typedef struct{
		int type;
		vec3 position;
		vec3 color;
		float fade;
	} light;
private:
	light lights[maxLights];
	int numLights;
public:
	Light()
	{
		initLightSet1();
	}
	void initLightSet0()
	{
		numLights = 4;
		lights[0].type = 0;
		lights[0].position = vec3(-1, -1, 0);
		lights[0].color = vec3(1, 0, 0);
		lights[0].fade = 0.0;
		lights[1].type = 0;
		lights[1].position = vec3(-100, 0, 0);
		lights[1].color = vec3(0, 1, 0.3);
		lights[1].fade = 0.01;
		lights[2].type = 0;
		lights[2].position = vec3(-1, -1, 1);
		lights[2].color = vec3(0.1, 0.5, 1.0);
		lights[2].fade = 0.0;
		lights[3].type = 0;
		lights[3].position = vec3(1, 0, 0);
		lights[3].color = vec3(1, 0.75, 0.5);
		lights[3].fade = 0.0;

	}
	void initLightSet1()
	{
		numLights = 1;
		lights[0].type = 0;
		lights[0].position = vec3(-1, -1, 0);
		lights[0].color = vec3(1, 0.75, 0.5) * 2.0f;
		lights[0].fade = 0.0;
	}
	void setLightProperties(GLuint program)
	{
		char LightsPropertiesName[100];
		glUniform1i(glGetUniformLocation(program, "numLights"), numLights);
		for (int i = 0; i < numLights; i++)
		{
			sprintf_s(LightsPropertiesName, "lights[%d].type\0", i);
			glUniform1i(glGetUniformLocation(program, LightsPropertiesName), lights[i].type);
			sprintf_s(LightsPropertiesName, "lights[%d].position\0", i);
			glUniform3fv(glGetUniformLocation(program, LightsPropertiesName), 1, &lights[i].position[0]);
			sprintf_s(LightsPropertiesName, "lights[%d].color\0", i);
			glUniform3fv(glGetUniformLocation(program, LightsPropertiesName), 1, &lights[i].color[0]);
			sprintf_s(LightsPropertiesName, "lights[%d].fade\0", i);
			glUniform1f(glGetUniformLocation(program, LightsPropertiesName), lights[i].fade);
		}
		
	}
};
#endif