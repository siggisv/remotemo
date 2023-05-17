#ifndef REMOTEMO_EXCEPTIONS_HPP
#define REMOTEMO_EXCEPTIONS_HPP

#include <exception>

namespace remotemo {
class Window_is_closed_exception : public std::exception {
public:
  const char* what() const noexcept override;
};

class User_quit_exception : public std::exception {
public:
  const char* what() const noexcept override;
};
} // namespace remotemo
#endif // REMOTEMO_EXCEPTIONS_HPP
