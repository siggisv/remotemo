#include <remotemo/remotemo.hpp>
#include <SDL_image.h>

#include <filesystem>

#include "texture.hpp"

namespace remotemo {
class Remotemo::Cleanup_handler {
  friend Remotemo;

public:
  constexpr explicit Cleanup_handler(bool do_sdl_quit, ::SDL_Window* window,
      ::SDL_Renderer* renderer, ::SDL_Texture* background,
      ::SDL_Texture* font_bitmap) noexcept
      : m_do_sdl_quit(do_sdl_quit), m_window(window), m_renderer(renderer),
        m_background(background), m_font_bitmap(font_bitmap)
  {}
  constexpr explicit Cleanup_handler(bool do_sdl_quit) noexcept
      : m_do_sdl_quit(do_sdl_quit)
  {}
  ~Cleanup_handler();
  Cleanup_handler(const Cleanup_handler&) = delete;
  Cleanup_handler& operator=(const Cleanup_handler&) = delete;
  Cleanup_handler(Cleanup_handler&&) = default;
  Cleanup_handler& operator=(Cleanup_handler&&) = default;

private:
  bool m_do_sdl_quit {false};
  ::Uint32 m_sdl_subsystems {0};
  ::SDL_Window* m_window {nullptr};
  ::SDL_Renderer* m_renderer {nullptr};
  ::SDL_Texture* m_background {nullptr};
  ::SDL_Texture* m_font_bitmap {nullptr};
};

Remotemo::Cleanup_handler::~Cleanup_handler()
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

Remotemo::~Remotemo() noexcept = default;
Remotemo::Remotemo(Remotemo&& other) noexcept
    : m_cleanup_handler(std::move(other.m_cleanup_handler)),
      m_background(std::move(other.m_background)),
      m_font_bitmap(std::move(other.m_font_bitmap)),
      m_text_area(std::move(other.m_text_area))
{}
bool Remotemo::initialize(const Config& config)
{
  ::SDL_Log("Remotemo::initialize() with config.m_cleanup_all: %s",
      config.m_cleanup_all ? "True" : "False");
  SDL_Renderer* conf_renderer = (config.m_the_window == nullptr)
                                    ? nullptr
                                    : ::SDL_GetRenderer(config.m_the_window);

  if (config.m_cleanup_all) {
    // Set cleanup_handler to handle SDL_Quit() AND all resources in config.
    // This is done right here at the start so that if something fails, then
    // everything that was handed over will be taken care of no matter where
    // in the setup process something failed.
    m_cleanup_handler =
        std::make_unique<Cleanup_handler>(true, config.m_the_window,
            conf_renderer, config.m_background, config.m_font_bitmap);
  } else {
    m_cleanup_handler = std::make_unique<Cleanup_handler>(false);
  }

  // Validate config resources:
  if (config.m_the_window != nullptr &&
      ::SDL_GetWindowID(config.m_the_window) == 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "Window in config is invalid: %s\n", ::SDL_GetError());
    return false;
  }
  if (conf_renderer == nullptr) {
    if (config.m_background != nullptr || config.m_font_bitmap != nullptr) {
      return false;
    }
  } else {
    ::SDL_RendererInfo info;
    if (::SDL_GetRendererInfo(conf_renderer, &info) != 0) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_GetRendererInfo() failed when trying to get info on the "
          "renderer of the window handed to remotemo::create(): %s\n",
          ::SDL_GetError());
      return false;
    }
    if ((info.flags & SDL_RENDERER_TARGETTEXTURE) !=
        SDL_RENDERER_TARGETTEXTURE) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "The renderer of the window handed to remotemo::create() did not "
          "have the correct flags (SDL_RENDERER_TARGETTEXTURE missing).\n");
      return false;
    }
    if (config.m_font_bitmap != nullptr) {
      // Have not found a more direct way to check if the texture has got the
      // correct renderer:
      ::SDL_Rect noop_rect {-3, -3, 1, 1}; // Definetly not inside window
      if (::SDL_RenderCopy(conf_renderer, config.m_font_bitmap, nullptr,
              &noop_rect) != 0) {
        ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
            "The font bitmap texture that was handed to remotemo::create() "
            "was created with a different renderer than the window.\n");
        return false;
      }
    }
    if (config.m_background != nullptr) {
      // Have not found a more direct way to check if the texture has got the
      // correct renderer:
      ::SDL_Rect noop_rect {-3, -3, 1, 1}; // Definetly not inside window
      if (::SDL_RenderCopy(
              conf_renderer, config.m_background, nullptr, &noop_rect) != 0) {
        ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
            "The background texture that was handed to remotemo::create() "
            "was created with a different renderer than the window.\n");
        return false;
      }
    }
  }

  ::SDL_Log("SDL_Init(%d)", sdl_init_flags);
  if (::SDL_Init(sdl_init_flags) < 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init() failed: %s\n",
        ::SDL_GetError());
    return false;
  }
  m_cleanup_handler->m_sdl_subsystems = sdl_init_flags;

  if (::SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear") == SDL_FALSE) {
    ::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_SetHint() (scale quality to linear) failed: %s\n",
        ::SDL_GetError());
  }
  if (config.m_the_window != nullptr) {
    m_window = config.m_the_window;
  } else {
    m_window = ::SDL_CreateWindow(config.m_window_title.c_str(),
        config.m_window_pos_x, config.m_window_pos_y, config.m_window_width,
        config.m_window_height,
        (config.m_window_is_resizable ? SDL_WINDOW_RESIZABLE : 0) |
            (config.m_window_is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP
                                           : 0));
    if (m_window == nullptr) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_CreateWindow() failed: %s\n", ::SDL_GetError());
      return false;
    }
    m_cleanup_handler->m_window = m_window;
  }
  if (conf_renderer != nullptr) {
    m_renderer = conf_renderer;
  } else {
    m_renderer =
        ::SDL_CreateRenderer(m_window, -1, SDL_RENDERER_TARGETTEXTURE);
    if (m_renderer == nullptr) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_CreateRenderer() failed: %s\n", ::SDL_GetError());
      return false;
    }
    m_cleanup_handler->m_renderer = m_renderer;
  }
  if (config.m_font_bitmap != nullptr) {
    m_font_bitmap =
        Texture::create(config.m_font_bitmap, config.m_cleanup_all);
    m_cleanup_handler->m_font_bitmap = nullptr;
  } else {
    m_font_bitmap = Texture::load(m_renderer, config.m_font_bitmap_file_path);
    if (!m_font_bitmap) {
      return false;
    }
  }
  if (config.m_background != nullptr) {
    m_background = Texture::create(config.m_background, config.m_cleanup_all);
    m_cleanup_handler->m_background = nullptr;
  } else {
    m_background = Texture::load(m_renderer, config.m_background_file_path);
    if (!m_background) {
      return false;
    }
  }
  m_text_area = Texture::create_text_area(m_renderer, config);
  if (!m_text_area) {
    return false;
  }
  return true;
}

std::optional<Remotemo> create()
{
  return create(Config {});
}

std::optional<Remotemo> create(const Config& config)
{
  Remotemo temo {};
  if (temo.initialize(config)) {
    return temo;
  }
  return {};
}
} // namespace remotemo
