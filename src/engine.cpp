#include "engine.hpp"
#include "font.hpp"

namespace remotemo {
Main_SDL_handler::~Main_SDL_handler()
{
  ::SDL_Log("Main_SDL_handler Destructor: m_do_sdl_quit: %s",
      m_do_sdl_quit ? "True" : "False");
  if (m_do_sdl_quit) {
    ::SDL_Log("Quit\n");
    ::SDL_Quit();
  } else {
    ::SDL_Log("QuitSubSystem(%d)\n", m_sdl_subsystems);
    if (m_sdl_subsystems != 0) {
      ::SDL_QuitSubSystem(m_sdl_subsystems);
    }
  }
}

Main_SDL_handler::Main_SDL_handler(Main_SDL_handler&& other) noexcept
    : m_do_sdl_quit(other.m_do_sdl_quit),
      m_sdl_subsystems(other.m_sdl_subsystems)
{
  other.m_do_sdl_quit = false;
  other.m_sdl_subsystems = 0;
}

Main_SDL_handler& Main_SDL_handler::operator=(
    Main_SDL_handler&& other) noexcept
{
  std::swap(m_do_sdl_quit, other.m_do_sdl_quit);
  std::swap(m_sdl_subsystems, other.m_sdl_subsystems);
  return *this;
}

bool Main_SDL_handler::setup(::Uint32 init_flags)
{
  ::SDL_Log("SDL_Init(%d)", init_flags);
  if (::SDL_Init(init_flags) < 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init() failed: %s\n",
        ::SDL_GetError());
    return false;
  }
  m_sdl_subsystems = init_flags;

  if (::SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear") == SDL_FALSE) {
    ::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_SetHint() (scale quality to linear) failed: %s\n",
        ::SDL_GetError());
  }
  return true;
}

std::unique_ptr<Engine> Engine::create(const Config& config)
{
  ::SDL_Log("Remotemo::initialize() with config.m_cleanup_all: %s",
      config.cleanup_all() ? "True" : "False");

  // Setup handling of resources and their cleanup.
  // This is done right here at the start so that if something fails, then
  // everything that was handed over will be taken care of no matter where
  // in the setup process something failed.
  // Note that the order in which the following objects are created is
  // important so that if the setup process fails, then everything that should
  // get cleaned up, does in the right order.
  Main_SDL_handler main_sdl_handler {config.cleanup_all()};
  std::optional<Window> window {};
  Res_handler<SDL_Window> conf_window {
      config.window().raw_sdl, config.cleanup_all()};
  std::optional<Renderer> renderer {};
  Res_handler<SDL_Renderer> conf_renderer {
      config.window().raw_sdl == nullptr
          ? nullptr
          : ::SDL_GetRenderer(config.window().raw_sdl),
      config.cleanup_all()};
  Res_handler<SDL_Texture> backgr_texture {
      config.background().raw_sdl, config.cleanup_all()};
  Res_handler<SDL_Texture> font_texture {
      config.font().raw_sdl, config.cleanup_all()};

  if (!config.validate(conf_renderer.res())) {
    return nullptr;
  }
  if (!main_sdl_handler.setup(sdl_init_flags)) {
    return nullptr;
  }
  window = Window::create(config.window(), std::move(conf_window));
  if (!window) {
    return nullptr;
  }
  renderer = Renderer::create(window->res(), std::move(conf_renderer));
  if (!renderer) {
    return nullptr;
  }
  auto font =
      Font::create(config.font(), std::move(font_texture), renderer->res());
  if (!font) {
    return nullptr;
  }
  auto background = Background::create(
      config.background(), std::move(backgr_texture), renderer->res());
  if (!background) {
    return nullptr;
  }
  auto text_display = Text_display::create(
      std::move(*font), config.text_area(), renderer->res());
  if (!text_display) {
    return nullptr;
  }
  return std::make_unique<Engine>(std::move(main_sdl_handler),
      std::move(*window), std::move(*renderer), std::move(*background),
      std::move(*text_display));
}

void Engine::main_loop_once()
{
  SDL_Log("Main loop once!");
}
} // namespace remotemo
