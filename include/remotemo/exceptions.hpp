/**
 * \file
 * \brief Header file for exceptions used by the public API of \c remoTemo
 */

#ifndef REMOTEMO_EXCEPTIONS_HPP
#define REMOTEMO_EXCEPTIONS_HPP

#include <exception>

namespace remotemo {
/**
 * \brief Exception to be thrown when the window is closed.
 *
 * It is to be thrown not only when the window gets closed, but also whenever
 * (almost) any of the member functions of the \c Remotemo object that was
 * tied to that window.
 *
 * \note
 * If case \c closing_same_as_quit is set to \c true, then the \c
 * User_quit_exception is thrown when the window gets closed but this
 * exception still gets thrown by the member functions after that.
 */
class Window_is_closed_exception : public std::exception {
public:
  /// \brief Returns a string explaining that the window has been closed.
  const char* what() const noexcept override;
};

/**
 * \brief Exception to be raised when the user choses to quit.
 *
 * It is to be raised as soon as it became obvious that the user has chosen to
 * quit the application.
 */
class User_quit_exception : public std::exception {
public:
  /// \brief Returns a string explaining that the user chose to quit.
  const char* what() const noexcept override;
};
} // namespace remotemo
#endif // REMOTEMO_EXCEPTIONS_HPP
