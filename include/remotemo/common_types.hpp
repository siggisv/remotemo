#ifndef REMOTEMO_COMMON_TYPES_HPP
#define REMOTEMO_COMMON_TYPES_HPP

#include <SDL.h>

namespace remotemo {

enum class Wrapping { off, character, word };
enum class Do_reset { none, cursor, inverse, all };

struct Size {
  int width;
  int height;
};

struct Point {
  int x;
  int y;
};

struct Color {
  Uint8 red {}, green {}, blue {};
};

enum class Mod_keys_strict {
  Ctrl = KMOD_CTRL,
  Alt = KMOD_ALT,
  Alt_shift = (KMOD_ALT | KMOD_SHIFT),
  Ctrl_shift = (KMOD_CTRL | KMOD_SHIFT),
  Ctrl_alt = (KMOD_CTRL | KMOD_ALT),
  Ctrl_alt_shift = (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT)
};

enum class Mod_keys {
  None = KMOD_NONE,
  Shift = KMOD_SHIFT,
  Ctrl = KMOD_CTRL,
  Alt = KMOD_ALT,
  Alt_shift = (KMOD_ALT | KMOD_SHIFT),
  Ctrl_shift = (KMOD_CTRL | KMOD_SHIFT),
  Ctrl_alt = (KMOD_CTRL | KMOD_ALT),
  Ctrl_alt_shift = (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT)
};

enum class F_key {
  F1 = SDL_SCANCODE_F1,
  F2 = SDL_SCANCODE_F2,
  F3 = SDL_SCANCODE_F3,
  F4 = SDL_SCANCODE_F4,
  F5 = SDL_SCANCODE_F5,
  F6 = SDL_SCANCODE_F6,
  F7 = SDL_SCANCODE_F7,
  F8 = SDL_SCANCODE_F8,
  F9 = SDL_SCANCODE_F9,
  F10 = SDL_SCANCODE_F10,
  F11 = SDL_SCANCODE_F11,
  F12 = SDL_SCANCODE_F12
};

enum class Key {
  K_esc = SDL_SCANCODE_ESCAPE,
  // Number row keys:
  K_grave = SDL_SCANCODE_GRAVE, // '`'
  K_1 = SDL_SCANCODE_1,
  K_2 = SDL_SCANCODE_2,
  K_3 = SDL_SCANCODE_3,
  K_4 = SDL_SCANCODE_4,
  K_5 = SDL_SCANCODE_5,
  K_6 = SDL_SCANCODE_6,
  K_7 = SDL_SCANCODE_7,
  K_8 = SDL_SCANCODE_8,
  K_9 = SDL_SCANCODE_9,
  K_0 = SDL_SCANCODE_0,
  K_minus = SDL_SCANCODE_MINUS,   // '-'
  K_equals = SDL_SCANCODE_EQUALS, // '='
  K_backspace = SDL_SCANCODE_BACKSPACE,
  // Top row keys:
  K_tab = SDL_SCANCODE_TAB,
  K_q = SDL_SCANCODE_Q,
  K_w = SDL_SCANCODE_W,
  K_e = SDL_SCANCODE_E,
  K_r = SDL_SCANCODE_R,
  K_t = SDL_SCANCODE_T,
  K_y = SDL_SCANCODE_Y,
  K_u = SDL_SCANCODE_U,
  K_i = SDL_SCANCODE_I,
  K_o = SDL_SCANCODE_O,
  K_p = SDL_SCANCODE_P,
  K_left_bracket = SDL_SCANCODE_LEFTBRACKET,   // '['
  K_right_bracket = SDL_SCANCODE_RIGHTBRACKET, // ']'
  K_backslash = SDL_SCANCODE_BACKSLASH,        // '\'
  // Home row keys:
  K_a = SDL_SCANCODE_A,
  K_s = SDL_SCANCODE_S,
  K_d = SDL_SCANCODE_D,
  K_f = SDL_SCANCODE_F,
  K_g = SDL_SCANCODE_G,
  K_h = SDL_SCANCODE_H,
  K_j = SDL_SCANCODE_J,
  K_k = SDL_SCANCODE_K,
  K_l = SDL_SCANCODE_L,
  K_semicolon = SDL_SCANCODE_SEMICOLON,   // ';'
  K_apostrophe = SDL_SCANCODE_APOSTROPHE, // '''
  K_return = SDL_SCANCODE_RETURN,
  // Bottom row keys:
  K_z = SDL_SCANCODE_Z,
  K_x = SDL_SCANCODE_X,
  K_c = SDL_SCANCODE_C,
  K_v = SDL_SCANCODE_V,
  K_b = SDL_SCANCODE_B,
  K_n = SDL_SCANCODE_N,
  K_m = SDL_SCANCODE_M,
  K_comma = SDL_SCANCODE_COMMA,   // ','
  K_period = SDL_SCANCODE_PERIOD, // '.'
  K_slash = SDL_SCANCODE_SLASH,   // '/'
  // Spacebar row:
  K_space = SDL_SCANCODE_SPACE,
  // Arrow keys:
  K_up = SDL_SCANCODE_UP,
  K_down = SDL_SCANCODE_DOWN,
  K_left = SDL_SCANCODE_LEFT,
  K_right = SDL_SCANCODE_RIGHT
};

class Key_combo {
public:
  Key_combo(Mod_keys modifier, F_key key)
      : m_modifier_keys {static_cast<SDL_Keymod>(modifier)},
        m_key {static_cast<SDL_Scancode>(key)}
  {}
  Key_combo(Mod_keys_strict modifier, Key key)
      : m_modifier_keys {static_cast<SDL_Keymod>(modifier)},
        m_key {static_cast<SDL_Scancode>(key)}
  {}
  void set(Mod_keys modifier, F_key key)
  {
    m_modifier_keys = static_cast<SDL_Keymod>(modifier);
    m_key = static_cast<SDL_Scancode>(key);
  }
  void set(Mod_keys_strict modifier, Key key)
  {
    m_modifier_keys = static_cast<SDL_Keymod>(modifier);
    m_key = static_cast<SDL_Scancode>(key);
  }
  [[nodiscard]] const SDL_Keymod& modifier_keys() const
  {
    return m_modifier_keys;
  }
  [[nodiscard]] const SDL_Scancode& key() const { return m_key; }

private:
  SDL_Keymod m_modifier_keys;
  SDL_Scancode m_key;
};

} // namespace remotemo
#endif // REMOTEMO_COMMON_TYPES_HPP
