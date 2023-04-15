#include "engine.hpp"

namespace remotemo {
Cleanup_handler::~Cleanup_handler()
{
  if (m_font_bitmap != nullptr) {
    ::SDL_DestroyTexture(m_font_bitmap);
  }
  if (m_background != nullptr) {
    ::SDL_DestroyTexture(m_background);
  }
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

  std::unique_ptr<Cleanup_handler> cleanup_handler;
  if (config.cleanup_all()) {
    // Set cleanup_handler to handle SDL_Quit() AND all resources in config.
    // This is done right here at the start so that if something fails, then
    // everything that was handed over will be taken care of no matter where
    // in the setup process something failed.
    cleanup_handler = std::make_unique<Cleanup_handler>(true, config.m_window,
        conf_renderer, config.background().raw_sdl, config.font().raw_sdl);
  } else {
    cleanup_handler = std::make_unique<Cleanup_handler>(false);
  }

  // Validate config resources:
  if (config.m_window != nullptr && ::SDL_GetWindowID(config.m_window) == 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "Window in config is invalid: %s\n", ::SDL_GetError());
    return nullptr;
  }
  if (conf_renderer == nullptr) {
    if (config.background().raw_sdl != nullptr ||
        config.font().raw_sdl != nullptr) {
      return nullptr;
    }
  } else {
    ::SDL_RendererInfo info;
    if (::SDL_GetRendererInfo(conf_renderer, &info) != 0) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_GetRendererInfo() failed when trying to get info on the "
          "renderer of the window handed to remotemo::create(): %s\n",
          ::SDL_GetError());
      return nullptr;
    }
    if ((info.flags & SDL_RENDERER_TARGETTEXTURE) !=
        SDL_RENDERER_TARGETTEXTURE) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "The renderer of the window handed to remotemo::create() did not "
          "have the correct flags (SDL_RENDERER_TARGETTEXTURE missing).\n");
      return nullptr;
    }
    if (config.font().raw_sdl != nullptr) {
      // Have not found a more direct way to check if the texture has got the
      // correct renderer:
      ::SDL_Rect noop_rect {-3, -3, 1, 1}; // Definetly not inside window
      if (::SDL_RenderCopy(conf_renderer, config.font().raw_sdl, nullptr,
              &noop_rect) != 0) {
        ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
            "The font bitmap texture that was handed to remotemo::create() "
            "was created with a different renderer than the window.\n");
        return nullptr;
      }
    }
    if (config.background().raw_sdl != nullptr) {
      // Have not found a more direct way to check if the texture has got the
      // correct renderer:
      ::SDL_Rect noop_rect {-3, -3, 1, 1}; // Definetly not inside window
      if (::SDL_RenderCopy(conf_renderer, config.background().raw_sdl,
              nullptr, &noop_rect) != 0) {
        ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
            "The background texture that was handed to remotemo::create() "
            "was created with a different renderer than the window.\n");
        return nullptr;
      }
    }
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
  auto font = Font::create(config.font(), config.cleanup_all(), renderer);
  if (font) {
    cleanup_handler->m_font_bitmap = nullptr;
  } else {
    return nullptr;
  }
  auto background =
      Background::create(config.background(), config.cleanup_all(), renderer);
  if (background) {
    cleanup_handler->m_background = nullptr;
  } else {
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
