#include "engine.hpp"

#include <algorithm>
#include <vector>

#include "font.hpp"
#include "keyboard.hpp"

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


bool Key_combo_handler::is_in_event(const SDL_Event& event) const
{
  if (!m_key_combo) {
    return false;
  }
  if (event.key.keysym.scancode != m_key_combo->key()) {
    return false;
  }
  auto required_mod_keys = m_key_combo->modifier_keys();
  const std::vector<int> mod_keys = {KMOD_SHIFT, KMOD_CTRL, KMOD_ALT};
  return std::all_of(mod_keys.cbegin(), mod_keys.cend(), [&](int mod_key) {
    return (((required_mod_keys & mod_key) == 0) ==
            ((event.key.keysym.mod & mod_key) == 0));
  });
}


Engine::Engine(Main_SDL_handler main_sdl_handler,
    std::optional<Window> window, std::optional<Renderer> renderer,
    std::optional<Background> background,
    std::optional<Text_display> text_display, const Config& config) noexcept
    : m_main_sdl_handler(std::move(main_sdl_handler)),
      m_window(std::move(window)), m_renderer(std::move(renderer)),
      m_background(std::move(background)),
      m_text_display(std::move(text_display)),
      m_key_fullscreen(config.m_key_fullscreen),
      m_key_close_window(config.m_key_close_window),
      m_key_quit(config.m_key_quit),
      m_is_closing_same_as_quit(config.m_is_closing_same_as_quit),
      m_pre_close_function(config.m_pre_close_function),
      m_pre_quit_function(config.m_pre_quit_function)
{
  set_screen_display_settings();
}

void Engine::set_screen_display_settings()
{
  auto backgr_texture_size = m_background->texture_size();
  m_background_target.w = backgr_texture_size.width;
  m_background_target.h = backgr_texture_size.height;
  auto backgr_text_area = m_background->text_area();
  m_text_target.w = backgr_text_area.width;
  m_text_target.h = backgr_text_area.height;
  refresh_screen_display_settings();
}

void Engine::refresh_screen_display_settings()
{
  auto window_size = m_window->size();
  auto backgr_min_area = m_background->min_area();
  float scale_w = static_cast<float>(window_size.width) /
                  static_cast<float>(backgr_min_area.width);
  float scale_h = static_cast<float>(window_size.height) /
                  static_cast<float>(backgr_min_area.height);
  m_screen_scale = std::min(scale_w, scale_h);
  m_background_target.x =
      ((static_cast<int>(
            static_cast<float>(window_size.width) / m_screen_scale) -
           backgr_min_area.width + 1) /
          2) -
      backgr_min_area.x;
  m_background_target.y =
      ((static_cast<int>(
            static_cast<float>(window_size.height) / m_screen_scale) -
           backgr_min_area.height + 1) /
          2) -
      backgr_min_area.y;
  auto backgr_text_area = m_background->text_area();
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
  Res_handler<SDL_Window> window_from_conf {
      config.window().raw_sdl, config.cleanup_all()};
  std::optional<Renderer> renderer {};
  Res_handler<SDL_Renderer> renderer_from_conf {
      config.window().raw_sdl == nullptr
          ? nullptr
          : ::SDL_GetRenderer(config.window().raw_sdl),
      config.cleanup_all()};
  Res_handler<SDL_Texture> backgr_texture {
      config.background().raw_sdl, config.cleanup_all()};
  Res_handler<SDL_Texture> font_texture {
      config.font().raw_sdl, config.cleanup_all()};

  if (!config.validate(renderer_from_conf.res())) {
    return nullptr;
  }
  if (!main_sdl_handler.setup(sdl_init_flags)) {
    return nullptr;
  }
  window = Window::create(config.window(), std::move(window_from_conf));
  if (!window) {
    return nullptr;
  }
  renderer = Renderer::create(window->res(), std::move(renderer_from_conf));
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
      std::move(window), std::move(renderer), std::move(background),
      std::move(text_display), config);
}


Point Engine::cursor_pos() const
{
  throw_if_window_closed();
  return m_text_display->cursor_pos();
}

Size Engine::text_area_size() const
{
  throw_if_window_closed();
  return Size {m_text_display->columns(), m_text_display->lines()};
}

char Engine::char_at(const Point& pos) const
{
  throw_if_window_closed();
  return m_text_display->char_at(pos);
}

bool Engine::is_inverse_at(const Point& pos) const
{
  throw_if_window_closed();
  return m_text_display->is_inverse_at(pos);
}

bool Engine::display_string_at_cursor(
    const std::string& text, Wrapping text_wrapping)
{
  throw_if_window_closed();
  auto cursor_pos = m_text_display->cursor_pos();
  for (const auto character : text) {
    if (!scroll_if_needed(&cursor_pos)) {
      return false;
    }
    delay(m_delay_between_chars_ms);
    switch (character) {

      case '\n': // New line
        cursor_pos.x = 0;
        cursor_pos.y++;
        m_text_display->cursor_pos(cursor_pos);
        break;

      case '\b': // Backspace
        if (cursor_pos.x == 0) {
          return false;
        }
        cursor_pos.x--;
        m_text_display->cursor_pos(cursor_pos);
        m_text_display->set_char_at_cursor(' ');
        break;

      default:
        if (cursor_pos.x == m_text_display->columns()) {
          // This can happen if wrapping was off the last time the cursor
          // moved. But wrap might have been set to on since then.
          if (text_wrapping == Wrapping::off) {
            // If wrapping is off, the cursor is allowed to go off the screen
            // but nothing can be displayed there.
            return false;
          }
          cursor_pos.x = 0;
          cursor_pos.y++;
          m_text_display->cursor_pos(cursor_pos);
          if (!scroll_if_needed(&cursor_pos)) {
            return false;
          }
        }
        m_text_display->set_char_at_cursor(character);
        cursor_pos.x++;
        if (cursor_pos.x == m_text_display->columns() &&
            text_wrapping != Wrapping::off) {
          // If wrapping is off, the cursor is allowed to go off the screen
          cursor_pos.x = 0;
          cursor_pos.y++;
        }
        m_text_display->cursor_pos(cursor_pos);
        break;
    }
    main_loop_once();
  }
  return true;
}

void Engine::clear_screen()
{
  auto lines = m_text_display->lines();
  for (int line = 0; line < lines; line++) {
    delay(m_delay_between_chars_ms);
    m_text_display->clear_line(line);
    main_loop_once();
  }
}

bool Engine::scroll_if_needed(Point* cursor_pos)
{
  if (!m_is_scrolling_allowed && cursor_pos->y >= m_text_display->lines()) {
    return false;
  }
  while (cursor_pos->y >= m_text_display->lines()) {
    delay(m_delay_between_chars_ms);
    m_text_display->scroll_up_one_line();
    cursor_pos->y--;
    m_text_display->cursor_pos(*cursor_pos);
    main_loop_once();
  }
  return true;
}

void Engine::cursor_pos(const Point& pos)
{
  delay(m_delay_between_chars_ms);
  m_text_display->cursor_pos(pos);
  main_loop_once();
}

void Engine::is_output_inversed(bool inverse)
{
  throw_if_window_closed();
  m_text_display->is_output_inversed(inverse);
}

bool Engine::is_output_inversed() const
{
  throw_if_window_closed();
  return m_text_display->is_output_inversed();
}

void Engine::delay(int delay_in_ms)
{
  throw_if_window_closed();
  auto time_now = SDL_GetTicks();
  const auto timeout = time_now + delay_in_ms;
  while (!SDL_TICKS_PASSED(time_now, timeout)) {
    if (SDL_WaitEventTimeout(nullptr, static_cast<int>(timeout - time_now)) !=
        0) {
      main_loop_once();
    }
    time_now = SDL_GetTicks();
  }
}

Key Engine::get_key()
{
  while (true) {
    throw_if_window_closed();
    SDL_Event event;
    if (SDL_WaitEvent(&event) == 0) {
      continue;
    }
    if (handle_standard_event(event)) {
      render_window();
      continue;
    }
    if (event.type == SDL_KEYDOWN) {
      // TODO remove this debugging logging:
      SDL_Log("Physical %s key (%d) acting as %s key (%d)",
          SDL_GetScancodeName(event.key.keysym.scancode),
          event.key.keysym.scancode, SDL_GetKeyName(event.key.keysym.sym),
          event.key.keysym.sym);
      auto key = Keyboard::scancode_to_key(event.key.keysym.scancode);
      if (!key) {
        continue;
      }
      return *key;
    }
  }
}

void Engine::main_loop_once()
{
  throw_if_window_closed();
  // SDL_Log("Main loop once!");
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    handle_standard_event(event);
  }
  render_window();
}

bool Engine::handle_standard_event(const SDL_Event& event)
{
  if (handle_window_event(event)) {
    return true;
  }
  // TODO Add other event handlers? See issue #62

  return false;
}

bool Engine::handle_window_event(const SDL_Event& event)
{
  switch (event.type) {
    case SDL_QUIT:
      user_closes_window();
      return true;
    case SDL_KEYDOWN:
      if (m_key_close_window.is_in_event(event)) {
        user_closes_window();
        return true;
      }
      if (m_key_quit.is_in_event(event)) {
        if (m_pre_quit_function()) {
          if (m_is_closing_same_as_quit) {
            close_window();
          }
          throw User_quit_exception();
        }
        return true;
      }
      if (m_key_fullscreen.is_in_event(event)) {
        if (event.key.repeat == 0) {
          m_window->set_fullscreen(!m_window->is_fullscreen());
        }
        return true;
      }
      break;
    case SDL_WINDOWEVENT:
      SDL_Log("Window event: %d", event.window.event);
      switch (event.window.event) {
        case SDL_WINDOWEVENT_LEAVE:
        case SDL_WINDOWEVENT_FOCUS_LOST:
        case SDL_WINDOWEVENT_CLOSE:
          return false;
        default:
          m_window->set_had_window_event(true);
          break;
      }
      switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          m_window->refresh_local_size();
          refresh_screen_display_settings();
          SDL_Log("Size changed");
          return true;
        case SDL_WINDOWEVENT_MINIMIZED:
          SDL_Log("-----Minimized-----");
          return true;
        default:
          SDL_Log("Unhandled window event: %d", event.window.event);
          return true;
      }
    default:
      break;
  }
  return false;
}

void Engine::user_closes_window()
{
  if (!m_pre_close_function()) {
    return;
  }
  close_window();
  if (m_is_closing_same_as_quit) {
    throw User_quit_exception();
  }
  throw Window_is_closed_exception();
}

void Engine::close_window()
{
  m_text_display = std::nullopt;
  m_background = std::nullopt;
  m_renderer = std::nullopt;
  m_window = std::nullopt;
  m_main_sdl_handler = Main_SDL_handler {false};
}

void Engine::render_window()
{
  if (m_window->had_window_event()) {
    SDL_Log("== window had event ==");
    m_text_display->set_texture_refresh_needed(true);
    m_window->set_had_window_event(false);
    m_window->refresh_local_flags();
    if (m_window->is_visible()) {
      SDL_Log("Window is <<<VISIBLE>>>");
    }
  }
  if (!m_window->is_visible()) {
    SDL_Log("Window is ===HIDDEN=== (or minimized)");
    return;
  }
  if (m_text_display->is_texture_refresh_needed()) {
    SDL_Log("++++ testure refresh needed +++");
    m_text_display->refresh_texture();
  }
  m_text_display->update_cursor();
  if (!m_text_display->has_texture_changed()) {
    SDL_Log("|||| No change to texture ||||");
    return;
  }
  SDL_Log(">>>>>>>>>>>>>>>>>>> rendering .....");
  auto* renderer = m_renderer->res();
  ::SDL_SetRenderTarget(renderer, nullptr);
  ::SDL_RenderClear(renderer);
  ::SDL_RenderSetScale(renderer, m_screen_scale, m_screen_scale);
  ::SDL_RenderCopy(
      renderer, m_background->res(), nullptr, &m_background_target);
  ::SDL_RenderCopyF(renderer, m_text_display->res(), nullptr, &m_text_target);

  ::SDL_RenderPresent(renderer);
  m_text_display->set_texture_changed(false);
}

void Engine::throw_if_window_closed() const
{
  if (!m_window || !m_renderer || !m_background || !m_text_display) {
    throw Window_is_closed_exception();
  }
}

} // namespace remotemo
