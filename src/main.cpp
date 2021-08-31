#ifdef WIN32
#include <windows.h>
#endif

#include "OGL/ogl_buffer.hpp"
#include "OGL/ogl_core.hpp"
#include "OGL/ogl_shader.hpp"

#include "Shaders/ogl_basic_shaders.hpp"

static const struct {
  float x, y, z;
} vertices[3] = {{-0.6f, -0.4f, 1.f}, {0.6f, -0.4f, 1.f}, {0.f, 0.6f, 1.f}};

int main(int argc, char *argv[]) {
#if defined(WIN32) && defined(_DEBUG)
  FreeConsole();
#endif

  OGL::OGLCore core("Sandbox", 640, 480);
  OGL::Buffer vertex_buffer(GL_ARRAY_BUFFER);
  OGL::ShaderProgram shader_program(vs_modern, fs_modern);
  OGL::VertexAttrib vpos(shader_program.getAttribLocation("vPos"), 3, GL_FLOAT,
                         GL_FALSE, sizeof(vertices[0]), (void *)0);

  GLint mvp_location = shader_program.getUniformLocaition("MVP");

  shader_program.bind();
  vertex_buffer.bind();
  vertex_buffer.bufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);
  vpos.enable();

  while (core.isRunning()) {

    glm::mat4 m = glm::rotate(glm::mat4(1.f), (float)glfwGetTime(), {0, 0, 1});
    glm::mat4 p = core.getOrthoProjection();
    glm::mat4 mvp = p * m;

    shader_program.bind();
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    core.update();
  }
}
