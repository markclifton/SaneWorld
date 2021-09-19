#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <filesystem>
#include <sstream>
#include <vector>

#include <shader_code.hpp>

#include <sane/entrypoint.hpp>
#include <sane/core/display.hpp>
#include <sane/graphics/buffer.hpp>
#include <sane/graphics/shaderprogram.hpp>
#include <sane/graphics/framebuffer.hpp>
#include <sane/graphics/texture.hpp>
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
    CreateDbgConsole("Debug Console");
    SANE_WARN("Current path is " << std::filesystem::current_path());

    Sane::ShaderProgram sProg(vs_modern_tex, fs_modern_tex);
    GLint mvp_location = sProg.GetUniformLocaition("MVP");
    Sane::VertexAttrib vPos(sProg.GetAttribLocation("aPos"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    Sane::VertexAttrib vCol(sProg.GetAttribLocation("aColor"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    Sane::VertexAttrib vUV(sProg.GetAttribLocation("aTexCoord"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    Sane::Buffer vbo(GL_ARRAY_BUFFER);
    Sane::Buffer ibo(GL_ELEMENT_ARRAY_BUFFER);

    float vertices[] = {
      // positions          // colors           // texture coords
       0.5f,  0.5f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
       0.5f, -0.5f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
      -0.5f,  0.5f, -1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    sProg.Bind();

    vbo.Bind();
    vbo.BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);

    ibo.Bind();
    ibo.BufferData(sizeof(indices), indices, GL_STATIC_DRAW);

    vPos.Enable();
    vCol.Enable();
    vUV.Enable();

    Sane::Texture tex("images/texture.png");

    Sane::Framebuffer scene(WIDTH, HEIGHT);
    ImVec2 sceneSize;
    while (display.IsRunning()) {
      if (*(glm::vec2*)&sceneSize != scene.GetSize())
      {
        scene.Resize(sceneSize.x, sceneSize.y);
      }

      scene.Bind();
      {
        float ratio = 16.f / 9.f;//sceneSize.x / sceneSize.y;
        glm::mat4 m = glm::mat4(1.f);
        glm::mat4 p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, 100.f);
        glm::mat4 mvp = p * m;

        tex.Bind();

        scene.Clear();
        sProg.Bind();
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
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

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
      ImGui::Begin("GameWindow");
      {
        ImGui::BeginChild("GameRender");
        sceneSize = ImGui::GetWindowSize();
        uint64_t temp = scene.GetAttachment(0);
        ImGui::Image((void*)temp, sceneSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndChild();
      }
      ImGui::End();
      ImGui::PopStyleVar();

      UpdateDbgConsole();

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
