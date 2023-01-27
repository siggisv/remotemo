#pragma once

#include <optional>

#include "remotemo/common_types.hpp"
#include "remotemo/config.hpp"

// remotemo/config.hpp already includes SDL.h so including it again is not
// needed. But we do it anyway for two reasons:
// - To make it really clear to anybody using this library that including it
//   does also pull in SDL.h
// - Just to make sure that any future refactoring/redesign does not take it
//   away by accident.
#include <SDL.h>

namespace remoTemo {
class Temo {
  friend std::optional<Temo> create(const Config& config);

public:
  ~Temo() noexcept;
  Temo(Temo&& other) noexcept;
  Temo& operator=(Temo&& other) noexcept;

  Temo(const Temo&) = delete;
  Temo& operator=(const Temo&) = delete;

private:
  constexpr Temo() noexcept = default;
  bool initialize(const Config& config);

  bool m_cleanup_all = true;
  static constexpr Uint32 sdl_init_flags = SDL_INIT_VIDEO;
};

std::optional<Temo> create();
std::optional<Temo> create(const Config& config);

} // namespace remoTemo
