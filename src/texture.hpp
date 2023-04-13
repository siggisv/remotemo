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
  constexpr Texture() noexcept = default;
  constexpr explicit Texture(
      SDL_Texture* texture, bool is_owned = true) noexcept
      : m_texture(texture), m_is_owned(is_owned)
  {}
  ~Texture() noexcept;
  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  static std::optional<Texture> create_or_load(
      const Texture_config& texture, bool is_owned, SDL_Renderer* renderer);
  static std::optional<Texture> load(
      SDL_Renderer* renderer, const std::string& file_path);
  static std::optional<Texture> create_text_area(
      SDL_Renderer* renderer, const Config& config);
  static void reset_base_path() { base_path.reset(); }

private:
  SDL_Texture* m_texture {nullptr};
  bool m_is_owned {false};
  static std::optional<std::filesystem::path> base_path;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_TEXTURE_HPP
