#ifndef _FBO_H_
#define _FBO_H_
#include "inc.h"
#include <iostream>
#include "Texture.h"

class FrameBuffer
{
private:
	static const int MaxColorBuffers = 10;
	GLuint FBO;
	int FBO_width, FBO_height;
	GLuint ColorTextures[MaxColorBuffers], DepthTexture;
	int NumColorBuffer;
private:
	FrameBuffer(){}
public:
	FrameBuffer(int _FBO_width, int _FBO_height, int NumColorBuffer) : FBO_width(_FBO_width), FBO_height(_FBO_height), NumColorBuffer(NumColorBuffer)
	{
		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glGenTextures(NumColorBuffer, ColorTextures);
		GLenum *mrt = new GLenum[NumColorBuffer];
		for (int i = 0; i < NumColorBuffer;i++)
		{
			
			glBindTexture(GL_TEXTURE_2D, ColorTextures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, FBO_width, FBO_height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, ColorTextures[i], 0);
			mrt[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers(NumColorBuffer, mrt);

		glGenTextures(1, &DepthTexture);
		glBindTexture(GL_TEXTURE_2D, DepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, FBO_width, FBO_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthTexture, 0);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0, 0, 0, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Failed to intialize FBO" << std::endl;
		else
			std::cout << "Succeeded to intialize FBO" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	void Begin()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,FBO);
		glViewport(0,0, FBO_width,FBO_height);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void End()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	}
	void Bilt(GLuint Target)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER,FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,Target);
		glBlitFramebuffer(0, 0, FBO_width, FBO_height, 0, 0, FBO_width, FBO_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	GLuint GetColorTexture(int index)
	{
		if (index>=0 && index<NumColorBuffer)
			return ColorTextures[index];
		else return 0;
	}

	GLuint GetDepthTexture()
	{
		return DepthTexture;
	}

	Texture2D *GetColorBufferData(int index)
	{
		if (index >= NumColorBuffer) return NULL;
		unsigned char *pixels = new unsigned char[FBO_width * FBO_height * 12];
		glBindTexture(GL_TEXTURE_2D, ColorTextures[index]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);
		glBindTexture(GL_TEXTURE_2D, 0);
		Texture2D *texture2D = new Texture2D(FBO_width, FBO_height, pixels);
		return texture2D;
	}
};
#endif