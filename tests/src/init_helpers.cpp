#include <sstream>

#include "remotemo/remotemo.hpp"

#include "init.hpp"

///////////////////////////////////
// Functions redirecting to the mock-functions:

Mock_SDL mock_SDL;
Dummy_test dummy_t;

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
// NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
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
using trompeloeil::_;
using trompeloeil::re;

// While mocking the SDL-functions, we don't care about the content of the
// SDL-objects. We just care WHICH pointer gets passed in to and returned from
// the mocked functions.
Dummy_object d_0 {{"Dummy conf window"}, 0};
Dummy_object d_1 {{"Dummy new window"}, 0};
Dummy_object d_2 {{"Dummy conf renderer"}, 1};
Dummy_object d_3 {{"Dummy new renderer"}, 1};
Dummy_object d_4 {{"Dummy conf font bitmap"}, 2};
Dummy_object d_5 {{"Dummy new font bitmap"}, 2};
Dummy_object d_6 {{"Dummy conf background"}, 3};
Dummy_object d_7 {{"Dummy new background"}, 3};
Dummy_object d_8 {{"Dummy new text area"}, 4};

SDL_Window* const d_conf_win = reinterpret_cast<SDL_Window*>(&d_0);
SDL_Window* const d_new_win = reinterpret_cast<SDL_Window*>(&d_1);
SDL_Renderer* const d_conf_render = reinterpret_cast<SDL_Renderer*>(&d_2);
SDL_Renderer* const d_new_render = reinterpret_cast<SDL_Renderer*>(&d_3);
SDL_Texture* const d_conf_font_bitmap = reinterpret_cast<SDL_Texture*>(&d_4);
SDL_Texture* const d_new_font_bitmap = reinterpret_cast<SDL_Texture*>(&d_5);
SDL_Texture* const d_conf_backgr = reinterpret_cast<SDL_Texture*>(&d_6);
SDL_Texture* const d_new_backgr = reinterpret_cast<SDL_Texture*>(&d_7);
SDL_Texture* const d_new_text_area = reinterpret_cast<SDL_Texture*>(&d_8);

const std::array<Conf_resources, 6> valid_conf_res {
    {{nullptr, nullptr, nullptr, nullptr},
        {d_conf_win, nullptr, nullptr, nullptr},
        {d_conf_win, d_conf_render, nullptr, nullptr},
        {d_conf_win, d_conf_render, nullptr, d_conf_font_bitmap},
        {d_conf_win, d_conf_render, d_conf_backgr, nullptr},
        {d_conf_win, d_conf_render, d_conf_backgr, d_conf_font_bitmap}}};

///////////////////////////////////
// Helper functions:

void Resources::expected_cleanup(
    std::list<tr_exp>* exps, Test_seqs* seqs) const
{
  if (t_area != nullptr) {
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyTexture(t_area))
                        .IN_SEQUENCE(seqs->t_area));
  }
  if (font != nullptr) {
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyTexture(font))
                        .IN_SEQUENCE(seqs->font));
  }
  if (backgr != nullptr) {
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyTexture(backgr))
                        .IN_SEQUENCE(seqs->backgr));
  }
  if (render != nullptr) {
    exps->push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyRenderer(render))
            .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr, seqs->t_area));
  }
  if (win != nullptr) {
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyWindow(win))
                        .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr));
  }
}

std::string Conf_resources::list_textures() const
{
  std::ostringstream oss;

  oss << "Textures in conf:" << (res.backgr == nullptr ? "" : " [background]")
      << (res.font == nullptr ? "" : " [font_bitmap]");
  if (res.backgr == nullptr && res.font == nullptr) {
    oss << " <NONE>";
  }
  return oss.str();
}

std::string Conf_resources::describe() const
{
  std::ostringstream oss;

  oss << std::boolalpha << "Window in conf:  " << (res.win != nullptr) << "\n"
      << "Has a renderer:  " << (res.render != nullptr) << "\n"
      << std::noboolalpha << list_textures();
  return oss.str();
}

void Conf_resources::expected_cleanup(
    std::list<tr_exp>* exps, Test_seqs* seqs) const
{
  res.expected_cleanup(exps, seqs);
  exps->push_back(
      NAMED_REQUIRE_CALL(mock_SDL, mock_Quit())
          .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr, seqs->opt));
}

void Conf_resources::check_win_has_renderer(
    std::list<tr_exp>* exps, Test_seqs* seqs) const
{
  exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_GetRenderer(res.win))
                      .TIMES(AT_LEAST(1))
                      .RETURN(res.render)
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
                        .IN_SEQUENCE(seqs->main));
  } else {
    exps->push_back(NAMED_REQUIRE_CALL(
        mock_SDL, mock_GetRendererInfo(renderer, ANY(SDL_RendererInfo*)))
                        .RETURN(ret_val)
                        .IN_SEQUENCE(seqs->main));
  }
}

void Conf_resources::all_checks_succeeds(
    std::list<tr_exp>* exps, Test_seqs* seqs) const
{
  if (res.win != nullptr) {
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_GetWindowID(res.win))
                        .RETURN(1)
                        .IN_SEQUENCE(seqs->opt));
    check_win_has_renderer(exps, seqs);
  }
  if (res.render != nullptr) {
    check_renderer_settings(
        exps, res.render, seqs, SDL_RENDERER_TARGETTEXTURE, 0);
  }
  if (res.backgr != nullptr) {
    exps->push_back(NAMED_REQUIRE_CALL(
        mock_SDL, mock_RenderCopy(res.render, res.backgr, _, _))
                        .IN_SEQUENCE(seqs->backgr)
                        .RETURN(0));
  }
  if (res.font != nullptr) {
    exps->push_back(NAMED_REQUIRE_CALL(
        mock_SDL, mock_RenderCopy(res.render, res.font, _, _))
                        .IN_SEQUENCE(seqs->font)
                        .RETURN(0));
  }
}

bool try_running_create(bool do_cleanup_all, const Conf_resources& conf)
{
  remotemo::Config config;
  config.cleanup_all(do_cleanup_all)
      .the_window(conf.res.win)
      .background(conf.res.backgr)
      .font_bitmap(conf.res.font);
  auto t = remotemo::create(config);
  // This dummy function is here so we can check if cleanup happens before
  // `t` goes out of scope or after:
  dummy_t.func();
  return t.has_value();
}

std::string Texture_results::describe() const
{
  std::ostringstream oss;

  oss << std::boolalpha
      << "Basepath succeeds:         " << (basepath != nullptr) << "\n"
      << "Load font succeeds:        " << (font != nullptr) << "\n"
      << "Load backgr succeeds:      " << (backgr != nullptr) << "\n"
      << "Create text area succeeds: " << (t_area != nullptr) << "\n"
      << std::noboolalpha;
  return oss.str();
}

void Init_status::set_from_config(const Conf_resources& conf)
{
  ready_res = conf.res;
  if (do_cleanup_all) {
    to_be_cleaned_up = conf.res;
  }
}

void Init_status::attempt_init(bool should_success)
{
  int init_ret = should_success ? 0 : -1;
  exps.push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_Init(ANY(Uint32)))
                     .SIDE_EFFECT(init_flags = _1)
                     .RETURN(init_ret)
                     .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr,
                         seqs.t_area, seqs.opt));
  init_did_succeed = should_success;
}

void Init_status::attempt_set_hint(bool should_success)
{
  auto set_hint_ret = should_success ? SDL_TRUE : SDL_FALSE;
  exps.push_back(NAMED_REQUIRE_CALL(mock_SDL,
      mock_SetHint(re("^" SDL_HINT_RENDER_SCALE_QUALITY "$"), re("^linear$")))
                     .RETURN(set_hint_ret)
                     .IN_SEQUENCE(seqs.main));
}

void Init_status::attempt_create_window(bool should_success)
{
  SDL_Window* create_win_ret = should_success ? d_new_win : nullptr;
  exps.push_back(NAMED_REQUIRE_CALL(
      mock_SDL, mock_CreateWindow(re("^Retro Monochrome Text Monitor$"),
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280,
                    720, SDL_WINDOW_RESIZABLE))
                     .RETURN(create_win_ret)
                     .IN_SEQUENCE(seqs.main, seqs.t_area));
  ready_res.win = create_win_ret;
  to_be_cleaned_up.win = create_win_ret;
}

void Init_status::attempt_create_renderer(bool should_success)
{
  SDL_Renderer* create_render_ret = should_success ? d_new_render : nullptr;
  exps.push_back(NAMED_REQUIRE_CALL(mock_SDL,
      mock_CreateRenderer(ready_res.win, -1, SDL_RENDERER_TARGETTEXTURE))
                     .RETURN(create_render_ret)
                     .IN_SEQUENCE(seqs.main, seqs.t_area));
  ready_res.render = create_render_ret;
  to_be_cleaned_up.render = create_render_ret;
}

void Init_status::attempt_setup_textures(Texture_results expected_results)
{
  exp_results = expected_results;
  /*
  if (exp_get_basepath) {
    UNSCOPED_INFO("WOW! Wasn't expecting 'exp_get_basepath' to be == true");
  } else {
    UNSCOPED_INFO("Ok. That is what I expected. :D");
  }
  */
  if (ready_res.backgr == nullptr || ready_res.font == nullptr) {
    exp_get_basepath =
        NAMED_REQUIRE_CALL(mock_SDL, mock_GetBasePath())
            .TIMES(0, 1)
            .RETURN(exp_results.basepath)
            .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr, seqs.opt);
    exp_free_basepath = NAMED_REQUIRE_CALL(
        mock_SDL, mock_free(static_cast<void*>(exp_results.basepath)))
                            .TIMES(0, 1)
                            .IN_SEQUENCE(seqs.opt);
  }
  if (exp_results.basepath != nullptr && ready_res.font == nullptr) {
    exp_load_font = NAMED_REQUIRE_CALL(mock_SDL,
        mock_LoadTexture(ready_res.render, re(&regex_dummy_font_path[0])))
                        .TIMES(0, 1)
                        .RETURN(exp_results.font)
                        .IN_SEQUENCE(seqs.font);
    ready_res.font = exp_results.font;
    might_be_cleaned_up.font = exp_results.font;
  }
  if (exp_results.basepath != nullptr && ready_res.backgr == nullptr) {
    exp_load_backgr = NAMED_REQUIRE_CALL(mock_SDL,
        mock_LoadTexture(ready_res.render, re(&regex_dummy_backgr_path[0])))
                          .TIMES(0, 1)
                          .RETURN(exp_results.backgr)
                          .IN_SEQUENCE(seqs.backgr);
    ready_res.backgr = exp_results.backgr;
    might_be_cleaned_up.backgr = exp_results.backgr;
  }
  exp_create_t_area = NAMED_REQUIRE_CALL(
      mock_SDL, mock_CreateTexture(ready_res.render, SDL_PIXELFORMAT_RGBA32,
                    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
                    SDL_TEXTUREACCESS_TARGET, (40 * 7) + 2, (24 * 18) + 2))
                          .TIMES(0, 1)
                          .RETURN(exp_results.t_area)
                          .IN_SEQUENCE(seqs.t_area);
  ready_res.t_area = exp_results.t_area;
  might_be_cleaned_up.t_area = exp_results.t_area;
  /*
  if (exp_get_basepath) {
    UNSCOPED_INFO("Ok. That is still what I expected. :D");
  } else {
    UNSCOPED_INFO("Wasn't expecting 'exp_get_basepath' to be false anymore!");
  }
  SUCCEED("TEST..................................");
  */
}

void Init_status::expected_cleanup()
{
  if (might_be_cleaned_up.t_area != nullptr) {
    exps.push_back(NAMED_REQUIRE_CALL(
        mock_SDL, mock_DestroyTexture(might_be_cleaned_up.t_area))
                       .TIMES(0, 1)
                       .IN_SEQUENCE(seqs.t_area));
  }
  if (might_be_cleaned_up.font != nullptr) {
    exps.push_back(NAMED_REQUIRE_CALL(
        mock_SDL, mock_DestroyTexture(might_be_cleaned_up.font))
                       .TIMES(0, 1)
                       .IN_SEQUENCE(seqs.font));
  }
  if (might_be_cleaned_up.backgr != nullptr) {
    exps.push_back(NAMED_REQUIRE_CALL(
        mock_SDL, mock_DestroyTexture(might_be_cleaned_up.backgr))
                       .TIMES(0, 1)
                       .IN_SEQUENCE(seqs.backgr));
  }
  to_be_cleaned_up.expected_cleanup(&exps, &seqs);
  if (do_cleanup_all) {
    exps.push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_Quit())
                       .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr,
                           seqs.t_area, seqs.opt));
  } else if (init_did_succeed) {
    exps.push_back(
        NAMED_REQUIRE_CALL(mock_SDL, mock_QuitSubSystem(ANY(Uint32)))
            .LR_SIDE_EFFECT(quit_flags = _1)
            .IN_SEQUENCE(seqs.main, seqs.opt));
  }
}

void require_init_has_ended(std::list<tr_exp>* exps, Test_seqs* seqs)
{
  exps->push_back(NAMED_REQUIRE_CALL(dummy_t, func())
                      .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr,
                          seqs->t_area, seqs->opt));
}
