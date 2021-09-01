#ifdef WIN32
#include <windows.h>
#endif

#include <vector>

#include "OGL/ogl_buffer.hpp"
#include "OGL/ogl_core.hpp"
#include "OGL/ogl_shader.hpp"
#include <OGL/ogl_shapeMgr.hpp>
#include "Shaders/ogl_basic_shaders.hpp"

int main(int argc, char* argv[]) {
#if defined(WIN32) && defined(_DEBUG)
  FreeConsole();
#endif

  OGL::OGLCore core("Sandbox", 640, 480);
  OGL::ShaderProgram shaders(vs_modern, fs_modern);

  GLint mvp_location = shaders.getUniformLocaition("MVP");
  OGL::VertexAttrib vpos(shaders.getAttribLocation("vPos"), 3, GL_FLOAT,
    GL_FALSE, sizeof(OGLVertex), (void*)0);

  OGL::VertexAttrib vcol(shaders.getAttribLocation("vCol"), 3, GL_FLOAT,
    GL_FALSE, sizeof(OGLVertex),
    (void*)(3 * sizeof(float)));

  OGL::VerticesManager verticesMgr;
  OGL::IndicesManager indicesMgr;

  std::vector<OGLRectangle> shapes;
  float scale = .5f;
  for (float x = -1.f; x < 1.f; x += scale) {
    for (float y = -1.f; y < 1.f; y += scale) {
      shapes.emplace_back(x, y, -1.f, scale, scale);
      verticesMgr.add(shapes.back().verticesCount(), shapes.back().vertices());
      indicesMgr.add(shapes.back().indicesCount(), shapes.back().indices());
    }
  }

  shaders.bind();
  verticesMgr.bind();

  verticesMgr.buffer(GL_STATIC_DRAW);
  indicesMgr.buffer(GL_STATIC_DRAW);
  vpos.enable();
  vcol.enable();

  while (core.isRunning()) {
    glm::mat4 m = glm::mat4(1.f);
    //glm::mat4 m = glm::rotate(glm::mat4(1.f), (float)glfwGetTime(), {0, 0, 1});
    glm::mat4 p = core.getPersProjection();
    glm::mat4 mvp = p * m;

    shaders.bind();
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
    indicesMgr.draw();

    core.update();
  }
}
