#include "keyboard.hpp"

namespace remotemo {

std::optional<Key> Keyboard::scancode_to_key(SDL_Scancode scancode)
{
  static const std::map<SDL_Scancode, Key> map_scancode_to_key = {
      {SDL_SCANCODE_ESCAPE, Key::K_esc}, //          esc
      // Number row keys:
      {SDL_SCANCODE_GRAVE, Key::K_grave},         // '`'
      {SDL_SCANCODE_1, Key::K_1},                 // '1'
      {SDL_SCANCODE_2, Key::K_2},                 // '2'
      {SDL_SCANCODE_3, Key::K_3},                 // '3'
      {SDL_SCANCODE_4, Key::K_4},                 // '4'
      {SDL_SCANCODE_5, Key::K_5},                 // '5'
      {SDL_SCANCODE_6, Key::K_6},                 // '6'
      {SDL_SCANCODE_7, Key::K_7},                 // '7'
      {SDL_SCANCODE_8, Key::K_8},                 // '8'
      {SDL_SCANCODE_9, Key::K_9},                 // '9'
      {SDL_SCANCODE_0, Key::K_0},                 // '0'
      {SDL_SCANCODE_MINUS, Key::K_minus},         // '-'
      {SDL_SCANCODE_EQUALS, Key::K_equals},       // '='
      {SDL_SCANCODE_BACKSPACE, Key::K_backspace}, // \b (backspace)
      // Top row keys:
      {SDL_SCANCODE_TAB, Key::K_tab},                    // \t (tab)
      {SDL_SCANCODE_Q, Key::K_q},                        // 'q'
      {SDL_SCANCODE_W, Key::K_w},                        // 'w'
      {SDL_SCANCODE_E, Key::K_e},                        // 'e'
      {SDL_SCANCODE_R, Key::K_r},                        // 'r'
      {SDL_SCANCODE_T, Key::K_t},                        // 't'
      {SDL_SCANCODE_Y, Key::K_y},                        // 'y'
      {SDL_SCANCODE_U, Key::K_u},                        // 'u'
      {SDL_SCANCODE_I, Key::K_i},                        // 'i'
      {SDL_SCANCODE_O, Key::K_o},                        // 'o'
      {SDL_SCANCODE_P, Key::K_p},                        // 'p'
      {SDL_SCANCODE_LEFTBRACKET, Key::K_left_bracket},   // '['
      {SDL_SCANCODE_RIGHTBRACKET, Key::K_right_bracket}, // ']'
      {SDL_SCANCODE_BACKSLASH, Key::K_backslash},        // '\'
      // Home row keys:
      {SDL_SCANCODE_A, Key::K_a},                   // 'a'
      {SDL_SCANCODE_S, Key::K_s},                   // 's'
      {SDL_SCANCODE_D, Key::K_d},                   // 'd'
      {SDL_SCANCODE_F, Key::K_f},                   // 'f'
      {SDL_SCANCODE_G, Key::K_g},                   // 'g'
      {SDL_SCANCODE_H, Key::K_h},                   // 'h'
      {SDL_SCANCODE_J, Key::K_j},                   // 'j'
      {SDL_SCANCODE_K, Key::K_k},                   // 'k'
      {SDL_SCANCODE_L, Key::K_l},                   // 'l'
      {SDL_SCANCODE_SEMICOLON, Key::K_semicolon},   // ';'
      {SDL_SCANCODE_APOSTROPHE, Key::K_apostrophe}, // '''
      {SDL_SCANCODE_RETURN, Key::K_return},         // \r (return)
      // Bottom row keys:
      {SDL_SCANCODE_Z, Key::K_z},           // 'z'
      {SDL_SCANCODE_X, Key::K_x},           // 'x'
      {SDL_SCANCODE_C, Key::K_c},           // 'c'
      {SDL_SCANCODE_V, Key::K_v},           // 'v'
      {SDL_SCANCODE_B, Key::K_b},           // 'b'
      {SDL_SCANCODE_N, Key::K_n},           // 'n'
      {SDL_SCANCODE_M, Key::K_m},           // 'm'
      {SDL_SCANCODE_COMMA, Key::K_comma},   // ','
      {SDL_SCANCODE_PERIOD, Key::K_period}, // '.'
      {SDL_SCANCODE_SLASH, Key::K_slash},   // '/'
      // Spacebar row:
      {SDL_SCANCODE_SPACE, Key::K_space},
      // Arrow keys:
      {SDL_SCANCODE_UP, Key::K_up},      // up
      {SDL_SCANCODE_DOWN, Key::K_down},  // down
      {SDL_SCANCODE_LEFT, Key::K_left},  // left
      {SDL_SCANCODE_RIGHT, Key::K_right} // right
  };
  auto search = map_scancode_to_key.find(scancode);
  if (search != map_scancode_to_key.end()) {
    return search->second;
  }
  return {};
}


} // namespace remotemo
