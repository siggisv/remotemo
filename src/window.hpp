#ifndef REMOTEMO_SRC_WINDOW_HPP
#define REMOTEMO_SRC_WINDOW_HPP

#include <string>
#include <utility>
#include <optional>

#include "remotemo/config.hpp"
#include "res_handler.hpp"
#include <SDL.h>

namespace remotemo {
class Window : public Res_handler<SDL_Window> {
public:
  constexpr explicit Window(Res_handler<SDL_Window>&& res_handler) noexcept
      : Res_handler<SDL_Window>(std::move(res_handler))
  {}

  static std::optional<Window> create(const Window_config& window_config,
      Res_handler<SDL_Window>&& res_handler);
  void refresh_local_size();
  void refresh_local_flags();
  void set_fullscreen(bool do_make_fullscreen);
  void set_minimized(bool is_minimized) {m_is_minimized = is_minimized; }
  [[nodiscard]] const Size& size() const { return m_size; }
  [[nodiscard]] bool is_fullscreen() const { return m_is_fullscreen; }
  [[nodiscard]] bool is_minimized() const { return m_is_minimized; }

private:
  bool setup(const Window_config& window_config);

  Size m_size {0, 0};
  bool m_is_fullscreen {false};
  bool m_is_minimized {false};
};
} // namespace remotemo
#endif // REMOTEMO_SRC_WINDOW_HPP
