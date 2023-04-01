#ifndef REMOTEMO_SRC_TEXTURE_HPP
#define REMOTEMO_SRC_TEXTURE_HPP

#include <memory>
#include <filesystem>
#include <string>
#include <optional>

#include "remotemo/config.hpp"
#include <SDL.h>

namespace remotemo {
class Texture {
public:
  ~Texture() noexcept;
  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  Texture() = delete;
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  static std::unique_ptr<Texture> create(SDL_Texture* texture, bool is_owned);
  static std::unique_ptr<Texture> load(
      SDL_Renderer* renderer, const std::string& file_path);
  static std::unique_ptr<Texture> create_text_area(
      SDL_Renderer* renderer, const Config& config);
  static void reset_base_path() { base_path.reset(); }

private:
  constexpr explicit Texture(
      SDL_Texture* texture, bool is_owned = true) noexcept
      : m_texture(texture), m_is_owned(is_owned)
  {}

  SDL_Texture* const m_texture;
  const bool m_is_owned;
  static std::optional<std::filesystem::path> base_path;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_TEXTURE_HPP
