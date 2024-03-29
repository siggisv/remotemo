#include "texture.hpp"

#include <SDL_image.h>

namespace remotemo {
// This is a private static member variable. That makes it **NOT** globally
// accessible but clang-tidy has a bug that makes it report it anyway
// See https://github.com/llvm/llvm-project/issues/47384
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::optional<std::filesystem::path> Texture::m_base_path {};

bool Texture::set_base_path()
{
  char* c_base_path = ::SDL_GetBasePath();
  if (c_base_path == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_GetBasePath() failed: %s\n", ::SDL_GetError());
    return false;
  }
  m_base_path = c_base_path;
  // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
  ::SDL_free(c_base_path); // NOLINT(cppcoreguidelines-owning-memory)
  return true;
}

bool Texture::load(SDL_Renderer* renderer, const std::string& file_path)
{
  if (!m_base_path.has_value()) {
    if (!set_base_path()) {
      return false;
    }
  }
  auto texture_path = (*m_base_path / file_path).lexically_normal();
#ifdef _WIN32
  auto texure_path_utf8 = texture_path.u8string();
#else // On non-Windows systems the native encoding is already utf8
  auto& texure_path_utf8 = texture_path;
#endif
  res(::IMG_LoadTexture(renderer, texure_path_utf8.c_str()));
  if (res() == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "IMG_LoadTexture(renderer, \"%s\") failed: %s\n",
        texure_path_utf8.c_str(), ::SDL_GetError());
    return false;
  }
  is_owned(true);
  if (::SDL_QueryTexture(res(), nullptr, nullptr, &m_texture_size.width,
          &m_texture_size.height) != 0) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "Getting size of texture \"%s\" failed: %s\n",
        texure_path_utf8.c_str(), ::SDL_GetError());
    return false;
  }
  return true;
}
} // namespace remotemo
