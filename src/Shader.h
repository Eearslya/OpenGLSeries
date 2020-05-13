#pragma once

#include <string>
#include <unordered_map>

#include "glm/gtc/matrix_transform.hpp"

struct ShaderProgramSource {
  std::string VertexSource;
  std::string FragmentSource;
};

class Shader {
 public:
  Shader(const std::string &filename);
  ~Shader();

  void Bind() const;
  void Unbind() const;

  void SetUniform4f(const std::string &name, float v0, float v1, float v2,
                    float v3);
  void SetUniform1i(const std::string &name, int value);
  void SetUniform1f(const std::string &name, float value);
  void SetUniformMat4f(const std::string &name, const glm::mat4 &mat);

 private:
  int GetUniformLocation(const std::string &name);
  ShaderProgramSource ParseShader(const std::string &filename);
  unsigned int CompileShader(const unsigned int &type,
                             const std::string &source);
  unsigned int CreateShader(const std::string &vertexShader,
                            const std::string &fragmentShader);

  std::string m_FileName;
  unsigned int m_RendererID;
  std::unordered_map<std::string, int> m_UniformLocationCache;
};
