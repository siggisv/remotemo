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

class Engine {
public:
  explicit Engine(Main_SDL_handler main_sdl_handler,
      std::optional<Window> window, std::optional<Renderer> renderer,
      std::optional<Background> background,
      std::optional<Text_display> text_display) noexcept;
  virtual ~Engine() noexcept = default;
  Engine(Engine&& other) noexcept = default;
  Engine& operator=(Engine&& other) noexcept = default;

  Engine() = delete;
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  static std::unique_ptr<Engine> create(const Config& config);

  [[nodiscard]] virtual SDL_Point cursor_pos() const;
  [[nodiscard]] virtual SDL_Point text_area_size() const;
  [[nodiscard]] virtual char char_at(const SDL_Point& pos) const;
  [[nodiscard]] virtual bool is_inverse_at(const SDL_Point& pos) const;

  virtual void cursor_pos(const SDL_Point& pos);
  virtual bool display_string_at_cursor(
      const std::string& text, Wrapping text_wrapping);

  virtual void delay(int delay_in_ms);
  virtual void main_loop_once();
  virtual void is_output_inversed(bool inverse);

protected:
  virtual void handle_events();
  virtual bool handle_window_event(const SDL_Event& event);
  virtual void render_window();
  bool scroll_if_needed(SDL_Point* cursor_pos);
  void set_screen_display_settings();
  void refresh_screen_display_settings();
  void throw_if_window_closed() const;
  void close_window();

private:
  Main_SDL_handler m_main_sdl_handler;
  std::optional<Window> m_window;
  std::optional<Renderer> m_renderer;
  std::optional<Background> m_background;
  std::optional<Text_display> m_text_display;
  bool m_is_scrolling_allowed {true};
  int m_delay_between_chars_ms {60};
  float m_screen_scale {1.0f};
  SDL_Rect m_background_target {};
  SDL_FRect m_text_target {};
  static constexpr Uint32 sdl_init_flags {SDL_INIT_VIDEO};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_ENGINE_HPP
