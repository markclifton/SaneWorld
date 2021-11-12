#include <sane/entrypoint.hpp>

#include <sane/ecs/common.hpp>
#include <sane/systems/common.hpp>

class Sandbox : public Sane::App
{
public:
  Sandbox()
    : Sane::App("Sandbox")
    , gameWindow(framebuffer.GetAttachment(0))
  {
    DisplayConsole(true);
    PushOverlay(&gameWindow);
    PushOverlay(&fpsCounter);
  }

  ~Sandbox()
  {
  }

private:
  Sane::FpsCounter fpsCounter;
  Sane::GameWindow gameWindow;
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
