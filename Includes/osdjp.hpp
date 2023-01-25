#include <CTRPluginFramework.hpp>

namespace CTRPluginFramework {

struct SysOSDNotify {
  std::string name;
  Color fg;
  Color bg;
  Clock time;
};

bool OSDJPNotify(const std::string& str,
                 const Color& foreground = Color::White,
                 const Color& background = Color::Black);

}  // namespace CTRPluginFramework