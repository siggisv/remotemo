/**
 * \file
 * \brief Header file for enums and structs used by the public API of \c
 *        remoTemo.
 */

#ifndef REMOTEMO_COMMON_TYPES_HPP
#define REMOTEMO_COMMON_TYPES_HPP

#include <string>
#include <SDL.h>

namespace remotemo {

/**
 * \struct Version
 * \brief Used for representing the version number of the library.
 *
 * \var Version::major
 * \brief Increased when incompatible changes are made to the public API.
 *
 * \var Version::minor
 * \brief Increased when backward compatible changes are made to the public
 * API.
 *
 * \var Version::patch
 * \brief Increased when backward compatible bugfixes are made.
 *
 * \var Version::pre_release_label
 * \brief "alpha", "beta" or "rc", followed by a dot and an incrementing
 * number.
 *
 * \var Version::full
 * \brief Full version number: "Major.Minor.Patch-PreReleaseLabel".
 */
struct Version {
  int major;
  int minor;
  int patch;
  std::string pre_release_label;
  std::string full;
};

/**
 * \enum Wrapping
 * \brief Used to set/get how wrapping of text behaves.
 *
 * \var off
 * text printed beyond the right border gets lost.
 *
 * \var character
 * text wraps to next line, possibly splitting a word in two.
 *
 * \var word
 * text wraps to next line, if possible without splitting any words.
 *
 * This will split a line that goes beyond the right border at the last
 * whitespace before the word that would go beyond that border.
 * \todo Word wrap NOT IMPLEMENTED YET
 */
enum class Wrapping { off, character, word };

/**
 * \enum Do_reset
 * \brief Used to control behaviour of Remotemo::clear()
 *
 * \var none
 * Clear screen without resetting either cursor nor inverse setting.
 *
 * \var cursor
 * Clear screen and reset cursor position to (0, 0).
 *
 * \var inverse
 * Clear screen and reset inverse to \c false.
 *
 * \var all
 * Clear screen and reset both.
 */
enum class Do_reset { none, cursor, inverse, all };

/**
 * \brief Error codes for when move_cursor() tries to go past any border.
 *
 * When trying to go past more than one border, the values are added together
 */
enum class Move_cursor_error {
  past_right_edge = -1,  ///< Trying to move cursor past the right edge.
  past_bottom_edge = -2, ///< Trying to move cursor past the bottom edge.
  past_left_edge = -4,   ///< Trying to move cursor past the left edge.
  past_top_edge = -8     ///< Trying to move cursor past the top edge.
};

/**
 * \brief Used to store the size (as opposed to a position) of an object
 *        (or a movement).
 */
struct Size {
  int width;
  int height;
};

/**
 * \struct Point
 * \brief Used to store a position.
 *
 * \var Point::x
 * \brief position along the x-axis (aka columns)
 *
 * \var Point::y
 * \brief position along the y-axis (aka rows)
 */
struct Point {
  int x;
  int y;
};

/**
 * \struct Rect
 * \brief Used to store the position and size of an object.
 *
 * \var Rect::x
 * \brief position along the x-axis (aka columns)
 *
 * \var Rect::y
 * \brief position along the y-axis (aka rows)
 */
template<class T> struct Rect {
  T x;
  T y;
  T width;
  T height;
};

/**
 * \brief Used to store the rgb-values of a color.
 *
 * Each value is an integer in the range 0 to 255 (0x00 to 0xff).
 */
struct Color {
  Uint8 red {}, green {}, blue {};
};

/**
 * \enum Mod_keys_strict
 * \brief Modifier key combos that can not result in a character.
 *
 * I.e. when pressing any key with one of those modifier combo, you will not
 * get a character.
 *
 * Used to set keyboard shortcuts that include keys that normally should be
 * handled as regular input (E.g. the key `q` normally results in the
 * character 'q' or 'Q' being input, depending on if the `shift` key is being
 * pressed. To use that key for a keyboard shortcut it \b must be modified
 * with one of those modifier key combos).
 *
 * \todo Should differentiate between `Alt` and `AltGr`.
 * \todo Should handle modifier keys being different on Mac.
 */
enum class Mod_keys_strict {
  Ctrl = KMOD_CTRL,                                    ///< .
  Alt = KMOD_ALT,                                      ///< .
  Alt_shift = (KMOD_ALT | KMOD_SHIFT),                 ///< .
  Ctrl_shift = (KMOD_CTRL | KMOD_SHIFT),               ///< .
  Ctrl_alt = (KMOD_CTRL | KMOD_ALT),                   ///< .
  Ctrl_alt_shift = (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT) ///< .
};

/**
 * \enum Mod_keys
 * \brief All possible modifier key combos (including none pressed).
 *
 * Used only to set shortcuts that include the 'F'-keys.
 *
 * \todo Should differentiate between `Alt` and `AltGr`.
 * \todo Should handle modifier keys being different on Mac.
 */
enum class Mod_keys {
  None = KMOD_NONE,                                    ///< .
  Shift = KMOD_SHIFT,                                  ///< .
  Ctrl = KMOD_CTRL,                                    ///< .
  Alt = KMOD_ALT,                                      ///< .
  Alt_shift = (KMOD_ALT | KMOD_SHIFT),                 ///< .
  Ctrl_shift = (KMOD_CTRL | KMOD_SHIFT),               ///< .
  Ctrl_alt = (KMOD_CTRL | KMOD_ALT),                   ///< .
  Ctrl_alt_shift = (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT) ///< .
};

/// \brief The 'F'-keys (i.e. F1 - F12).
enum class F_key {
  F1 = SDL_SCANCODE_F1,   ///< .
  F2 = SDL_SCANCODE_F2,   ///< .
  F3 = SDL_SCANCODE_F3,   ///< .
  F4 = SDL_SCANCODE_F4,   ///< .
  F5 = SDL_SCANCODE_F5,   ///< .
  F6 = SDL_SCANCODE_F6,   ///< .
  F7 = SDL_SCANCODE_F7,   ///< .
  F8 = SDL_SCANCODE_F8,   ///< .
  F9 = SDL_SCANCODE_F9,   ///< .
  F10 = SDL_SCANCODE_F10, ///< .
  F11 = SDL_SCANCODE_F11, ///< .
  F12 = SDL_SCANCODE_F12  ///< .
};

/**
 * \enum Key
 * \brief All keys of a compact keyboard (except modifier keys).
 *
 * The following keys of a regular keyboard are not included:
 * the F-keys, the keypad, the modifier keys and keys such as `Print screen`,
 * `Page up`, etc.
 *
 * I.e. those are only the alphanumerical keys, `Esc`, `Tab`, `Backspace`,
 * `Return`, `Space` and the arrow keys.
 *
 * \note Those values represent the physical location of the keys on the
 * keyboard and are not connected in any way to the current keyboard layout.
 * \note
 * E.g. `K_q` always represent the key just right of the `Tab`-key, even when
 * using an "azerty"- or "Dworak"-layout.
 *
 * \note
 * The names of the values are based on the "US-ANSI"-layout.
 *
 */
enum class Key {
  K_esc = SDL_SCANCODE_ESCAPE, ///< \c ESC
  /// \c ` - The leftmost key of the number row.
  K_grave = SDL_SCANCODE_GRAVE,
  K_1 = SDL_SCANCODE_1,           ///< .
  K_2 = SDL_SCANCODE_2,           ///< .
  K_3 = SDL_SCANCODE_3,           ///< .
  K_4 = SDL_SCANCODE_4,           ///< .
  K_5 = SDL_SCANCODE_5,           ///< .
  K_6 = SDL_SCANCODE_6,           ///< .
  K_7 = SDL_SCANCODE_7,           ///< .
  K_8 = SDL_SCANCODE_8,           ///< .
  K_9 = SDL_SCANCODE_9,           ///< .
  K_0 = SDL_SCANCODE_0,           ///< .
  K_minus = SDL_SCANCODE_MINUS,   ///< \c -
  K_equals = SDL_SCANCODE_EQUALS, ///< \c =
  K_backspace = SDL_SCANCODE_BACKSPACE,
  ///< \c Backspace - The rightmost key of the number row.

  /// \c Tab - The leftmost key of the top row.
  K_tab = SDL_SCANCODE_TAB,
  K_q = SDL_SCANCODE_Q,                        ///< .
  K_w = SDL_SCANCODE_W,                        ///< .
  K_e = SDL_SCANCODE_E,                        ///< .
  K_r = SDL_SCANCODE_R,                        ///< .
  K_t = SDL_SCANCODE_T,                        ///< .
  K_y = SDL_SCANCODE_Y,                        ///< .
  K_u = SDL_SCANCODE_U,                        ///< .
  K_i = SDL_SCANCODE_I,                        ///< .
  K_o = SDL_SCANCODE_O,                        ///< .
  K_p = SDL_SCANCODE_P,                        ///< .
  K_left_bracket = SDL_SCANCODE_LEFTBRACKET,   ///< \c [
  K_right_bracket = SDL_SCANCODE_RIGHTBRACKET, ///< \c ]
  K_backslash = SDL_SCANCODE_BACKSLASH,
  /**< \c \\ - The rightmost key of the top row on ANSI keyboards.
   *
   * On an ISO keyboard, this key would be to the lower left of the \c
   * Return/Enter key.
   */

  /// The leftmost key of the home row (not counting Caps Lock).
  K_a = SDL_SCANCODE_A,
  K_s = SDL_SCANCODE_S,                   ///< .
  K_d = SDL_SCANCODE_D,                   ///< .
  K_f = SDL_SCANCODE_F,                   ///< .
  K_g = SDL_SCANCODE_G,                   ///< .
  K_h = SDL_SCANCODE_H,                   ///< .
  K_j = SDL_SCANCODE_J,                   ///< .
  K_k = SDL_SCANCODE_K,                   ///< .
  K_l = SDL_SCANCODE_L,                   ///< .
  K_semicolon = SDL_SCANCODE_SEMICOLON,   ///< \c ;
  K_apostrophe = SDL_SCANCODE_APOSTROPHE, ///< \c '
  K_return = SDL_SCANCODE_RETURN,
  /**< \c Return/Enter - The rightmost key of the home row on ANSI keyboards.
   *
   * On an ISO keyboard, this key would have a height of two rows. It would be
   * the rightmost key of the top row \b and the home row.
   */

  /// The leftmost key of the bottom row (not counting Left Shift).
  K_z = SDL_SCANCODE_Z,
  K_x = SDL_SCANCODE_X,           ///< .
  K_c = SDL_SCANCODE_C,           ///< .
  K_v = SDL_SCANCODE_V,           ///< .
  K_b = SDL_SCANCODE_B,           ///< .
  K_n = SDL_SCANCODE_N,           ///< .
  K_m = SDL_SCANCODE_M,           ///< .
  K_comma = SDL_SCANCODE_COMMA,   ///< \c ,
  K_period = SDL_SCANCODE_PERIOD, ///< \c .
  K_slash = SDL_SCANCODE_SLASH,
  ///< \c / - The rightmost key of the bottom row (not counting Right Shift).

  K_space = SDL_SCANCODE_SPACE, ///< The space bar
  // Arrow keys:
  K_up = SDL_SCANCODE_UP,      ///< Up arrow key
  K_down = SDL_SCANCODE_DOWN,  ///< Down arrow key
  K_left = SDL_SCANCODE_LEFT,  ///< Left arrow key
  K_right = SDL_SCANCODE_RIGHT ///< Right arrow key
};

/**
 * \brief Used for keyboard shortcuts (consist of modifier keys + a key)
 *
 * To restrict the shortcuts to being only combinations that do not result in
 * input that needs to be handled as normal input, the constructors and the
 * setters only accept either:
 * - A \p remotemo::Mod_keys and a \p remotemo::F_key
 * - A \p remotemo::Mod_keys_strict and a \p remotemo::Key
 *
 * even though internally the combo is stored using less strict enums.
 *
 * \note
 * Yes. We know that \p static_cast can easily be used to get past those
 * restrictions.
 * \note
 * But absolutly no promisses are made regarding what happens if you do. It
 * might be fine in one version, it might crash your application (right away
 * or later, who knows?) in another version, it might cast an exception in yet
 * another version or it might silently filter out invalid input.
 *
 * \todo
 * Change the \p Key_combo class, the enums it uses and any code that handless
 * the keyboard so that it can return (and store?) \p remotemo::Mod_keys and
 * \p remotemo::Key.
 *
 * \todo
 * Add some method for the user of the library to check if a key really is
 * part of the enums (to be used in case the library user needs to take some
 * input from the end user and doesn't want to just blindly cast that to one
 * of those enums).
 */
class Key_combo {
public:
  /**
   * \brief Constructor that accept an F-key and any modifier keys.
   *
   * \param modifier The modifier keys to be used
   * \param f_key    The key to be used
   */
  Key_combo(Mod_keys modifier, F_key f_key)
      : m_modifier_keys {static_cast<SDL_Keymod>(modifier)},
        m_key {static_cast<SDL_Scancode>(f_key)}
  {}
  /**
   * \brief Constructor that accept a "normal" key and a strict subset of the
   *        modifier key combinations.
   *
   * \param modifier The modifier keys to be used
   * \param a_key    The key to be used
   */
  Key_combo(Mod_keys_strict modifier, Key a_key)
      : m_modifier_keys {static_cast<SDL_Keymod>(modifier)},
        m_key {static_cast<SDL_Scancode>(a_key)}
  {}
  /**
   * \brief Setter that accept an F-key and any modifier keys
   *
   * \param modifier The modifier keys to be used
   * \param f_key    The key to be used
   */
  void set(Mod_keys modifier, F_key f_key)
  {
    m_modifier_keys = static_cast<SDL_Keymod>(modifier);
    m_key = static_cast<SDL_Scancode>(f_key);
  }
  /**
   * \brief Setter that accept a "normal" key and a strict subset of the
   *        modifier key combinations.
   *
   * \param modifier The modifier keys to be used
   * \param a_key    The key to be used
   */
  void set(Mod_keys_strict modifier, Key a_key)
  {
    m_modifier_keys = static_cast<SDL_Keymod>(modifier);
    m_key = static_cast<SDL_Scancode>(a_key);
  }
  /**
   * \brief Getter that returns the modifier keys of the shortcut
   * \return The modifier keys of the shortcut.
   */
  [[nodiscard]] const SDL_Keymod& modifier_keys() const
  {
    return m_modifier_keys;
  }
  /**
   * \brief Getter that returns the "key" part of the shortcut
   * \return The "key" part of the shortcut.
   */
  [[nodiscard]] const SDL_Scancode& key() const { return m_key; }

private:
  SDL_Keymod m_modifier_keys;
  SDL_Scancode m_key;
};

} // namespace remotemo
#endif // REMOTEMO_COMMON_TYPES_HPP
