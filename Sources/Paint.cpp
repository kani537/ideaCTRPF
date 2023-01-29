#include "Paint.hpp"

#include "KaniCodes.hpp"
#include "cheats.hpp"

namespace CTRPluginFramework
{
  Paint::Paint() {}

  Paint::~Paint() {}

  std::string Paint::ModeToString(PaintMode mode)
  {
    switch (mode)
    {
    case PaintMode::Pen:
      return "Pen";
    case PaintMode::Eraser:
      return "Eraser";
    case PaintMode::Bucket:
      return "Bucket";
    default: break;
    }

    return "";
  }

  bool Paint::IsValid(const Pallet &paintPallet, int x, int y, Color prevC, Color newC)
  {
    return x >= 0 && (u32)x < Pallet::Width && y >= 0 && (u32)y < Pallet::Width && paintPallet.GetColor(x, y) == prevC && paintPallet.GetColor(x, y) != newC;
  }

  void Paint::FloodFill(Pallet &paintPallet, int x, int y, Color prevC, Color newC)
  {
    std::vector<std::pair<int, int>> queue = {{x, y}};

    std::pair<int, int> p(x, y);
    queue.push_back(p);

    paintPallet.SetColor(x, y, newC);

    while (queue.size() > 0)
    {
      std::pair<int, int> currPixel = queue[queue.size() - 1];
      queue.pop_back();

      int posX = currPixel.first;
      int posY = currPixel.second;

      if (IsValid(paintPallet, posX + 1, posY, prevC, newC))
      {
        paintPallet.SetColor(posX + 1, posY, newC);
        p.first = posX + 1;
        p.second = posY;
        queue.push_back(p);
      }
      if (IsValid(paintPallet, posX - 1, posY, prevC, newC))
      {
        paintPallet.SetColor(posX - 1, posY, newC);
        p.first = posX - 1;
        p.second = posY;
        queue.push_back(p);
      }
      if (IsValid(paintPallet, posX, posY + 1, prevC, newC))
      {
        paintPallet.SetColor(posX, posY + 1, newC);
        p.first = posX;
        p.second = posY + 1;
        queue.push_back(p);
      }
      if (IsValid(paintPallet, posX, posY - 1, prevC, newC))
      {
        paintPallet.SetColor(posX, posY - 1, newC);
        p.first = posX;
        p.second = posY - 1;
        queue.push_back(p);
      }
    }
  }

  void Paint::DrawLine(Pallet &paintPallet, const Screen &screen, int srcX, int srcY, int dstX, int dstY, u32 size, const Color &color)
  {
    float x, y, dx, dy, step;
    int i;

    dx = dstX - srcX;
    dy = dstY - srcY;

    step = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

    dx /= step;
    dy /= step;
    x = srcX;
    y = srcY;
    i = 1;

    std::vector<UIntVector> poses;
    while (i <= step)
    {
      if (x < 0 || y < 0 || Pallet::Width <= x || Pallet::Width <= y)
        break;

      u32 originX = x - (size / 2);
      u32 originY = y - (size / 2);
      for (u32 drawX = 0; drawX < size; drawX++)
      {
        for (u32 drawY = 0; drawY < size; drawY++)
        {
          paintPallet.SetColor(originX + drawX, originY + drawY, color);
          poses.push_back({static_cast<u32>(originX + drawX), static_cast<u32>(originY + drawY)});
        }
      }

      x += dx;
      y += dy;
      i++;
    }

    for (u8 i = 0; i < 2; i++)
    {
      for (auto &&pos : poses)
      {
        if (color.a) // pen
          screen.DrawPixel(pos.x + 20, pos.y + 10, color);
        else // eraser
          screen.DrawPixel(pos.x + 20, pos.y + 10, (int(pos.x) / 10 + int(pos.y) / 10) % 2 ? Color::White : Color::DarkGrey);
      }
      if (i == 0)
        OSD::SwapBuffers();
    }
  }

  bool Paint::Open()
  {
    const Screen &topScr = OSD::GetTopScreen();
    const Screen &btmScr = OSD::GetBottomScreen();

    Color paintColor = Color::Black;
    u32 penSize = 1;
    UIntVector lastPos;
    Pallet paintPallet;
    PaintMode mode = PaintMode::Pen;
    Clock dropperClock;
  START:
    for (size_t i = 0; i < 2; i++)
    {
      topScr.DrawRect(0, 0, 400, 240, Color::Gray);
      btmScr.DrawRect(0, 0, 320, 240, Color::Gray);
      btmScr.DrawRect(200, 215, 50, 22, Color::Gray);
      btmScr.DrawRect(200, 215, 50, 22, Color::White, false);
      btmScr.DrawSysfont("cancel", 202, 218);
      btmScr.DrawRect(260, 215, 50, 22, Color::Gray);
      btmScr.DrawRect(260, 215, 50, 22, Color::White, false);
      btmScr.DrawSysfont("OK", 272, 218);
      btmScr.DrawSysfont("モード", 230, 10);
      OSD::SwapBuffers();
    }

    paintPallet.Draw(btmScr);

    while (true)
    {
      Controller::Update();
      lastPos = Touch::GetPosition();
      UIntVector dropperPos = lastPos;
      dropperClock.Restart();
      while (paintPallet.IsTouched())
      {
        UIntVector curPos = Touch::GetPosition();
        switch (mode)
        {
          case PaintMode::Bucket:
          {
            FloodFill(paintPallet, curPos.x - 20, curPos.y - 10, paintPallet.GetColor(curPos.x, curPos.y), paintColor);
            paintPallet.Draw(btmScr);
          }
          case PaintMode::Pen:
          case PaintMode::Eraser:
          {
            // スポイト
            while (TouchRect(dropperPos.x - 2, dropperPos.y - 2, 5, 5))
            {
              if (dropperClock.HasTimePassed(Seconds(1)))
              {
                paintColor = paintPallet.GetColor(dropperPos.x, dropperPos.y);
                btmScr.DrawRect(231, 51, 10, 10, paintColor);
                OSD::SwapBuffers();
                btmScr.DrawRect(231, 51, 10, 10, paintColor);
                paintPallet.Draw(btmScr);
              }
              Controller::Update();
            }
            DrawLine(paintPallet, btmScr, curPos.x - 20, curPos.y - 10, lastPos.x - 20, lastPos.y - 10, penSize, mode == PaintMode::Pen ? paintColor : Color(0, 0, 0, 0));
            lastPos = curPos;
            break;
          }

          default: break;
        }
        
        OSD::SwapBuffers();
        Controller::Update();
      }

      if (Controller::IsKeyPressed(Key::B) || TouchRect(200, 215, 50, 22))
      {
        return false;
      }

      if (TouchRect(260, 215, 50, 22))
      {
        for (size_t x = 0; x < 200; x++)
        {
          for (size_t y = 0; y < 200; y++)
          {
            setScreenBuffer(x + 100, y + 20, paintPallet.GetColor(x, y));
          }
        }
        return true;
      }

      if (Controller::IsKeyPressed(Key::X) || TouchRect(230, 50, 12, 12))
      {
        colorPicker(paintColor);
        goto START;
      }

      if (Controller::IsKeyPressed(Key::Y))
      {
        if (mode == PaintMode::Bucket)
          mode = PaintMode::Pen;
        else
          mode = (PaintMode)((u32)mode + 1);
      }
      if (Controller::IsKeyPressed(Key::R))
      {
        if (penSize < 30)
          penSize++;
      }
      if (Controller::IsKeyPressed(Key::L))
      {
        if (penSize > 1)
          penSize--;
      }
      btmScr.DrawRect(230, 25, 90, 20, Color::Gray);
      btmScr.DrawSysfont(ModeToString(mode), 230, 25);

      btmScr.DrawRect(231, 51, 10, 10, paintColor);

      btmScr.DrawRect(230, 65, 90, 20, Color::Gray);
      btmScr.DrawSysfont(Utils::Format("Pen: %upx", penSize), 230, 65);
      OSD::SwapBuffers();
    }

    return false;
  }
} // namespace CTRPluginFramework
