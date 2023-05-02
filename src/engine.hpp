#ifndef REMOTEMO_SRC_ENGINE_HPP
#define REMOTEMO_SRC_ENGINE_HPP

#include <utility>
#include <memory>
#include <optional>

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
  explicit Engine(Main_SDL_handler main_sdl_handler, Window window,
      Renderer renderer, Background background,
      Text_display text_display) noexcept
      : m_main_sdl_handler(std::move(main_sdl_handler)),
        m_window(std::move(window)), m_renderer(std::move(renderer)),
        m_background(std::move(background)),
        m_text_display(std::move(text_display))
  {
    set_screen_display_settings();
  }
  virtual ~Engine() noexcept = default;
  Engine(Engine&& other) noexcept = default;
  Engine& operator=(Engine&& other) noexcept = default;

  Engine() = delete;
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  static std::unique_ptr<Engine> create(const Config& config);

  virtual void main_loop_once();

protected:
  virtual void handle_events();
  virtual void render_window();
  void set_screen_display_settings();
  void refresh_screen_display_settings();

private:
  Main_SDL_handler m_main_sdl_handler;
  Window m_window;
  Renderer m_renderer;
  Background m_background;
  Text_display m_text_display;
  float m_screen_scale;
  SDL_Rect m_background_target;
  SDL_FRect m_text_target;
  static constexpr Uint32 sdl_init_flags {SDL_INIT_VIDEO};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_ENGINE_HPP
