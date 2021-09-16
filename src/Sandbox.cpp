#ifdef WIN32
#include <windows.h>
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <sstream>
#include <vector>

#include <shape_helper.hpp>
#include <shader_code.hpp>

#include <sane/entrypoint.hpp>
#include <sane/core/display.hpp>
#include <sane/graphics/buffer.hpp>
#include <sane/graphics/shaderprogram.hpp>

#include <sane/debugging/logging.hpp>
#include <sane/debugging/console.hpp>

class Sandbox : public Sane::App
{
public:
  Sandbox()
  {
#if defined(WIN32)
    FreeConsole();
#endif
    ENABLE_LOGS();
    core = new Sane::Display("Sandbox", WIDTH, HEIGHT);
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
    Sane::Console debugConsole(DebugConsole);

    Sane::ShaderProgram shaders(vs_modern, fs_modern);

    GLint mvp_location = shaders.GetUniformLocaition("MVP");

    Sane::VertexAttrib vpos(shaders.GetAttribLocation("vPos"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    Sane::VertexAttrib vcol(shaders.GetAttribLocation("vCol"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));

    VerticesManager verticesMgr;
    IndicesManager indicesMgr;

    while (core->IsRunning()) {
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

      core->Update();
    }
  }

private:
  const int WIDTH = 1280;
  const int HEIGHT = 720;

  Sane::Display* core{ nullptr };
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
