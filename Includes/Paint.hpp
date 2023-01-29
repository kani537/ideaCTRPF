#pragma once

#include <CTRPluginFramework.hpp>
#include <3ds.h>

#include "KaniCodes.hpp"

namespace CTRPluginFramework
{
  struct Pallet {
    static constexpr u32 Width = 200;

    Pallet(): data(Width, std::vector<Color>(Width, Color(0, 0, 0, 0))) {}
    inline void SetColor(u32 x, u32 y, const Color &color)
    {
      data[x][y] = color;
    }
    inline Color GetColor(u32 x, u32 y) const
    {
      return data[x][y];
    }
    inline bool IsTouched()
    {
      return TouchRect(20, 20, Width, Width);
    }
    inline void Draw(const Screen &scr)
    {
      for(u8 i = 0; i < 2; i++)
      {
        scr.DrawRect(19, 9, 202, 202, Color::Black, false);

        for (size_t x = 0; x < 200; x++)
        {
          for (size_t y = 0; y < 200; y++)
          {
            Color color = GetColor(x, y);
            if (color.a)
              scr.DrawPixel(x + 20, y + 10, color);
            else
              scr.DrawPixel(x + 20, y + 10, (x / 10 + y / 10) % 2 ? Color::White : Color::DarkGrey);
          }
        }

        if(i == 0) OSD::SwapBuffers();
      }
    }

    std::vector<std::vector<Color>> data;
  };

  class Paint {
  public:
    Paint();
    ~Paint(void);

    bool Open(void);

  private:

    enum class PaintMode
    {
      Pen,
      Eraser,
      Bucket
    };

    std::string ModeToString(PaintMode mode);
    bool IsValid(const Pallet &paintPallet, int x, int y, Color prevC, Color newC);
    void FloodFill(Pallet &paintPallet, int x, int y, Color prevC, Color newC);
    void DrawLine(Pallet &paintPallet, const Screen &screen, int srcX, int srcY, int dstX, int dstY, u32 size, const Color &color);
  };
} // namespace CTRPluginFramework
