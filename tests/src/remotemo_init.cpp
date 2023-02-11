#include "remotemo/remotemo.hpp"

#include <string>
#include <catch2/catch_all.hpp>
#include <catch2/trompeloeil.hpp>

using trompeloeil::eq;
using trompeloeil::re;


class Mock_SDL {
  MAKE_MOCK1(mock_Init, int(Uint32));
  MAKE_MOCK0(mock_Quit, void());
  MAKE_MOCK1(mock_QuitSubSystem, void(Uint32));
  MAKE_MOCK2(mock_SetHint, SDL_bool(const char*, const char*));
  MAKE_MOCK6(mock_CreateWindow,
      SDL_Window*(const char*, int, int, int, int, Uint32));
  MAKE_MOCK1(mock_DestroyWindow, void(SDL_Window*));
  MAKE_MOCK3(mock_CreateRenderer, SDL_Renderer*(SDL_Window*, int, Uint32));
  MAKE_MOCK1(mock_DestroyRenderer, void(SDL_Renderer*));
  MAKE_MOCK0(mock_GetBasePath, char*());
  MAKE_MOCK1(mock_free, void(void*));
  MAKE_MOCK2(mock_LoadTexture, SDL_Texture*(SDL_Renderer*, const char*));
  MAKE_MOCK1(mock_DestroyTexture, void(SDL_Texture*));
};
Mock_SDL mock_SDL;


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
  return nullptr;
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
} // extern "C"


class Dummy_test {
  MAKE_MOCK0(func, void());
};
Dummy_test dummy_t;

struct Dummy_object {
  char dummy_text[2000];
  int dummy_int;
};
Dummy_object dummy_w {{"Dummy window."}, 5};
SDL_Window* dummy_window = reinterpret_cast<SDL_Window*>(&dummy_w);

Dummy_object dummy_r {{"Dummy renderer."}, 5};
SDL_Renderer* dummy_renderer = reinterpret_cast<SDL_Renderer*>(&dummy_r);

Dummy_object dummy_f {{"Dummy font bitmap."}, 5};
SDL_Texture* dummy_font_bitmap = reinterpret_cast<SDL_Texture*>(&dummy_f);

Dummy_object dummy_b {{"Dummy background."}, 5};
SDL_Texture* dummy_background = reinterpret_cast<SDL_Texture*>(&dummy_b);

#ifdef _WIN32
constexpr char dummy_basepath[] = "\\dummy\\base\\path\\";
constexpr char regex_dummy_font_path[] = "^\\dummy\\base\\path\\"
                                         "res\\img\\font_bitmap.png$";
constexpr char regex_dummy_backgr_path[] = "^\\dummy\\base\\path\\"
                                           "res\\img\\terminal_screen.png$";
#else
constexpr char dummy_basepath[] = "/dummy/base/path/";
constexpr char regex_dummy_font_path[] = "^/dummy/base/path/"
                                         "res/img/font_bitmap.png$";
constexpr char regex_dummy_backgr_path[] = "^/dummy/base/path/"
                                           "res/img/terminal_screen.png$";
#endif
Uint32 init_flags = 0, quit_flags = 0;
constexpr char regex_hint_name[] = "^" SDL_HINT_RENDER_SCALE_QUALITY "$";
constexpr char separator[] = "====================================="
                             "====================================";

enum Resources {
  Res_Init = 0,
  Res_SetHint,
  Res_CreateWin,
  Res_CreateRender,
  Res_GetBasePath,
  Res_Load_Font,
  Res_Load_Backgr,
  Res_MAX_NUM,
};

TEST_CASE("Test create() ...")
{
  trompeloeil::sequence main_seq, basepath_seq, font_seq, backgr_seq;
  std::unique_ptr<trompeloeil::expectation> setups[Res_MAX_NUM];
  std::unique_ptr<trompeloeil::expectation> cleanups[Res_MAX_NUM];
  std::unique_ptr<trompeloeil::expectation> dummy_exp;
  bool should_succeed = true;

  auto do_cleanup_all = GENERATE(true, false);
  UNSCOPED_INFO(separator);
  UNSCOPED_INFO("... with config.cleanup_all("
                << (do_cleanup_all ? "true) ..." : "false) ..."));

  auto init_ret = GENERATE(-1, 0);
  UNSCOPED_INFO(
      "... SDL_Init() " << (init_ret == 0 ? "succeeds, ..." : "fails!"));
  setups[Res_Init] = NAMED_REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32)))
                         .SIDE_EFFECT(init_flags = _1)
                         .RETURN(init_ret)
                         .IN_SEQUENCE(main_seq);
  if (init_ret == -1) {
    should_succeed = false;
  } else {
    auto set_hint_ret = GENERATE(SDL_TRUE, SDL_FALSE);
    UNSCOPED_INFO(
        "... SDL_SetHint() "
        << (set_hint_ret == SDL_TRUE
                   ? "succeeds, ..."
                   : "fails! (but isn't fatal so we continue), ..."));
    setups[Res_SetHint] = NAMED_REQUIRE_CALL(
        mock_SDL, mock_SetHint(re(regex_hint_name), re("^linear$")))
                              .RETURN(set_hint_ret)
                              .IN_SEQUENCE(main_seq);

    auto create_win_ret =
        GENERATE(static_cast<SDL_Window*>(nullptr), dummy_window);
    UNSCOPED_INFO(
        "... SDL_CreateWindow() "
        << (create_win_ret == dummy_window ? "succeeds, ..." : "fails!"));
    setups[Res_CreateWin] = NAMED_REQUIRE_CALL(
        mock_SDL, mock_CreateWindow(re("^Retro Monochrome Text Monitor$"),
                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280,
                      720, SDL_WINDOW_RESIZABLE))
                                .RETURN(create_win_ret)
                                .IN_SEQUENCE(main_seq);
    if (create_win_ret == nullptr) {
      should_succeed = false;
    } else {
      auto create_render_ret =
          GENERATE(static_cast<SDL_Renderer*>(nullptr), dummy_renderer);
      UNSCOPED_INFO("... SDL_CreateRenderer() "
                    << (create_render_ret == dummy_renderer ? "succeeds, ..."
                                                            : "fails!"));
      setups[Res_CreateRender] = NAMED_REQUIRE_CALL(mock_SDL,
          mock_CreateRenderer(dummy_window, -1, SDL_RENDERER_TARGETTEXTURE))
                                     .RETURN(create_render_ret)
                                     .IN_SEQUENCE(main_seq);
      if (create_render_ret == nullptr) {
        should_succeed = false;
      } else {
        auto get_basepath_ret =
            GENERATE(static_cast<char*>(nullptr), dummy_basepath);
        UNSCOPED_INFO("... SDL_GetBasePath() "
                      << (get_basepath_ret == dummy_basepath ? "succeeds, ..."
                                                             : "fails!"));
        setups[Res_GetBasePath] =
            NAMED_REQUIRE_CALL(mock_SDL, mock_GetBasePath())
                .RETURN(get_basepath_ret)
                .IN_SEQUENCE(main_seq, basepath_seq);
        if (get_basepath_ret == nullptr) {
          should_succeed = false;
        } else {
          cleanups[Res_GetBasePath] = NAMED_REQUIRE_CALL(
              mock_SDL, mock_free(static_cast<void*>(get_basepath_ret)))
                                          .IN_SEQUENCE(basepath_seq);
          auto load_font_ret =
              GENERATE(static_cast<SDL_Texture*>(nullptr), dummy_font_bitmap);
          UNSCOPED_INFO(
              "... SDL_LoadTexture(font_bitmap) "
              << (load_font_ret == dummy_font_bitmap ? "succeeds, ..."
                                                     : "fails!"));
          setups[Res_Load_Font] = NAMED_REQUIRE_CALL(mock_SDL,
              mock_LoadTexture(dummy_renderer, re(regex_dummy_font_path)))
                                      .RETURN(load_font_ret)
                                      .IN_SEQUENCE(main_seq, font_seq);
          if (load_font_ret == nullptr) {
            should_succeed = false;
          } else {
            // Load background
            auto load_backgr_ret = GENERATE(
                static_cast<SDL_Texture*>(nullptr), dummy_background);
            UNSCOPED_INFO(
                "... SDL_LoadTexture(background) "
                << (load_backgr_ret == dummy_background ? "succeeds, ..."
                                                        : "fails!"));
            setups[Res_Load_Backgr] = NAMED_REQUIRE_CALL(mock_SDL,
                mock_LoadTexture(dummy_renderer, re(regex_dummy_backgr_path)))
                                          .RETURN(load_backgr_ret)
                                          .IN_SEQUENCE(main_seq, backgr_seq);
            if (load_backgr_ret == nullptr) {
              should_succeed = false;
            } else {
              // If no fatal failures happened, cleanup shouldn't happen until
              // the returned object goes out of scope.
              dummy_exp = NAMED_REQUIRE_CALL(dummy_t, func())
                              .IN_SEQUENCE(main_seq, font_seq, backgr_seq);
              cleanups[Res_Load_Backgr] = NAMED_REQUIRE_CALL(
                  mock_SDL, mock_DestroyTexture(dummy_background))
                                              .IN_SEQUENCE(backgr_seq);
            }
            cleanups[Res_Load_Font] = NAMED_REQUIRE_CALL(
                mock_SDL, mock_DestroyTexture(dummy_font_bitmap))
                                          .IN_SEQUENCE(font_seq);
          }
        }
        cleanups[Res_CreateRender] =
            NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyRenderer(dummy_renderer))
                .IN_SEQUENCE(main_seq, font_seq, backgr_seq);
      }
      cleanups[Res_CreateWin] =
          NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyWindow(dummy_window))
              .IN_SEQUENCE(main_seq);
    }
    if (!do_cleanup_all) {
      cleanups[Res_Init] =
          NAMED_REQUIRE_CALL(mock_SDL, mock_QuitSubSystem(ANY(Uint32)))
              .SIDE_EFFECT(quit_flags = _1)
              .IN_SEQUENCE(main_seq);
    }
  }
  if (do_cleanup_all) {
    // Unless do_cleanup_all is false, SDL_Quit should always be called at the
    // end of cleanup.
    cleanups[Res_Init] =
        NAMED_REQUIRE_CALL(mock_SDL, mock_Quit()).IN_SEQUENCE(main_seq);
  }

  if (should_succeed == false) {
    // If some non-optional setup failed, cleanup should happen before the
    // end of the create() function.
    dummy_exp = NAMED_REQUIRE_CALL(dummy_t, func())
                    .IN_SEQUENCE(main_seq, font_seq, backgr_seq);
  }

  // This is just to trigger output of all unscoped_info messages:
  SUCCEED(separator);

  {
    auto t = remoTemo::create(remoTemo::Config().cleanup_all(do_cleanup_all));
    REQUIRE(t.has_value() == should_succeed);
    // This dummy function is here so we can check if cleanup happens before
    // `t` goes out of scope or after:
    dummy_t.func();
  }
  if (!do_cleanup_all && init_ret == 0) {
    REQUIRE(init_flags == quit_flags);
  }
}
