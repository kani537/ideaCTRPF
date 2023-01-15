#ifndef CHEATS_H
#define CHEATS_H

#include <CTRPluginFramework.hpp>
#include "Helpers.hpp"
#include "Unicode.h"

namespace CTRPluginFramework
{
  using StringVector = std::vector<std::string>;

  void Test1(MenuEntry *entry);
  void Pipes(MenuEntry *entry);
  void Search(MenuEntry *entry);
  void Cube(MenuEntry *entry);
  void BadApple(MenuEntry *entry);
  void JPNotify(MenuEntry *entry);
  void ChangeBackGround(MenuEntry *entry);
  void PlayMusic(MenuEntry *entry);
  void Command(MenuEntry *entry);
  void ColorPicker(MenuEntry *entry);
  class Tetris_Class
  {
  public:
    void Tetris_Loop(HotkeyManager Hotkeys);

    void SetLevel(u8 level)
    {
      GetInstance()->tetris_level = level;
    }

    void SetColorful(bool colorful)
    {
      GetInstance()->tetris_colorful = colorful;
    }

    static Tetris_Class *GetInstance()
    {
      if(!_instance)
        _instance = new Tetris_Class();
      return _instance;
    }

    ~Tetris_Class()
    {
      _instance = nullptr;
    }

  private:
    std::vector<std::vector<u8>> tetris_field = std::vector<std::vector<u8>>(10, std::vector<u8>(18, 0));

    std::vector<Color> tetris_colors = {Color::White, Color::SkyBlue, Color::Blue, Color::Orange, Color::Yellow, Color::LimeGreen, Color::Purple, Color::Red};

    std::vector<std::vector<std::vector<UIntVector>>> _tetris_blocks = {{{{1, 0}, {1, 1}, {1, 2}, {1, 3}}, {{2, 0}, {2, 1}, {1, 2}, {2, 2}}, {{1, 0}, {1, 1}, {1, 2}, {2, 2}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{1, 1}, {2, 1}, {0, 2}, {1, 2}}, {{0, 1}, {1, 1}, {2, 1}, {1, 2}}, {{0, 1}, {1, 1}, {1, 2}, {2, 2}}}, {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}, {{0, 1}, {1, 1}, {2, 1}, {2, 2}}, {{2, 1}, {0, 2}, {1, 2}, {2, 2}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{1, 0}, {1, 1}, {2, 1}, {2, 2}}, {{1, 0}, {1, 1}, {2, 1}, {1, 2}}, {{2, 1}, {1, 2}, {2, 2}, {1, 3}}}, {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}, {{1, 1}, {2, 1}, {1, 2}, {1, 3}}, {{1, 1}, {2, 1}, {2, 2}, {2, 3}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{1, 1}, {2, 1}, {0, 2}, {1, 2}}, {{1, 0}, {0, 1}, {1, 1}, {2, 1}}, {{1, 1}, {2, 1}, {2, 2}, {3, 2}}}, {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}, {{1, 1}, {1, 2}, {2, 2}, {3, 2}}, {{1, 1}, {2, 1}, {3, 1}, {1, 2}}, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, {{1, 1}, {1, 2}, {2, 2}, {2, 3}}, {{1, 0}, {0, 1}, {1, 1}, {1, 2}}, {{2, 0}, {1, 1}, {2, 1}, {1, 2}}}};
    std::vector<UIntVector> tetris_blocks = {{4, 0}, {4, 1}, {4, 2}, {4, 3}};
    u8 mino = 0, mino_turn = 0, tetris_score = 0, tetris_level = 0;
    bool tetris_colorful = false;
    static Tetris_Class *_instance;
    Clock clock;

    void Restart(void);
    void TurnBlock(bool turn_right);
    Tetris_Class()
    {
      _instance = this;
    }
  };

  void Tetris(MenuEntry *entry);
  void SetTetrisSetting(MenuEntry *entry);

  std::vector<u64> getFrame(int &frame);
}
#endif