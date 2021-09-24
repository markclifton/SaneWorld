#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <filesystem>
#include <functional>
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

#define USE_D3D 1
#include <MaskedOcclusionCulling.hpp>

float deltaX = 0.f;
float deltaY = 0.f;
float deltaZ = 0.f;

class Sandbox : public Sane::App
{
public:
  Sandbox()
    : display("Sandbox", WIDTH, HEIGHT)
    , ssink(Sane::Logging::GetLogSink())
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(display, true);
    ImGui_ImplOpenGL3_Init(nullptr);

    moc = MaskedOcclusionCulling::Create();
    moc->SetResolution(WIDTH, HEIGHT);
  }

  ~Sandbox()
  {
    MaskedOcclusionCulling::Destroy(moc);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  virtual void Run() override
  {
    SANE_WARN("Current path is {}", std::filesystem::current_path().c_str());

    struct Vertex {
      float x, y, z, w, r, g, b;
    };

    unsigned int indices[] = {
        0, 1, 2, // first triangle
        2, 3, 0,  // second triangle
        4, 5, 6, // first triangle
        6, 7, 4  // second triangle
    };

    float vertices[] = {
      // positions          // colors           // texture coords
       0.5f,  0.5f, -1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
       0.5f, -0.5f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, -1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
      -0.5f,  0.5f, -1.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };

    Sane::Texture tex("images/texture.png");

    Sane::ShaderProgram sProg(vs_modern_tex, fs_modern_tex);
    GLint mvp_location = sProg.GetUniformLocaition("MVP");
    Sane::VertexAttrib vPos(sProg.GetAttribLocation("aPos"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    Sane::VertexAttrib vCol(sProg.GetAttribLocation("aColor"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    Sane::VertexAttrib vUV(sProg.GetAttribLocation("aTexCoord"), 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    Sane::Buffer vbo(GL_ARRAY_BUFFER);
    Sane::Buffer ibo(GL_ELEMENT_ARRAY_BUFFER);
    {
      sProg.Bind();

      vbo.Bind();
      vbo.BufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);

      ibo.Bind();
      ibo.BufferData(sizeof(indices), indices, GL_STATIC_DRAW);

      ibo.Unbind();
      vbo.Unbind();

      sProg.Unbind();
    }

    auto createSquare = [](Vertex verts[4], float x, float y, float z, float w, float h)
    {
      verts[0] = { x - w, y + h, z, 1, 1, 0, 0 };
      verts[1] = { x - w, y - h, z, 1, 0, 1, 0 };
      verts[2] = { x + w, y - h, z, 1, 0, 1, 1 };
      verts[3] = { x + w, y + h, z, 1, 0, 0, 1 };
    };

    auto moveSquare = [](Vertex verts[4], float deltaX, float deltaY, float deltaZ)
    {
      verts[0].x += deltaX;
      verts[1].x += deltaX;
      verts[2].x += deltaX;
      verts[3].x += deltaX;

      verts[0].y += deltaY;
      verts[1].y += deltaY;
      verts[2].y += deltaY;
      verts[3].y += deltaY;

      verts[0].z += deltaZ;
      verts[1].z += deltaZ;
      verts[2].z += deltaZ;
      verts[3].z += deltaZ;
    };

    Vertex s[8];
    createSquare(s, -.5f, 0, -3, .5f, .5f);
    createSquare(&s[4], -.5f, 0, -3, .5f, .5f);

    Sane::ShaderProgram sProg_no_tex(vs_modern, fs_modern);
    Sane::VertexAttrib vPos_no_tex(sProg_no_tex.GetAttribLocation("vPos"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    Sane::VertexAttrib vCol_no_tex(sProg_no_tex.GetAttribLocation("vCol"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
    GLint mvp_location_no_tex = sProg_no_tex.GetUniformLocaition("MVP");
    Sane::Buffer vbo_no_tex(GL_ARRAY_BUFFER);
    Sane::Buffer ibo_no_tex(GL_ELEMENT_ARRAY_BUFFER);
    {
      sProg_no_tex.Bind();

      vbo_no_tex.Bind();
      vbo_no_tex.BufferData(sizeof(s), &s[0], GL_STATIC_DRAW);

      ibo_no_tex.Bind();
      ibo_no_tex.BufferData(sizeof(indices), indices, GL_STATIC_DRAW);

      ibo_no_tex.Unbind();
      vbo_no_tex.Unbind();

      sProg_no_tex.Unbind();
    }

    float direction = .1f;
    float min = -7.f;
    float max = -1.f;

    bool visible = true;

    Sane::Framebuffer scene(WIDTH, HEIGHT);
    ImVec2 sceneSize;
    while (display.IsRunning()) {
      if (*(glm::vec2*)&sceneSize != scene.GetSize())
      {
        scene.Resize(static_cast<size_t>(sceneSize.x), static_cast<size_t>(sceneSize.y));
      }

      scene.Bind();
      {
        scene.Clear();

        float ratio = sceneSize.x / sceneSize.y;
        glm::mat4 m = glm::mat4(1.f);
        glm::mat4 p = display.GetPersProjection();// glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, 100.f);
        glm::mat4 mvp = p * m;

        {
          moveSquare(&s[0], deltaX, -deltaY, deltaZ);
          SANE_INFO("z: {}", s[0].z);

          {
            sProg_no_tex.Bind();

            vbo_no_tex.Bind();
            vbo_no_tex.BufferData((visible ? sizeof(s) : sizeof(s) / 2), &s[0], GL_STATIC_DRAW);
            vbo_no_tex.Unbind();

            sProg_no_tex.Unbind();
          }

          moc->ClearBuffer();

          float xformVerts[32];

          moc->TransformVertices(&p[0][0], (float*)&s[0], xformVerts, 8, MaskedOcclusionCulling::VertexLayout(28, 4, 8));

          moc->RenderTriangles(&xformVerts[0], indices, 2);
          auto result = moc->TestTriangles(&xformVerts[16], indices, 2);

          SANE_WARN("Square visible: {}", !result);
          visible = !result;
        }

        {
          sProg_no_tex.Bind();

          vbo_no_tex.Bind();
          vPos_no_tex.Enable();
          vCol_no_tex.Enable();

          ibo_no_tex.Bind();
          glUniformMatrix4fv(mvp_location_no_tex, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
          glDrawElements(GL_TRIANGLES, (visible ? 12 : 6), GL_UNSIGNED_INT, (void*)0);
          ibo_no_tex.Unbind();

          vCol_no_tex.Disable();
          vPos_no_tex.Disable();
          vbo_no_tex.Unbind();

          sProg_no_tex.Unbind();
        }

        if (0)
        {
          sProg.Bind();

          tex.Bind();

          vbo.Bind();
          vPos.Enable();
          vCol.Enable();
          vUV.Enable();

          ibo.Bind();
          glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
          glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
          ibo.Unbind();

          vUV.Disable();
          vCol.Disable();
          vPos.Disable();
          vbo.Unbind();

          tex.Unbind();

          sProg.Unbind();
        }
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

      //UpdateDbgConsole();
      ssink->Render();

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

  const Sane::Logging::LogSink ssink;
  Sane::Display display;
  MaskedOcclusionCulling* moc;
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
