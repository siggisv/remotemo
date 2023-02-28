#ifndef REMOTEMO_TESTS_SRC_MOCK_SDL_HPP
#define REMOTEMO_TESTS_SRC_MOCK_SDL_HPP

#include <SDL.h>
#include <SDL_image.h>

class Mock_SDL {
  MAKE_MOCK1(mock_Init, int(Uint32));
  MAKE_MOCK0(mock_Quit, void());
  MAKE_MOCK1(mock_QuitSubSystem, void(Uint32));
  MAKE_MOCK2(mock_SetHint, SDL_bool(const char*, const char*));
  MAKE_MOCK1(mock_GetWindowID, Uint32(SDL_Window*));
  MAKE_MOCK6(mock_CreateWindow,
      SDL_Window*(const char*, int, int, int, int, Uint32));
  MAKE_MOCK1(mock_DestroyWindow, void(SDL_Window*));
  MAKE_MOCK1(mock_GetRenderer, SDL_Renderer*(SDL_Window*));
  MAKE_MOCK2(mock_GetRendererInfo, int(SDL_Renderer*, SDL_RendererInfo*));
  MAKE_MOCK4(mock_RenderCopy,
      int(SDL_Renderer*, SDL_Texture*, const SDL_Rect*, const SDL_Rect*));
  MAKE_MOCK3(mock_CreateRenderer, SDL_Renderer*(SDL_Window*, int, Uint32));
  MAKE_MOCK1(mock_DestroyRenderer, void(SDL_Renderer*));
  MAKE_MOCK0(mock_GetBasePath, char*());
  MAKE_MOCK1(mock_free, void(void*));
  MAKE_MOCK2(mock_LoadTexture, SDL_Texture*(SDL_Renderer*, const char*));
  MAKE_MOCK1(mock_DestroyTexture, void(SDL_Texture*));
  MAKE_MOCK5(
      mock_CreateTexture, SDL_Texture*(SDL_Renderer*, Uint32, int, int, int));
};
Mock_SDL mock_SDL;

class Dummy_test {
  MAKE_MOCK0(func, void());
};
Dummy_test dummy_t;

struct Dummy_object {
  char dummy_text[2000];
  int dummy_int;
};

#endif // REMOTEMO_TESTS_SRC_MOCK_SDL_HPP
