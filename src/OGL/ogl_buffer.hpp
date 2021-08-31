#pragma once

#include "OGL/ogl_core.hpp"
#include "Utils/noncopyable.hpp"

namespace OGL {

class Buffer : public NonCopyable {
public:
  Buffer(GLenum target);
  ~Buffer();

  void bind();
  void bufferData(GLsizeiptr size, const void *data, GLenum usage);

  operator GLuint() { return buffer; }

private:
  GLuint buffer;
  GLenum target;
};

} // namespace OGL