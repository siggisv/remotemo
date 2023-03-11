#include <string>

#include "remotemo/remotemo.hpp"

#include "init.hpp"

using trompeloeil::eq;
using trompeloeil::re;

Uint32 init_flags = 0, quit_flags = 0;
constexpr char regex_hint_name[] = "^" SDL_HINT_RENDER_SCALE_QUALITY "$";
constexpr char separator[] = "====================================="
                             "====================================";

enum Res_expectations {
  Res_Init = 0,
  Res_SetHint,
  Res_CreateWin,
  Res_CreateRender,
  Res_GetBasePath,
  Res_Load_Font,
  Res_Load_Backgr,
  Res_Create_Text_area,
  Res_MAX_NUM,
};

TEST_CASE("Test create() ...")
{
  tr_seq main_seq, basepath_seq, font_seq, backgr_seq, text_area_seq;
  tr_exp setups[Res_MAX_NUM];
  tr_exp cleanups[Res_MAX_NUM];
  tr_exp dummy_exp;
  bool should_succeed = true;

  ALLOW_CALL(mock_SDL, mock_GetRenderer(d_new_win)).RETURN(nullptr);
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
        GENERATE(static_cast<SDL_Window*>(nullptr), d_new_win);
    UNSCOPED_INFO(
        "... SDL_CreateWindow() "
        << (create_win_ret == d_new_win ? "succeeds, ..." : "fails!"));
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
          GENERATE(static_cast<SDL_Renderer*>(nullptr), d_new_render);
      UNSCOPED_INFO("... SDL_CreateRenderer() "
                    << (create_render_ret == d_new_render ? "succeeds, ..."
                                                          : "fails!"));
      setups[Res_CreateRender] = NAMED_REQUIRE_CALL(mock_SDL,
          mock_CreateRenderer(d_new_win, -1, SDL_RENDERER_TARGETTEXTURE))
                                     .RETURN(create_render_ret)
                                     .IN_SEQUENCE(main_seq);
      if (create_render_ret == nullptr) {
        should_succeed = false;
      } else {
        auto get_basepath_ret =
            GENERATE(static_cast<char*>(nullptr), d_basepath);
        UNSCOPED_INFO(
            "... SDL_GetBasePath() "
            << (get_basepath_ret == d_basepath ? "succeeds, ..." : "fails!"));
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
              GENERATE(static_cast<SDL_Texture*>(nullptr), d_new_font_bitmap);
          UNSCOPED_INFO(
              "... SDL_LoadTexture(font_bitmap) "
              << (load_font_ret == d_new_font_bitmap ? "succeeds, ..."
                                                     : "fails!"));
          setups[Res_Load_Font] = NAMED_REQUIRE_CALL(mock_SDL,
              mock_LoadTexture(d_new_render, re(regex_dummy_font_path)))
                                      .RETURN(load_font_ret)
                                      .IN_SEQUENCE(main_seq, font_seq);
          if (load_font_ret == nullptr) {
            should_succeed = false;
          } else {
            // Load background
            auto load_backgr_ret =
                GENERATE(static_cast<SDL_Texture*>(nullptr), d_new_backgr);
            UNSCOPED_INFO(
                "... SDL_LoadTexture(background) "
                << (load_backgr_ret == d_new_backgr ? "succeeds, ..."
                                                    : "fails!"));
            setups[Res_Load_Backgr] = NAMED_REQUIRE_CALL(mock_SDL,
                mock_LoadTexture(d_new_render, re(regex_dummy_backgr_path)))
                                          .RETURN(load_backgr_ret)
                                          .IN_SEQUENCE(main_seq, backgr_seq);
            if (load_backgr_ret == nullptr) {
              should_succeed = false;
            } else {
              auto create_text_area_ret = GENERATE(
                  static_cast<SDL_Texture*>(nullptr), d_new_text_area);
              UNSCOPED_INFO("... SDL_CreateTexture(text area) "
                            << (create_text_area_ret == d_new_text_area
                                       ? "succeeds, ..."
                                       : "fails!"));
              setups[Res_Create_Text_area] = NAMED_REQUIRE_CALL(mock_SDL,
                  mock_CreateTexture(d_new_render, SDL_PIXELFORMAT_RGBA32,
                      SDL_TEXTUREACCESS_TARGET, (40 * 7) + 2, (24 * 18) + 2))
                                                 .RETURN(create_text_area_ret)
                                                 .IN_SEQUENCE(
                                                     main_seq, text_area_seq);
              if (create_text_area_ret == nullptr) {
                should_succeed = false;
              } else {
                // If no fatal failures happened, cleanup shouldn't happen
                // until the returned object goes out of scope.
                dummy_exp = NAMED_REQUIRE_CALL(dummy_t, func())
                                .IN_SEQUENCE(main_seq, font_seq, backgr_seq,
                                    text_area_seq);
                cleanups[Res_Create_Text_area] =
                    NAMED_REQUIRE_CALL(
                        mock_SDL, mock_DestroyTexture(d_new_text_area))
                        .IN_SEQUENCE(text_area_seq);
              }
              cleanups[Res_Load_Backgr] = NAMED_REQUIRE_CALL(
                  mock_SDL, mock_DestroyTexture(d_new_backgr))
                                              .IN_SEQUENCE(backgr_seq);
            }
            cleanups[Res_Load_Font] = NAMED_REQUIRE_CALL(
                mock_SDL, mock_DestroyTexture(d_new_font_bitmap))
                                          .IN_SEQUENCE(font_seq);
          }
        }
        cleanups[Res_CreateRender] =
            NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyRenderer(d_new_render))
                .IN_SEQUENCE(main_seq, font_seq, backgr_seq, text_area_seq);
      }
      cleanups[Res_CreateWin] =
          NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyWindow(d_new_win))
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
    dummy_exp =
        NAMED_REQUIRE_CALL(dummy_t, func())
            .IN_SEQUENCE(main_seq, font_seq, backgr_seq, text_area_seq);
  }

  // This is just to trigger output of all unscoped_info messages:
  SUCCEED(separator);

  REQUIRE(try_running_create(do_cleanup_all) == should_succeed);
  if (!do_cleanup_all && init_ret == 0) {
    REQUIRE(init_flags == quit_flags);
  }
}
