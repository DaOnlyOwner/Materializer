#pragma once

#include "glad/glad.h"
#include <cstdint>

namespace Halide
{
template<typename T>
class Buffer;
};

GLuint HalideBuffer_GPUPtr(const Halide::Buffer<uint8_t>& buffer, int output_format = GL_RGB,
                            int input_format = GL_RGBA);
