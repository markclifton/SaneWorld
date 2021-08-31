#include "OGL/ogl_buffer.hpp"

namespace OGL {
Buffer::Buffer(GLenum target) : target(target) { glGenBuffers(1, &buffer); }

Buffer::~Buffer() { glDeleteBuffers(1, &buffer); }

void Buffer::bind() { glBindBuffer(target, buffer); }

void Buffer::bufferData(GLsizeiptr size, const void *data, GLenum usage) {
  glBufferData(target, size, data, usage);
}

} // namespace OGL