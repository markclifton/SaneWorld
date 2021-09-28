
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

#include <sane/math/vector.hpp>

class Sandbox : public Sane::App
{
public:
  Sandbox()
    : Sane::App("Sandbox")
    , console()
    , gameWindow(framebuffer.GetAttachment(0))
  {
    PushOverlay(&console);
    PushOverlay(&gameWindow);
    PushOverlay(&fpsCounter);
  }

  ~Sandbox()
  {
  }

private:
  Sane::Console console;
  Sane::FpsCounter fpsCounter;
  Sane::GameWindow gameWindow;
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
