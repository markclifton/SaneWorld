#include <sane/entrypoint.hpp>

#include <sane/ecs/common.hpp>
#include <sane/systems/common.hpp>

class SaneWorld : public Sane::App
{
public:
  SaneWorld()
    : Sane::App("SaneWorld", 1920, 1080)
    , camSystem(Registry())
    , grid(Registry(), 16)
    , loader(Registry())
  {
#ifndef NDEBUG
    DisplayConsole(true);
    PushOverlay(&fpsCounter);
#endif

    PushLayer(&loader);
    PushLayer(&grid);
    PushLayer(&camSystem);

    StartGame();
  }

  void StartGame()
  {
    Sane::LoadEvent le("level/sandbox.xml");
    SubmitEvent(std::make_unique<Sane::Event>(Sane::kLoadEvent, &le, sizeof(le)));
  }

private:
  Sane::FpsCounter fpsCounter;
  Sane::ECS::Camera camSystem;
  Sane::ECS::Grid grid;
  Sane::ECS::Loader loader;
};

Sane::App* Sane::CreateApp()
{
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#else
  spdlog::set_level(spdlog::level::warn);
#endif

  return new SaneWorld();
}
