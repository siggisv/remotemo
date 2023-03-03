#ifndef REMOTEMO_TESTS_SRC_INIT_HPP
#define REMOTEMO_TESTS_SRC_INIT_HPP

#include <list>
#include <memory>

#include "mock_sdl.hpp"

// Functions redirecting to the mock-functions:

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

///////////////////////////////////
// Helper structures and objects:

using tr_seq = trompeloeil::sequence;
using tr_exp = std::unique_ptr<trompeloeil::expectation>;

Dummy_object dummies[] {
    {{"Dummy conf window"}, 0},
    {{"Dummy new window"}, 1},
    {{"Dummy conf renderer"}, 2},
    {{"Dummy new renderer"}, 3},
    {{"Dummy conf font bitmap"}, 4},
    {{"Dummy new font bitmap"}, 5},
    {{"Dummy conf background"}, 6},
    {{"Dummy new background"}, 7},
    {{"Dummy new text area"}, 8},
};
SDL_Window* d_conf_win = reinterpret_cast<SDL_Window*>(&dummies[0]);
SDL_Window* d_new_win = reinterpret_cast<SDL_Window*>(&dummies[1]);
SDL_Renderer* d_conf_render = reinterpret_cast<SDL_Renderer*>(&dummies[2]);
SDL_Renderer* d_new_render = reinterpret_cast<SDL_Renderer*>(&dummies[3]);
SDL_Texture* d_conf_font_bitmap = reinterpret_cast<SDL_Texture*>(&dummies[4]);
SDL_Texture* d_new_font_bitmap = reinterpret_cast<SDL_Texture*>(&dummies[5]);
SDL_Texture* d_conf_backgr = reinterpret_cast<SDL_Texture*>(&dummies[6]);
SDL_Texture* d_new_backgr = reinterpret_cast<SDL_Texture*>(&dummies[7]);
SDL_Texture* d_new_text_area = reinterpret_cast<SDL_Texture*>(&dummies[8]);

struct Test_seqs {
  tr_seq main;
  tr_seq opt;
  tr_seq backgr;
  tr_seq font;
  tr_seq t_area;
};

struct Conf_resources {
  SDL_Window* conf_win;
  SDL_Renderer* conf_render;
  SDL_Texture* conf_backgr;
  SDL_Texture* conf_font;
  bool conf_backgr_is_valid;
  bool conf_font_is_valid;

  void check_cleanup(std::list<tr_exp>* exps, Test_seqs* seqs) const;
  void check_win_has_renderer(std::list<tr_exp>* exps, Test_seqs* seqs) const;
};

// Helper functions:

void Conf_resources::check_cleanup(
    std::list<tr_exp>* exps, Test_seqs* seqs) const
{
  if (conf_font != nullptr) {
    exps->push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyTexture(conf_font))
            .IN_SEQUENCE(seqs->font));
  }
  if (conf_backgr != nullptr) {
    exps->push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyTexture(conf_backgr))
            .IN_SEQUENCE(seqs->backgr));
  }
  if (conf_render != nullptr) {
    exps->push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyRenderer(conf_render))
            .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  }
  if (conf_win != nullptr) {
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyWindow(conf_win))
                        .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  }
  exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_Quit())
                      .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
}

void Conf_resources::check_win_has_renderer(
    std::list<tr_exp>* exps, Test_seqs* seqs) const
{
  exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_GetRenderer(conf_win))
                      .TIMES(AT_LEAST(1))
                      .RETURN(conf_render)
                      .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
}

void check_renderer_settings(std::list<tr_exp>* exps, SDL_Renderer* renderer,
    Test_seqs* seqs, Uint32 render_flag, int ret_val)
{
  if (ret_val == 0) {
    exps->push_back(NAMED_REQUIRE_CALL(
        mock_SDL, mock_GetRendererInfo(renderer, ANY(SDL_RendererInfo*)))
                        .SIDE_EFFECT(_2->flags = render_flag)
                        .RETURN(0)
                        .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  } else {
    exps->push_back(NAMED_REQUIRE_CALL(
        mock_SDL, mock_GetRendererInfo(renderer, ANY(SDL_RendererInfo*)))
                        .RETURN(ret_val)
                        .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  }
}

bool try_running_create(bool do_cleanup_all, const Conf_resources& res = {})
{
  remotemo::Config config;
  config.cleanup_all(do_cleanup_all)
      .the_window(res.conf_win)
      .background(res.conf_backgr)
      .font_bitmap(res.conf_font);
  auto t = remotemo::create(config);
  // This dummy function is here so we can check if cleanup happens before
  // `t` goes out of scope or after:
  dummy_t.func();
  return t.has_value();
}

#endif // REMOTEMO_TESTS_SRC_INIT_HPP
