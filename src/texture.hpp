#ifndef REMOTEMO_SRC_TEXTURE_HPP
#define REMOTEMO_SRC_TEXTURE_HPP

#include <utility>
#include <filesystem>
#include <string>
#include <optional>

#include "remotemo/config.hpp"
#include "res_handler.hpp"
#include <SDL.h>

namespace remotemo {
class Texture : public Res_handler<SDL_Texture> {
public:
  constexpr explicit Texture(
      SDL_Texture* texture, bool is_owned = true) noexcept
      : Res_handler<SDL_Texture>(texture, is_owned)
  {}
  constexpr explicit Texture(Res_handler<SDL_Texture>&& res_handler) noexcept
      : Res_handler<SDL_Texture>(std::move(res_handler))
  {}

  bool load(SDL_Renderer* renderer, const std::string& file_path);
  static void reset_base_path() { m_base_path.reset(); }
  static bool set_base_path();

private:
  static std::optional<std::filesystem::path> m_base_path;
};
} // namespace remotemo
#endif // REMOTEMO_SRC_TEXTURE_HPP
