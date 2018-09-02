#pragma once

#include "glad/glad.h"
#include "Halide.h"
#include "GPUTools.h"

class Texture
{
public:
	Texture(){}
	Texture(const Halide::Buffer<uint8_t>& img_, int input_format_ = GL_RGBA) : img(img_), input_format(input_format_){}
	GLuint GetGPUPointer()
	{
		if (gpu_ptr == 0) gpu_ptr = HalideBuffer_GPUPtr(img, GL_RGBA, input_format);
		return gpu_ptr;
	}

	Halide::Buffer<uint8_t>& GetImgBuffer()
	{
		return img;
	}

private:
	GLuint gpu_ptr = 0;
	Halide::Buffer<uint8_t> img;
	int input_format;

};