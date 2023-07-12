#ifndef REMOTEMO_REMOTEMO_HPP
#define REMOTEMO_REMOTEMO_HPP

#include <string>
#include <optional>
#include <memory>

#include "remotemo/exceptions.hpp"
#include "remotemo/common_types.hpp"
#include "remotemo/config.hpp"

namespace remotemo {
class Engine;
class Remotemo {
  friend std::optional<Remotemo> create(const Config& config);
  // To allow unit tests to access private ctor:
  friend Remotemo create_remotemo(
      std::unique_ptr<Engine> engine, const Config& config);

public:
  ~Remotemo() noexcept;
  Remotemo(Remotemo&& other) noexcept;
  Remotemo& operator=(Remotemo&& other) noexcept;

  Remotemo(const Remotemo&) = delete;
  Remotemo& operator=(const Remotemo&) = delete;

  int move_cursor(int width, int height)
  {
    return move_cursor(Size {width, height});
  }
  int move_cursor(const Size& move);
  int set_cursor(int column, int line)
  {
    return set_cursor(Point {column, line});
  }
  int set_cursor(const Point& position);
  int set_cursor_column(int column);
  int set_cursor_line(int line);
  [[nodiscard]] Point get_cursor_position() const;

  int pause(int pause_in_ms);
  void clear(Do_reset do_reset = Do_reset::all);
  Key get_key();
  std::string get_input(int max_length);

  int print(const std::string& text);
  int print_at(int column, int line, const std::string& text)
  {
    return print_at(Point {column, line}, text);
  }
  int print_at(const Point& position, const std::string& text);
  [[nodiscard]] char get_char_at(const Point& pos) const;
  [[nodiscard]] char get_char_at(int column, int line) const
  {
    return get_char_at(Point {column, line});
  }
  [[nodiscard]] bool is_inverse_at(const Point& pos) const;
  [[nodiscard]] bool is_inverse_at(int column, int line) const
  {
    return is_inverse_at(Point {column, line});
  }
  bool set_text_delay(int delay_in_ms);
  bool set_text_speed(double char_per_second);
  void set_scrolling(bool is_scrolling);
  void set_wrapping(Wrapping wrapping);
  void set_inverse(bool inverse);
  [[nodiscard]] int get_text_delay() const;
  [[nodiscard]] double get_text_speed() const;
  [[nodiscard]] bool get_scrolling() const;
  [[nodiscard]] Wrapping get_wrapping() const { return m_text_wrapping; }
  [[nodiscard]] bool get_inverse() const;

private:
  Remotemo(std::unique_ptr<Engine> engine, const Config& config) noexcept;
  std::unique_ptr<Engine> m_engine {};
  Wrapping m_text_wrapping {Wrapping::character};
};

std::optional<Remotemo> create();
std::optional<Remotemo> create(const Config& config);

std::string full_name();
std::string version_full();
int version_major();
int version_minor();
int version_patch();
std::string version_pre_release_label();
bool version_is_pre_release();
} // namespace remotemo
#endif // REMOTEMO_REMOTEMO_HPP
