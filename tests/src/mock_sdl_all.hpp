#ifndef REMOTEMO_TESTS_SRC_MOCK_SDL_ALL_HPP
#define REMOTEMO_TESTS_SRC_MOCK_SDL_ALL_HPP

#include "mock_sdl.hpp"

extern "C" {
int SDL_Init(Uint32 flags)
{
  return mock_SDL.mock_Init(flags);
}
void SDL_Quit()
{
  mock_SDL.mock_Quit();
}
void SDL_QuitSubSystem(Uint32 flags)
{
  mock_SDL.mock_QuitSubSystem(flags);
}
SDL_bool SDL_SetHint(const char* name, const char* value)
{
  return mock_SDL.mock_SetHint(name, value);
}
Uint32 SDL_GetWindowID(SDL_Window* window)
{
  return mock_SDL.mock_GetWindowID(window);
}
SDL_Window* SDL_CreateWindow(
    const char* title, int x, int y, int w, int h, Uint32 flags)
{
  return mock_SDL.mock_CreateWindow(title, x, y, w, h, flags);
}
void SDL_DestroyWindow(SDL_Window* window)
{
  mock_SDL.mock_DestroyWindow(window);
}
SDL_Renderer* SDL_GetRenderer(SDL_Window* window)
{
  return mock_SDL.mock_GetRenderer(window);
}
int SDL_GetRendererInfo(SDL_Renderer* renderer, SDL_RendererInfo* info)
{
  return mock_SDL.mock_GetRendererInfo(renderer, info);
}
int SDL_RenderCopy(SDL_Renderer* renderer, SDL_Texture* texture,
    const SDL_Rect* srcrect, const SDL_Rect* dstrect)
{
  return mock_SDL.mock_RenderCopy(renderer, texture, srcrect, dstrect);
}
SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, int index, Uint32 flags)
{
  return mock_SDL.mock_CreateRenderer(window, index, flags);
}
void SDL_DestroyRenderer(SDL_Renderer* renderer)
{
  mock_SDL.mock_DestroyRenderer(renderer);
}
char* SDL_GetBasePath()
{
  return mock_SDL.mock_GetBasePath();
}
void SDL_free(void* mem)
{
  mock_SDL.mock_free(mem);
}
SDL_Texture* IMG_LoadTexture(SDL_Renderer* renderer, const char* file_path)
{
  return mock_SDL.mock_LoadTexture(renderer, file_path);
}
void SDL_DestroyTexture(SDL_Texture* texture)
{
  return mock_SDL.mock_DestroyTexture(texture);
}
SDL_Texture* SDL_CreateTexture(
    SDL_Renderer* renderer, Uint32 format, int access, int w, int h)
{
  return mock_SDL.mock_CreateTexture(renderer, format, access, w, h);
}
} // extern "C"

#endif // REMOTEMO_TESTS_SRC_MOCK_SDL_ALL_HPP
