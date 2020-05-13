#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Renderer.h"

Shader::Shader(const std::string &filename)
    : m_FileName(filename), m_RendererID(0) {
  ShaderProgramSource source = ParseShader(filename);
  m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader() { GLCall(glDeleteProgram(m_RendererID)); }

void Shader::Bind() const { GLCall(glUseProgram(m_RendererID)); }

void Shader::Unbind() const { GLCall(glUseProgram(0)); }

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2,
                          float v3) {
  Bind();
  GLint location = GetUniformLocation(name);
  GLCall(glUniform4f(location, v0, v1, v2, v3));
}

void Shader::SetUniform1i(const std::string &name, int value) {
  Bind();
  GLint location = GetUniformLocation(name);
  GLCall(glUniform1i(location, value));
}

void Shader::SetUniform1f(const std::string &name, float value) {
  Bind();
  GLint location = GetUniformLocation(name);
  GLCall(glUniform1f(location, value));
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &mat) {
  Bind();
  GLint location = GetUniformLocation(name);
  GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]));
}

int Shader::GetUniformLocation(const std::string &name) {
  if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
    m_UniformLocationCache[name];
  }

  GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
  if (location == -1) {
    std::cout << "[WARN] Uniform " << name << " does not exist!" << std::endl;
  }
  m_UniformLocationCache[name] = location;

  return location;
}

ShaderProgramSource Shader::ParseShader(const std::string &filename) {
  std::ifstream stream(filename);

  enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

  std::stringstream ss[2];
  std::string line;
  ShaderType type;

  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") != std::string::npos) {
        type = ShaderType::FRAGMENT;
      }
    } else {
      ss[(int)type] << line << '\n';
    }
  }

  return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::CompileShader(const unsigned int &type,
                                   const std::string &source) {
  GLCall(unsigned int id = glCreateShader(type));
  const char *src = source.c_str();
  GLCall(glShaderSource(id, 1, &src, nullptr));
  GLCall(glCompileShader(id));

  int result;
  GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
  if (result == GL_FALSE) {
    int length;
    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
    char *message = (char *)alloca(length * sizeof(char));
    GLCall(glGetShaderInfoLog(id, length, &length, message));
    std::cout << "Shader compilation failed!" << std::endl;
    std::cout << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
              << " shader error:" << std::endl;
    std::cout << "  " << message << std::endl;
    GLCall(glDeleteShader(id));
    return 0;
  }

  return id;
}

unsigned int Shader::CreateShader(const std::string &vertexShader,
                                  const std::string &fragmentShader) {
  GLCall(unsigned int program = glCreateProgram());
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  GLCall(glAttachShader(program, vs));
  GLCall(glAttachShader(program, fs));
  GLCall(glLinkProgram(program));
  GLCall(glValidateProgram(program));

  GLCall(glDeleteShader(vs));
  GLCall(glDeleteShader(fs));

  return program;
}