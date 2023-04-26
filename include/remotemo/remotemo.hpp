#ifndef REMOTEMO_REMOTEMO_HPP
#define REMOTEMO_REMOTEMO_HPP

#include <string>
#include <optional>
#include <memory>

#include "remotemo/common_types.hpp"
#include "remotemo/config.hpp"

#include <SDL.h>

namespace remotemo {
class Engine;
class Remotemo {
  friend std::optional<Remotemo> create(const Config& config);

public:
  ~Remotemo() noexcept;
  Remotemo(Remotemo&& other) noexcept;
  Remotemo& operator=(Remotemo&& other) noexcept;

  Remotemo(const Remotemo&) = delete;
  Remotemo& operator=(const Remotemo&) = delete;

  int move_cursor(int x, int y);
  int move_cursor(const SDL_Point& move)
  {
    return move_cursor(move.x, move.y);
  }
  int set_cursor(int column, int line);
  int set_cursor(const SDL_Point& position)
  {
    return set_cursor(position.x, position.y);
  }
  int set_cursor_column(int column);
  int set_cursor_line(int line);
  SDL_Point get_cursor_position();

  int pause(int pause);
  int clear();
  Key get_key();
  std::string get_input(int max_length);

  int print(const std::string& text);
  int print_at(int column, int line, const std::string& text);
  int print_at(const SDL_Point& position, const std::string& text)
  {
    return print_at(position.x, position.y, text);
  }
  int set_inverse(bool inverse);

private:
  constexpr Remotemo() noexcept = default;
  bool initialize(const Config& config);

  std::unique_ptr<Engine> m_engine {};
};

std::optional<Remotemo> create();
std::optional<Remotemo> create(const Config& config);
} // namespace remotemo
#endif // REMOTEMO_REMOTEMO_HPP
