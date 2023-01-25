#include "osdjp.hpp"

static constexpr auto notify_list_elements_max = 12;

namespace CTRPluginFramework {
static std::vector<SysOSDNotify> OSDJPlist;

static bool OSDJP_CallBack(const Screen& screen)
{
  // don't draw to bottom screen
  if (!screen.IsTop)
    return false;

  for (size_t i = 0; i < OSDJPlist.size();) {
    auto& notify = OSDJPlist[i];

    if (notify.time.GetElapsedTime() <= Seconds(5)) {
      int width = OSD::GetTextWidth(true, notify.name);
      int dx = 380 - widt;
      int dy = 220 - ((i << 4) + (i << 2));

      // background
      screen.DrawRect(dx, dy, width + 4, 16 + 2,
                      Color(notify.bg.ToU32()));

      // text
      screen.DrawSysfont(notify.name, dx, dy,
                         Color(notify.fg.ToU32()));

      if (++i == notify_list_elements_max)
        break;
    }
    else {
      OSDJPlist.erase(OSDJPlist.begin() + i);
    }
  }

  return true;
}

bool OSDJPNotify(const std::string& str, const Color& fg,
                 const Color& bg)
{
  struct _reg_cb {
    _reg_cb()
    {
      OSD::Run(OSDJP_CallBack);
    }
  };

  static const _reg_cb __cb;

  if (OSDJPlist.size() >= notify_list_elements_max) {
    OSDJPlist.erase(OSDJPlist.begin());
  }

  OSDJPlist.push_back(
    {str, Color(fg.ToU32()), Color(bg.ToU32()), Clock()});

  return true;
}
}  // namespace CTRPluginFramework