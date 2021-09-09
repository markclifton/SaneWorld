#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <vector>

#include "OGL/ogl_buffer.hpp"
#include "OGL/ogl_core.hpp"
#include "OGL/ogl_shader.hpp"
#include <OGL/ogl_shapeMgr.hpp>
#include "Shaders/ogl_basic_shaders.hpp"

#define USE_D3D 1
#include "MaskedOcclusionCulling/MaskedOcclusionCulling.h"

float deltaX = 0.f;
float deltaY = 0.f;
float deltaZ = 0.f;

int main(int argc, char* argv[]) {
#if defined(WIN32) && defined(_DEBUG)
  FreeConsole();
#endif

  const int WIDTH = 640;
  const int HEIGHT = 480;

  MaskedOcclusionCulling* moc = MaskedOcclusionCulling::Create();
  moc->SetResolution(WIDTH, HEIGHT);   // Set full HD resolution
  moc->SetNearClipPlane(1.f);

  OGL::OGLCore core("Sandbox", WIDTH, HEIGHT);
  OGL::ShaderProgram shaders(vs_modern, fs_modern);

  GLint mvp_location = shaders.getUniformLocaition("MVP");
  OGL::VertexAttrib vpos(shaders.getAttribLocation("vPos"), 3, GL_FLOAT,
    GL_FALSE, sizeof(OGLVertex), (void*)0);

  OGL::VertexAttrib vcol(shaders.getAttribLocation("vCol"), 3, GL_FLOAT,
    GL_FALSE, sizeof(OGLVertex),
    (void*)(4 * sizeof(float)));

  OGL::VerticesManager verticesMgr;
  OGL::IndicesManager indicesMgr;

  std::vector<OGLRectangle> shapes;
  float scale = .5f;
  for (float x = -1.f; x < 1.f; x += scale) {
    for (float y = -1.f; y < 1.f; y += scale) {
      shapes.emplace_back(x, y, -5.f, scale, scale);
    }
  }

  OGLRectangle test(0, 0, -1.f, .75, .75);
  for (int i = 0; i < 4; i++)
  {
    test.vertices()[i].r = 1;
    test.vertices()[i].g = 1;
    test.vertices()[i].b = 1;
  }

  auto moveTest = [&]() -> void {
    test.vertices()[0].x += deltaX;
    test.vertices()[1].x += deltaX;
    test.vertices()[2].x += deltaX;
    test.vertices()[3].x += deltaX;

    test.vertices()[0].y -= deltaY;
    test.vertices()[1].y -= deltaY;
    test.vertices()[2].y -= deltaY;
    test.vertices()[3].y -= deltaY;

    test.vertices()[0].z += deltaZ;
    test.vertices()[1].z += deltaZ;
    test.vertices()[2].z += deltaZ;
    test.vertices()[3].z += deltaZ;
  };

  glEnable(GL_DEPTH_TEST);

  while (core.isRunning()) {
    glm::mat4 m = glm::mat4(1.f);
    glm::mat4 p = core.getPersProjection();
    glm::mat4 mvp = p * m;

    moc->ClearBuffer();

    float out[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    moveTest();
    moc->TransformVertices(&p[0][0], (float*)test.vertices(), out, 4, MaskedOcclusionCulling::VertexLayout(28, 4, 8));
    moc->RenderTriangles(out, test.indices(), 2);

    verticesMgr.reset();
    indicesMgr.reset();

    int countRenderedShapes = 0;

    for (auto& shape : shapes)
    {
      moc->TransformVertices(&p[0][0], (float*)shape.vertices(), out, 4, MaskedOcclusionCulling::VertexLayout(28, 4, 8));

      MaskedOcclusionCulling::CullingResult result = moc->TestTriangles(out, shape.indices(), 2);
      if (result == MaskedOcclusionCulling::CullingResult::VISIBLE)
      {
        verticesMgr.add(shape.verticesCount(), shape.vertices());
        indicesMgr.add(shape.indicesCount(), shape.indices());
        countRenderedShapes++;
      }
    }

    std::cout << "Rendering: " << countRenderedShapes << " shapes.\n";

    verticesMgr.add(test.verticesCount(), test.vertices());
    indicesMgr.add(test.indicesCount(), test.indices());

    shaders.bind();
    verticesMgr.bind();

    verticesMgr.buffer(GL_STATIC_DRAW);
    indicesMgr.buffer(GL_STATIC_DRAW);
    vpos.enable();
    vcol.enable();

    shaders.bind();
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
    indicesMgr.draw();

    core.update();
  }

  MaskedOcclusionCulling::Destroy(moc);
}
