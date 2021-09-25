#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <shader_code.hpp>

#include <sane/entrypoint.hpp>
#include <sane/core/display.hpp>
#include <sane/graphics/buffer.hpp>
#include <sane/graphics/shaderprogram.hpp>
#include <sane/graphics/framebuffer.hpp>
#include <sane/graphics/texture.hpp>
#include <sane/logging/log.hpp>
#include <sane/layers/console.hpp>
#include <sane/layers/fpscounter.hpp>

class RenderSurface : public Sane::Events::Listener
{
public:
  RenderSurface(size_t width, size_t height)
    : framebuffer(width, height)
    , Sane::Events::Listener("RenderSurface")
  {
  }

  virtual bool ProcessEvent(Sane::Event& evt) override
  {
    switch (evt.action)
    {
    case 1000:
    {
      ImVec2 size = *(ImVec2*)evt.data;
      framebuffer.Resize(static_cast<size_t>(size.x), static_cast<size_t>(size.y));
      return true;
    }
    default:
      return false;
    }
  }

  uint64_t GetColorAttachment() {
    return framebuffer.GetAttachment(0);
  }

  void Bind()
  {
    framebuffer.Bind();
  }

  void Unbind()
  {
    framebuffer.Unbind();
  }

  void Clear()
  {
    framebuffer.Clear();
  }

private:
  Sane::Framebuffer framebuffer;
};

class GameFrame : public Sane::Layer, public Sane::Events::Dispatcher
{
public:
  GameFrame(uint64_t frameId)
    : frameId(frameId)
  {}

  virtual void Process() override
  {
    auto newSize = ImGui::GetWindowSize();
    if (frameSize.x != newSize.x
      || frameSize.y != newSize.y)
    {
      frameSize = newSize;

      Sane::Event event;
      event.action = 1000;
      event.data = &frameSize;
      event.size = sizeof(frameSize);
      SubmitEvent(event);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::Begin("GameWindow");
    {
      ImGui::BeginChild("GameRender");
      ImGui::Image((void*)frameId, frameSize, ImVec2(0, 1), ImVec2(1, 0));
      ImGui::EndChild();
    }
    ImGui::End();
    ImGui::PopStyleVar();
  }

private:
  uint64_t frameId;
  ImVec2 frameSize;
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
  }

  ~Sandbox()
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void BeginFrame()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
  }

  void EndFrame()
  {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(display);
    }
  }

  virtual void Run() override
  {
    RenderSurface surface(1280, 720);

    Sane::Console console;
    Sane::FpsCounter fpsCounter;
    GameFrame frame(surface.GetColorAttachment());
    overlay_layers_.PushLayer(&console);
    overlay_layers_.PushLayer(&fpsCounter);
    overlay_layers_.PushLayer(&frame);

    SANE_WARN("Current path is {}", std::filesystem::current_path().c_str());

    while (display.IsRunning())
    {
      BeginFrame();
      surface.Bind();
      surface.Clear();
      for (auto& layer : layers_)
      {
        layer->Process();
      }
      surface.Unbind();

      for (auto& layer : overlay_layers_)
      {
        layer->Process();
      }

      EndFrame();

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
  return new Sandbox();
}
