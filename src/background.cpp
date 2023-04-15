#include "background.hpp"

namespace remotemo {

std::optional<Background> Background::create(
    const Backgr_config& backgr_config, bool is_owned, SDL_Renderer* renderer)
{
  Background backgr {backgr_config, is_owned};
  if (backgr_config.raw_sdl == nullptr) {
    if (!backgr.load(renderer, backgr_config.file_path)) {
      return {};
    }
  }
  return backgr;
}
} // namespace remotemo
