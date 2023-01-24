#include <CTRPluginFramework.hpp>

namespace CTRPluginFramework
{
  struct Sys_OSD
  {
    std::string name;
    Color fg;
    Color bg;
    Clock time;
  };

  // bool OSDJP(const Screen &screen);

  bool OSDJPNotify(const std::string &str, const Color &foreground = Color::White, const Color &background = Color::Black);
}