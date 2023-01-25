#include "cheats.hpp"
#include "osdjp.hpp"
#include "KaniCodes.hpp"

namespace CTRPluginFramework {
void Test1(MenuEntry* entry)
{
}

void JPNotify(MenuEntry* entry)
{
  StringVector words = {"コ", "ン", "ニ", "チ", "ワ"};
  if (Controller::IsKeyDown(Key::Start) &&
      Controller::IsKeyDown(Key::Y))
    words = {"オ", "チ", "ン", "チ", "ン"};
  if (Controller::IsKeyPressed(Key::Start)) {
    std::string buff;
    for (int j = 0; j < 5; j++) {
      buff += words[Utils::Random(0, 4)];
    }
    u8 red = Utils::Random(0, 255), green = Utils::Random(0, 255),
       blue = Utils::Random(0, 255);
    u16 sum =
      std::max({red, green, blue}) + std::min({red, green, blue});
    OSDJPNotify(buff, Color(red, green, blue),
                Color(sum - red, sum - green, sum - blue));
  }
}

// たまにクラッシュするからだれか直してくれ
void Pipes(MenuEntry* entry)
{
  static u32 xPos = 150, yPos = 120;
  static u8 direct = 3, temp_direct, buff_direct, i = 0, len, j;
  Color colorList[7] = {Color::Red,   Color::Blue,   Color::Cyan,
                        Color::Green, Color::Yellow, Color::Orange,
                        Color::White};
  static bool isReset = false;
  if (entry->WasJustActivated()) {
    fillScreenBuffer(Color(0, 0, 0, 0));
    setFlagShowScreenBuffer(true);
    OSD::Run(ShowScreenBuffer);
  }
  if (!entry->IsActivated()) {
    setFlagShowScreenBuffer(false);
    OSD::Stop(ShowScreenBuffer);
  }
  if (isReset) {
    switch (direct) {
      case 0:
        xPos = Utils::Random(2, 397);
        yPos = 2;
        break;
      case 1:
        xPos = 2;
        yPos = Utils::Random(2, 237);
        break;
      case 2:
        xPos = 398;
        yPos = Utils::Random(2, 237);
        break;
      default:
        xPos = Utils::Random(2, 397);
        yPos = 238;
        break;
    }
    isReset = false;
    j = 0;
    if (i < 6)
      i++;
    else
      i = 0;
  }
  if (len > 0) {
    if (xPos == 1 || xPos == 399 || yPos == 1 || yPos == 239) {
      isReset = true;
    }
    else {
      for (int k = -1; k < 2; k++) {
        setScreenBuffer(xPos + k, yPos + k, colorList[i]);
      }
      switch (direct) {
        case 0:
          if (j == 0) {
            if (buff_direct == 1)
              setScreenBuffer(xPos + 2, yPos, Color(0, 0, 0, 0));
            else
              setScreenBuffer(xPos - 2, yPos, Color(0, 0, 0, 0));
            yPos++;
            break;
          }
          setScreenBuffer(xPos + 2, yPos, Color(0, 0, 0, 0));
          setScreenBuffer(xPos - 2, yPos, Color(0, 0, 0, 0));
          yPos++;
          break;
        case 1:
          if (j == 0) {
            if (buff_direct == 0)
              setScreenBuffer(xPos, yPos + 2, Color(0, 0, 0, 0));
            else
              setScreenBuffer(xPos, yPos - 2, Color(0, 0, 0, 0));
            xPos++;
            break;
          }
          setScreenBuffer(xPos, yPos + 2, Color(0, 0, 0, 0));
          setScreenBuffer(xPos, yPos - 2, Color(0, 0, 0, 0));
          xPos++;
          break;
        case 2:
          if (j == 0) {
            if (buff_direct == 0)
              setScreenBuffer(xPos, yPos + 2, Color(0, 0, 0, 0));
            else
              setScreenBuffer(xPos, yPos - 2, Color(0, 0, 0, 0));
            xPos--;
            break;
          }
          setScreenBuffer(xPos, yPos + 2, Color(0, 0, 0, 0));
          setScreenBuffer(xPos, yPos - 2, Color(0, 0, 0, 0));
          xPos--;
          break;
        default:
          if (j == 0) {
            if (buff_direct == 1)
              setScreenBuffer(xPos + 2, yPos, Color(0, 0, 0, 0));
            else
              setScreenBuffer(xPos - 2, yPos, Color(0, 0, 0, 0));
            yPos--;
            break;
          }
          setScreenBuffer(xPos + 2, yPos, Color(0, 0, 0, 0));
          setScreenBuffer(xPos - 2, yPos, Color(0, 0, 0, 0));
          yPos--;
          break;
      }
      len--;
    }
  }
  else {
    len = Utils::Random(3, 30);
    j = 0;
    while (1) {
      temp_direct = Utils::Random(0, 3);
      if ((temp_direct == 0 && direct != 3) ||
          (temp_direct == 1 && direct != 2) ||
          (temp_direct == 2 && direct != 1) ||
          (temp_direct == 3 && direct != 0))
        break;
    }
    buff_direct = direct;
    direct = temp_direct;
  }
  if (Controller::IsKeyPressed(Key::Start)) {
    fillScreenBuffer(Color(0, 0, 0, 0));
    isReset = true;
  }
}

void addSearch(MenuFolder* folder, MenuFolder* SearchFolder,
               std::string input)
{
  if (folder->Name() == "Search")
    return;
  std::vector<CTRPluginFramework::MenuEntry*> entries =
    folder->GetEntryList();
  std::vector<MenuFolder*> folders = folder->GetFolderList();
  for (auto folder1 : folders) {
    addSearch(folder1, SearchFolder, input);
  }
  for (auto entry : entries) {
    if ((Convert::toLower(entry->Name()).find(input) !=
         std::string::npos) ||
        (Convert::hiraganaToKatakana(entry->Note()).find(input) !=
         std::string::npos) ||
        (Convert::katakanaToHiragana(entry->Note()).find(input) !=
         std::string::npos)) {
      *SearchFolder +=
        new MenuEntry(entry->Name(), entry->GetGameFunc(),
                      entry->GetMenuFunc(), entry->Note());
    }
  }
}

void Search(MenuEntry* entry)
{
  std::string input;
  PluginMenu* menu = PluginMenu::GetRunningInstance();
  if (!JPKeyboard("エントリー名を入力してください").Open(input))
    return;

  input = Convert::hiraganaToKatakana(Convert::toLower(input));
  std::vector<MenuFolder*> folders = menu->GetFolderList();
  MenuFolder* SearchFolder;
  for (auto folder : folders) {
    if (folder->Name() == "Search") {
      folder->Clear();
      *folder += new MenuEntry("Search", nullptr, Search);
      SearchFolder = folder;
    }
  }
  std::vector<CTRPluginFramework::MenuEntry*> entries =
    menu->GetEntryList();
  for (auto menu_entry : entries) {
    if ((Convert::toLower(menu_entry->Name()).find(input) !=
         std::string::npos) ||
        (Convert::toLower(menu_entry->Note()).find(input) !=
         std::string::npos))
      *SearchFolder +=
        new MenuEntry(menu_entry->Name(), menu_entry->GetGameFunc(),
                      menu_entry->GetMenuFunc(), menu_entry->Note());
  }

  for (auto folder : folders) addSearch(folder, SearchFolder, input);
}

float AA, BB, CC;

float cubeWidth = 20;
int width = 400, height = 240;
float zBuffer[400 * 240];
char buffer[400 * 240];
int distanceFromCam = 100;
float horizontalOffset;
float K1 = 40;

float incrementSpeed = 0.6;

float x, y, z;
float ooz;
int xp, yp;
int idx;

float calculateX(int i, int j, int k)
{
  return j * sin(AA) * sin(BB) * cos(CC) -
         k * cos(AA) * sin(BB) * cos(CC) + j * cos(AA) * sin(CC) +
         k * sin(AA) * sin(CC) + i * cos(BB) * cos(CC);
}

float calculateY(int i, int j, int k)
{
  return j * cos(AA) * cos(CC) + k * sin(AA) * cos(CC) -
         j * sin(AA) * sin(BB) * sin(CC) +
         k * cos(AA) * sin(BB) * sin(CC) - i * cos(BB) * sin(CC);
}

float calculateZ(int i, int j, int k)
{
  return k * cos(AA) * cos(BB) - j * sin(AA) * cos(BB) + i * sin(BB);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ,
                         int ch)
{
  x = calculateX(cubeX, cubeY, cubeZ);
  y = calculateY(cubeX, cubeY, cubeZ);
  z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

  ooz = 1 / z;

  xp = (int)(width / 2 + horizontalOffset + K1 * ooz * x * 2);
  yp = (int)(height / 2 + K1 * ooz * y);

  idx = xp + yp * width;
  if (idx >= 0 && idx < width * height) {
    if (ooz > zBuffer[idx]) {
      zBuffer[idx] = ooz;
      buffer[idx] = ch;
    }
  }
}
void rotateCube(void)
{
  memset(buffer, 10, sizeof(buffer));
  memset(zBuffer, 0, width * height * 4);
  horizontalOffset = 1 * cubeWidth;
  // second cube
  for (float cubeX = -cubeWidth; cubeX < cubeWidth;
       cubeX += incrementSpeed) {
    for (float cubeY = -cubeWidth; cubeY < cubeWidth;
         cubeY += incrementSpeed) {
      calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
      calculateForSurface(cubeWidth, cubeY, cubeX, '$');
      calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
      calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
      calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
      calculateForSurface(cubeX, cubeWidth, cubeY, '+');
    }
  }
  fillScreenBuffer(Color(0, 0, 0, 0));
  for (int k = 0; k < width * height; k++) {
    switch (buffer[k]) {
      case 64:
        setScreenBuffer(k % width, k / width, Color::Red);
        break;
      case 36:
        setScreenBuffer(k % width, k / width, Color::Blue);
        break;
      case 126:
        setScreenBuffer(k % width, k / width, Color::Orange);
        break;
      case 35:
        setScreenBuffer(k % width, k / width, Color::Green);
        break;
      case 59:
        setScreenBuffer(k % width, k / width, Color::White);
        break;
      case 43:
        setScreenBuffer(k % width, k / width, Color::Yellow);
        break;
    }
  }
}

void Cube(MenuEntry* entry)
{
  if (entry->WasJustActivated()) {
    setFlagShowScreenBuffer(true);
    OSD::Run(ShowScreenBuffer);
  }
  if (!entry->IsActivated()) {
    setFlagShowScreenBuffer(false);
    OSD::Stop(ShowScreenBuffer);
  }
  rotateCube();
  if (Controller::IsKeyDown(Key::CPadLeft))
    BB -= 0.05;
  if (Controller::IsKeyDown(Key::CPadRight))
    BB += 0.05;
  if (Controller::IsKeyDown(Key::CPadUp))
    AA += 0.05;
  if (Controller::IsKeyDown(Key::CPadDown))
    AA -= 0.05;
  if (Controller::IsKeyDown(Key::CStickUp))
    CC -= 0.05;
  if (Controller::IsKeyDown(Key::CStickDown))
    CC += 0.05;
  if (Controller::IsKeyDown(Key::CStickLeft))
    distanceFromCam -= 5;
  if (Controller::IsKeyDown(Key::CStickRight))
    distanceFromCam += 5;
}

int frame_num = 0;
void BadApple(MenuEntry* entry)
{
  if (!entry->IsActivated()) {
    Process::Play();
    return;
  }
  Process::Pause();
  if (entry->WasJustActivated())
    frame_num = 0;

  std::vector<u64> frame = getFrame(frame_num);

  const Screen& screen = OSD::GetTopScreen();
  for (size_t i = 0; i < frame.size(); i++) {
    u8 index = 0;
    for (u64 j = 1; j < 0x1000000000000000; j *= 2) {
      if (frame[i] & j)
        screen.DrawRect(380 - index * 6, 10 + i * 10, 6, 10,
                        Color::White);
      else
        screen.DrawRect(380 - index * 6, 10 + i * 10, 6, 10,
                        Color(0, 0, 0, 0));
      index++;
    }
  }
  Sleep(Milliseconds(100));
  frame_num++;
  OSD::SwapBuffers();
}

void PlayMusic(MenuEntry* entry)
{
  StringVector files_name;
  Directory("MUSIC", true).ListFiles(files_name, ".bcwav");
  if (!files_name.size()) {
    MessageBox("no files found")();
    return;
  }
  s8 i = Keyboard("select file:", files_name).Open();
  if (i != -1) {
    // u32 temp;
    // File fp("MUSIC/" + files_name[i]);
    // Result res = svcControlMemoryEx(&temp, (u32)0x04000000,
    // (u32)0x04000000, fp.GetSize(), MEMOP_MAP,
    // (MemPerm)(MEMPERM_READ | MEMPERM_WRITE), true); if
    // (R_SUCCEEDED(res))
    // {
    //   fp.Inject(temp, fp.GetSize());
    //   Sound((u8 *)temp).Play();
    // }
    // else
    //   MessageBox(Utils::Format("memory error%08X", (u32)temp))();
    // fp.Close();
    Sound("MUSIC/" + files_name[i]).Play();
  }
}

std::vector<int> find_all(const std::string str,
                          const std::string subStr)
{
  std::vector<int> result;

  int subStrSize = subStr.size();
  size_t pos = str.find(subStr);

  while (pos != std::string::npos) {
    result.push_back(pos);
    pos = str.find(subStr, pos + subStrSize);
  }

  return result;
}

std::string treeFolder(std::string path, u8 space)
{
  std::string buff = "";
  StringVector files, folders;
  Directory dir(path);
  dir.ListFiles(files);
  dir.ListDirectories(folders);
  for (auto file : files)
    buff += std::string(space * 2, ' ') + "├" + file + "\n";
  for (auto folder : folders) {
    buff += std::string(space * 2, ' ') + "├" +
              (Color::Green << folder) + "\n"
            << Color::White;
    buff += treeFolder(path + "/" + folder, space + 1);
  }
  return buff;
}

std::string getFileObscurely(Directory dir, std::string word)
{
  StringVector files;
  dir.ListFiles(files);
  for (auto file : files)
    if (file.find(word) != std::string::npos)
      return file;
  return "no file";
}
std::string getFolderObscurely(Directory dir, std::string word)
{
  StringVector folders;
  dir.ListDirectories(folders);
  for (auto folder : folders)
    if (folder.find(word) != std::string::npos)
      return folder;
  return "no folder";
}

size_t getReturnCount(std::string& str)
{
  size_t count = 0;
  count = find_all(str, "\n").size();
  for (size_t i = 0; i < count - 1; i++) {
    if (OSD::GetTextWidth(true,
                          str.substr(find_all(str, "\n")[i],
                                     find_all(str, "\n")[i + 1] -
                                       find_all(str, "\n")[i])) > 330)
      for (int j = 0;; j++) {
        if (OSD::GetTextWidth(
              true, str.substr(find_all(str, "\n")[i], j)) > 330) {
          str = str.substr(0, find_all(str, "\n")[i] + j - 1) + "\n" +
                str.substr(find_all(str, "\n")[i] + j - 1);
          break;
        }
      }
  }
  count = find_all(str, "\n").size();
  return count;
}

void Cd(Directory& dir, std::string& path, std::string& str)
{
  u64 pos = 18446744073709551615UL;
  if (path.substr(0, 2) == "..") {
    Directory::Open(
      dir, dir.GetFullName().substr(
             0, find_all(
                  dir.GetFullName(),
                  "/")[find_all(dir.GetFullName(), "/").size() - 1]));
    if (path.find("/") == std::string::npos)
      str += dir.GetFullName() + "\n";
    else {
      path.erase(0, 3);
      StringVector files, folders;
      dir.ListFiles(files);
      dir.ListDirectories(folders);
      Cd(dir, path, str);
    }
  }
  else if (Directory::IsExists(
             dir.GetFullName() + "/" +
             getFolderObscurely(dir,
                                path.find("/") != std::string::npos
                                  ? path.substr(0, path.find("/"))
                                  : path))) {
    Directory::Open(dir, dir.GetFullName() + "/" +
                           getFolderObscurely(
                             dir, path.find("/") != std::string::npos
                                    ? path.substr(0, path.find("/"))
                                    : path));
    if ((pos = path.find("/")) == std::string::npos)
      str += dir.GetFullName() + "\n";
    else {
      path.erase(0, pos + 1);
      StringVector files, folders;
      dir.ListFiles(files);
      dir.ListDirectories(folders);
      Cd(dir, path, str);
    }
  }
  else
    str += "invalid directory\n" + path;
}

void getEntries(MenuFolder* folder, std::vector<MenuEntry*>& entries)
{
  std::vector<MenuEntry*> menuEntries = folder->GetEntryList();
  for (auto menuEntry : menuEntries)
    if (menuEntry->GetMenuFunc() == nullptr)
      entries.push_back(menuEntry);
  std::vector<MenuFolder*> menuFolders = folder->GetFolderList();
  for (auto menuFolder : menuFolders) getEntries(menuFolder, entries);
}

void Command(MenuEntry* entry)
{
  std::string commandLine_buffer = Color::Green << "command"
                                                << Color::White;
  Directory dir("");

  while (1) {
    if (dir.GetFullName().substr(dir.GetFullName().length() - 1, 1) ==
        "/")
      Directory::Open(dir, dir.GetFullName().substr(
                             0, dir.GetFullName().length() - 1));
    StringVector files, folders;
    dir.ListFiles(files);
    dir.ListDirectories(folders);
    commandLine_buffer += "\n->";
    std::string input;
    u8 count = getReturnCount(commandLine_buffer);

    Keyboard key(
      11 < count
        ? commandLine_buffer.substr(
            find_all(commandLine_buffer, "\n")[count - 12] + 1)
        : commandLine_buffer);
    if (key.Open(input) <= -1)
      break;
    std::string space_delimiter = " ";
    input += space_delimiter;
    commandLine_buffer += input + "\n";
    std::vector<std::string> args{};

    size_t pos = 0;
    while ((pos = input.find(space_delimiter)) != std::string::npos) {
      args.push_back(input.substr(0, pos));
      input.erase(0, pos + space_delimiter.length());
    }
    if (args[0] == "ls") {
      size_t buff_size = commandLine_buffer.size();
      for (auto file : files) commandLine_buffer += "\n" + file;
      for (auto folder : folders)
        commandLine_buffer += Color::Green << "\n" + folder;
      if (files.size() + folders.size() > 10)
        MessageBox(commandLine_buffer.substr(buff_size) + "")();
      commandLine_buffer = commandLine_buffer << Color::White;
    }
    else if (args[0] == "exit")
      break;
    else if ((args[0] == "clear") || (args[0] == "cls"))
      commandLine_buffer = Color::Green << "command" << Color::White;
    else if (args[0] == "cd") {
      if (args.size() == 1)
        commandLine_buffer += "enter directory name";
      else
        Cd(dir, args[1], commandLine_buffer);
    }
    else if (args[0] == "pwd")
      commandLine_buffer += dir.GetFullName();
    else if (args[0] == "help") {
      if (args.size() == 1) {
        std::string _help =
          "command "
          "list\n・ls\n・clear|"
          "cls\n・cd\n・pwd\n・mkdir\n・tree\n・secret\n・cat\n・echo"
          "\n・rm\n・touch\n・entry\n・exit\n・help [command]\n" +
          (Color::Green << "Can be done even when files and "
                           "directories are ambiguous."
                        << Color::White);
        commandLine_buffer += _help;
        MessageBox(_help + "")();
      }
      else if (args[1] == "ls")
        commandLine_buffer += "show files and folders";
      else if ((args[1] == "cls") || (args[1] == "clear"))
        commandLine_buffer += "clear the screen";
      else if (args[1] == "cd")
        commandLine_buffer += "change directory\ncd [directory]";
      else if (args[1] == "pwd")
        commandLine_buffer += "show current directory";
      else if (args[1] == "mkdir")
        commandLine_buffer += "make directory\nmkdir [directory]";
      else if (args[1] == "tree")
        commandLine_buffer +=
          "show all files and directories like tree";
      else if (args[1] == "cat")
        commandLine_buffer += "display in binary\ncat [file]";
      else if (args[1] == "echo")
        commandLine_buffer += "return word\necho [word]";
      else if (args[1] == "rm")
        commandLine_buffer +=
          "remove file or folder\nrm [file|folder]";
      else if (args[1] == "touch")
        commandLine_buffer += "make file\ntouch [file]";
      else if (args[1] == "entry")
        commandLine_buffer +=
          "enable or disable entry(only GameFunc)\nentry\n-l list "
          "entries\n-e [entry] enable entry\n-d [entry] disable "
          "entry";
      else if (args[1] == "exit")
        commandLine_buffer += "exit command";
    }
    else if (args[0] == "mkdir") {
      if (args.size() == 1)
        commandLine_buffer += "enter directory's name";
      else {
        dir.Create(dir.GetFullName() + "/" + args[1]);
        Directory::Open(dir, dir.GetFullName());
      }
    }
    else if (args[0] == "tree") {
      commandLine_buffer += treeFolder(dir.GetFullName(), 0);
      MessageBox(treeFolder(dir.GetFullName(), 0) + "")();
    }
    else if (args[0] == "kani") {
      commandLine_buffer += "(/)(;,,;)(/)\n>=<\nV=(° °)=V";
    }
    else if (args[0] == "cat") {
      if (args.size() == 1)
        commandLine_buffer += "enter file's name";
      else {
        if (File::Exists(dir.GetFullName() + "/" +
                         getFileObscurely(dir, args[1]))) {
          File file(dir.GetFullName() + "/" +
                    getFileObscurely(dir, args[1]));
          std::string text =
                        dir.GetFullName() + "/" +
                        getFileObscurely(dir, args[1]) + "\n" +
                        Utils::Format("%d bytes", file.GetSize()) +
                        "\n",
                      out;
          while (1) {
            u8 buffer;
            file.Read((void*)&buffer, 1);
            text += Utils::Format("%02X", buffer);
            if (file.Tell() % 8 == 0)
              text += "\n";
            else if (file.Tell() % 4 == 0)
              text += " ";
            if (file.GetSize() == file.Tell())
              break;
          }
          if (find_all(text, "\n").size() > 10)
            MessageBox(text + "")();
          commandLine_buffer += text;
        }
        else {
          commandLine_buffer += dir.GetFullName() + "/" +
                                getFileObscurely(dir, args[1]) +
                                " not found";
        }
      }
    }
    else if (args[0] == "echo") {
      if (args.size() == 1)
        commandLine_buffer += "enter word";
      else
        commandLine_buffer += args[1];
    }
    else if (args[0] == "rm") {
      if (args.size() == 1)
        commandLine_buffer += "enter file's or directory's name";
      else {
        if (args[1].find("/") == std::string::npos) {
          if (Directory::IsExists(dir.GetFullName() + "/" +
                                  getFolderObscurely(dir, args[1]))) {
            commandLine_buffer +=
              "really delete " + dir.GetFullName() + "/" +
              getFolderObscurely(dir, args[1]) + "?[y/n]:\n";
            u8 count = getReturnCount(commandLine_buffer);
            std::string output;
            Keyboard(11 < count ? commandLine_buffer.substr(
                                    find_all(commandLine_buffer,
                                             "\n")[count - 12] +
                                    1)
                                : commandLine_buffer);
            if (key.Open(output) <= -1)
              break;
            if (output == "y")
              Directory::Remove(dir.GetFullName() + "/" +
                                getFolderObscurely(dir, args[1]));
          }
          else if (File::Exists(dir.GetFullName() + "/" +
                                getFileObscurely(dir, args[1]))) {
            commandLine_buffer +=
              "really delete " + dir.GetFullName() + "/" +
              getFileObscurely(dir, args[1]) + "?[y/n]:\n";
            u8 count = getReturnCount(commandLine_buffer);
            std::string output;
            Keyboard(11 < count ? commandLine_buffer.substr(
                                    find_all(commandLine_buffer,
                                             "\n")[count - 12] +
                                    1)
                                : commandLine_buffer);
            if (key.Open(output) <= -1)
              break;
            if (output == "y")
              File::Remove(dir.GetFullName() + "/" +
                           getFileObscurely(dir, args[1]));
          }
          else
            commandLine_buffer += "no files found";
        }
        else
          commandLine_buffer +=
            "Only files in this directory can be deleted";
        Directory::Open(dir, dir.GetFullName());
      }
    }
    else if (args[0] == "touch") {
      if (args.size() == 1)
        commandLine_buffer += "enter file's name";
      else {
        File::Create(dir.GetFullName() + "/" + args[1]);
        commandLine_buffer += dir.GetFullName() + "/" + args[1];
      }
    }
    else if (args[0] == "entry") {
      if (args.size() >= 2) {
        PluginMenu* menu = PluginMenu::GetRunningInstance();
        std::vector<CTRPluginFramework::MenuEntry*> entries;
        std::vector<CTRPluginFramework::MenuEntry*> menuEntries =
          menu->GetEntryList();
        for (auto menuEntry : menuEntries)
          if (menuEntry->GetMenuFunc() == nullptr)
            entries.push_back(menuEntry);
        std::vector<CTRPluginFramework::MenuFolder*> menuFolders =
          menu->GetFolderList();
        for (auto menuFolder : menuFolders)
          getEntries(menuFolder, entries);
        if (args[1] == "-l") {
          std::string buff = "only GameFunc:";
          for (MenuEntry* _entry : entries)
            buff += "\n" + _entry->Name();
          MessageBox(buff + "")();
          commandLine_buffer += buff;
        }
        else if ((args[1] == "-e") || (args[1] == "-d")) {
          if (args.size() == 2)
            commandLine_buffer += "enter entry name";
          else {
            for (MenuEntry* _entry : entries)
              if (_entry->Name() == args[2]) {
                if (args[1] == "-e")
                  _entry->Enable();
                else
                  _entry->Disable();
                goto END_TOGGLE_ENTRY;
              }
            commandLine_buffer += "can't find that GameFunc";
            continue;
          END_TOGGLE_ENTRY:
            commandLine_buffer += args[1] == "-e"
                                    ? "enabled " + args[2]
                                    : "disabled " + args[2];
          }
        }
        else
          commandLine_buffer += "unknown argment";
      }
      else
        commandLine_buffer += "Please add an argument\n-l -e -d";
    }
    else
      commandLine_buffer += "unknown command";
  }
}

void ColorPicker(MenuEntry* entry)
{
  Color out;
  colorPicker(out);
}

Tetris_Class::Tetris_Class()
{
  _instance = this;

  _nexts.clear();
  for (u8 i = 0; i < NEXT_COUNT; i++) {
    _nexts.push_back(GenerateNextMino());
  }

  NextMino();
}

Tetris_Class::~Tetris_Class()
{
  _instance = nullptr;
}

u8 Tetris_Class::GenerateNextMino(void)
{
  if (_srcNexts.empty()) {
    _srcNexts = {0, 1, 2, 3, 4, 5, 6};

    for (u8 i = 0; i < MINO_KINDS_COUNT; i++) {
      u8 r = (u8)Utils::Random(i, 6);
      std::swap(_srcNexts[i], _srcNexts[r]);
    }
  }

  u8 next = _srcNexts[0];
  _srcNexts.erase(_srcNexts.begin());

  return next;
}

void Tetris_Class::NextMino(void)
{
  _mino.kind = _nexts[0];
  _nexts.erase(_nexts.begin());
  _nexts.push_back(GenerateNextMino());
  _mino.turn = 0;
  _mino.dropClock.Restart();
  _mino.blocks = _mino_templates[0][_mino.kind];
  _mino.IsHeld = false;
  for (auto&& block : _mino.blocks) block.x += FIELD_WIDTH / 2 - 2;
}

void Tetris_Class::GameOver(void)
{
  MessageBox(Utils::Format("Game Over\nyour score is %d", _score))();
  _score = 0;
  _level = 0;
  _mino.heldKind = 9;
  _field = std::vector<std::vector<u8>>(
    FIELD_WIDTH, std::vector<u8>(FIELD_HEIGHT, 0));
}

bool Tetris_Class::Restart(void)
{
  for (auto&& block : _mino.blocks) {
    if (block.y == 1 || _field[block.x][block.y]) {
      GameOver();
      return false;
    }

    _field[block.x][block.y] = _mino.kind + 1;
  }

  NextMino();
  return true;
}

void Tetris_Class::TurnBlock(bool turn_right)
{
  IntVector pos = {
    (int)((int)_mino.blocks[0].x -
          (int)_mino_templates[_mino.turn][_mino.kind][0].x),
    (int)((int)_mino.blocks[0].y -
          (int)_mino_templates[_mino.turn][_mino.kind][0].y)};
  if (turn_right) {
    if (_mino.turn == 3)
      _mino.turn = 0;
    else
      _mino.turn++;
  }
  else {
    if (_mino.turn == 0)
      _mino.turn = 3;
    else
      _mino.turn--;
  }
  for (auto&& block : _mino_templates[_mino.turn][_mino.kind]) {
    if ((int)(block.x) + pos.x < 0) {
      pos.x += abs((int)(block.x) + pos.x);
      if (_field[block.x + pos.x][block.y + pos.y])
        goto RESET_TURN;
    }
    if (_field.size() - 1 < block.x + pos.x) {
      pos.x -= block.x + pos.x - _field.size() + 1;
      if (_field[block.x + pos.x][block.y + pos.y])
        goto RESET_TURN;
    }
    if ((int)(block.y) + pos.y < 0)
      pos.y += abs((int)(block.y) + pos.y);
    if (_field[block.x + pos.x][block.y + pos.y])
      goto RESET_TURN;
  }
  _mino.blocks = _mino_templates[_mino.turn][_mino.kind];
  for (auto&& block : _mino.blocks) {
    block.x += pos.x;
    block.y += pos.y;
  }
  return;
RESET_TURN:
  if (turn_right) {
    if (_mino.turn == 0)
      _mino.turn = 3;
    else
      _mino.turn--;
  }
  else {
    if (_mino.turn == 3)
      _mino.turn = 0;
    else
      _mino.turn++;
  }
  pos = {(int)(_mino.blocks[0].x -
               _mino_templates[_mino.turn][_mino.kind][0].x),
         (int)(_mino.blocks[0].y -
               _mino_templates[_mino.turn][_mino.kind][0].y)};
  _mino.blocks = _mino_templates[_mino.turn][_mino.kind];
  for (auto&& block : _mino.blocks) {
    block.x += pos.x;
    block.y += pos.y;
  }
}

void Tetris_Class::MoveMino(int moveX, int moveY)
{
  for (auto&& block : _mino.blocks) {
    block.x += moveX;
    block.y += moveY;
  }
}

void Tetris_Class::HoldMino(void)
{
  if (_mino.heldKind == 9) {
    _mino.heldKind = _mino.kind;
    NextMino();
  }
  else {
    _mino.turn = 0;
    _mino.dropClock.Restart();
    _mino.blocks = _mino_templates[0][_mino.heldKind];
    std::swap(_mino.heldKind, _mino.kind);
    _mino.IsHeld = true;
    for (auto&& block : _mino.blocks) block.x += FIELD_WIDTH / 2 - 2;
  }
}

void Tetris_Class::Tetris_Loop(HotkeyManager Hotkeys)
{
  bool isOpened = true;
  const Screen& topScr = OSD::GetTopScreen();
  const Screen& btmScr = OSD::GetBottomScreen();
  topScr.DrawRect(0, 0, 400, 240, Color::Gray);
  draw_tetris_image();
  OSD::SwapBuffers();
  topScr.DrawRect(0, 0, 400, 240, Color::Gray);
  draw_tetris_image();
  while (isOpened) {
    Controller::Update();

    topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2, 9,
                    FIELD_WIDTH * BLOCK_WIDTH + 6, 222, Color::Black);
    topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + 2, 11,
                    FIELD_WIDTH * BLOCK_WIDTH + 6 - 4, 218,
                    Color::White, false);

    if (Hotkeys[0].IsDown()) {
      for (auto&& block : _mino.blocks)
        if (block.x <= 0 || _field[block.x - 1][block.y])
          goto END;
      if (Hotkeys[0].IsPressed()) {
        MoveMino(-1, 0);
        _mino.moveClock.Restart();
      }
      else if (_mino.moveClock.HasTimePassed(Seconds(0.2))) {
        MoveMino(-1, 0);
      }
    }
    else if (Hotkeys[1].IsDown()) {
      for (auto&& block : _mino.blocks)
        if (_field.size() - 1 <= block.x ||
            _field[block.x + 1][block.y])
          goto END;
      if (Hotkeys[1].IsPressed()) {
        MoveMino(1, 0);
        _mino.moveClock.Restart();
      }
      else if (_mino.moveClock.HasTimePassed(Seconds(0.2))) {
        MoveMino(1, 0);
      }
    }
    else if (Hotkeys[2].IsDown() &&
             _mino.softdropClock.HasTimePassed(Seconds(0.08))) {
      for (auto&& block : _mino.blocks)
        if (_field[0].size() - 1 <= block.y ||
            _field[block.x][block.y + 1])
          goto END;
      MoveMino(0, 1);
      _mino.softdropClock.Restart();
    }
    else if (Hotkeys[3].IsPressed()) {
      for (size_t i = 0; i < _field[0].size(); i++)
        for (auto&& block : _mino.blocks)
          if (block.y + i >= _field[0].size() - 1 ||
              _field[block.x][block.y + 1 + i]) {
            for (auto&& block : _mino.blocks) block.y += i;
            isOpened = Restart();
            goto END;
          }
    }
    else if (Hotkeys[4].IsPressed())
      TurnBlock(false);
    else if (Hotkeys[5].IsPressed())
      TurnBlock(true);
    else if (Hotkeys[6].IsPressed() && !_mino.IsHeld)
      HoldMino();
    else if (Hotkeys[7].IsPressed()) {
      s8 ans;
      if (0 <=
          (ans = Keyboard("Paused", {"continue", "settings", "quit"})
                   .Open())) {
        if (ans == 1) {
          s8 answer;
          u8 out;
          if (0 <= (answer = Keyboard("tetris color",
                                      {"monochrome", "colorful"})
                               .Open()))
            Tetris_Class::GetInstance()->SetColorful(answer);
          if (0 <= (answer = Keyboard("level",
                                      {"easy", "normal", "difficult"})
                               .Open()))
            Tetris_Class::GetInstance()->SetLevel(answer * 5);
          Keyboard key(
            "input field's width\ndefault is 10\n4以上23以下でよろ");
          key.IsHexadecimal(false);
          if (0 <= key.Open(out))
            Tetris_Class::GetInstance()->SetField_width(out);
        }
        else if (ans == 2)
          isOpened = false;
      }
      topScr.DrawRect(0, 0, 400, 240, Color::Gray);
      draw_tetris_image();
      OSD::SwapBuffers();
      topScr.DrawRect(0, 0, 400, 240, Color::Gray);
      draw_tetris_image();
    }
  END:

    // ミノ落下
    if (_mino.dropClock.HasTimePassed(
          Milliseconds(1000 - _level * 30))) {
      for (auto&& block : _mino.blocks)
        if (block.y >= _field[0].size() - 1 ||
            _field[block.x][block.y + 1]) {
          isOpened = Restart();
          break;
        }
      for (auto&& block : _mino.blocks) block.y++;
      _mino.dropClock.Restart();
    }

    // フィールド上のブロック描画
    for (size_t i = 0; i < _field.size(); i++)
      for (size_t j = 0; j < _field[i].size(); j++)
        if (_field[i][j])
          topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 +
                            3 + i * BLOCK_WIDTH + 1,
                          BLOCK_WIDTH + j * BLOCK_WIDTH + 1,
                          BLOCK_WIDTH - 2, BLOCK_WIDTH - 2,
                          _colorfulMode ? _mino_colors[_field[i][j]]
                                        : _mino_colors[0]);

    // 落下中のミノ描画
    for (auto&& block : _mino.blocks)
      topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + 3 +
                        block.x * BLOCK_WIDTH + 1,
                      BLOCK_WIDTH + block.y * BLOCK_WIDTH + 1,
                      BLOCK_WIDTH - 2, BLOCK_WIDTH - 2,
                      _colorfulMode ? _mino_colors[_mino.kind + 1]
                                    : _mino_colors[0]);

    // Next
    for (u8 i = 0; i < NEXT_COUNT; i++) {
      u8 next = _nexts[i];
      u32 posY = 12 + 40 * i;
      topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 +
                        FIELD_WIDTH * BLOCK_WIDTH + 6 + 2,
                      posY, 55, 40, Color::White, false);
      topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 +
                        FIELD_WIDTH * BLOCK_WIDTH + 6 + 3,
                      posY + 1, 53, 38, Color::Black);
      for (auto&& block : _mino_templates[0][next])
        topScr.DrawRect(
          200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 +
            FIELD_WIDTH * BLOCK_WIDTH + 6 + 7 +
            block.x * BLOCK_WIDTH + 1,
          posY + block.y * BLOCK_WIDTH + 3, BLOCK_WIDTH - 2,
          BLOCK_WIDTH - 2,
          _colorfulMode ? _mino_colors[next + 1] : _mino_colors[0]);
    }

    // Hold
    topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 - 58,
                    12, 55, 40, Color::White, false);
    topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 - 57,
                    12 + 1, 53, 38, Color::Black);
    if (_mino.heldKind != 9)
      for (auto&& block : _mino_templates[0][_mino.heldKind])
        topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 -
                          53 + block.x * BLOCK_WIDTH + 1,
                        12 + block.y * BLOCK_WIDTH + 3,
                        BLOCK_WIDTH - 2, BLOCK_WIDTH - 2,
                        _colorfulMode
                          ? _mino_colors[_mino.heldKind + 1]
                          : _mino_colors[0]);

    // 落下位置描画
    for (int i = 0; i < FIELD_HEIGHT; i++) {
      for (auto&& block : _mino.blocks) {
        if (block.y + i >= FIELD_HEIGHT - 1 ||
            _field[block.x][block.y + 1 + i]) {
          for (auto&& block : _mino.blocks) {
            u32 y = block.y + i;
            topScr.DrawRect(
              200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + 3 +
                block.x * BLOCK_WIDTH + 1,
              BLOCK_WIDTH + y * BLOCK_WIDTH + 1, BLOCK_WIDTH - 2,
              BLOCK_WIDTH - 2,
              _colorfulMode ? _mino_colors[_mino.kind + 1]
                            : _mino_colors[0],
              false);
          }
          goto SUPER_BREAK;
        }
      }
    }

  SUPER_BREAK:

    for (int i = 0; i < FIELD_HEIGHT; i++) {
      u8 count = 0;
      for (int j = 0; j < FIELD_WIDTH; j++)
        count += _field[j][i] ? 1 : 0;
      if (count == FIELD_WIDTH) {
        _score++;
        // 10ごとにレベルを上げる
        if (!(_score % 10) && _level < 11)
          _level++;

        for (int k = i - 1; k >= 0; k--)
          for (int j = 0; j < FIELD_WIDTH; j++)
            _field[j][k + 1] = _field[j][k];
      }
    }
    OSD::SwapBuffers();
  }
}

Tetris_Class* Tetris_Class::_instance = nullptr;

void Tetris(MenuEntry* entry)
{
  if (!Process::IsPaused())
    return;
  Tetris_Class::GetInstance()->Tetris_Loop(entry->Hotkeys);
}

void ShowPallet(MenuEntry* entry)
{
  if (entry->WasJustActivated()) {
    setFlagShowScreenBuffer(true);
    OSD::Run(ShowScreenBuffer);
  }
  if (!entry->IsActivated()) {
    setFlagShowScreenBuffer(false);
    OSD::Stop(ShowScreenBuffer);
  }
}

void PaintDrawLine(std::vector<std::vector<Color>>& paintPallet,
                   const Screen& screen, int srcX, int srcY, int dstX,
                   int dstY, const Color& color)
{
  float x, y, dx, dy, step;
  int i;

  dx = (dstX - srcX);
  dy = (dstY - srcY);

  if (abs(dx) >= abs(dy)) {
    step = abs(dx);
  }
  else {
    step = abs(dy);
  }

  dx = dx / step;
  dy = dy / step;
  x = srcX;
  y = srcY;
  i = 1;

  std::vector<UIntVector> poses;
  while (i <= step) {
    if (x < 0 || y < 0 || paintPallet.size() <= x ||
        paintPallet[0].size() <= y)
      break;
    paintPallet[x][y] = color;
    poses.push_back({x, y});
    x = x + dx;
    y = y + dy;
    i++;
  }
  for (UIntVector pos : poses) {
    if (color.a)
      screen.DrawPixel(pos.x + 20, pos.y + 10, color);
    else
      screen.DrawPixel(pos.x + 20, pos.y + 10,
                       (int(pos.x) / 10 + int(pos.y) / 10) % 2
                         ? Color::White
                         : Color::DarkGrey);
  }
  OSD::SwapBuffers();
  for (UIntVector pos : poses) {
    if (color.a)
      screen.DrawPixel(pos.x + 20, pos.y + 10, color);
    else
      screen.DrawPixel(pos.x + 20, pos.y + 10,
                       (int(pos.x) / 10 + int(pos.y) / 10) % 2
                         ? Color::White
                         : Color::DarkGrey);
  }
}
bool isValid(const std::vector<std::vector<Color>>& paintPallet,
             int x, int y, Color prevC, Color newC)
{
  if (x < 0 || x >= paintPallet.size() || y < 0 ||
      y >= paintPallet[0].size() || paintPallet[x][y] != prevC ||
      paintPallet[x][y] == newC)
    return false;
  return true;
}

void floodFill(std::vector<std::vector<Color>>& paintPallet, int x,
               int y, Color prevC, Color newC)
{
  std::vector<std::pair<int, int>> queue = {{x, y}};

  std::pair<int, int> p(x, y);
  queue.push_back(p);

  paintPallet[x][y] = newC;

  while (queue.size() > 0) {
    std::pair<int, int> currPixel = queue[queue.size() - 1];
    queue.pop_back();

    int posX = currPixel.first;
    int posY = currPixel.second;

    if (isValid(paintPallet, posX + 1, posY, prevC, newC)) {
      paintPallet[posX + 1][posY] = newC;
      p.first = posX + 1;
      p.second = posY;
      queue.push_back(p);
    }
    if (isValid(paintPallet, posX - 1, posY, prevC, newC)) {
      paintPallet[posX - 1][posY] = newC;
      p.first = posX - 1;
      p.second = posY;
      queue.push_back(p);
    }
    if (isValid(paintPallet, posX, posY + 1, prevC, newC)) {
      paintPallet[posX][posY + 1] = newC;
      p.first = posX;
      p.second = posY + 1;
      queue.push_back(p);
    }
    if (isValid(paintPallet, posX, posY - 1, prevC, newC)) {
      paintPallet[posX][posY - 1] = newC;
      p.first = posX;
      p.second = posY - 1;
      queue.push_back(p);
    }
  }
}

void Paint(MenuEntry* entry)
{
  bool isOpened = true;
  const Screen& topScr = OSD::GetTopScreen();
  const Screen& btmScr = OSD::GetBottomScreen();
  Color paintColor = Color::Black;
  UIntVector lastPos;
  std::vector<std::vector<Color>> paintPallet =
    std::vector<std::vector<Color>>(
      200, std::vector<Color>(200, Color(0, 0, 0, 0)));
  enum {
    PEN,
    ERASER,
    BUCKET
  };
  std::string paintModeName[] = {"PEN", "ERASER", "BUCKET"};
  u8 paintMode = PEN;
  Clock dropperClock;
START:
  topScr.DrawRect(0, 0, 400, 240, Color::Gray);
  btmScr.DrawRect(0, 0, 320, 240, Color::Gray);
  btmScr.DrawRect(19, 9, 202, 202, Color::Black, false);
  for (size_t x = 0; x < 200; x++)
    for (size_t y = 0; y < 200; y++) {
      Color color = paintPallet[x][y];
      if (color.a)
        btmScr.DrawPixel(x + 20, y + 10, color);
      else
        btmScr.DrawPixel(
          x + 20, y + 10,
          (x / 10 + y / 10) % 2 ? Color::White : Color::DarkGrey);
    }
  btmScr.DrawRect(200, 215, 50, 22, Color::Gray);
  btmScr.DrawRect(200, 215, 50, 22, Color::White, false);
  btmScr.DrawSysfont("cancel", 202, 218);
  btmScr.DrawRect(260, 215, 50, 22, Color::Gray);
  btmScr.DrawRect(260, 215, 50, 22, Color::White, false);
  btmScr.DrawSysfont("OK", 272, 218);
  btmScr.DrawSysfont("モード", 230, 10);
  btmScr.DrawRect(230, 50, 12, 12, Color::White, false);
  OSD::SwapBuffers();
  topScr.DrawRect(0, 0, 400, 240, Color::Gray);
  btmScr.DrawRect(0, 0, 320, 240, Color::Gray);
  btmScr.DrawRect(19, 9, 202, 202, Color::Black, false);
  for (size_t x = 0; x < 200; x++)
    for (size_t y = 0; y < 200; y++) {
      Color color = paintPallet[x][y];
      if (color.a)
        btmScr.DrawPixel(x + 20, y + 10, color);
      else
        btmScr.DrawPixel(
          x + 20, y + 10,
          (x / 10 + y / 10) % 2 ? Color::White : Color::DarkGrey);
    }
  btmScr.DrawRect(200, 215, 50, 22, Color::Gray);
  btmScr.DrawRect(200, 215, 50, 22, Color::White, false);
  btmScr.DrawSysfont("cancel", 202, 218);
  btmScr.DrawRect(260, 215, 50, 22, Color::Gray);
  btmScr.DrawRect(260, 215, 50, 22, Color::White, false);
  btmScr.DrawSysfont("OK", 272, 218);
  btmScr.DrawSysfont("モード", 230, 10);
  btmScr.DrawRect(230, 50, 12, 12, Color::White, false);
  while (isOpened) {
    Controller::Update();
    lastPos = Touch::GetPosition();
    UIntVector dropperPos = lastPos;
    dropperClock.Restart();
    while (TouchRect(20, 10, 200, 200) &&
           (paintMode == PEN || paintMode == ERASER)) {
      UIntVector pos = Touch::GetPosition();
      if (dropperClock.HasTimePassed(Seconds(1)) &&
          TouchRect(dropperPos.x - 5, dropperPos.y - 5, 10, 10)) {
        paintColor = paintPallet[pos.x][pos.y];
        btmScr.DrawRect(231, 51, 10, 10, paintColor);
      }
      PaintDrawLine(paintPallet, btmScr, pos.x - 20, pos.y - 10,
                    lastPos.x - 20, lastPos.y - 10,
                    paintMode ? Color(0, 0, 0, 0) : paintColor);
      lastPos = Touch::GetPosition();
      OSD::SwapBuffers();
      Controller::Update();
    }
    if (TouchRect(20, 10, 200, 200) && paintMode == BUCKET) {
      UIntVector pos = Touch::GetPosition();
      floodFill(paintPallet, pos.x - 20, pos.y - 10,
                paintPallet[pos.x][pos.y], paintColor);
      goto START;
    }
    if (TouchRect(200, 215, 50, 22))
      isOpened = false;
    if (TouchRect(260, 215, 50, 22)) {
      for (size_t x = 0; x < 200; x++)
        for (size_t y = 0; y < 200; y++) {
          Color color = paintPallet[x][y];
          setScreenBuffer(x + 100, y + 20, color);
        }
      entry->SetGameFunc(ShowPallet);
      isOpened = false;
    }
    if (Controller::IsKeyPressed(Key::B))
      isOpened = false;
    if (Controller::IsKeyPressed(Key::X)) {
      colorPicker(paintColor);
      goto START;
    }
    if (Controller::IsKeyPressed(Key::Y)) {
      if (paintMode == BUCKET)
        paintMode = PEN;
      else
        paintMode++;
    }
    btmScr.DrawRect(230, 20,
                    OSD::GetTextWidth(true, paintModeName[paintMode]),
                    20, Color::Gray);
    btmScr.DrawSysfont(paintModeName[paintMode], 230, 20);
    btmScr.DrawRect(231, 51, 10, 10, paintColor);
    OSD::SwapBuffers();
  }
}
}  // namespace CTRPluginFramework