#include <remotemo/remotemo.hpp>
#include <SDL_image.h>

namespace remoTemo {
class Temo::Cleanup_handler {
  friend Temo;

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
  ::SDL_Texture* m_text_area {nullptr};
};

Temo::Cleanup_handler::~Cleanup_handler()
{
  if (m_text_area != nullptr) {
    ::SDL_DestroyTexture(m_text_area);
  }
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

Temo::~Temo() noexcept = default;
Temo::Temo(Temo&& other) noexcept
    : m_cleanup_handler(std::move(other.m_cleanup_handler))
{}
bool Temo::initialize(const Config& config)
{
  ::SDL_Log("Temo::initialize() with config.m_cleanup_all: %s",
      config.m_cleanup_all ? "True" : "False");

  if (config.m_cleanup_all) {
    // Set cleanup_handler to handle SDL_Quit() + all resources in config
    m_cleanup_handler =
        std::make_unique<Cleanup_handler>(true, config.m_the_window,
            (config.m_the_window == nullptr)
                ? nullptr
                : ::SDL_GetRenderer(config.m_the_window),
            config.m_background, config.m_font_bitmap);
  } else {
    m_cleanup_handler = std::make_unique<Cleanup_handler>(false);
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
  if (config.m_the_window == nullptr) {
    auto* window = ::SDL_CreateWindow(config.m_window_title.c_str(),
        config.m_window_pos_x, config.m_window_pos_y, config.m_window_width,
        config.m_window_height,
        (config.m_window_is_resizable ? SDL_WINDOW_RESIZABLE : 0) |
            (config.m_window_is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP
                                           : 0));
    if (window == nullptr) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_CreateWindow() failed: %s\n", ::SDL_GetError());
      return false;
    }
    m_cleanup_handler->m_window = window;
  }
  return true;
}

std::optional<Temo> create()
{
  return create(Config {});
}

std::optional<Temo> create(const Config& config)
{
  Temo temo {};
  if (temo.initialize(config)) {
    return temo;
  }
  return {};
}
} // namespace remoTemo
