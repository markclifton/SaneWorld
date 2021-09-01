#ifdef WIN32
#include <windows.h>
#endif

#include <vector>

#include "OGL/ogl_buffer.hpp"
#include "OGL/ogl_core.hpp"
#include "OGL/ogl_shader.hpp"
#include "Shaders/ogl_basic_shaders.hpp"

struct OGLVertex {
  float x, y, z;
  float r, g, b;
};

class OGLRectangle {
public:
  OGLRectangle(float x, float y, float z, float h, float w) {
    w = w / 2.f;
    h = h / 2.f;

    _vertices[0] = { x - w, y + h, z, 1, 0, 0 };
    _vertices[1] = { x - w, y - h, z, 0, 1, 0 };
    _vertices[2] = { x + w, y - h, z, 0, 1, 1 };
    _vertices[3] = { x + w, y + h, z, 0, 0, 1 };
  }

  int indicesCount() { return sizeof(_indices) / sizeof(int); }
  int* indices() { return &_indices[0]; }

  int verticesCount() { return sizeof(_vertices) / sizeof(OGLVertex); }
  OGLVertex* vertices() { return &_vertices[0]; }

private:
  OGLVertex _vertices[4];
  int _indices[6]{ 0, 1, 2, 2, 3, 0 };
};

namespace OGL {
  class VerticesManager {
  public:
    VerticesManager() : _buffer(GL_ARRAY_BUFFER) {}

    void add(int count, OGLVertex* data) {
      for (int i = 0; i < count; i++) {
        _data.push_back(data[i]);
      }
    }

    void bind() { _buffer.bind(); }

    void buffer(GLenum type) {
      _buffer.bind();
      _buffer.bufferData(_data.size() * sizeof(OGLVertex), &_data[0], type);
    }

    void reset() { _data.clear(); }

  private:
    OGL::Buffer _buffer;
    std::vector<OGLVertex> _data;
  };

  class IndicesManager {
  public:
    IndicesManager() : _buffer(GL_ELEMENT_ARRAY_BUFFER), _vertexCount(0) {}
    ~IndicesManager() = default;

    void add(int count, int* data) {
      int vertexCount = 0;
      for (int i = 0; i < count; i++) {
        vertexCount = vertexCount > (data[i] + 1) ? vertexCount : data[i] + 1;
        _data.push_back(data[i] + _vertexCount);
      }
      _vertexCount += vertexCount;
    }

    void buffer(GLenum type) {
      _buffer.bind();
      _buffer.bufferData(_data.size() * sizeof(int), &_data[0], type);
    }

    void draw() {
      _buffer.bind();
      glDrawElements(GL_TRIANGLES, (int)_data.size(), GL_UNSIGNED_INT, (void*)0);
    }

    void reset() {
      _vertexCount = 0;
      _data.clear();
    }

  private:
    OGL::Buffer _buffer;
    std::vector<int> _data;
    int _vertexCount;
  };
} // namespace OGL

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
  float scale = .00625f;
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
