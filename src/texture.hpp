#ifndef REMOTEMO_SRC_TEXTURE_HPP
#define REMOTEMO_SRC_TEXTURE_HPP

#include <filesystem>
#include <string>
#include <optional>

#include "remotemo/config.hpp"
#include <SDL.h>

namespace remotemo {
class Texture {
public:
  constexpr explicit Texture(
      SDL_Texture* texture, bool is_owned = true) noexcept
      : m_texture(texture), m_is_owned(is_owned)
  {}
  virtual ~Texture() noexcept;
  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  Texture() = delete;
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  bool load(SDL_Renderer* renderer, const std::string& file_path);
  static std::optional<Texture> create_text_area(SDL_Renderer* renderer,
      const Font_config& font, const Text_area_config& text_area);
  static void reset_base_path() { m_base_path.reset(); }
  static bool set_base_path();

protected:
  SDL_Texture* m_texture;
  bool m_is_owned;

private:
  static std::optional<std::filesystem::path> m_base_path;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_TEXTURE_HPP
