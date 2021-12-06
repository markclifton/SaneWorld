#include <sane/entrypoint.hpp>

#include <sane/ecs/common.hpp>
#include <sane/systems/common.hpp>

class SaneWorld : public Sane::App
{
public:
  SaneWorld()
    : Sane::App("SaneWorld")
    , gameWindow(framebuffer.GetAttachment(0))
    , scene(Registry())
    , camSystem(Registry())
    , grid(Registry(), 16)
    , creator(Registry())
  {
    DisplayConsole(true);

    {
      const auto camera0 = Registry().create();
      Registry().emplace<Sane::Components::Camera>(camera0, true, glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f), 10.0f);
      Registry().emplace<Sane::Components::Position>(camera0, glm::vec4(2.f, -4.f, 0.f, 0.f));
      Registry().emplace<Sane::Components::Rotation>(camera0, 0.f, 0.f, 0.f);
      Registry().emplace<Sane::Components::RenderContext>(camera0, 2560, 1440);
      Registry().emplace<Sane::Components::Player>(camera0, true);
    }

    PushOverlay(&gameWindow);
    PushOverlay(&fpsCounter);
    PushLayer(&creator);
    PushLayer(&grid);
    PushLayer(&scene);
    PushLayer(&camSystem);

    creator.LoadFile("gamedata/sandbox.xml");
  }

private:
  Sane::FpsCounter fpsCounter;
  Sane::GameWindow gameWindow;
  Sane::ECS::Scene scene;
  Sane::ECS::Camera camSystem;
  Sane::ECS::Grid grid;
  Sane::ECS::Creator creator;
};

Sane::App* Sane::CreateApp()
{
  return new SaneWorld();
}
