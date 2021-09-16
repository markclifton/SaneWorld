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
#include <sane/graphics/framebuffer.hpp>

#include <sane/debugging/logging.hpp>
#include <sane/debugging/console.hpp>

class Sandbox : public Sane::App
{
public:
  Sandbox()
    : display("Sandbox", WIDTH, HEIGHT)
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(display, true);
    ImGui_ImplOpenGL3_Init(nullptr);
  }

  ~Sandbox()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  virtual void Run() override
  {
    Sane::Console debugConsole("Debug Console");

    Sane::ShaderProgram sProg(vs_modern, fs_modern);
    GLint mvp_location = sProg.GetUniformLocaition("MVP");
    Sane::VertexAttrib vPos(sProg.GetAttribLocation("vPos"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    Sane::VertexAttrib vCol(sProg.GetAttribLocation("vCol"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
    VerticesManager vMgr;
    IndicesManager iMgr;

    Rectangle test(0, 0, -1, 1, 1);
    vMgr.add(test.verticesCount(), test.vertices());
    iMgr.add(test.indicesCount(), test.indices());

    sProg.Bind();

    vMgr.bind();
    vMgr.buffer(GL_STATIC_DRAW);
    iMgr.buffer(GL_STATIC_DRAW);

    vPos.Enable();
    vCol.Enable();

    Sane::Framebuffer scene(WIDTH, HEIGHT);

    while (display.IsRunning()) {
      glm::mat4 m = glm::mat4(1.f);
      glm::mat4 p = display.GetPersProjection();
      glm::mat4 mvp = p * m;

      scene.Bind();
      {
        scene.Clear();
        sProg.Bind();
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
        iMgr.draw();
      }
      scene.Unbind();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
      if (ImGui::Begin("Framerate"))
      {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      }
      ImGui::End();

      ImGui::Begin("GameWindow");
      {
        ImGui::BeginChild("GameRender");
        ImVec2 wsize = ImGui::GetWindowSize();
        ImGui::Image(reinterpret_cast<ImTextureID>(scene.GetAttachment(0)), wsize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
      }
      ImGui::End();

      PROCESS_LOGS(debugConsole);

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(display);
      }

      display.Update();
    }
  }

private:
  const size_t WIDTH = 1280;
  const size_t HEIGHT = 720;

  Sane::Display display;
};

Sane::App* Sane::CreateApp()
{
  ENABLE_LOGS();
  return new Sandbox();
}
