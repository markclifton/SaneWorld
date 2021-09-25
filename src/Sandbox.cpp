
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
    : frameId(frameId), Sane::Layer("GameFrame")
  {}

  virtual void Render() override
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
    : Sane::App("Sandbox")
    , surface(1280, 720)
    , frame(surface.GetColorAttachment())
  {
    PushLayer(&console);
    PushLayer(&fpsCounter);
    PushLayer(&frame);
  }

  ~Sandbox()
  {
  }

private:
  RenderSurface surface;
  Sane::Console console;
  Sane::FpsCounter fpsCounter;
  GameFrame frame;
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
