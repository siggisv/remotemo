#include "texture.hpp"

#include <SDL_image.h>

namespace remotemo {
std::optional<std::filesystem::path> Texture::m_base_path {};

Texture::~Texture() noexcept
{
  if (m_is_owned && m_texture != nullptr) {
    ::SDL_DestroyTexture(m_texture);
  }
}

Texture::Texture(Texture&& other) noexcept
    : m_texture(other.m_texture), m_is_owned(other.m_is_owned)
{
  other.m_is_owned = false;
  other.m_texture = nullptr;
}

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
  ::SDL_Log("Texture::base_path: %s", m_base_path->c_str());
  return true;
}

bool Texture::load(SDL_Renderer* renderer, const std::string& file_path)
{
  if (!m_base_path.has_value()) {
    if (!set_base_path()) {
      return false;
    }
  }
  auto texture_path = *m_base_path / file_path;
  SDL_Log("Texture path: %s", texture_path.c_str());
  m_texture = ::IMG_LoadTexture(renderer, texture_path.c_str());
  if (m_texture == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "IMG_LoadTexture(renderer, \"%s\") failed: %s\n",
        texture_path.c_str(), ::SDL_GetError());
    return false;
  }
  m_is_owned = true;
  return true;
}

std::optional<Texture> Texture::create_text_area(SDL_Renderer* renderer,
    const Font_config& font, const Text_area_config& text_area)
{
  auto* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
      SDL_TEXTUREACCESS_TARGET, (font.width * text_area.columns) + 2,
      (font.height * text_area.lines) + 2);
  if (texture == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_CreateTexture() failed: %s\n", ::SDL_GetError());
    return {};
  }
  SDL_SetTextureBlendMode(texture, text_area.blend_mode);
  SDL_SetTextureColorMod(texture, text_area.color.red, text_area.color.green,
      text_area.color.blue);
  return Texture {texture};
}
} // namespace remotemo
