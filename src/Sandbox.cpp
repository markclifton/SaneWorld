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

#define USE_D3D 1
#include "MaskedOcclusionCulling/MaskedOcclusionCulling.h"
#include "MaskedOcclusionCulling/CullingThreadpool.h"

#include <../dependencies/sane/src/debugging/logging.hpp>
#include <../dependencies/sane/src/debugging/imgui_console.hpp>

#include <app.hpp>
#include <entrypoint.hpp>

class Sandbox : public Sane::App
{
public:
  Sandbox()
  {
#if defined(WIN32)
    FreeConsole();
#endif
    ENABLE_LOGS();
    core = new OGL::OGLCore("Sandbox", WIDTH, HEIGHT);
  }

  ~Sandbox()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    DISABLE_LOGS();
  }

  virtual void Run() override
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(*core, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    const char* DebugConsole = "Debug Console";
    Sane::ImguiConsole debugConsole(DebugConsole);

    OGL::ShaderProgram shaders(vs_modern, fs_modern);

    GLint mvp_location = shaders.getUniformLocaition("MVP");
    OGL::VertexAttrib vpos(shaders.getAttribLocation("vPos"), 3, GL_FLOAT,
      GL_FALSE, sizeof(OGLVertex), (void*)0);

    OGL::VertexAttrib vcol(shaders.getAttribLocation("vCol"), 3, GL_FLOAT,
      GL_FALSE, sizeof(OGLVertex),
      (void*)(4 * sizeof(float)));

    OGL::VerticesManager verticesMgr;
    OGL::IndicesManager indicesMgr;

    while (core->isRunning()) {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
      if (ImGui::Begin("Framerate"))
      {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      }
      ImGui::End();

      PROCESS_LOGS(debugConsole);

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(*core);
      }

      core->update();
    }
  }

private:
  const int WIDTH = 1280;
  const int HEIGHT = 720;

  OGL::OGLCore* core{ nullptr };
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
