#include "engine.hpp"

namespace remotemo {
Cleanup_handler::~Cleanup_handler()
{
  if (m_renderer != nullptr) {
    ::SDL_DestroyRenderer(m_renderer);
  }
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

Engine::~Engine() noexcept = default;
Engine::Engine(Engine&& other) noexcept
    : m_cleanup_handler(std::move(other.m_cleanup_handler)),
      m_background(std::move(other.m_background)),
      m_font(std::move(other.m_font)),
      m_text_area(std::move(other.m_text_area))
{}

std::unique_ptr<Engine> Engine::create(const Config& config)
{
  ::SDL_Log("Remotemo::initialize() with config.m_cleanup_all: %s",
      config.cleanup_all() ? "True" : "False");
  SDL_Renderer* conf_renderer = (config.m_window == nullptr)
                                    ? nullptr
                                    : ::SDL_GetRenderer(config.m_window);

  // Setup handling of cleanup.
  // This is done right here at the start so that if something fails, then
  // everything that was handed over will be taken care of no matter where
  // in the setup process something failed.
  auto cleanup_handler = std::make_unique<Cleanup_handler>(
      config.cleanup_all(), config.m_window, conf_renderer);
  auto backgr_texture =
      Texture {config.background().raw_sdl, config.cleanup_all()};
  auto font_texture = Texture {config.font().raw_sdl, config.cleanup_all()};

  if (!config.validate(conf_renderer)) {
    return nullptr;
  }

  ::SDL_Log("SDL_Init(%d)", sdl_init_flags);
  if (::SDL_Init(sdl_init_flags) < 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init() failed: %s\n",
        ::SDL_GetError());
    return nullptr;
  }
  cleanup_handler->m_sdl_subsystems = sdl_init_flags;

  if (::SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear") == SDL_FALSE) {
    ::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_SetHint() (scale quality to linear) failed: %s\n",
        ::SDL_GetError());
  }
  auto* window = config.m_window;
  if (window == nullptr) {
    window = ::SDL_CreateWindow(config.m_window_title.c_str(),
        config.m_window_pos_x, config.m_window_pos_y, config.m_window_width,
        config.m_window_height,
        (config.m_window_is_resizable ? SDL_WINDOW_RESIZABLE : 0) |
            (config.m_window_is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP
                                           : 0));
    if (window == nullptr) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_CreateWindow() failed: %s\n", ::SDL_GetError());
      return nullptr;
    }
    cleanup_handler->m_window = window;
  }
  auto* renderer = conf_renderer;
  if (renderer == nullptr) {
    renderer = ::SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
    if (renderer == nullptr) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_CreateRenderer() failed: %s\n", ::SDL_GetError());
      return nullptr;
    }
    cleanup_handler->m_renderer = renderer;
  }
  auto font = Font::create(config.font(), std::move(font_texture), renderer);
  if (!font) {
    return nullptr;
  }
  auto background = Background::create(
      config.background(), std::move(backgr_texture), renderer);
  if (!background) {
    return nullptr;
  }
  auto text_area =
      Texture::create_text_area(renderer, config.font(), config.text_area());
  if (!text_area) {
    return nullptr;
  }
  return std::make_unique<Engine>(std::move(cleanup_handler), window,
      renderer, std::move(*background), std::move(*font),
      std::move(*text_area));
}
} // namespace remotemo
