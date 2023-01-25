#ifndef CHEATS_H
#define CHEATS_H

#include <CTRPluginFramework.hpp>
#include "Helpers.hpp"
#include "Unicode.h"

namespace CTRPluginFramework
{
  using StringVector = std::vector<std::string>;

  enum {
    BMP_TOP,
    BMP_BOTTOM
  };

  void Test1(MenuEntry *entry);
  void Pipes(MenuEntry *entry);
  void Search(MenuEntry *entry);
  void Cube(MenuEntry *entry);
  void BadApple(MenuEntry *entry);
  void JPNotify(MenuEntry *entry);
  void PlayMusic(MenuEntry *entry);
  void Command(MenuEntry *entry);
  void ColorPicker(MenuEntry *entry);

  class Tetris_Class {
   public:
    void Tetris_Loop(HotkeyManager Hotkeys);

    void SetLevel(u8 level)
    {
      _level = level;
    }

    void SetColorful(bool colorful)
    {
      _colorfulMode = colorful;
    }

    void SetField_width(u8 width)
    {
      width = width < 4 || 23 < width ? 10 : width;
      if (FIELD_WIDTH > width)
        for (int i = 0; i < FIELD_WIDTH - width; i++)
          _field.erase(_field.end());

      if (FIELD_WIDTH < width)
        for (int i = 0; i < width - FIELD_WIDTH; i++)
          _field.push_back(std::vector<u8>(FIELD_HEIGHT, 0));

      FIELD_WIDTH = width;
      NextMino();
    }

    static Tetris_Class *GetInstance()
    {
      if (!_instance)
        _instance = new Tetris_Class();
      return _instance;
    }

  private:
    static constexpr u8 MINO_KINDS_COUNT = 7;
    u8 FIELD_WIDTH = 10;
    static constexpr u8 FIELD_HEIGHT = 18;
    static constexpr u8 BLOCK_WIDTH = 12;
    static constexpr u8 NEXT_COUNT = 5;

    const std::vector<Color> _mino_colors = {Color::White, Color::SkyBlue, Color::Blue, Color::Orange, Color::Yellow, Color::LimeGreen, Color::Purple, Color::Red};
    std::vector<std::vector<u8>> _field = std::vector<std::vector<u8>>(FIELD_WIDTH, std::vector<u8>(FIELD_HEIGHT, 0));
    std::vector<u8> _nexts;
    std::vector<u8> _srcNexts;

    static Tetris_Class *_instance;

    // 落下中のミノ
    struct
    {
      u8 kind = 0;
      u8 turn = 0;
      Clock dropClock;
      Clock moveClock;
      Clock softdropClock;
      std::vector<UIntVector> blocks;
      bool IsHeld = false;
      u8 heldKind = 9;
    } _mino;

    u8 _score = 0;
    u8 _level = 0;
    bool _colorfulMode = true;

    bool Restart(void);
    void GameOver(void);
    void TurnBlock(bool turn_right);
    void MoveMino(int moveX, int moveY);
    void NextMino(void);
    void HoldMino(void);
    u8 GenerateNextMino(void);

    Tetris_Class(void);
    ~Tetris_Class(void);

    const std::vector<std::vector<std::vector<UIntVector>>> _mino_templates = 
    {
      {
        {{0, 2}, {1, 2}, {2, 2}, {3, 2}}, // I
        {{1, 1}, {1, 2}, {2, 2}, {3, 2}}, // J
        {{2, 1}, {0, 2}, {1, 2}, {2, 2}}, // L
        {{1, 1}, {2, 1}, {1, 2}, {2, 2}}, // O
        {{1, 1}, {2, 1}, {0, 2}, {1, 2}}, // S
        {{1, 0}, {0, 1}, {1, 1}, {2, 1}}, // T
        {{0, 1}, {1, 1}, {1, 2}, {2, 2}}  // Z
      }, 
      {
        {{2, 0}, {2, 1}, {2, 2}, {2, 3}}, // I
        {{2, 0}, {2, 1}, {1, 2}, {2, 2}}, // J
        {{1, 1}, {2, 1}, {2, 2}, {2, 3}}, // L
        {{1, 1}, {2, 1}, {1, 2}, {2, 2}}, // O
        {{1, 0}, {1, 1}, {2, 1}, {2, 2}}, // S
        {{1, 0}, {0, 1}, {1, 1}, {1, 2}}, // T
        {{2, 1}, {1, 2}, {2, 2}, {1, 3}}  // Z
      }, 
      {
        {{0, 2}, {1, 2}, {2, 2}, {3, 2}}, // I
        {{0, 1}, {1, 1}, {2, 1}, {2, 2}}, // J
        {{1, 1}, {2, 1}, {3, 1}, {1, 2}}, // L
        {{1, 1}, {2, 1}, {1, 2}, {2, 2}}, // O
        {{1, 1}, {2, 1}, {0, 2}, {1, 2}}, // S
        {{0, 1}, {1, 1}, {2, 1}, {1, 2}}, // T
        {{1, 1}, {2, 1}, {2, 2}, {3, 2}}  // Z
      }, 
      {
        {{1, 0}, {1, 1}, {1, 2}, {1, 3}}, // I
        {{1, 1}, {2, 1}, {1, 2}, {1, 3}}, // J
        {{1, 0}, {1, 1}, {1, 2}, {2, 2}}, // L
        {{1, 1}, {2, 1}, {1, 2}, {2, 2}}, // O
        {{1, 1}, {1, 2}, {2, 2}, {2, 3}}, // S
        {{1, 0}, {1, 1}, {2, 1}, {1, 2}}, // T
        {{2, 0}, {1, 1}, {2, 1}, {1, 2}}  // Z
      }, 
    };
  };
  void draw_tetris_image(void);
  void Tetris(MenuEntry *entry);
  void Paint(MenuEntry *entry);

  std::vector<u64> getFrame(int &frame);
}
#endif