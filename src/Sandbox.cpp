
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
#include <sane/layers/gamewindow.hpp>

#include <sane/rendering/renderer2d.hpp>

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

class Sandbox : public Sane::App
{
public:
  Sandbox()
    : Sane::App("Sandbox")
    , surface(1280, 720)
    , gameWindow(surface.GetColorAttachment())
  {
    PushOverlay(&console);
    PushOverlay(&fpsCounter);
    PushOverlay(&gameWindow);
  }

  ~Sandbox()
  {
  }

private:
  RenderSurface surface;
  Sane::Console console;
  Sane::FpsCounter fpsCounter;
  Sane::GameWindow gameWindow;
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
