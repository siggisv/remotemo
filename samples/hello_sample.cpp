#include <remotemo/remotemo.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  auto text_monitor = remotemo::create();
  if (!text_monitor) {
    return -1;
  }

  text_monitor->set_cursor(10, 2);
  text_monitor->print("Hello world!!!");

  text_monitor->pause(2500);

  text_monitor->set_inverse(true);
  text_monitor->print_at(5, 15, "Press any key to quit");
  text_monitor->set_inverse(false);
  text_monitor->print("\n");

  while (text_monitor->get_key() == remotemo::Key::K_n) {
    text_monitor->print("Actually no! Any key except that key :P\n");
  }
}
