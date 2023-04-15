#ifndef REMOTEMO_SRC_ENGINE_HPP
#define REMOTEMO_SRC_ENGINE_HPP

#include <utility>
#include <memory>

#include "remotemo/config.hpp"
#include "texture.hpp"
#include "font.hpp"
#include "background.hpp"

#include <SDL.h>

namespace remotemo {
class Engine;
class Cleanup_handler {
  friend Engine;

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

class Engine {
public:
  explicit Engine(std::unique_ptr<Cleanup_handler> cleanup_handler,
      SDL_Window* window, SDL_Renderer* renderer, Background background,
      Font font, Texture text_area) noexcept
      : m_cleanup_handler(std::move(cleanup_handler)), m_window(window),
        m_renderer(renderer), m_background(std::move(background)),
        m_font(std::move(font)), m_text_area(std::move(text_area))
  {}
  ~Engine() noexcept;
  Engine(Engine&& other) noexcept;
  Engine& operator=(Engine&& other) noexcept;

  Engine() = delete;
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;

  static std::unique_ptr<Engine> create(const Config& config);

private:
  std::unique_ptr<Cleanup_handler> m_cleanup_handler;

  SDL_Window* m_window;
  SDL_Renderer* m_renderer;

  Background m_background;
  Font m_font;
  Texture m_text_area;
  static constexpr Uint32 sdl_init_flags {SDL_INIT_VIDEO};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_ENGINE_HPP
