#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include "inc.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <float.h>
using namespace std;
class Texture2D
{
public:
	unsigned char *pixels;
	unsigned int width, height, depth;
	GLuint OGLTexture;
	bool loaded;
private:
	Texture2D(){}
	Texture2D(const Texture2D &tex){}
public:
	Texture2D(unsigned int width, unsigned int height, unsigned char *pixels)
	{
		this->width = width;
		this->height = height;
		this->pixels = pixels;
		this->depth = 4;
	}
	Texture2D(const char *filename)
	{
		loaded = LoadFromFile(filename);
	}
	~Texture2D()
	{
		if (pixels!=NULL)
			delete []pixels;
	}
	bool LoadFromFile(const char *filename)
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

		width = FreeImage_GetWidth(dib);
		height = FreeImage_GetHeight(dib);
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
			depth = bih->biBitCount / 8;
			pixels = new unsigned char[width * height * depth];
			memcpy(pixels, bits, sizeof(unsigned char)*(width * height * depth));
			break;
		}
		default:
			FreeImage_Unload(dib);
			return false;
			break;
		};

		FreeImage_Unload(dib);

		//gen OGL texture
		glGenTextures(1, &OGLTexture);
		glBindTexture(GL_TEXTURE_2D, OGLTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);//reverse
		glBindTexture(GL_TEXTURE_2D, 0);

		cout << "Image: " << filename << " loading succeeded" << endl;

		return true;
	}
	void SaveAsPfmFile(const std::string & file_name, bool revese, float SRGB)
	{
		std::vector<float>  pixels_;
		pixels_.resize(width * height * 3);

		int size = 12;// depth * 4;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				unsigned char *color = nullptr;
				if (revese)
					color = pixels + (height - i - 1) * width * size + j * size;
				else
					color = pixels + i * width * size + j * size;
				float *f_color = (float *)color;
				pixels_[(i * width + j) * 3] = pow(*(f_color), SRGB);
				pixels_[(i * width + j) * 3 + 1] = pow(*(f_color + 1), SRGB);
				pixels_[(i * width + j) * 3 + 2] = pow(*(f_color + 2), SRGB);
		}
		}

		// Create file
		std::ofstream pfm_output(file_name, std::ios::binary | std::ios::out);

		pfm_output << "PF\n";
		pfm_output << (size_t)width << " " << (size_t)height << "\n";
		pfm_output << (double)-1 << "\n";

		for (int i = 0; i < height; i++)
		{
			pfm_output.write(reinterpret_cast<const char *>(&pixels_[i * width * 3]), width * 3 * sizeof(float));
		}
		pfm_output.close();

	}

};
#endif