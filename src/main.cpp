/*
#ifdef WIN32
#include <windows.h>
#endif

#include <vector>

#include "OGL/ogl_buffer.hpp"
#include "OGL/ogl_core.hpp"
#include "OGL/ogl_shader.hpp"
#include <OGL/ogl_shapeMgr.hpp>
#include "Shaders/ogl_basic_shaders.hpp"

class Interval_Map {
public:

};

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
*/

#include <map>
#include <limits>
#include <cassert>
#include <vector>
#include <stdlib.h>
#include <iostream>

template<typename K, typename V>
class interval_map {
protected:
  std::map<K, V> m_map;

public:
  interval_map() {
    m_map.insert(m_map.end(), std::make_pair(0, 1000));
  }

  void assign(K const& keyBegin, K const& keyEnd, V const& val) {
    if (!(keyBegin < keyEnd))
      return;

    auto nextInterval = --m_map.upper_bound(keyEnd);

    auto inserted1 = m_map.end();
    auto inserted2 = m_map.end();
    assert(nextInterval != m_map.end());

    if (nextInterval->second == val)
      ++nextInterval;
    else if (nextInterval->first < keyEnd)
    {
      if (nextInterval->second < val) {
        assign(keyBegin, nextInterval->first, val);
        return;
      }
      else
      {
        const V& nextValue = nextInterval->second;
        ++nextInterval;
        inserted1 = nextInterval = m_map.emplace_hint(nextInterval, keyEnd, nextValue);
      }
    }

    try
    {
      auto prevInterval = nextInterval;
      --prevInterval;
      assert(prevInterval != m_map.end());

      if (keyBegin < prevInterval->first)
        prevInterval = --m_map.upper_bound(keyBegin);
      assert(prevInterval != m_map.end());

      if (!(prevInterval->second == val))
      {
        if (prevInterval->first < keyBegin)
        {
          ++prevInterval;
          inserted2 = prevInterval = m_map.emplace_hint(prevInterval, keyBegin, val);
        }
        else
        {
          prevInterval->second = val;
          if (prevInterval != m_map.begin() && !((--prevInterval)->second == val))
          {
            ++prevInterval;
          }
        }
      }

      assert(prevInterval != m_map.end());
      m_map.erase(++prevInterval, nextInterval);
    }
    catch (...)
    {
      if (inserted1 != m_map.end())
        m_map.erase(inserted1);
      if (inserted2 != m_map.end())
        m_map.erase(inserted2);
      throw;
    }
  }

  V const& operator[](K const& key) const {
    return (--m_map.upper_bound(key))->second;
  }
};


int main()
{
  std::map<int, interval_map<int, int>> maps;

  maps[0] = interval_map<int, int>();
  maps[0].assign(0, 1920, 100);

  maps[0].assign(100, 200, 200);
  maps[0].assign(400, 800, 50);

  //maps[0].print();
  return 0;
}
