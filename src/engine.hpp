#ifndef REMOTEMO_SRC_ENGINE_HPP
#define REMOTEMO_SRC_ENGINE_HPP

#include <utility>
#include <memory>

#include "remotemo/config.hpp"
#include "res_handler.hpp"
#include "background.hpp"
#include "text_display.hpp"

#include <SDL.h>

namespace remotemo {
class Engine;
class Cleanup_handler {
  friend Engine;

public:
  constexpr explicit Cleanup_handler(
      bool do_sdl_quit, ::SDL_Window* window) noexcept
      : m_do_sdl_quit(do_sdl_quit), m_window(do_sdl_quit ? window : nullptr)
  {}
  ~Cleanup_handler();
  Cleanup_handler(const Cleanup_handler&) = delete;
  Cleanup_handler& operator=(const Cleanup_handler&) = delete;
  Cleanup_handler(Cleanup_handler&& other) noexcept;
  Cleanup_handler& operator=(Cleanup_handler&&) = default;

private:
  bool m_do_sdl_quit {false};
  ::Uint32 m_sdl_subsystems {0};
  ::SDL_Window* m_window {nullptr};
};

class Renderer : public Res_handler<SDL_Renderer> {
public:
  constexpr explicit Renderer(
      SDL_Renderer* renderer, bool is_owned = true) noexcept
      : Res_handler<SDL_Renderer>(renderer, is_owned)
  {}
  constexpr explicit Renderer(
      SDL_Window* window, bool is_owned = true) noexcept
      : Res_handler<SDL_Renderer>(
            window == nullptr ? nullptr : ::SDL_GetRenderer(window), is_owned)
  {}

  bool setup(SDL_Window* window);
};

class Engine {
public:
  explicit Engine(Cleanup_handler cleanup_handler, SDL_Window* window,
      Renderer renderer, Background background,
      Text_display text_display) noexcept
      : m_cleanup_handler(std::move(cleanup_handler)), m_window(window),
        m_renderer(std::move(renderer)), m_background(std::move(background)),
        m_text_display(std::move(text_display))
  {}
  ~Engine() noexcept = default;
  Engine(Engine&& other) noexcept;
  Engine& operator=(Engine&& other) noexcept;

  Engine() = delete;
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  static std::unique_ptr<Engine> create(const Config& config);

private:
  Cleanup_handler m_cleanup_handler;

  SDL_Window* m_window;

  Renderer m_renderer;
  Background m_background;
  Text_display m_text_display;
  static constexpr Uint32 sdl_init_flags {SDL_INIT_VIDEO};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_ENGINE_HPP
