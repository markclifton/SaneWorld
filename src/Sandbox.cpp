#include <sane/entrypoint.hpp>

#include <sane/ecs/common.hpp>
#include <sane/layers/common.hpp>

class Sandbox : public Sane::App
{
public:
  Sandbox()
    : Sane::App("Sandbox")
    , gameWindow(framebuffer.GetAttachment(0))
    , movementSystem(Registry())
    , physicsSystem(Registry())
    , collisionSystem(Registry())
  {
    DisplayConsole(true);
    PushOverlay(&gameWindow);
    PushOverlay(&fpsCounter);

    const auto entity = Registry().create();
    Registry().emplace<Sane::Components::Player>(entity);
    Registry().emplace<Sane::Components::Position>(entity, 0.f, 0.f, 0.f, 1.f);
    Registry().emplace<Sane::Components::Translation>(entity, 0.f, 0.f, 0.f);
    Registry().emplace<Sane::Components::Scale>(entity, .1f, .1f, 1.f);
    Registry().emplace<Sane::Components::Collidable>(entity, true);
    Registry().emplace<Sane::Components::Gravity>(entity, .01f);

    for (int i = 0; i < 25; i++)
    {
      const auto entity = Registry().create();
      Registry().emplace<Sane::Components::Position>(entity, -1.f + .1f * i, -.25f, 0.f, 1.f);
      Registry().emplace<Sane::Components::Translation>(entity, 0.f, 0.f, 0.f);
      Registry().emplace<Sane::Components::Scale>(entity, .1f, .1f, .1f);
      Registry().emplace<Sane::Components::Collidable>(entity, true);
    }

    PushSystem(&movementSystem);
    PushSystem(&physicsSystem);
    PushSystem(&collisionSystem);
  }

  ~Sandbox()
  {
  }

private:
  Sane::FpsCounter fpsCounter;
  Sane::GameWindow gameWindow;
  Sane::Systems::Movement movementSystem;
  Sane::Systems::Physics physicsSystem;
  Sane::Systems::Collision collisionSystem;
};

Sane::App* Sane::CreateApp()
{
  return new Sandbox();
}
