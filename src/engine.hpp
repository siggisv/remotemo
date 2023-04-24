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
class Cleanup_handler {
  friend Engine;

public:
  constexpr explicit Cleanup_handler(bool do_sdl_quit) noexcept
      : m_do_sdl_quit(do_sdl_quit)
  {}
  ~Cleanup_handler();
  Cleanup_handler(Cleanup_handler&& other) noexcept;
  Cleanup_handler& operator=(Cleanup_handler&&) noexcept;

  Cleanup_handler(const Cleanup_handler&) = delete;
  Cleanup_handler& operator=(const Cleanup_handler&) = delete;

private:
  bool m_do_sdl_quit {false};
  ::Uint32 m_sdl_subsystems {0};
};

class Engine {
public:
  explicit Engine(Cleanup_handler cleanup_handler, Window window,
      Renderer renderer, Background background,
      Text_display text_display) noexcept
      : m_cleanup_handler(std::move(cleanup_handler)),
        m_window(std::move(window)), m_renderer(std::move(renderer)),
        m_background(std::move(background)),
        m_text_display(std::move(text_display))
  {}
  ~Engine() noexcept = default;
  Engine(Engine&& other) noexcept = default;
  Engine& operator=(Engine&& other) noexcept = default;

  Engine() = delete;
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  static std::unique_ptr<Engine> create(const Config& config);

private:
  Cleanup_handler m_cleanup_handler;

  Window m_window;
  Renderer m_renderer;
  Background m_background;
  Text_display m_text_display;
  static constexpr Uint32 sdl_init_flags {SDL_INIT_VIDEO};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_ENGINE_HPP
