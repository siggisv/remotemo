#ifndef REMOTEMO_TESTS_SRC_INIT_HPP
#define REMOTEMO_TESTS_SRC_INIT_HPP

#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <memory>

#include "mock_sdl.hpp"

///////////////////////////////////
// Helper structures and objects:

using tr_seq = trompeloeil::sequence;
using tr_exp = std::unique_ptr<trompeloeil::expectation>;

extern SDL_Window* d_conf_win;
extern SDL_Window* d_new_win;
extern SDL_Renderer* d_conf_render;
extern SDL_Renderer* d_new_render;
extern SDL_Texture* d_conf_font_bitmap;
extern SDL_Texture* d_new_font_bitmap;
extern SDL_Texture* d_conf_backgr;
extern SDL_Texture* d_new_backgr;
extern SDL_Texture* d_new_text_area;

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
  bool conf_backgr_is_valid {true};
  bool conf_font_is_valid {true};

  void check_cleanup(std::list<tr_exp>* exps, Test_seqs* seqs) const;
  void check_win_has_renderer(std::list<tr_exp>* exps, Test_seqs* seqs) const;
  void all_checks_succeeds(std::list<tr_exp>* exps, Test_seqs* seqs) const;
  std::string list_textures() const;
  std::string describe() const;
};

extern const std::vector<Conf_resources> valid_resources;

// Helper functions:

void check_renderer_settings(std::list<tr_exp>* exps, SDL_Renderer* renderer,
    Test_seqs* seqs, Uint32 render_flag, int ret_val);
bool try_running_create(bool do_cleanup_all, const Conf_resources& res = {});

#endif // REMOTEMO_TESTS_SRC_INIT_HPP
