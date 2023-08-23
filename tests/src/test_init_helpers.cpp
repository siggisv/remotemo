#include <sstream>
#include <filesystem>

#include "remotemo/remotemo.hpp"
#include "../../src/texture.hpp"

#include "test_init.hpp"

using std::string_literals::operator""s;

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
void SDL_GetWindowSize([[maybe_unused]] SDL_Window* window, int* w, int* h)
{ // Stub, not mock
  *w = 1000;
  *h = 1000;
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
int SDL_QueryTexture( // Stub, not mock
    [[maybe_unused]] SDL_Texture* texture, [[maybe_unused]] Uint32* format,
    [[maybe_unused]] int* access, int* w, int* h)
{
  *w = 1000;
  *h = 1000;
  return 0;
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
int SDL_SetTextureBlendMode(SDL_Texture* texture, SDL_BlendMode blendMode)
{
  return mock_SDL.mock_SetTextureBlendMode(texture, blendMode);
}
int SDL_SetTextureColorMod(SDL_Texture* texture, Uint8 r, Uint8 g, Uint8 b)
{
  return mock_SDL.mock_SetTextureColorMod(texture, r, g, b);
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
    exps->push_back(NAMED_REQUIRE_CALL(mock_SDL, mock_DestroyRenderer(render))
                        .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr,
                            seqs->t_area));
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

bool try_running_create(bool do_cleanup_all, const Conf_resources& conf_res,
    const Win_conf& win_conf, const Texture_conf& texture_conf)
{
  remotemo::Texture::reset_base_path();
  remotemo::Config config;
  config.cleanup_all(do_cleanup_all)
      .window(conf_res.res.win)
      .background(conf_res.res.backgr)
      .font_bitmap(conf_res.res.font);
  if (win_conf.title.has_value()) {
    config.window_title(*win_conf.title);
  }
  if (win_conf.size.has_value()) {
    config.window_size(*win_conf.size);
  }
  if (win_conf.pos.has_value()) {
    config.window_position(*win_conf.pos);
  }
  if (win_conf.is_resizable.has_value()) {
    config.window_resizable(*win_conf.is_resizable);
  }
  if (win_conf.is_fullscreen.has_value()) {
    config.window_fullscreen(*win_conf.is_fullscreen);
  }
  if (texture_conf.backgr_file_path.has_value()) {
    config.background_file_path(*texture_conf.backgr_file_path);
  }
  if (texture_conf.font_bitmap_file_path.has_value()) {
    config.font_bitmap_file_path(*texture_conf.font_bitmap_file_path);
  }
  if (texture_conf.font_size.has_value()) {
    config.font_size(*texture_conf.font_size);
  }
  if (texture_conf.text_area_size.has_value()) {
    config.text_area_size(*texture_conf.text_area_size);
  }
  if (texture_conf.text_blend_mode.has_value()) {
    config.text_blend_mode(*texture_conf.text_blend_mode);
  }
  if (texture_conf.text_color.has_value()) {
    config.text_color(*texture_conf.text_color);
  }
  // Silence all logging:
  SDL_LogSetAllPriority(SDL_NUM_LOG_PRIORITIES);

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

std::string Win_conf::describe() const
{
  std::ostringstream oss;

  oss << std::boolalpha;
  if (title.has_value()) {
    oss << "Title:           " << *title << "\n";
  }
  if (size.has_value()) {
    oss << "Size:            (" << size->width << ", " << size->height
        << ")\n";
  }
  if (pos.has_value()) {
    oss << "Position:        (" << pos->x << ", " << pos->y << ")\n";
  }
  if (is_resizable.has_value()) {
    oss << "Is resizable:    " << *is_resizable << "\n";
  }
  if (is_fullscreen.has_value()) {
    oss << "Is fullscreened: " << *is_fullscreen << "\n";
  }
  oss << std::noboolalpha;
  return oss.str();
}

std::string Texture_conf::describe() const
{
  std::ostringstream oss;

  if (backgr_file_path.has_value()) {
    oss << "Backgr. path:    " << *backgr_file_path << "\n";
  }
  if (font_bitmap_file_path.has_value()) {
    oss << "Font bitm. path: " << *font_bitmap_file_path << "\n";
  }
  if (font_size.has_value()) {
    oss << "Font size:       (" << font_size->width << ", "
        << font_size->height << ")\n";
  }
  if (text_area_size.has_value()) {
    oss << "Text area size:  (" << text_area_size->width << ", "
        << text_area_size->height << ")\n";
  }
  if (text_blend_mode.has_value()) {
    oss << "Text blend mode: ";
    switch (*text_blend_mode) {
      case SDL_BLENDMODE_NONE:
        oss << "[No blending]\n";
        break;
      case SDL_BLENDMODE_BLEND:
        oss << "[Alpha blending]\n";
        break;
      case SDL_BLENDMODE_ADD:
        oss << "[Additive blending]\n";
        break;
      case SDL_BLENDMODE_MOD:
        oss << "[Color modulate]\n";
        break;
      /*
      case SDL_BLENDMODE_MUL:
        oss << "[Color multiply]\n";
        break;
      */
      default:
        oss << "[UNKNOWN: " << *text_blend_mode << "] (Custom blend mode?)\n";
    }
  }
  if (text_color.has_value()) {
    oss << "Text color:      (r:" << text_color->red
        << " g:" << text_color->green << " b:" << text_color->blue << ")\n";
  }
  return oss.str();
}

void Init_status::set_res_from_config(const Conf_resources& conf)
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

void Init_status::attempt_create_window(
    bool should_success, const Win_conf& win_conf)
{
  std::string regex_title =
      "^"s + win_conf.title.value_or("Retro Monochrome Text Monitor") + "$";
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  remotemo::Size size = win_conf.size.value_or(remotemo::Size {1280, 720});
  remotemo::Point pos = win_conf.pos.value_or(
      remotemo::Point {SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED});
  Uint32 flags =
      (win_conf.is_resizable.value_or(true) ? SDL_WINDOW_RESIZABLE : 0) |
      (win_conf.is_fullscreen.value_or(false) ? SDL_WINDOW_FULLSCREEN_DESKTOP
                                              : 0);
  SDL_Window* create_win_ret = should_success ? d_new_win : nullptr;
  exps.push_back(
      NAMED_REQUIRE_CALL(mock_SDL, mock_CreateWindow(re(regex_title), pos.x,
                                       pos.y, size.width, size.height, flags))
          .RETURN(create_win_ret)
          .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr, seqs.t_area));
  ready_res.win = create_win_ret;
  to_be_cleaned_up.win = create_win_ret;
}

void Init_status::attempt_create_renderer(bool should_success)
{
  SDL_Renderer* create_render_ret = should_success ? d_new_render : nullptr;
  exps.push_back(
      NAMED_REQUIRE_CALL(mock_SDL,
          mock_CreateRenderer(ready_res.win, -1, SDL_RENDERER_TARGETTEXTURE))
          .RETURN(create_render_ret)
          .IN_SEQUENCE(seqs.main, seqs.font, seqs.backgr, seqs.t_area));
  ready_res.render = create_render_ret;
  to_be_cleaned_up.render = create_render_ret;
}

void Init_status::attempt_setup_textures(
    Texture_results expected_results, const Texture_conf& texture_conf)
{
  exp_results = expected_results;
  if (ready_res.backgr == nullptr || ready_res.font == nullptr) {
    exps_basepath.setup =
        NAMED_REQUIRE_CALL(mock_SDL, mock_GetBasePath())
            .TIMES(0, 1)
            .RETURN(exp_results.basepath)
            .IN_SEQUENCE(seqs.font, seqs.backgr, seqs.opt);
    exps_basepath.cleanup = NAMED_REQUIRE_CALL(
        mock_SDL, mock_free(static_cast<void*>(exp_results.basepath)))
                                .TIMES(0, 1)
                                .IN_SEQUENCE(seqs.opt);
  }
  if (exp_results.basepath != nullptr) {
    std::filesystem::path base_path {exp_results.basepath};
    if (ready_res.font == nullptr) {
      auto font_full_path =
          base_path / texture_conf.font_bitmap_file_path.value_or(
                          "res/img/font_bitmap.png");
      std::string regex_path = "^"s + font_full_path.string() + "$";
      exps_font.setup = NAMED_REQUIRE_CALL(
          mock_SDL, mock_LoadTexture(ready_res.render, re(regex_path)))
                            .TIMES(0, 1)
                            .RETURN(exp_results.font)
                            .IN_SEQUENCE(seqs.font);
      ready_res.font = exp_results.font;
      might_be_cleaned_up.font = exp_results.font;
    }
    if (ready_res.backgr == nullptr) {
      auto backgr_full_path =
          base_path / texture_conf.backgr_file_path.value_or(
                          "res/img/terminal_screen.png");
      std::string regex_path = "^"s + backgr_full_path.string() + "$";
      exps_backgr.setup = NAMED_REQUIRE_CALL(
          mock_SDL, mock_LoadTexture(ready_res.render, re(regex_path)))
                              .TIMES(0, 1)
                              .RETURN(exp_results.backgr)
                              .IN_SEQUENCE(seqs.backgr);
      ready_res.backgr = exp_results.backgr;
      might_be_cleaned_up.backgr = exp_results.backgr;
    }
  }
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  auto font_size = texture_conf.font_size.value_or(remotemo::Size {7, 18});
  auto t_area_size = texture_conf.text_area_size.value_or(remotemo::Size {
      40, 24}); // NOLINT(cppcoreguidelines-avoid-magic-numbers)
  exps_t_area.setup = NAMED_REQUIRE_CALL(mock_SDL,
      mock_CreateTexture(ready_res.render, SDL_PIXELFORMAT_RGBA32,
          SDL_TEXTUREACCESS_TARGET, (t_area_size.width * font_size.width) + 2,
          ((t_area_size.height + 1) * font_size.height) + 2))
                          .TIMES(0, 1)
                          .RETURN(exp_results.t_area)
                          .IN_SEQUENCE(seqs.main, seqs.t_area);
  ready_res.t_area = exp_results.t_area;
  might_be_cleaned_up.t_area = exp_results.t_area;
  if (ready_res.t_area != nullptr) {
    SDL_BlendMode text_blend_mode =
        texture_conf.text_blend_mode.value_or(SDL_BLENDMODE_ADD);
    exps_text_blend = NAMED_REQUIRE_CALL(
        mock_SDL, mock_SetTextureBlendMode(ready_res.t_area, text_blend_mode))
                          .TIMES(0, 1)
                          .RETURN(0)
                          .IN_SEQUENCE(seqs.main);
    auto text_color =
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
        texture_conf.text_color.value_or(remotemo::Color {89, 221, 0});
    exps_text_color = NAMED_REQUIRE_CALL(
        mock_SDL, mock_SetTextureColorMod(ready_res.t_area, text_color.red,
                      text_color.green, text_color.blue))
                          .TIMES(0, 1)
                          .RETURN(0)
                          .IN_SEQUENCE(seqs.t_area);
  }
}

void Init_status::expected_cleanup()
{
  if (might_be_cleaned_up.t_area != nullptr) {
    exps_t_area.cleanup = NAMED_REQUIRE_CALL(
        mock_SDL, mock_DestroyTexture(might_be_cleaned_up.t_area))
                              .TIMES(0, 1)
                              .IN_SEQUENCE(seqs.main, seqs.t_area);
  }
  if (might_be_cleaned_up.font != nullptr) {
    exps_font.cleanup = NAMED_REQUIRE_CALL(
        mock_SDL, mock_DestroyTexture(might_be_cleaned_up.font))
                            .TIMES(0, 1)
                            .IN_SEQUENCE(seqs.font);
  }
  if (might_be_cleaned_up.backgr != nullptr) {
    exps_backgr.cleanup = NAMED_REQUIRE_CALL(
        mock_SDL, mock_DestroyTexture(might_be_cleaned_up.backgr))
                              .TIMES(0, 1)
                              .IN_SEQUENCE(seqs.backgr);
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

bool Init_status::check_a_texture_failed() const
{
  if (exp_results.basepath == nullptr && exps_basepath.setup &&
      exps_basepath.setup->is_saturated()) {
    return true;
  }
  if (might_be_cleaned_up.font == nullptr && exps_font.setup &&
      exps_font.setup->is_saturated()) {
    return true;
  }
  if (might_be_cleaned_up.backgr == nullptr && exps_backgr.setup &&
      exps_backgr.setup->is_saturated()) {
    return true;
  }
  if (might_be_cleaned_up.t_area == nullptr && exps_t_area.setup &&
      exps_t_area.setup->is_saturated()) {
    return true;
  }
  return false;
}

void Init_status::check_texture_cleanup() const
{
  if (exp_results.basepath != nullptr && exps_basepath.setup) {
    REQUIRE(exps_basepath.setup->is_saturated() ==
            exps_basepath.cleanup->is_saturated());
  }
  if (might_be_cleaned_up.font != nullptr) {
    REQUIRE(
        exps_font.setup->is_saturated() == exps_font.cleanup->is_saturated());
  }
  if (might_be_cleaned_up.backgr != nullptr) {
    REQUIRE(exps_backgr.setup->is_saturated() ==
            exps_backgr.cleanup->is_saturated());
  }
  if (might_be_cleaned_up.t_area != nullptr) {
    REQUIRE(exps_t_area.setup->is_saturated() ==
            exps_t_area.cleanup->is_saturated());
    REQUIRE(
        exps_t_area.setup->is_saturated() == exps_text_blend->is_saturated());
    REQUIRE(
        exps_t_area.setup->is_saturated() == exps_text_color->is_saturated());
  }
}

void require_init_has_ended(std::list<tr_exp>* exps, Test_seqs* seqs)
{
  exps->push_back(NAMED_REQUIRE_CALL(dummy_t, func())
                      .IN_SEQUENCE(seqs->main, seqs->font, seqs->backgr,
                          seqs->t_area, seqs->opt));
}
