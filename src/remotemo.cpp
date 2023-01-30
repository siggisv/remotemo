#include <remotemo/remotemo.hpp>
#include <SDL_image.h>

namespace remoTemo {
class Temo::Cleanup_handler {
public:
  constexpr explicit Cleanup_handler(bool do_sdl_quit, ::SDL_Window* window,
      ::SDL_Renderer* renderer, ::SDL_Texture* background,
      ::SDL_Texture* font_bitmap) noexcept
      : m_do_sdl_quit(do_sdl_quit), m_window(window), m_renderer(renderer),
        m_background(background), m_font_bitmap(font_bitmap),
        m_text_area(nullptr)
  {}
  constexpr explicit Cleanup_handler(bool do_sdl_quit) noexcept
      : m_do_sdl_quit(do_sdl_quit)
  {}
  ~Cleanup_handler();
  Cleanup_handler(const Cleanup_handler&) = delete;
  Cleanup_handler& operator=(const Cleanup_handler&) = delete;

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
  if (m_text_area) {
    ::SDL_DestroyTexture(m_text_area);
  }
  if (m_font_bitmap) {
    ::SDL_DestroyTexture(m_font_bitmap);
  }
  if (m_background) {
    ::SDL_DestroyTexture(m_background);
  }
  if (m_renderer) {
    ::SDL_DestroyRenderer(m_renderer);
  }
  if (m_window) {
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

Temo::~Temo() noexcept {}
Temo::Temo(Temo&& other) noexcept
    : m_cleanup_handler(std::move(other.m_cleanup_handler))
{}
bool Temo::initialize(const Config& config)
{
  ::SDL_Log("Temo::initialize() with config.m_cleanup_all: %s",
      config.m_cleanup_all ? "True" : "False");

  if (config.m_cleanup_all) {
    // Set cleanup_handler to handle SDL_Quit() + all resources in config
    m_cleanup_handler = std::make_unique<Cleanup_handler>(true,
        config.m_the_window, ::SDL_GetRenderer(config.m_the_window),
        config.m_background, config.m_font_bitmap);
  } else {
    m_cleanup_handler = std::make_unique<Cleanup_handler>(false);
  }
  ::SDL_Log("SDL_Init(%d)", sdl_init_flags);
  if (::SDL_Init(sdl_init_flags) < 0) {
    ::SDL_LogCritical(::SDL_LOG_CATEGORY_APPLICATION,
        "SDL_Init() failed: %s\n", ::SDL_GetError());
    return false;
  } else {
    m_cleanup_handler->m_sdl_subsystems = sdl_init_flags;
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
  } else {
    return {};
  }
}
} // namespace remoTemo
