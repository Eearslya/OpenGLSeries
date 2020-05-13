#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

#define ASSERT(x) \
  if (!(x)) __debugbreak();
#define GLCall(x) \
  GLClearError(); \
  x;              \
  ASSERT(GLCheckError(#x, __FILE__, __LINE__));

void GLClearError();
bool GLCheckError(const char *function, const char *file, int line);

class Renderer {
 public:
  void Clear() const;
  void Draw(const VertexArray &va, const IndexBuffer &ib,
            const Shader &shader) const;

 private:
};