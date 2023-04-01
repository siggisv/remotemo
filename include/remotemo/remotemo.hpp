#ifndef REMOTEMO_REMOTEMO_HPP
#define REMOTEMO_REMOTEMO_HPP

#include <optional>
#include <memory>

#include "remotemo/common_types.hpp"
#include "remotemo/config.hpp"

// remotemo/config.hpp already includes SDL.h so including it again is not
// needed. But we do it anyway for two reasons:
// - To make it really clear to anybody using this library that including it
//   does also pull in SDL.h
// - Just to make sure that any future refactoring/redesign does not take it
//   away by accident.
#include <SDL.h>

namespace remotemo {
class Texture;
class Remotemo {
  friend std::optional<Remotemo> create(const Config& config);

public:
  ~Remotemo() noexcept;
  Remotemo(Remotemo&& other) noexcept;
  Remotemo& operator=(Remotemo&& other) noexcept;

  Remotemo(const Remotemo&) = delete;
  Remotemo& operator=(const Remotemo&) = delete;

private:
  constexpr Remotemo() noexcept = default;
  bool initialize(const Config& config);

  class Cleanup_handler;
  std::unique_ptr<Cleanup_handler> m_cleanup_handler;

  SDL_Window* m_window {nullptr};
  SDL_Renderer* m_renderer {nullptr};

  std::unique_ptr<Texture> m_background {};
  std::unique_ptr<Texture> m_font_bitmap {};
  std::unique_ptr<Texture> m_text_area {};
  static constexpr Uint32 sdl_init_flags {SDL_INIT_VIDEO};
};

std::optional<Remotemo> create();
std::optional<Remotemo> create(const Config& config);

} // namespace remotemo
#endif // REMOTEMO_REMOTEMO_HPP
