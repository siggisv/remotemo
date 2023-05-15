#include <remotemo/remotemo.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  auto text_monitor = remotemo::create();
  if (!text_monitor) {
    return -1;
  }

  constexpr int one_second = 1000;
  text_monitor->pause(one_second);
  text_monitor->set_inverse(true);
  text_monitor->set_cursor(10, 5);
  text_monitor->print("== Wrap'n'scroll test ==");
  text_monitor->set_inverse(false);

  text_monitor->pause(one_second);
  text_monitor->print_at(2, 15, "Check that cursor does not overwrite.");
  for (int column = 0; column < 25; column++) {
    text_monitor->set_cursor(column, 15);
  }

  text_monitor->print("\n\n\n");

  text_monitor->pause(one_second);
  text_monitor->set_inverse(true);
  for (int tab = 3; tab < 15; tab++) {
    text_monitor->set_cursor_column(tab);
    text_monitor->print("At some point this should scroll...");

    for (int column = 5; column < 20; column++) {
      text_monitor->set_cursor_column(column);
    }
    text_monitor->print("\n");
  }
  text_monitor->set_inverse(false);

  text_monitor->pause(one_second);
  text_monitor->print("\n\n\n\n\n\n\n");
  for (int column = 5; column < 25; column++) {
    text_monitor->set_cursor(column, 20);
  }

  text_monitor->pause(3 * one_second);
}
