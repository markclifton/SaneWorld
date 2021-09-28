#include <sane/entrypoint.hpp>
#include <sane/layers/console.hpp>
#include <sane/layers/fpscounter.hpp>
#include <sane/layers/gamewindow.hpp>

class Sandbox : public Sane::App
{
public:
  Sandbox()
    : Sane::App("Sandbox")
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
