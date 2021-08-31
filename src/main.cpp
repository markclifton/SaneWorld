#ifdef WIN32
#include <windows.h>
#endif

#include "OGL/ogl_buffer.hpp"
#include "OGL/ogl_core.hpp"
#include "OGL/ogl_shader.hpp"

#include "Shaders/ogl_basic_shaders.hpp"

static const struct { int index; } indices[6] = {0, 1, 2, 3, 0, 1};

namespace {
struct Vertex {
  float x, y, z;
  float r, g, b;
};

struct Rectangle {
  Vertex vertices[4];
  inline Vertex &tl() { return vertices[0]; }
  inline Vertex &bl() { return vertices[1]; }
  inline Vertex &tr() { return vertices[2]; }
  inline Vertex &br() { return vertices[3]; }
  operator void *() { return &vertices[0]; }
};

Rectangle generateRectangleAtCenter(float x, float y, float z, float h,
                                    float w) {
  Rectangle newRect;
  newRect.tl() = {-1.f, -1.f, -10.f, 1.f, 0.f, 0.f};
  newRect.bl() = {1.f, -1.f, -10.f, 0.f, 1.f, 0.f};
  newRect.tr() = {-1.f, -1.f, -10.f, 0.f, 0.f, 10.f};
  newRect.br() = {0.f, 0.f, -10.f, 0.f, 0.f, 0.f};
  return newRect;
}
} // namespace

int main(int argc, char *argv[]) {
#if defined(WIN32) && defined(_DEBUG)
  FreeConsole();
#endif

  OGL::OGLCore core("Sandbox", 640, 480);
  OGL::Buffer vertex_buffer(GL_ARRAY_BUFFER);
  OGL::Buffer index_buffer(GL_ELEMENT_ARRAY_BUFFER);
  OGL::ShaderProgram shader_program(vs_modern, fs_modern);
  OGL::VertexAttrib vpos(shader_program.getAttribLocation("vPos"), 3, GL_FLOAT,
                         GL_FALSE, sizeof(Vertex), (void *)0);
  OGL::VertexAttrib vcol(shader_program.getAttribLocation("vCol"), 3, GL_FLOAT,
                         GL_FALSE, sizeof(Vertex), (void *)(3 * sizeof(float)));

  GLint mvp_location = shader_program.getUniformLocaition("MVP");

  auto rect = generateRectangleAtCenter(0, 0, 0, .5, .5);

  shader_program.bind();
  index_buffer.bind();
  index_buffer.bufferData(sizeof(indices), indices, GL_STATIC_DRAW);
  vertex_buffer.bind();
  vertex_buffer.bufferData(sizeof(rect), rect, GL_STATIC_DRAW);
  vpos.enable();
  vcol.enable();

  while (core.isRunning()) {
    glm::mat4 m = glm::mat4(1.f);
    // glm::rotate(glm::mat4(1.f), (float)glfwGetTime(), {0, 0, 1});
    glm::mat4 p = core.getOrthoProjection();
    glm::mat4 mvp = p; // * m;

    shader_program.bind();
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp[0][0]);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)0);

    core.update();
  }
}
