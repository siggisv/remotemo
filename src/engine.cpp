#include "engine.hpp"

namespace remotemo {
Cleanup_handler::~Cleanup_handler()
{
  if (m_window != nullptr) {
    ::SDL_DestroyWindow(m_window);
  }
  ::SDL_Log("Cleanup_handler Destructor: m_do_sdl_quit: %s",
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

Cleanup_handler::Cleanup_handler(Cleanup_handler&& other) noexcept
    : m_do_sdl_quit(other.m_do_sdl_quit),
      m_sdl_subsystems(other.m_sdl_subsystems), m_window(other.m_window)
{
  other.m_do_sdl_quit = false;
  other.m_sdl_subsystems = 0;
  other.m_window = nullptr;
}

Cleanup_handler& Cleanup_handler::operator=(Cleanup_handler&& other) noexcept
{
  std::swap(m_do_sdl_quit, other.m_do_sdl_quit);
  std::swap(m_sdl_subsystems, other.m_sdl_subsystems);
  std::swap(m_window, other.m_window);
  return *this;
}

bool Renderer::setup(SDL_Window* window)
{
  res(::SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE));
  if (res() == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_CreateRenderer() failed: %s\n", ::SDL_GetError());
    return false;
  }
  is_owned(true);
  return true;
}

std::unique_ptr<Engine> Engine::create(const Config& config)
{
  ::SDL_Log("Remotemo::initialize() with config.m_cleanup_all: %s",
      config.cleanup_all() ? "True" : "False");

  // Setup handling of cleanup.
  // This is done right here at the start so that if something fails, then
  // everything that was handed over will be taken care of no matter where
  // in the setup process something failed.
  Cleanup_handler cleanup_handler {
      config.cleanup_all(), config.window().raw_sdl};

  Renderer renderer {config.window().raw_sdl, config.cleanup_all()};
  Res_handler<SDL_Texture> backgr_texture {
      config.background().raw_sdl, config.cleanup_all()};
  Res_handler<SDL_Texture> font_texture {
      config.font().raw_sdl, config.cleanup_all()};

  if (!config.validate(renderer.res())) {
    return nullptr;
  }

  ::SDL_Log("SDL_Init(%d)", sdl_init_flags);
  if (::SDL_Init(sdl_init_flags) < 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init() failed: %s\n",
        ::SDL_GetError());
    return nullptr;
  }
  cleanup_handler.m_sdl_subsystems = sdl_init_flags;

  if (::SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear") == SDL_FALSE) {
    ::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_SetHint() (scale quality to linear) failed: %s\n",
        ::SDL_GetError());
  }
  auto* window = config.window().raw_sdl;
  if (window == nullptr) {
    window = ::SDL_CreateWindow(config.window().title.c_str(),
        config.window().pos_x, config.window().pos_y, config.window().width,
        config.window().height,
        (config.window().is_resizable ? SDL_WINDOW_RESIZABLE : 0) |
            (config.window().is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP
                                           : 0));
    if (window == nullptr) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_CreateWindow() failed: %s\n", ::SDL_GetError());
      return nullptr;
    }
    cleanup_handler.m_window = window;
  }
  if (renderer.res() == nullptr && !renderer.setup(window)) {
    return nullptr;
  }
  auto font =
      Font::create(config.font(), std::move(font_texture), renderer.res());
  if (!font) {
    return nullptr;
  }
  auto background = Background::create(
      config.background(), std::move(backgr_texture), renderer.res());
  if (!background) {
    return nullptr;
  }
  auto text_display = Text_display::create(
      std::move(*font), config.text_area(), renderer.res());
  if (!text_display) {
    return nullptr;
  }
  return std::make_unique<Engine>(std::move(cleanup_handler), window,
      std::move(renderer), std::move(*background), std::move(*text_display));
}
} // namespace remotemo
