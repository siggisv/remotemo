#ifndef REMOTEMO_SRC_KEYBOARD_HPP
#define REMOTEMO_SRC_KEYBOARD_HPP

#include <map>
#include <optional>

#include "remotemo/common_types.hpp"

#include <SDL.h>

namespace remotemo {
class Keyboard {
public:
  static constexpr SDL_Scancode key_to_scancode(Key key)
  {
    return static_cast<SDL_Scancode>(key);
  }
  static std::optional<Key> scancode_to_key(SDL_Scancode scancode);
};

} // namespace remotemo
#endif // REMOTEMO_SRC_KEYBOARD_HPP
