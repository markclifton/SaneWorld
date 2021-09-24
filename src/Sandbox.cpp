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

class ImguiConsole : public Sane::Layer
{
public:
  ImguiConsole()
    : logSink(Sane::Logging::GetLogSink())
  {}

  virtual void Process() override
  {
    logSink->Render();
  }

private:
  const Sane::Logging::LogSink logSink;
};

class ImguiFpsCounter : public Sane::Layer
{
public:
  virtual void Process() override
  {
    if (ImGui::Begin("Framerate"))
    {
      ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();
  }
};

class KeyEvent : public Sane::EventDispatcher
{
public:
  KeyEvent() = default;
  void Submit()
  {
    int key = 52;

    Sane::Event event;
    event.id = 0;
    event.data = &key;
    event.size = sizeof(key);

    SubmitEvent(event);
    //  SANE_INFO("Submitted event: {}", *(int*)event.data);

  }
};

class KeyHandler : public Sane::EventListener
{
public:
  KeyHandler()
    : Sane::EventListener("KeyHandler")
  {
  }

  virtual bool ProcessEvent(Sane::Event& evt) override
  {
    SANE_INFO("Processed event: {} w/ {}", *(int*)evt.data, Name());
    return false;
  }

};


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
    KeyHandler kh;
    KeyEvent ke;

    ImguiConsole console;
    ImguiFpsCounter fpsCounter;
    layers.PushOverlay(&console);
    layers.PushOverlay(&fpsCounter);

    unsigned int box_indices[] = {
      0, 2, 3, 0, 3, 1,
      2, 6, 7, 2, 7, 3,
      6, 4, 5, 6, 5, 7,
      4, 0, 1, 4, 1, 5,
      0, 4, 6, 0, 6, 2,
      1, 5, 7, 1, 7, 3,
    };

    SANE_WARN("Current path is {}", std::filesystem::current_path().c_str());

    struct Vertex {
      float x, y, z, w, r, g, b;
    };

    auto getBBox = [](Vertex verts[8], float minx, float miny, float minz, float maxx, float maxy, float maxz) {
      verts[0] = { minx, maxy, maxz, 1, 1, 0, 0 };
      verts[1] = { minx, miny, maxz, 1, 1, 0, 0 };
      verts[2] = { maxx, maxy, maxz, 1, 1, 0, 0 };
      verts[3] = { maxx, miny, maxz, 1, 1, 0, 0 };

      verts[4] = { minx, maxy, minz, 1, 1, 0, 0 };
      verts[5] = { minx, miny, minz, 1, 1, 0, 0 };
      verts[6] = { maxx, maxy, minz, 1, 1, 0, 0 };
      verts[7] = { maxx, miny, minz, 1, 1, 0, 0 };
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

    Vertex cube[8];
    getBBox(cube, -5, -1, -10, -4, 0, -9);

    Sane::ShaderProgram sProg_no_tex(vs_modern, fs_modern);
    Sane::VertexAttrib vPos_no_tex(sProg_no_tex.GetAttribLocation("vPos"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    Sane::VertexAttrib vCol_no_tex(sProg_no_tex.GetAttribLocation("vCol"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
    GLint mvp_location_no_tex = sProg_no_tex.GetUniformLocaition("MVP");
    Sane::Buffer vbo_no_tex(GL_ARRAY_BUFFER);
    Sane::Buffer ibo_no_tex(GL_ELEMENT_ARRAY_BUFFER);

    Sane::Buffer vbo_no_tex_cube(GL_ARRAY_BUFFER);
    Sane::Buffer ibo_no_tex_cube(GL_ELEMENT_ARRAY_BUFFER);
    {
      sProg_no_tex.Bind();

      vbo_no_tex.Bind();
      vbo_no_tex.BufferData(sizeof(s), &s[0], GL_STATIC_DRAW);
      vbo_no_tex.Unbind();

      ibo_no_tex.Bind();
      ibo_no_tex.BufferData(sizeof(indices), indices, GL_STATIC_DRAW);
      ibo_no_tex.Unbind();

      vbo_no_tex_cube.Bind();
      vbo_no_tex_cube.BufferData(8 * sizeof(Vertex), &s[0], GL_STATIC_DRAW);
      vbo_no_tex_cube.Unbind();

      ibo_no_tex_cube.Bind();
      ibo_no_tex_cube.BufferData(sizeof(box_indices), box_indices, GL_STATIC_DRAW);
      ibo_no_tex_cube.Unbind();

      sProg_no_tex.Unbind();
    }

    float direction = .1f;
    float min = -7.f;
    float max = -1.f;

    bool squareVisible = true;
    bool cubeVisible = true;

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
        glm::mat4 m = glm::rotate(glm::mat4(1.f), (float)glfwGetTime(), { 0, 0, 1 });
        glm::mat4 p = display.GetPersProjection();
        glm::mat4 mvp = p * m;

        {
          moveSquare(&s[0], deltaX, -deltaY, deltaZ);
          {
            sProg_no_tex.Bind();

            vbo_no_tex.Bind();
            vbo_no_tex.BufferData((squareVisible ? sizeof(s) : sizeof(s) / 2), &s[0], GL_STATIC_DRAW);
            vbo_no_tex.Unbind();

            vbo_no_tex_cube.Bind();
            vbo_no_tex_cube.BufferData(sizeof(cube), (void*)&cube, GL_STATIC_DRAW);
            vbo_no_tex_cube.Unbind();

            sProg_no_tex.Unbind();
          }

          moc->ClearBuffer();

          float xformVerts[32];

          moc->TransformVertices(&p[0][0], (float*)&s[0], xformVerts, 8, MaskedOcclusionCulling::VertexLayout(28, 4, 8));

          moc->RenderTriangles(&xformVerts[0], indices, 2);
          squareVisible = !moc->TestTriangles(&xformVerts[16], indices, 2);

          float outVerts[32];
          moc->TransformVertices(&mvp[0][0], (float*)&cube, outVerts, 8, { 28, 4, 8 });
          cubeVisible = !moc->TestTriangles(outVerts, box_indices, 12);
        }

        {
          sProg_no_tex.Bind();

          {
            vbo_no_tex.Bind();
            vPos_no_tex.Enable();
            vCol_no_tex.Enable();

            ibo_no_tex.Bind();
            glUniformMatrix4fv(mvp_location_no_tex, 1, GL_FALSE, (const GLfloat*)&p[0][0]);
            glDrawElements(GL_TRIANGLES, (squareVisible ? 12 : 6), GL_UNSIGNED_INT, (void*)0);
            ibo_no_tex.Unbind();

            vCol_no_tex.Disable();
            vPos_no_tex.Disable();
            vbo_no_tex.Unbind();
          }

          if (cubeVisible)
          {
            vbo_no_tex_cube.Bind();
            vPos_no_tex.Enable();
            vCol_no_tex.Enable();

            ibo_no_tex_cube.Bind();
            glUniformMatrix4fv(mvp_location_no_tex, 1, GL_FALSE, (const GLfloat*)&mvp[0][0]);
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void*)0);
            ibo_no_tex_cube.Unbind();

            vCol_no_tex.Disable();
            vPos_no_tex.Disable();
            vbo_no_tex_cube.Unbind();
          }

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
          glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&p[0][0]);
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

      for (auto& layer : layers)
      {
        layer->Process();
      }


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
  MaskedOcclusionCulling* moc;
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
