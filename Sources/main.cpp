#include <3ds.h>
#include "csvc.h"
#include <CTRPluginFramework.hpp>
#include "cheats.hpp"
#include "KaniCodes.hpp"
#include "../libctrpf/include/CTRPluginFrameworkImpl/Menu/KeyboardImpl.hpp"
#include <ctime>

namespace CTRPluginFramework
{
  static MenuEntry *TopSeparator(MenuEntry *entry)
  {
    entry->UseTopSeparator();
    return entry;
  }

  static MenuEntry *BottomSeparator(MenuEntry *entry)
  {
    entry->UseBottomSeparator();
    return entry;
  }

  // HotKeyの部分
  static MenuEntry *EntryWithHotkey(MenuEntry *entry, const Hotkey &hotkey)
  {
    if (entry != nullptr)
    {
      entry->Hotkeys += hotkey;
      entry->SetArg(new std::string(entry->Name()));
      entry->Name() += " " + hotkey.ToString();
      entry->Hotkeys.OnHotkeyChangeCallback([](MenuEntry *entry, int index)
                                            { std::string* name = reinterpret_cast<std::string*>(entry->GetArg()); entry->Name() = *name + " " + entry->Hotkeys[0].ToString(); });
    }
    return (entry);
  }
  static MenuEntry *EntryWithHotkey(MenuEntry *entry, const std::vector<Hotkey> &hotkeys)
  {
    if (entry != nullptr)
    {
      for (const Hotkey &hotkey : hotkeys)
        entry->Hotkeys += hotkey;
    }
    return (entry);
  }
  static MenuEntry *EnableEntry(MenuEntry *entry)
  {
    if (entry != nullptr)
    {
      entry->SetArg(new std::string(entry->Name()));
      entry->Enable();
    }
    return (entry);
  }

  static Handle mcuHwcHandle;
  static int mcuHwcRefCount;
  Result mcuHwcInit(void)
  {
    if (AtomicPostIncrement(&mcuHwcRefCount))
      return (0);
    Result res = srvGetServiceHandle(&mcuHwcHandle, "mcu::HWC");
    if (res < 0)
      AtomicDecrement(&mcuHwcRefCount);
    return (res);
  }
  void mcuHwcExit(void)
  {
    if (AtomicDecrement(&mcuHwcRefCount))
      return;
    svcCloseHandle(mcuHwcHandle);
  }
  Result MCUHWC_ReadRegister(u8 reg, void *data, u32 size)
  {
    Result ret = 0;
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x1, 2, 2);
    cmdbuf[1] = reg;
    cmdbuf[2] = size;
    cmdbuf[3] = IPC_Desc_Buffer(size, IPC_BUFFER_W);
    cmdbuf[4] = (u32)data;
    if ((ret = svcSendSyncRequest(mcuHwcHandle)) < 0)
      return (ret);
    return ((Result)cmdbuf[1]);
  }
  void StoreBatteryPercentage(float &percentage)
  {
    u8 data[4];
    mcuHwcInit();
    MCUHWC_ReadRegister(0xA, data, 4);
    percentage = data[1] + data[2] / 256.0f;
    percentage = (u32)((percentage + 0.05f) * 10.0f) / 10.0f;
    mcuHwcExit();
  }

  std::string SystemInfomation()
  {
    char date[6400];
    std::time_t unix_time = std::time(nullptr);
    struct tm *time_struct = gmtime((const time_t *)&unix_time);
    std::string am_or_pm;
    const std::vector<std::string> kListTimeOfTheWeek{"日", "月", "火", "水", "木", "金", "土"};
    int time_of_the_week = time_struct->tm_wday;
    const int kStartYear = 1900;
    const int kStartMonth = 1;
    int year = time_struct->tm_year + kStartYear;
    int month = time_struct->tm_mon + kStartMonth;
    int day = time_struct->tm_mday;
    int hour_24 = time_struct->tm_hour;
    int hour_12;
    int minute = time_struct->tm_min;
    int second = time_struct->tm_sec;
    if (hour_24 / 12)
    {
      am_or_pm = "午後";
    }
    else
    {
      am_or_pm = "午前";
    }
    if (hour_24 % 12)
    {
      hour_12 = hour_24 % 12;
    }
    else
    {
      hour_12 = 12;
    }
    std::string systeminfo = Utils::Format("%d/%02d/%02d(%s) %s:%02d:%02d:%02d", year, month, day, kListTimeOfTheWeek[time_of_the_week].c_str(), am_or_pm.c_str(), hour_12, minute, second);
    return systeminfo;
  }

  std::string BatteryInfomation()
  {
    float percentage = 0;
    StoreBatteryPercentage(percentage);
    std::string batteryinfo = Utils::Format("%d%%%", (u32)percentage);
    return batteryinfo;
  }

  bool LoadGameTitle(const Screen &scr)
  {
    std::string tid;
    Process::GetTitleID(tid);
    std::string name;
    Process::GetName(name);
    if (scr.IsTop)
    {
      scr.DrawRect(163, 222, 236, 17, Color::Black);
      scr.DrawRect(163, 218, 236, 17, Color::Black);
      scr.DrawRect(165, 220, 232, 17, Color::White, false);
      scr.DrawSysfont(tid + ":" << Color::LimeGreen << "対応ゲーム", 166, 22 * 10, Color::White);
    }
    return true;
  }

  void DrawCallBack(Time time)
  {
    const Screen &scr = OSD::GetTopScreen();
    scr.DrawRect(30, 4, 340, 18, Color::Black, true);
    scr.DrawRect(32, 6, 336, 14, Color::White, false);
    scr.DrawSysfont(SystemInfomation(), 35, 5, Color::White);
    if (System::IsCitra())
      return;
    scr.DrawSysfont(BatteryInfomation(), 321, 5, Color::White);
  }

  void LoadKanji(void)
  {
    if (File::Exists("kanji.txt"))
    {
      File file("kanji.txt");
      file.Seek(2);
      bool flag = true;
      while (flag)
      {
        std::string hiragana, kanji;
        while (1)
        {
          u16 buff;
          std::string str_buff;
          file.Read((void *)&buff, sizeof(u16));
          Utils::ConvertUTF16ToUTF8(str_buff, (u16 *)&buff);
          if (buff != 0x2C)
          {
            if (buff == 0x3B)
            {
              flag = false;
              break;
            }
            else if ((buff > 0x80) || (buff < 0xA0))
              hiragana += str_buff.substr(0, 3);
          }
          else
            break;
        }
        while (1)
        {
          u16 buff;
          std::string str_buff;
          file.Read((void *)&buff, sizeof(u16));
          Utils::ConvertUTF16ToUTF8(str_buff, (u16 *)&buff);
          if (buff != 0xA)
          {
            if (buff == 0x3B)
            {
              flag = false;
              break;
            }
            else if (0x1000 < buff)
              kanji += str_buff.substr(0, 3);
            else
              kanji += str_buff.substr(0, 1);
          }
          else
            break;
        }
        Convert::SetHiraganaKanji(hiragana, kanji);
      }
      OSD::Notify("kanji.txt loaded");
    }
    else
      OSD::Notify("kanji.txt not found.");
  }

  bool checkPass(void)
  {
    std::vector<u16> answer = {0x6A, 0x3EC, 0x2175, 0x29FB, 0x2CF, 0x5C, 0xBB};
    std::vector<u16> diff = {0x84, 0x25, 0x266, 0x2999, 0x2114, 0x37A, 0x7};
    u8 answer_length = answer.size();
    u64 hash;
    CFGU_GenHashConsoleUnique(0, &hash);
    hash = (hash % 0x1000) * 2 + (hash >> 0x36);

    File::Create("pass.bin");
    File file("pass.bin");
    for (int j = 0; j < answer_length; j++)
    {
      u16 buff;
      file.Read((void *)&buff, sizeof(u16));
      if (buff != (answer[j] + diff[j] + hash))
      {
        u16 utf16[answer_length];
        KeyboardImpl key("input password");
        key.SetLayout(Layout::QWERTY);
        key.Run();
        Process::WriteString((u32)&utf16, key.GetInput().substr(0, answer_length), StringFormat::Utf16);
        for (int i = 0; i < answer_length; i++)
          utf16[i] += diff[answer_length - i - 1];
        Sleep(Seconds(1));

        for (int i = 0; i < answer_length; i++)
        {
          if (utf16[i] != answer[i])
          {
            MessageBox("invalid")();
            file.Close();
            return false;
          }
        }
        file.Rewind();
        for (int i = 0; i < answer_length; i++)
        {
          buff = answer[i] + diff[i] + hash;
          file.Write((void *)&buff, sizeof(u16));
        }
        file.Close();

        return true;
      }
    }
    file.Close();
    return true;
  }

  // This patch the NFC disabling the touchscreen when scanning an amiibo, which prevents ctrpf to be used
  static void ToggleTouchscreenForceOn(void)
  {
    static u32 original = 0;
    static u32 *patchAddress = nullptr;

    if (patchAddress && original)
    {
      *patchAddress = original;
      return;
    }

    static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003};

    Result res;
    Handle processHandle;
    s64 textTotalSize = 0;
    s64 startAddress = 0;
    u32 *found;

    if (R_FAILED(svcOpenProcess(&processHandle, 16)))
      return;

    svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
    svcGetProcessInfo(&startAddress, processHandle, 0x10005);
    if (R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
      goto exit;

    found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

    if (found != nullptr)
    {
      original = found[13];
      patchAddress = (u32 *)PA_FROM_VA((found + 13));
      found[13] = 0xE1A00000;
    }

    svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
  exit:
    svcCloseHandle(processHandle);
  }

  // This function is called before main and before the game starts
  // Useful to do code edits safely
  void PatchProcess(FwkSettings &settings)
  {
    ToggleTouchscreenForceOn();
  }

  // This function is called when the process exits
  // Useful to save settings, undo patchs or clean up things
  void OnProcessExit(void)
  {
    ToggleTouchscreenForceOn();
  }

  MenuEntry *EnabledEntry(MenuEntry *entry)
  {
    if (entry != nullptr)
      entry->Enable();
    return (entry);
  }

  void InitMenu(PluginMenu &menu)
  {
    MenuFolder *searchFolder = new MenuFolder("Search");
    *searchFolder += new MenuEntry("Search", nullptr, Search);
    menu += searchFolder;

    menu += new MenuEntry("Test1", nullptr, Test1);
    menu += new MenuEntry("command",nullptr, Command, "コマンド");
    menu += new MenuEntry("pipes", Pipes, "パイプス");
    menu += new MenuEntry("Cube", Cube,"キューブ");
    menu += new MenuEntry("Bad Apple!!", BadApple,"バッドアップル!!");
    menu += new MenuEntry("JPNotify", JPNotify, "ジェーピーノティファイ\nstartで表示\n(Y押しながら押すんじゃないぞ！)");
    menu += new MenuEntry("ChangeBackGround", nullptr, ChangeBackGround, "チェンジバックグラウンド\nBMPフォルダに画像を入れてください");
    menu += new MenuEntry("PlayMusic", nullptr, PlayMusic,"プレイミュージック");
  }

  int main(void)
  {
    PluginMenu *menu = new PluginMenu("Action Replay", 0, 7, 4, "made by kani537");

    // OSD::Run(LoadGameTitle);
    // Sleep(Seconds(1));
    // OSD::Stop(LoadGameTitle);

    // if (!checkPass())
    //   return (0);
    // OSD::Notify("verified");
    // LoadKanji();

    menu->OnNewFrame = DrawCallBack;
    menu->SynchronizeWithFrame(true);
    menu->ShowWelcomeMessage(false);

    // Init our menu entries & folders
    InitMenu(*menu);

    // std::vector<MenuFolder *> folders = menu->GetFolderList();
    // for (auto folder : folders)
    // {
    //   if (folder->Name() == "other")
    //   {
    //     std::vector<MenuEntry *> entries = folder->GetEntryList();
    //     for (auto entry : entries)
    //     {
    //       if (entry->Name() == "Cheat1")
    //         entry->Enable();
    //     }
    //   }
    // }

    // Launch menu and mainloop
    menu->Run();
    delete menu;

    // Exit plugin
    return (0);
  }
}
