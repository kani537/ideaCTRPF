#include "osdjp.hpp"

static constexpr auto notify_list_elements_max = 12;

namespace CTRPluginFramework
{
  static std::vector<Sys_OSD> OSDJPlist;

  static bool OSDJP_CallBack(const Screen &screen) {
    // don't draw to bottom screen
    if (!screen.IsTop)
      return false;

    for( size_t i = 0; i < OSDJPlist.size(); ) {
      if (OSDJPlist[i].time.GetElapsedTime() <= Seconds(5)) {
        int width = OSD::GetTextWidth(true, OSDJPlist[i].name);

        screen.DrawRect(380 - (width + 2), 220 - (i * 20), width + 4, 16 + 2, Color(OSDJPlist[i].bg.ToU32()));
        screen.DrawSysfont(OSDJPlist[i].name, 380 - width, 220 - (i * 20), Color(OSDJPlist[i].fg.ToU32()));

        if (++i == 12)
          break;
      }
      else {
        OSDJPlist.erase(OSDJPlist.begin() + i);
      }
    }

    return true;
  }

  bool OSDJPNotify(const std::string &str, const Color &fg, const Color &bg) {
    struct _reg_cb {
      _reg_cb() {
        OSD::Run(OSDJP_CallBack);
      }
    };

    static const _reg_cb __cb;

    if (OSDJPlist.size() >= notify_list_elements_max) {
      OSDJPlist.erase(OSDJPlist.begin());
    }

    OSDJPlist.push_back({str, Color(fg.ToU32()), Color(bg.ToU32()), Clock()});

    return true;
  }
}