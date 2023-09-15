#ifndef REMOTEMO_SRC_ENGINE_HPP
#define REMOTEMO_SRC_ENGINE_HPP

#include <string>
#include <utility>
#include <memory>
#include <optional>

#include "remotemo/exceptions.hpp"
#include "remotemo/config.hpp"
#include "res_handler.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "background.hpp"
#include "text_display.hpp"

#include <SDL.h>

namespace remotemo {
class Engine;
class Main_SDL_handler {
  friend Engine;

public:
  constexpr explicit Main_SDL_handler(bool do_sdl_quit) noexcept
      : m_do_sdl_quit(do_sdl_quit)
  {}
  ~Main_SDL_handler();
  Main_SDL_handler(Main_SDL_handler&& other) noexcept;
  Main_SDL_handler& operator=(Main_SDL_handler&&) noexcept;

  Main_SDL_handler(const Main_SDL_handler&) = delete;
  Main_SDL_handler& operator=(const Main_SDL_handler&) = delete;

  bool setup(::Uint32 init_flags);

private:
  bool m_do_sdl_quit {false};
  ::Uint32 m_sdl_subsystems {0};
};

class Key_combo_handler {
public:
  constexpr Key_combo_handler() noexcept = default;
  constexpr explicit Key_combo_handler(
      const std::optional<Key_combo>& combo) noexcept
      : m_key_combo(combo)
  {}
  [[nodiscard]] bool is_in_event(const SDL_Event& event) const;

private:
  std::optional<Key_combo> m_key_combo {};
};

class Engine {
public:
  explicit Engine(Main_SDL_handler main_sdl_handler,
      std::optional<Window> window, std::optional<Renderer> renderer,
      std::optional<Background> background,
      std::optional<Text_display> text_display,
      const Config& config) noexcept;
  virtual ~Engine() noexcept = default;
  Engine(Engine&& other) noexcept = default;
  Engine& operator=(Engine&& other) noexcept = default;

  Engine() = delete;
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  static std::unique_ptr<Engine> create(const Config& config);

  [[nodiscard]] Point cursor_pos() const;
  [[nodiscard]] Size text_area_size() const;
  [[nodiscard]] char char_at(const Point& pos) const;
  [[nodiscard]] bool is_inverse_at(const Point& pos) const;

  void cursor_pos(const Point& pos);
  bool display_string_at_cursor(
      const std::string& text, Wrapping text_wrapping);

  void delay(int delay_in_ms);
  Key get_key();
  void main_loop_once();
  void delay_between_chars_ms(int delay_in_ms)
  {
    m_delay_between_chars_ms = delay_in_ms;
  }
  void is_scrolling_allowed(bool is_scrolling_allowed)
  {
    m_is_scrolling_allowed = is_scrolling_allowed;
  }
  void is_output_inversed(bool inverse);
  [[nodiscard]] int delay_between_chars_ms() const
  {
    return m_delay_between_chars_ms;
  }
  [[nodiscard]] bool is_scrolling_allowed() const
  {
    return m_is_scrolling_allowed;
  }
  [[nodiscard]] bool is_output_inversed() const;
  void clear_screen();
  void close_window();

protected:
  bool handle_standard_event(const SDL_Event& event);
  bool handle_window_event(const SDL_Event& event);
  void render_window();
  bool scroll_if_needed(Point* cursor_pos);
  void set_screen_display_settings();
  void refresh_screen_display_settings();
  void throw_if_window_closed() const;
  void user_closes_window();

private:
  Main_SDL_handler m_main_sdl_handler;
  std::optional<Window> m_window;
  std::optional<Renderer> m_renderer;
  std::optional<Background> m_background;
  std::optional<Text_display> m_text_display;
  Key_combo_handler m_key_fullscreen;
  Key_combo_handler m_key_close_window;
  Key_combo_handler m_key_quit;
  bool m_is_closing_same_as_quit;
  std::function<bool()> m_pre_close_function;
  std::function<bool()> m_pre_quit_function;
  bool m_is_scrolling_allowed {true};
  int m_delay_between_chars_ms {60};
  float m_screen_scale {1.0f};
  SDL_Rect m_background_target {};
  SDL_FRect m_text_target {};
  static constexpr Uint32 sdl_init_flags {SDL_INIT_VIDEO};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_ENGINE_HPP
