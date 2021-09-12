#ifdef WIN32
#include <windows.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <sstream>
#include <vector>

#include <OGL/ogl_buffer.hpp>
#include <OGL/ogl_core.hpp>
#include <OGL/ogl_shader.hpp>
#include <OGL/ogl_shapeMgr.hpp>
#include <Shaders/ogl_basic_shaders.hpp>
#include <Utils/logging.hpp>

#define USE_D3D 1
#include "MaskedOcclusionCulling/MaskedOcclusionCulling.h"
#include "MaskedOcclusionCulling/CullingThreadpool.h"


int main(int argc, char* argv[]) {
#if defined(WIN32)
  FreeConsole();
#endif
  Sane::LogHandler::Create(Sane::LOG_LEVEL::WARN);

  const int WIDTH = 1280;
  const int HEIGHT = 720;

  MaskedOcclusionCulling* moc = MaskedOcclusionCulling::Create();
  moc->SetResolution(WIDTH, HEIGHT);
  moc->SetNearClipPlane(1.f);

  CullingThreadpool pool(8, 8, 8);
  pool.SetBuffer(moc);
  pool.WakeThreads();

  OGL::OGLCore core("Sandbox", WIDTH, HEIGHT);
  OGL::ShaderProgram shaders(vs_modern, fs_modern);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  ImGui::StyleColorsDark();

  const char* glsl_version = "#version 130";
  ImGui_ImplGlfw_InitForOpenGL(core, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  GLint mvp_location = shaders.getUniformLocaition("MVP");
  OGL::VertexAttrib vpos(shaders.getAttribLocation("vPos"), 3, GL_FLOAT,
    GL_FALSE, sizeof(OGLVertex), (void*)0);

  OGL::VertexAttrib vcol(shaders.getAttribLocation("vCol"), 3, GL_FLOAT,
    GL_FALSE, sizeof(OGLVertex),
    (void*)(4 * sizeof(float)));

  OGL::VerticesManager verticesMgr;
  OGL::IndicesManager indicesMgr;

  std::vector<OGLRectangle> shapes;
  float scale = .25f;
  for (float x = -1.f; x < 1.f; x += scale) {
    for (float y = -1.f; y < 1.f; y += scale) {
      shapes.emplace_back(x, y, -5.f, scale, scale);
    }
  }

  OGLRectangle test(0, 0, -1.f, .175f, .175f);
  for (int i = 0; i < 4; i++)
  {
    test.vertices()[i].r = 1;
    test.vertices()[i].g = 1;
    test.vertices()[i].b = 1;
  }

  bool show = true;
  while (core.isRunning()) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    if (ImGui::Begin("Framerate"))
    {
      ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();

    Sane::LogHandler::Display();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    /*
        glm::mat4 m = glm::mat4(1.f);
        glm::mat4 p = core.getPersProjection();
        glm::mat4 mvp = p * m;

        pool.ClearBuffer();

        float out[16]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

        moveTest();
        moc->TransformVertices(&p[0][0], (float*)test.vertices(), out, 4, MaskedOcclusionCulling::VertexLayout(28, 4, 8));

        pool.RenderTriangles(out, test.indices(), 2);

        verticesMgr.reset();
        indicesMgr.reset();

        pool.Flush();
        int countRenderedShapes = 0;
        for (auto& shape : shapes)
        {
          moc->TransformVertices(&p[0][0], (float*)shape.vertices(), out, 4, MaskedOcclusionCulling::VertexLayout(28, 4, 8));

          MaskedOcclusionCulling::CullingResult result = pool.TestTriangles(out, shape.indices(), 2); // moc->TestTriangles(out, shape.indices(), 2);
          if (result == MaskedOcclusionCulling::CullingResult::VISIBLE)
          {
            //verticesMgr.add(shape.verticesCount(), shape.vertices());
            //indicesMgr.add(shape.indicesCount(), shape.indices());
            //countRenderedShapes++;
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
    */

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(core);
    }

    core.update();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  MaskedOcclusionCulling::Destroy(moc);
  Sane::LogHandler::Destroy();
}
