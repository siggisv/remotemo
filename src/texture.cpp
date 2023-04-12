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

std::unique_ptr<Texture> Texture::load(
    SDL_Renderer* renderer, const std::string& file_path)
{
  if (!base_path.has_value()) {
    char* c_base_path = ::SDL_GetBasePath();
    if (c_base_path == nullptr) {
      ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
          "SDL_GetBasePath() failed: %s\n", ::SDL_GetError());
      return nullptr;
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
    return nullptr;
  }
  return std::make_unique<Texture>(texture);
}

std::unique_ptr<Texture> Texture::create_text_area(
    SDL_Renderer* renderer, const Config& config)
{
  auto* text_area = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
      SDL_TEXTUREACCESS_TARGET,
      (config.m_font_width * config.m_text_area_columns) + 2,
      (config.m_font_height * config.m_text_area_lines) + 2);
  if (text_area == nullptr) {
    ::SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
        "SDL_CreateTexture() failed: %s\n", ::SDL_GetError());
    return nullptr;
  }
  SDL_SetTextureBlendMode(text_area, config.m_text_blend_mode);
  SDL_SetTextureColorMod(text_area, config.m_text_color.red,
      config.m_text_color.green, config.m_text_color.blue);
  return std::make_unique<Texture>(text_area);
}
} // namespace remotemo
