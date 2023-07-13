#include <iostream>
#include <string>

#include <remotemo/remotemo.hpp>

void run_test(remotemo::Remotemo* text_monitor)
{
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
  for (int tab = 3; tab < 10; tab++) {
    text_monitor->set_cursor_column(tab);
    text_monitor->print("At some point this should scroll...");

    for (int column = 5; column < 10; column++) {
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

  text_monitor->set_inverse(true);
  text_monitor->pause(one_second);
  text_monitor->clear(remotemo::Do_reset::none);

  text_monitor->pause(one_second);
  text_monitor->print("Bye bye!");

  text_monitor->pause(3 * one_second);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  std::cout << "Name: " << remotemo::full_name() << '\n';
  std::cout << "Version: " << remotemo::version().full << '\n';
  std::cout << "Major: " << remotemo::version().major << '\n';
  std::cout << "Minor: " << remotemo::version().minor << '\n';
  std::cout << "Patch: " << remotemo::version().patch << '\n';
  std::cout << "Pre-release label: " << remotemo::version().pre_release_label
            << '\n';
  std::cout << "Is pre-release? " << std::boolalpha
            << remotemo::version_is_pre_release() << std::noboolalpha << '\n';
  auto conf = remotemo::Config()
                  .function_pre_close([]() -> bool {
                    std::string ans;
                    std::cout << "Really close window (Y/N)? ";
                    std::getline(std::cin, ans);
                    if (ans[0] == 'Y' || ans[0] == 'y') {
                      return true;
                    }
                    return false;
                  })
                  .function_pre_quit([]() -> bool {
                    std::cout << "Oh! Quitting already?\nFine, bye!\n";
                    return true;
                  })
                  .closing_same_as_quit(true);
  auto text_monitor = remotemo::create(conf);
  if (!text_monitor) {
    return -1;
  }

  try {
    run_test(&text_monitor.value());
  } catch (const remotemo::Window_is_closed_exception& e) {
    std::cerr << "Window_is_closed_exception: " << e.what() << '\n';
  } catch (const remotemo::User_quit_exception& e) {
    std::cerr << "User_quit_exception: " << e.what() << '\n';
  }
}
