#include "texture.hpp"

#include <SDL_image.h>

namespace remotemo {
std::optional<std::filesystem::path> Texture::base_path {};

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

std::optional<Texture> Texture::create_or_load(
    const Texture_config& texture, bool is_owned, SDL_Renderer* renderer)
{
  if (texture.raw_sdl != nullptr) {
    return Texture {texture.raw_sdl, is_owned};
  }
  return Texture::load(renderer, texture.file_path);
}

std::optional<Texture> Texture::load(
    SDL_Renderer* renderer, const std::string& file_path)
{
  if (!base_path.has_value()) {
    char* c_base_path = ::SDL_GetBasePath();
    if (c_base_path == nullptr) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_GetBasePath() failed: %s\n", ::SDL_GetError());
      return {};
    }
    base_path = c_base_path;
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    ::SDL_free(c_base_path); // NOLINT(cppcoreguidelines-owning-memory)
    ::SDL_Log("Texture::base_path: %s", base_path->c_str());
  }
  auto texture_path = *base_path / file_path;
  SDL_Log("Texture path: %s", texture_path.c_str());
  auto* texture = ::IMG_LoadTexture(renderer, texture_path.c_str());
  if (texture == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "IMG_LoadTexture(renderer, \"%s\") failed: %s\n",
        texture_path.c_str(), ::SDL_GetError());
    return {};
  }
  return Texture {texture};
}

std::optional<Texture> Texture::create_text_area(
    SDL_Renderer* renderer, const Config& config)
{
  auto* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
      SDL_TEXTUREACCESS_TARGET,
      (config.m_font_width * config.m_text_area_columns) + 2,
      (config.m_font_height * config.m_text_area_lines) + 2);
  if (texture == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_CreateTexture() failed: %s\n", ::SDL_GetError());
    return {};
  }
  SDL_SetTextureBlendMode(texture, config.m_text_blend_mode);
  SDL_SetTextureColorMod(texture, config.m_text_color.red,
      config.m_text_color.green, config.m_text_color.blue);
  return Texture {texture};
}
} // namespace remotemo
