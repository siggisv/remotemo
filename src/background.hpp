#ifndef REMOTEMO_SRC_BACKGROUND_HPP
#define REMOTEMO_SRC_BACKGROUND_HPP

#include <utility>
#include <string>
#include <optional>

#include "remotemo/config.hpp"
#include "res_handler.hpp"
#include "texture.hpp"

namespace remotemo {
class Background : public Texture {
public:
  Background(
      const Backgr_config& backgr_config, Texture&& backgr_texture) noexcept
      : Texture(std::move(backgr_texture)),
        m_min_area(backgr_config.min_area),
        m_text_area(backgr_config.text_area)
  {}

  static std::optional<Background> create(const Backgr_config& backgr_config,
      Res_handler<SDL_Texture>&& backgr_texture, SDL_Renderer* renderer);
  [[nodiscard]] const Rect<int>& min_area() const { return m_min_area; }
  [[nodiscard]] const Rect<float>& text_area() const { return m_text_area; }

private:
  Rect<int> m_min_area;
  Rect<float> m_text_area;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_BACKGROUND_HPP
