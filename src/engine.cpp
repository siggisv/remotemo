#include "engine.hpp"

#include <algorithm>

#include "font.hpp"

namespace remotemo {
Main_SDL_handler::~Main_SDL_handler()
{
  if (m_do_sdl_quit) {
    ::SDL_Quit();
  } else {
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

Engine::Engine(Main_SDL_handler main_sdl_handler, Window window,
    Renderer renderer, Background background,
    Text_display text_display) noexcept
    : m_main_sdl_handler(std::move(main_sdl_handler)),
      m_window(std::move(window)), m_renderer(std::move(renderer)),
      m_background(std::move(background)),
      m_text_display(std::move(text_display))
{
  set_screen_display_settings();
  render_window();
}

void Engine::set_screen_display_settings()
{
  auto backgr_texture_size = m_background.texture_size();
  m_background_target.w = backgr_texture_size.x;
  m_background_target.h = backgr_texture_size.y;
  auto backgr_text_area = m_background.text_area();
  m_text_target.w = backgr_text_area.w;
  m_text_target.h = backgr_text_area.h;
  refresh_screen_display_settings();
}

void Engine::refresh_screen_display_settings()
{
  auto window_size = m_window.size();
  auto backgr_min_area = m_background.min_area();
  float scale_w = static_cast<float>(window_size.x) /
                  static_cast<float>(backgr_min_area.w);
  float scale_h = static_cast<float>(window_size.y) /
                  static_cast<float>(backgr_min_area.h);
  m_screen_scale = std::min(scale_w, scale_h);
  m_background_target.x =
      ((static_cast<int>(static_cast<float>(window_size.x) / m_screen_scale) -
           backgr_min_area.w + 1) /
          2) -
      backgr_min_area.x;
  m_background_target.y =
      ((static_cast<int>(static_cast<float>(window_size.y) / m_screen_scale) -
           backgr_min_area.h + 1) /
          2) -
      backgr_min_area.y;
  auto backgr_text_area = m_background.text_area();
  m_text_target.x =
      backgr_text_area.x + static_cast<float>(m_background_target.x);
  m_text_target.y =
      backgr_text_area.y + static_cast<float>(m_background_target.y);
}

std::unique_ptr<Engine> Engine::create(const Config& config)
{
  // Setup handling of resources and their cleanup.
  // This is done right here at the start so that if something fails, then
  // everything that was handed over will be taken care of no matter where
  // in the setup process something failed.
  // Note that the order in which the following variables are created is
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

SDL_Point Engine::cursor_pos() const
{
  return m_text_display.cursor_pos();
}

SDL_Point Engine::text_area_size() const
{
  return SDL_Point {m_text_display.columns(), m_text_display.lines()};
}

void Engine::cursor_pos(const SDL_Point& pos)
{
  m_text_display.hide_cursor();
  main_loop_once();
  m_text_display.cursor_pos(pos);
  m_text_display.show_cursor();
  main_loop_once();
}

void Engine::main_loop_once()
{
  // TODO cast exception if window has been closed
  SDL_Log("Main loop once!");
  handle_events();
  render_window();
}

void Engine::handle_events()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (handle_window_event(event)) {
      continue;
    }
    // TODO Add other event handlers
  }
}

bool Engine::handle_window_event(const SDL_Event& event)
{
  switch (event.type) {
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          m_window.refresh_local_size();
          refresh_screen_display_settings();
          return true;
        default:
          break;
      }
    default:
      break;
  }
  return false;
}

void Engine::render_window()
{
  auto* renderer = m_renderer.res();
  ::SDL_SetRenderTarget(renderer, nullptr);
  ::SDL_RenderClear(renderer);
  ::SDL_RenderSetScale(renderer, m_screen_scale, m_screen_scale);
  ::SDL_RenderCopy(
      renderer, m_background.res(), nullptr, &m_background_target);
  ::SDL_RenderCopyF(renderer, m_text_display.res(), nullptr, &m_text_target);

  ::SDL_RenderPresent(renderer);
}

} // namespace remotemo
