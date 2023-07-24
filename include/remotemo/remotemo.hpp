/**
 * \file
 * \brief Main header file of the remoTemo library
 *
 * remoTemo, the retro monochrome text monitor - a library for building
 * applications with a late 70s looking, monochrome, text-only user interface.
 */

#ifndef REMOTEMO_REMOTEMO_HPP
#define REMOTEMO_REMOTEMO_HPP

#include <string>
#include <optional>
#include <memory>

#include "remotemo/exceptions.hpp"
#include "remotemo/common_types.hpp"
#include "remotemo/config.hpp"

/// \brief Main namespace of the remoTemo library
namespace remotemo {
class Engine;

/**
 * \brief Main class of remoTemo, the retro monochrome text monitor.
 *
 * An object of this class will represent the monitor and contain the window
 * showing it (along with the needed textures and properties). That object
 * will also contain all the needed member functions to change its settings
 * and for I/O (i.e. keyboard input from user and output to the monitor).
 */
class Remotemo {
  friend std::optional<Remotemo> create(const Config& config);

  /// \brief Not part of public API. Allows unit tests to access private ctor
  friend Remotemo create_remotemo(
      std::unique_ptr<Engine> engine, const Config& config);

public:
  /**
   * \brief Destructor
   *
   * Takes care of cleaning up any resource owned by this object. That
   * includes not only freeing up memory, but also things like closing the
   * window, if owned.
   */
  ~Remotemo() noexcept;

  /// \brief Move-constructor
  Remotemo(Remotemo&& other) noexcept;

  /// \brief Move assignment operator
  Remotemo& operator=(Remotemo&& other) noexcept;

  /// \brief Copy-constructor (DELETED)
  Remotemo(const Remotemo&) = delete;

  /// \brief Copy assignment operator (DELETED)
  Remotemo& operator=(const Remotemo&) = delete;


  /**
   * \brief Moves the cursor a given distance
   *
   * If trying to move the cursor outside of the text area, it will move as
   * far as it can get without leaving the area and return an error code.
   *
   * E.g. if trying to make a move that would end at a row that is within the
   * area but at a column to the right of the right edge, then it would be
   * move to that row but the column would be the rightmost one within the
   * area.
   *
   * \note
   * When it comes to the cursor position, the text area is considered to have
   * one extra, empty, hidden row below the bottom one shown on screen. The
   * cursor can be moved there.
   *
   * \param width  Number of columns to move to the right (negative value to
   *               move left)
   * \param height Number of rows to move down (negative value to move up)
   * \return \c 0 on success. On error, the total value of one or two \c
   *         remotemo::Move_cursor_error, depending on which edge(s) it tried
   *         to get past.
   *
   * \sa Move_cursor_error
   */
  int move_cursor(int width, int height)
  {
    return move_cursor(Size {width, height});
  }

  /**
   * \overload
   * \param move \c remotemo::Size containing the \c width and \c height of
   *             how far to move the cursor.
   */
  int move_cursor(const Size& move);

  /**
   * \brief Moves the cursor to the given position
   *
   * If trying to set the cursor to a position outside of the text area, the
   * cursor stays where it was and the function returns \c -1
   *
   * \note
   * When it comes to the cursor position, the text area is considered to have
   * one extra, empty, hidden row below the bottom one shown on screen. The
   * cursor can be moved there.
   *
   * \todo Replace \p line with the more correct term: \c row
   *
   * \param column Desired column
   * \param line   Desired row
   * \return \c 0 on success. \c -1 on error
   */
  int set_cursor(int column, int line)
  {
    return set_cursor(Point {column, line});
  }

  /**
   * \overload
   * \param position \c remotemo::Point containing the desired position, \c x
   * for \c column and \c y for \c row.
   */
  int set_cursor(const Point& position);

  /**
   * \brief Moves the cursor to the given column (without changing its row)
   * \param column Desired column
   * \return \c 0 on success. \c -1 on error
   *
   * \sa set_cursor()
   */
  int set_cursor_column(int column);

  /**
   * \brief Moves the cursor to the given row (without changing its column)
   * \todo Replace \p line with the more correct term: \c row
   * \param line   Desired row
   * \return \c 0 on success. \c -1 on error
   *
   * \sa set_cursor()
   */
  int set_cursor_line(int line);

  /// \brief Returns the current position of the cursor
  [[nodiscard]] Point get_cursor_position() const;

  /**
   * \brief Wait for the given time (in ms)
   *
   * Checks for events and updates the window as needed while waiting.
   *
   * \param pause_in_ms Number of milliseconds to wait
   * \return \c 0 on success. \c -1 on error
   */
  int pause(int pause_in_ms);

  /**
   * \brief Clears all the text area at once
   *
   * Does so without inversing the foreground and background colors.
   *
   * \param do_reset Controls if cursor position and the inverse setting
   * should also be reset.
   *
   * \sa Do_reset
   */
  void clear(Do_reset do_reset = Do_reset::all);

  /**
   * \brief Waits for a key being pressed and then returns it.
   *
   * The character generated by the key is not echoed to the screen.
   *
   * While waiting, all events are checked and the window updated as needed.
   *
   * \note Keys that are not included in \c remotemo::Key are ignored (i.e.
   * the function continues waiting).
   *
   * \return \c remotemo::Key - represent the physical location of the key
   *
   * \sa Key
   */
  Key get_key();

  /**
   * \brief Allows the user to enter some text (NOT IMPLEMENTED YET)
   *
   * \todo Implement this function
   */
  std::string get_input(int max_length);

  /**
   * \brief Display the given string at current cursor position
   *
   * Display the given string, starting at the current position of the cursor,
   * one character at the time with a slight delay inbetween (as set by the
   * 'delay between chars' property).
   *
   * With the exception of special characters like \c backspace and \c
   * newline, each character displayed will move the cursor one space to the
   * right. If already at the right border of the text area, it will either
   * stay put or wrap to the next line depending on the wrapping settings (To
   * be implemented: If wrapping is set to \c word it might wrap even sooner).
   *
   * \param text The string to be displayed
   * \return \c 0 on success. \c -2 if some of the text could not get
   * displayed (e.g. reached end of line while wrapping was set to off).
   *
   * \warning Any non-ASCII character will be displayed as �. This will not
   * count as "text could not get displayed".
   *
   * \note If the inverse property is set to true, the string will be
   * displayed with the foreground and background colors switched.
   *
   * \note The \c newline character just moves the cursor to the beginning of
   * the next line without changing the rest of the current line.
   *
   * \note The \c backspace character will:
   *         - make the function stop and return -2 if the cursor was already
   *           at the left border. This happens even when wrapping is on.
   *         - move the cursor one space to the left and overwrite the content
   *           there with a single space character (using the current inverse
   *           property).
   *
   * \sa set_text_delay()
   * \sa set_text_speed()
   * \sa set_scrolling() for more info on how scrolling behaves
   * \sa set_wrapping()
   * \sa Wrapping for more info on how wrapping behaves
   * \sa set_inverse()
   */
  int print(const std::string& text);

  /**
   * \brief Display the given string at the given position
   *
   * Behaves as if calling first \c set_cursor() and then (if successful)
   * calling \c print()
   *
   * \param column Column to move the cursor to before printing
   * \param line Row to move the cursor to before printing
   * \param text The string to be displayed
   * \return \c 0 on success. \c -1 (without moving cursor nor printing
   * anything) if the given position is outside the text area. \c -2 if some
   * of the text could not get displayed (e.g. reached end of line while
   * wrapping was set to off).
   */
  int print_at(int column, int line, const std::string& text)
  {
    return print_at(Point {column, line}, text);
  }

  /**
   * \overload
   * \param position \c remotemo::Point containing the position to move the
   * cursor to before printing, \c x for \c column and \c y for \c row.
   */
  int print_at(const Point& position, const std::string& text);

  /**
   * \brief Return the character at the given position of the screen
   *
   * \param column Column of the given position
   * \param line Row of the given position
   *
   * \return \c '\\0' if the given position is outside the text area.
   * Otherwise returns the character that is displayed there.
   */
  [[nodiscard]] char get_char_at(int column, int line) const
  {
    return get_char_at(Point {column, line});
  }

  /**
   * \overload
   * \param pos \c remotemo::Point containing the desired position, \c x
   * for \c column and \c y for \c row.
   */
  [[nodiscard]] char get_char_at(const Point& pos) const;

  /**
   * \brief Check if foreground and background colors are inversed at given
   * position
   *
   * \param column Column of the given position
   * \param line Row of the given position
   *
   * \return \c true if the colors are inversed at the given position. \c
   * false if not inversed OR if the given position is outside the text area.
   */
  [[nodiscard]] bool is_inverse_at(int column, int line) const
  {
    return is_inverse_at(Point {column, line});
  }

  /**
   * \overload
   * \param pos \c remotemo::Point containing the desired position, \c x
   * for \c column and \c y for \c row.
   */
  [[nodiscard]] bool is_inverse_at(const Point& pos) const;

  /**
   * \brief Set the speed at which text gets displayed
   *
   * \param delay_in_ms The delay, in milliseconds, between each character
   * being displayed.
   * \return \c true on success. \c false on failure (e.g. negative number)
   */
  bool set_text_delay(int delay_in_ms);

  /**
   * \brief Set the speed at which text gets displayed
   *
   * \param char_per_second The number of characters that can get displayed
   * each second.
   * \return \c true on success. \c false on failure (e.g. negative number)
   *
   * \note \p char_per_second will be converted internally to \c delay_in_ms.
   * Some rounding might happen.
   */
  bool set_text_speed(double char_per_second);

  /**
   * \brief Return the speed at which text gets displayed
   *
   * \return The delay, in milliseconds, between each character being
   * displayed.
   */
  [[nodiscard]] int get_text_delay() const;

  /**
   * \brief Return the speed at which text gets displayed
   *
   * \return The number of characters that can get displayed each second.
   *
   * \note The speed is internally stored as \c delay_in_ms. Some rounding
   * might happen when converting to \c char_per_second.
   */
  [[nodiscard]] double get_text_speed() const;

  /**
   * \brief Set the \c scrolling property
   *
   * If the cursor moves down (because of wrapping or the \c newline
   * character) while it's already at the bottom visible row, it will move
   * down to the extra, hidden row below the visible area.
   *
   * Then when trying to print any text to the hidden row, depending on the
   * 'scrolling' settings:
   *   - If set to \c true then the whole content of the screen moves up one
   *     row (moving the cursor up to the bottom visible row at the same time)
   *     before continuing printing the rest of the text.
   *   - If set to \c false then the rest of the string is lost.
   *
   *
   * \param is_scrolling New setting of the property
   *
   * \sa print()
   * \sa print_at()
   * \sa get_scrolling()
   */
  void set_scrolling(bool is_scrolling);

  /**
   * \brief Get the \c scrolling property
   *
   * \return The current state of the \c scrolling property
   * \sa set_scrolling()
   */
  [[nodiscard]] bool get_scrolling() const;

  /**
   * \brief Set the \c wrapping property
   *
   * \param wrapping New setting of the property
   *
   * \sa print()
   * \sa print_at()
   * \sa get_wrapping()
   * \sa Wrapping for meaning of each setting
   */
  void set_wrapping(Wrapping wrapping);

  /**
   * \brief Get the \c wrapping property
   *
   * \return The current state of the \c wrapping property
   * \sa set_wrapping()
   */
  [[nodiscard]] Wrapping get_wrapping() const { return m_text_wrapping; }

  /**
   * \brief Set the \c inverse property
   *
   * \param inverse New setting of the property
   *
   * While set to \c true, printing to the screen is done with the foreground
   * and background color switched. Changing this property does not affect
   * text that had already been printed to the screen.
   *
   * \sa print()
   * \sa print_at()
   * \sa get_inverse()
   */
  void set_inverse(bool inverse);

  /**
   * \brief Get the \c inverse property
   *
   * \return The current state of the \c inverse property
   * \sa set_inverse()
   */
  [[nodiscard]] bool get_inverse() const;

private:
  Remotemo(std::unique_ptr<Engine> engine, const Config& config) noexcept;
  std::unique_ptr<Engine> m_engine {};
  Wrapping m_text_wrapping {Wrapping::character};
};

/**
 * \brief Create and initialize the monitor using the given configuration.
 *
 * Create and initialize an object representing an ASCII-only text
 * monitor. That object will also contain the window showing the monitor.
 *
 * \param config The configuration to be used
 * \return \p std::optional containing the created object unless setup failed.
 */
std::optional<Remotemo> create(const Config& config);

/**
 * \overload
 *
 * Uses the default configuration.
 */
std::optional<Remotemo> create();

/**
 * \brief Returns name of the library and its version
 *
 * The returned string is of the format "remoTemo
 * vMajor.Minor.Patch[-PreReleaseLabel]
 *
 * E.g.:
 *   - If the current version is \c 1.0.3 then "remoTemo v1.0.3" will be
 *     returned.
 *   - If the current version is \c 1.1.0-beta.2 then "remoTemo v1.1.0-beta.2"
 *     will be returned.
 *
 * \return a string containing the name and version of this library
 */
std::string full_name();

/**
 * \brief Return the version of the library
 *
 * \return A struct (\c Version) containing the version of this library
 *
 * \sa Version
 */
Version version();

/**
 * \brief Return \c true if current version is a pre-release version (alpha,
 *        beta or release-candidate)
 */
bool version_is_pre_release();
} // namespace remotemo
#endif // REMOTEMO_REMOTEMO_HPP
