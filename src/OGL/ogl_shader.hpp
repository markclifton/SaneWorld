#pragma once

#include "OGL/ogl_core.hpp"
#include "Utils/noncopyable.hpp"

namespace OGL {

class ShaderProgram : public NonCopyable {
public:
  ShaderProgram(const char *VS_Contents, const char *FS_Contents);
  ~ShaderProgram();

  void bind();

  const GLuint ShaderProgram::getUniformLocaition(const char *name);
  const GLuint ShaderProgram::getAttribLocation(const char *name);

  operator GLuint() { return program; }

private:
  GLuint program;
  GLuint vertex_shader;
  GLuint fragment_shader;
};

class VertexAttrib {
public:
  VertexAttrib(GLint location, GLint size, GLenum type, GLboolean normalized,
               GLsizei stride, const void *pointer);
  ~VertexAttrib();

  void enable();

private:
  GLint location;
  GLint size;
  GLenum type;
  GLboolean normalized;
  GLsizei stride;
  const void *pointer;
};

} // namespace OGL