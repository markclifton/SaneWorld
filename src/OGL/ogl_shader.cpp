#include "OGL/ogl_shader.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace OGL {

ShaderProgram::ShaderProgram(const char *VS_Contents, const char *FS_Contents) {
  GLint compiled;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &VS_Contents, NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint log_length;
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> v(static_cast<size_t>(log_length));
    glGetShaderInfoLog(vertex_shader, log_length, nullptr, v.data());
    std::string err(begin(v), end(v));

    glDeleteShader(vertex_shader);
    std::cerr << "Failed to compile vertex shader: " << err << std::endl;
  }

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &FS_Contents, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint log_length;
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> v(static_cast<size_t>(log_length));
    glGetShaderInfoLog(fragment_shader, log_length, nullptr, v.data());
    std::string err(begin(v), end(v));

    glDeleteShader(fragment_shader);
    std::cerr << "Failed to compile fragment shader: " << err << std::endl;
  }

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(program); }

void ShaderProgram::bind() { glUseProgram(program); }

const GLuint ShaderProgram::getUniformLocaition(const char *name) {
  return glGetUniformLocation(program, name);
}

const GLuint ShaderProgram::getAttribLocation(const char *name) {
  return glGetAttribLocation(program, name);
}

VertexAttrib::VertexAttrib(GLint location, GLint size, GLenum type,
                           GLboolean normalized, GLsizei stride,
                           const void *pointer)
    : location(location), size(size), type(type), normalized(normalized),
      stride(stride), pointer(pointer) {}

VertexAttrib::~VertexAttrib() {}

void VertexAttrib::enable() {
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, size, type, normalized, stride, pointer);
}
} // namespace OGL