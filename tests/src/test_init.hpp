#ifndef REMOTEMO_TESTS_SRC_TEST_INIT_HPP
#define REMOTEMO_TESTS_SRC_TEST_INIT_HPP

#include <string>
#include <list>
#include <array>
#include <memory>
#include <optional>

#include "mock_sdl.hpp"

///////////////////////////////////
// Helper structures and objects:

using tr_seq = trompeloeil::sequence;
using tr_exp = std::unique_ptr<trompeloeil::expectation>;

extern SDL_Window* const d_conf_win;
extern SDL_Window* const d_new_win;
extern SDL_Renderer* const d_conf_render;
extern SDL_Renderer* const d_new_render;
extern SDL_Texture* const d_conf_font_bitmap;
extern SDL_Texture* const d_new_font_bitmap;
extern SDL_Texture* const d_conf_backgr;
extern SDL_Texture* const d_new_backgr;
extern SDL_Texture* const d_new_text_area;

#ifdef _WIN32
constexpr char d_basepath[] = "\\dummy\\base\\path\\";
constexpr char dummy_font_path[] = "\\dummy\\base\\path\\"
                                   "res\\img\\font_bitmap.png";
constexpr char dummy_backgr_path[] = "\\dummy\\base\\path\\"
                                     "res\\img\\terminal_screen.png";
#else
constexpr char d_basepath[] = "/dummy/base/path/";
constexpr char dummy_font_path[] = "/dummy/base/path/"
                                   "res/img/font_bitmap.png";
constexpr char dummy_backgr_path[] = "/dummy/base/path/"
                                     "res/img/terminal_screen.png";
#endif

struct Test_seqs {
  tr_seq main;
  tr_seq opt;
  tr_seq backgr;
  tr_seq font;
  tr_seq t_area;
};

struct Resources {
  SDL_Window* win {nullptr};
  SDL_Renderer* render {nullptr};
  SDL_Texture* backgr {nullptr};
  SDL_Texture* font {nullptr};
  SDL_Texture* t_area {nullptr};

  void expected_cleanup(std::list<tr_exp>* exps, Test_seqs* seqs) const;
};

struct Conf_resources {
  Resources res;
  bool backgr_is_valid {true};
  bool font_is_valid {true};

  void expected_cleanup(std::list<tr_exp>* exps, Test_seqs* seqs) const;
  void check_win_has_renderer(std::list<tr_exp>* exps, Test_seqs* seqs) const;
  void all_checks_succeeds(std::list<tr_exp>* exps, Test_seqs* seqs) const;
  std::string list_textures() const;
  std::string describe() const;
};
extern const std::array<Conf_resources, 6> valid_conf_res;

struct Win_conf {
  std::optional<std::string> title {};
  std::optional<SDL_Point> size {};
  std::optional<SDL_Point> pos {};
  std::optional<bool> is_resizable {};
  std::optional<bool> is_fullscreen {};

  std::string describe() const;
};

struct Texture_conf {
  std::optional<std::string> backgr_file_path {};
  std::optional<std::string> font_bitmap_file_path {};
  std::optional<SDL_Point> font_size {};
  std::optional<SDL_Point> text_area_size {};
  std::optional<SDL_BlendMode> text_blend_mode {};
  std::optional<remotemo::Color> text_color {};

  std::string describe() const;
};

struct Texture_results {
  char* basepath {nullptr};
  SDL_Texture* backgr {nullptr};
  SDL_Texture* font {nullptr};
  SDL_Texture* t_area {nullptr};

  std::string describe() const;
};
extern const std::array<Texture_results, 10> all_texture_results;

struct Setup_cleanup_exps {
  tr_exp setup {};
  tr_exp cleanup {};
};

struct Init_status {
  bool do_cleanup_all {false};
  Uint32 init_flags {0};
  Uint32 quit_flags {0};
  std::list<tr_exp> exps {};
  Setup_cleanup_exps exps_basepath {};
  Setup_cleanup_exps exps_backgr {};
  Setup_cleanup_exps exps_font {};
  Setup_cleanup_exps exps_t_area {};
  tr_exp exps_text_blend {};
  tr_exp exps_text_color {};
  Test_seqs seqs;
  bool init_did_succeed {false};
  Resources to_be_cleaned_up {};
  Resources might_be_cleaned_up {};
  Resources ready_res {};
  Texture_results exp_results {};

  void set_res_from_config(const Conf_resources& conf);
  void attempt_init(bool should_success);
  void attempt_set_hint(bool should_success);
  void attempt_create_window(
      bool should_success, const Win_conf& win_conf = {});
  void attempt_create_renderer(bool should_success);
  void attempt_setup_textures(Texture_results expected_results,
      const Texture_conf& texture_conf = {});
  void expected_cleanup();
  bool check_a_texture_failed() const;
  void check_texture_cleanup() const;
};

// Helper functions:

void check_renderer_settings(std::list<tr_exp>* exps, SDL_Renderer* renderer,
    Test_seqs* seqs, Uint32 render_flag, int ret_val);
bool try_running_create(bool do_cleanup_all,
    const Conf_resources& conf_res = {}, const Win_conf& win_conf = {},
    const Texture_conf& texture_conf = {});
void require_init_has_ended(std::list<tr_exp>* exps, Test_seqs* seqs);

#endif // REMOTEMO_TESTS_SRC_TEST_INIT_HPP
