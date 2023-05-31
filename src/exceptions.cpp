#include "remotemo/exceptions.hpp"

namespace remotemo {
const char* Window_is_closed_exception::what() const noexcept
{
  return "The window displaying this Remotemo (Retro monochrome text "
         "monitor) has been closed!";
}

const char* User_quit_exception::what() const noexcept
{
  return "The user has chosen to quit!";
}
} // namespace remotemo
