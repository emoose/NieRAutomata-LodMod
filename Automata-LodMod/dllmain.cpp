﻿#include "pch.h"
#include "resource.h"

HMODULE DllHModule;
HMODULE GameHModule;
uintptr_t mBaseAddress;

TCHAR	szIniBuffer[65535];

#ifdef _DEBUG
bool enableIniUpdateThread = false;
#endif

const char* GameVersionName[] = { "Steam/Win10", "Steam/Win7", "UWP/MS Store", "Steam/2017", "Debug/2017" };

// Address of PE header timestamp, so we can check what EXE is being used
const uint32_t TimestampAddr[] = { 0x178, 0x1A8, 0x180, 0xD8, 0xE0 };
const uint32_t Timestamp[] = { 1624484050, 1624484031, 1624482254, 1493111701, 1490166596 };

// Addresses of misc things to patch
const uint32_t CommunicationScreenTexture_Init1_Addr[] = { 0x772658, 0x76A528, 0x781058, 0x53B1DB, 0x836904 };
const uint32_t CommunicationScreenTexture_Init2_Addr[] = { 0x7750DC, 0x76CFAC, 0x783ADC, 0x53C206, 0x837E74 };

const uint32_t DlcCheckPatch1_Addr[] = { 0x28207C, 0x27C5FC, 0, 0x510958F, 0x410AD8F };
const uint32_t DlcCheckPatch2_Addr[] = { 0x282099, 0x27C619, 0, 0x51095AC, 0x410ADAC };

// Configurables
LodModSettings Settings = {
  .DebugLog = false,
  .LODMultiplier = 0, // disables LODs if set to 0
  .AOMultiplierWidth = 1,
  .AOMultiplierHeight = 1,
  .ShadowResolution = 2048,
  .ShadowDistanceMinimum = 0,
  .ShadowDistanceMaximum = 0,
  .ShadowDistanceMultiplier = 1,
  .ShadowDistancePSS = 0,
  .DisableManualCulling = false,
  .ShadowFilterStrengthBias = 0,
  .ShadowFilterStrengthMinimum = 0,
  .ShadowFilterStrengthMaximum = 0,
  .ShadowModelHQ = false,
  .ShadowModelForceAll = false,
  .CommunicationScreenResolution = 256,
  .HQMapSlots = 7,
  .WrapperLoadLibrary = { 0 },
  .BuffersMovieMultiplier = -1,
  .BuffersExtendTo2021 = true,
  .MoviesEnableH264 = -1,
  .MoviesEncryptionKey = 0,
  .MiscFixJapaneseEncoding = true,
  .MiscTranslateEnable = true,
  .MiscSkipIntroMovies = false,
  .MiscSkipBootingScreens = false,
  .MiscMakeIntroScreenLoadGame = false,
  .MiscDisableVignette = false,
  .MiscDisableFakeHDR = false
};

GameVersion version; // which GameVersion we're injected into

WCHAR ModuleName[4096];
WCHAR IniDir[4096];
WCHAR IniPath[4096];
WCHAR LogPath[4096];
WCHAR GameDir[4096] = { 0 };
bool GotGameDir = false;

#ifdef _DEBUG
HANDLE hIniUpdateThread;
DWORD dwIniUpdateThread;

DWORD WINAPI IniUpdateThread(LPVOID lpParam)
{
  while (true)
  {
    Sleep(1000);
    Settings.ShadowDistanceMultiplier = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMultiplier", 1);
    Settings.ShadowDistanceMinimum = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMinimum", 0);
    Settings.ShadowDistanceMaximum = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMaximum", 0);
    Settings.ShadowFilterStrengthBias = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthBias", 0);
    Settings.ShadowFilterStrengthMinimum = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthMinimum", 0);
    Settings.ShadowFilterStrengthMaximum = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthMaximum", 0);
  }
}
#endif

// Reimpl of games save folder func (0x7A5790 in win10), using unicode instead of ascii
bool GetSaveFolder(wchar_t* destBuf, size_t sizeInBytes)
{
  WCHAR pszPath[4096];
  auto result = SHGetSpecialFolderPathW(0, pszPath, CSIDL_PERSONAL, 0);
  if (result)
    swprintf_s(destBuf, sizeInBytes, L"%s\\%s\\%s\\", pszPath, L"My Games", version == GameVersion::UWP ? L"NieR_Automata_PC" : L"NieR_Automata");

  return result;
}

uint32_t Settings_FindLargestMovie(const std::filesystem::path& path)
{
  if (!DirExists(path.wstring().c_str()) || FileExists(path.wstring().c_str()))
    return 0;

  uint32_t largest = 0;
  dlog("Scanning %S for USM movie files...\n", path.wstring().c_str());

  int read = 0;
  for (const auto& entry : std::filesystem::directory_iterator(path))
  {
    if (entry.is_directory())
      continue;
    if (entry.path().extension().compare(".usm") != 0)
      continue;

    std::ifstream stream(entry.path(), std::ios::binary);
    if (!stream.is_open())
      continue;

    CriUsm movie_usm;
    bool res = movie_usm.read(stream);
    stream.close();

    if (!res)
      continue;

    // If user set EnableH264 to negative number, we'll set it based on the movies codec
    uint32_t codec_raw;
    if (movie_usm.get_u32("mpeg_codec", codec_raw))
    {
      if (Settings.MoviesEnableH264 < 0)
      {
        CriManaCodec codec = static_cast<CriManaCodec>(codec_raw);
        if (codec == CriManaCodec::H264)
          Settings.MoviesEnableH264 = 1;
      }
    }

    // Multiply width x height to find number of pixels
    // TODO: finding what criManaPlayer_CalculatePlaybackWorkSize would return for the file would be better
    // (mem buffer is based on result from there)
    uint32_t width;
    uint32_t height;

    bool has_read = false;

    if (movie_usm.get_u32("width", width) && movie_usm.get_u32("height", height))
    {
      has_read = true;
      uint32_t num_px = width * height;
      if (num_px > largest)
        largest = num_px;
    }
    if (movie_usm.get_u32("mat_width", width) && movie_usm.get_u32("mat_height", height))
    {
      has_read = true;
      uint32_t num_px = width * height;
      if (num_px > largest)
        largest = num_px;
    }
    if (movie_usm.get_u32("disp_width", width) && movie_usm.get_u32("disp_height", height))
    {
      has_read = true;
      uint32_t num_px = width * height;
      if (num_px > largest)
        largest = num_px;
    }
    if (has_read)
      read++;
  }

  dlog("Parsed %d USM movies\n", read);
  return largest;
}

void Settings_ReadINI(const WCHAR* iniPath)
{
  if (!FileExists(iniPath))
    return;

  if (!wcslen(origModulePath))
    GetPrivateProfileStringW(L"Wrapper", L"LoadLibrary", L"", origModulePath, 4096, iniPath);

  Settings.DebugLog = INI_GetBool(iniPath, L"LodMod", L"DebugLog", Settings.DebugLog);

  static bool printedHeader = false;
  if (Settings.DebugLog && !printedHeader)
  {
    dlog("\nNieR Automata LodMod " MODULE_VERSION_STR " - by emoose\n");
    if (GetModuleName(DllHModule, ModuleName, 4096))
      dlog("LodMod module name: %S\n", ModuleName);

    dlog("Detected game type: %s\n", GameVersionName[int(version)]);
    dlog("Wrapping DLL from %S\n", wcslen(origModulePath) > 0 ? origModulePath : L"system folder");
    dlog("Reading INI from %S...\n", IniPath);

    printedHeader = true;
  }

  Settings.LODMultiplier = INI_GetFloat(iniPath, L"LodMod", L"LODMultiplier", Settings.LODMultiplier);
  Settings.AOMultiplierWidth = INI_GetFloat(iniPath, L"LodMod", L"AOMultiplierWidth", Settings.AOMultiplierWidth);
  Settings.AOMultiplierHeight = INI_GetFloat(iniPath, L"LodMod", L"AOMultiplierHeight", Settings.AOMultiplierHeight);
  Settings.ShadowDistanceMultiplier = INI_GetFloat(iniPath, L"LodMod", L"ShadowDistanceMultiplier", Settings.ShadowDistanceMultiplier);
  Settings.ShadowDistanceMinimum = INI_GetFloat(iniPath, L"LodMod", L"ShadowDistanceMinimum", Settings.ShadowDistanceMinimum);
  Settings.ShadowDistanceMaximum = INI_GetFloat(iniPath, L"LodMod", L"ShadowDistanceMaximum", Settings.ShadowDistanceMaximum);
  Settings.ShadowDistancePSS = INI_GetFloat(iniPath, L"LodMod", L"ShadowDistancePSS", Settings.ShadowDistancePSS);
  Settings.ShadowResolution = GetPrivateProfileIntW(L"LodMod", L"ShadowResolution", Settings.ShadowResolution, iniPath);
  Settings.ShadowFilterStrengthBias = INI_GetFloat(iniPath, L"LodMod", L"ShadowFilterStrengthBias", Settings.ShadowFilterStrengthBias);
  Settings.ShadowFilterStrengthMinimum = INI_GetFloat(iniPath, L"LodMod", L"ShadowFilterStrengthMinimum", Settings.ShadowFilterStrengthMinimum);
  Settings.ShadowFilterStrengthMaximum = INI_GetFloat(iniPath, L"LodMod", L"ShadowFilterStrengthMaximum", Settings.ShadowFilterStrengthMaximum);
  Settings.DisableManualCulling = INI_GetBool(iniPath, L"LodMod", L"DisableManualCulling", Settings.DisableManualCulling);
  Settings.CommunicationScreenResolution = GetPrivateProfileIntW(L"LodMod", L"CommunicationScreenResolution", Settings.CommunicationScreenResolution, iniPath);
  Settings.HQMapSlots = GetPrivateProfileIntW(L"LodMod", L"HQMapSlots", Settings.HQMapSlots, iniPath);
  Settings.ShadowModelHQ = INI_GetBool(iniPath, L"LodMod", L"ShadowModelHQ", Settings.ShadowModelHQ);
  Settings.ShadowModelForceAll = INI_GetBool(iniPath, L"LodMod", L"ShadowModelForceAll", Settings.ShadowModelForceAll);
  Settings.BuffersMovieMultiplier = INI_GetFloat(iniPath, L"Buffers", L"MovieMultiplier", Settings.BuffersMovieMultiplier);
  Settings.BuffersExtendTo2021 = INI_GetBool(iniPath, L"Buffers", L"ExtendTo2021", Settings.BuffersExtendTo2021);
  Settings.MoviesEnableH264 = GetPrivateProfileIntW(L"Movies", L"EnableH264", Settings.MoviesEnableH264, iniPath);
  Settings.MiscFixJapaneseEncoding = INI_GetBool(iniPath, L"Misc", L"FixJapaneseEncoding", Settings.MiscFixJapaneseEncoding);
  Settings.MiscTranslateEnable = INI_GetBool(iniPath, L"Misc", L"TranslateEnable", Settings.MiscTranslateEnable);
  Settings.MiscSkipIntroMovies = INI_GetBool(iniPath, L"Misc", L"SkipIntroMovies", Settings.MiscSkipIntroMovies);
  Settings.MiscSkipBootingScreens = INI_GetBool(iniPath, L"Misc", L"SkipBootingScreens", Settings.MiscSkipBootingScreens);
  Settings.MiscMakeIntroScreenLoadGame = INI_GetBool(iniPath, L"Misc", L"MakeIntroScreenLoadGame", Settings.MiscMakeIntroScreenLoadGame);
  Settings.MiscDisableVignette = INI_GetBool(iniPath, L"Misc", L"DisableVignette", Settings.MiscDisableVignette);
  Settings.MiscDisableFakeHDR = INI_GetBool(iniPath, L"Misc", L"DisableFakeHDR", Settings.MiscDisableFakeHDR);

  WCHAR encryptionKey[256];
  int sz = sizeof(encryptionKey);
  if (GetPrivateProfileStringW(L"Movies", L"EncryptionKey", L"0", encryptionKey, 256, iniPath))
  {
    uint64_t value = std::stoull(encryptionKey, nullptr, 0);
    if (value != 0)
      Settings.MoviesEncryptionKey = value;
  }

  // Old INI keynames...
  {
    if (INI_GetBool(iniPath, L"LodMod", L"DisableLODs", false))
      Settings.LODMultiplier = 0;

    if (INI_GetBool(iniPath, L"LodMod", L"FullResAO", false))
      Settings.AOMultiplierWidth = Settings.AOMultiplierHeight = 2;

    auto old_aomultiplier = INI_GetFloat(iniPath, L"LodMod", L"AOMultiplier", 0);
    if (old_aomultiplier != 0)
      Settings.AOMultiplierWidth = Settings.AOMultiplierHeight = old_aomultiplier;

    auto old_dist = INI_GetFloat(iniPath, L"LodMod", L"ShadowMinimumDistance", 0);
    if (old_dist != 0)
      Settings.ShadowDistanceMinimum = old_dist;

    old_dist = INI_GetFloat(iniPath, L"LodMod", L"ShadowMaximumDistance", 0);
    if (old_dist != 0)
      Settings.ShadowDistanceMaximum = old_dist;
  }

  // Only allow AO multiplier from 0.1-2 (higher than 2 adds artifacts...)
  Settings.AOMultiplierWidth = fmaxf(fminf(Settings.AOMultiplierWidth, 2), 0.1f);
  Settings.AOMultiplierHeight = fmaxf(fminf(Settings.AOMultiplierHeight, 2), 0.1f);

  if (Settings.BuffersMovieMultiplier <= 0 || Settings.MoviesEnableH264 < 0)
  {
    // Settings.BuffersMovieMultiplier is 0 or below, lets have LodMod decide!
    dlog("\n");
    if (!GotGameDir)
    {
      dlog("!!! BuffersMovieMultiplier asked LodMod to work out size, but we haven't found GameDir for some reason!\n");
      Settings.BuffersMovieMultiplier = 1;
    }
    else
    {
      // Analyze movie files & find the largest, use it to find default Settings.BufferMovieMultiplier
      // (will also check codecs and set MovieEnableH264 if required)
      uint32_t movie_size = Settings_FindLargestMovie(std::filesystem::path(GameDir) / "data" / "movie");
      uint32_t logo_size = Settings_FindLargestMovie(std::filesystem::path(GameDir) / "data" / "movie_logo");

      if (Settings.BuffersMovieMultiplier <= 0)
      {
        if (logo_size > movie_size)
          movie_size = logo_size;

        float multiplier = float(movie_size) / (1920 * 1080);
        Settings.BuffersMovieMultiplier = ceil(multiplier);

        dlog("Largest movie size is %d pixels, MovieMultiplier set to %f\n", movie_size, Settings.BuffersMovieMultiplier);
      }
    }
  }

  Settings.BuffersMovieMultiplier = fmaxf(Settings.BuffersMovieMultiplier, 1.f);

  if (Settings.HQMapSlots < 7)
    Settings.HQMapSlots = 7;

#ifdef _DEBUG
  if (!hIniUpdateThread && enableIniUpdateThread)
    hIniUpdateThread = CreateThread(
      NULL,                   // default security attributes
      0,                      // use default stack size
      IniUpdateThread,       // thread function name
      nullptr,          // argument to thread function
      0,                      // use default creation flags
      &dwIniUpdateThread);   // returns the thread identifier
#endif
}

void Settings_LoadAllFromPath(const std::filesystem::path& path)
{
  for (const auto& entry : std::filesystem::directory_iterator(path))
  {
    if (!entry.is_directory())
      continue;

    // Check if this dir has any INI
    auto iniEntry = entry.path() / L"LodMod.ini";
    auto iniEntryString = iniEntry.wstring();
    auto iniEntryData = iniEntryString.c_str();
    if (FileExists(iniEntryData) && _wcsicmp(iniEntryData, IniPath) != 0)
    {
      dlog("Reading INI from %S...\n", iniEntryData);
      Settings_ReadINI(iniEntryData);
    }

    // Check if any subfolders of this dir have INIs
    Settings_LoadAllFromPath(entry.path());
  }
}

std::unordered_map<std::wstring, HANDLE> LoadedPlugins;

int LoadExtraPlugins(const wchar_t* listName, std::unordered_map<std::wstring, HANDLE>& list)
{
  szIniBuffer[0] = 0;

  if (GetPrivateProfileSection(listName, szIniBuffer, 65535, IniPath))
    dlog("LoadExtraPlugins: loading plugins\n");
  else
    return 0;

  int count = 0;
  TCHAR* curString = szIniBuffer;
  for (TCHAR* cp = szIniBuffer; ; cp++)
  {
    if (!cp[0])
    {
      std::wstring key = curString;

      if (key.length() > 0)
      {
        if (list.count(key) > 0)
        {
          dlog("LoadExtraPlugins: tried loading in %S, already loaded!\n", key.c_str());
        }
        else
        {
          dlog("LoadExtraPlugins: loading in %S\n", key.c_str());
          auto handle = LoadLibraryW(key.c_str());
          if (!handle)
            dlog("LoadExtraPlugins: failed to load plugin %S, LastError = %d\n", key.c_str(), GetLastError());
          else
          {
            list[key] = handle;
            count++;
          }
        }
      }

      curString = &cp[1];

      if (!curString[0])
      {
        // end of file
        break;
      }
    }
  }

  dlog("LoadExtraPlugins: %d plugins loaded!\n", count);
  return count;
}

bool injected = false;
void LodMod_Init()
{
  if (injected)
    return;

  injected = true;

  MH_Initialize();

  CriH264_Init();
  ShadowFixes_Init();
  AOFixes_Init();
  Rebug_Init();
  MapMod_Init();
  Translate_Init();

  MH_EnableHook(MH_ALL_HOOKS);

  if (Settings.MiscMakeIntroScreenLoadGame && version == GameVersion::Win10)
  {
    // TODO: figure out a cleaner way to do this...

    // Set NewGameLoadGame menu to "Load Game" mode (a1 + 0x28 = 1)
    // (overwrites code that would setup "press key to start" text
    uint8_t loadMode[] = { 0xc7, 0x43, 0x28, 0x01, 0x00, 0x00, 0x00 };
    SafeWriteModule(0x99076F, loadMode, 7);

    // Skip intro-screen wait, go to TitleScreen::AdvanceToNext state nearly immediately
    // ("nearly" because it has to init some stuff first for things to work)
    SafeWriteModule(0x99077C + 3, uint32_t(8));

    // Skip some check before it updates MenuState
    SafeWriteModule(0x9908F6, uint16_t(0x9090));

    // Make it set MenuState to NewGameLoadGame instead
    SafeWriteModule(0x9908F8 + 3, uint32_t(MainMenuState::NewGameLoadGame));

    // Prevent intro animation from playing
    // (commented out because it stops intro 'area' from loading...)
    //uint8_t nop[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
    //SafeWriteModule(0x8909BD, nop, 5);
  }

  if (Settings.MiscSkipIntroMovies)
  {
    const uint32_t IntroMoviesPatch_Addr[] = { 0x760D76, 0x758C46, 0x76F126, 0x51E5AE, 0x80DDD1 };
    SafeWrite(GameAddress(IntroMoviesPatch_Addr), uint16_t(0x9090));
  }

  if (Settings.MiscSkipBootingScreens)
  {
    // Patch code which normally increases boot screen stage from 0, change it to 0 -> 4 instead
    const uint32_t BootScreenStage0to2_Addr[] = { 0x97C5DC, 0x973D2C, 0x9A356C, 0x688046, 0xBBA0E6 };
    const uint32_t BootScreenStage0to1_Addr[] = { 0x97C5F1, 0x973D41, 0x9A3581, 0x6880B1, 0xBBA0FB }; // Steam2017 is editing Stage1to2
    SafeWrite(GameAddress(BootScreenStage0to2_Addr), uint32_t(3));
    SafeWrite(GameAddress(BootScreenStage0to1_Addr), uint32_t(3));

    // Skipping boot screens ends up making the load-screen BGM continue playing even after loading into a save
    // Nopping out the call that starts that BGM seems to fix it though - I'm not sure why the BGM isn't stopping itself though, boot stage 5 should be handling it afaik...
    const uint32_t BootScreenPlayBGMCall_Addr[] = { 0x97C5B9, 0x973D09, 0x9A3549, 0x688020, 0xBBA0C3 };
    uint8_t nop[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
    SafeWrite(GameAddress(BootScreenPlayBGMCall_Addr), nop, 5);
  }

  if (version == GameVersion::Win10 && Settings.MiscDisableFakeHDR)
  {
    SafeWrite(GameAddress(0x25E779), uint8_t(0xEB));
  }

  // Fix CAS Enabled/Amount settings not saving
  if (version == GameVersion::Win10 || version == GameVersion::Win7 || version == GameVersion::UWP)
  {
    // CASEnabled & CASAmount are both setup to read/write to SystemData file
    // But some reason they forgot to let these set a "settings were changed" variable
    // So game won't actually write them out unless you change a different setting at the same time (classic QLOC L)
    // Luckily we can patch them to redirect to the code which sets that
    const uint32_t UISettings_CASEnabledChanged_JmpAddr[] = { 0x985A55, 0x97D1A5, 0x9AC9E5, 0, 0 };
    const uint32_t UISettings_CASAmountChanged_JmpAddr[] = { 0x985A8B, 0x97D1DB, 0x9ACA1B, 0, 0 };
    const uint32_t UISettings_SetHasUpdatedSettings_Addr[] = { 0x985AA2, 0x97D1F2, 0x9ACA32, 0, 0 };

    uintptr_t CASEnabledJmp = GameAddress(UISettings_SetHasUpdatedSettings_Addr) - (GameAddress(UISettings_CASEnabledChanged_JmpAddr) + 5);
    uintptr_t CASAmountJmp = GameAddress(UISettings_SetHasUpdatedSettings_Addr) - (GameAddress(UISettings_CASAmountChanged_JmpAddr) + 5);

    SafeWrite(GameAddress(UISettings_CASEnabledChanged_JmpAddr) + 1, uint32_t(CASEnabledJmp));
    SafeWrite(GameAddress(UISettings_CASAmountChanged_JmpAddr) + 1, uint32_t(CASAmountJmp));
  }

  // Change SystemData.dat filename in the 2017 builds, since it seems to be slightly different format to 2021
  // (switching between 2017/2021 builds usually ends up changing settings due to these format differences...)
  char s2017[] = "2017";
  if (version == GameVersion::Steam2017)
    SafeWriteModule(0xEA1CB0 + 8, s2017, 4);
  else if (version == GameVersion::Debug2017)
  {
    SafeWriteModule(0x1AB8F30 + 8, s2017, 4);

    // Skip IsDebuggerPresent check
    uint8_t xorEaxEax[] = { 0x31, 0xc0, 0x90, 0x90, 0x90, 0x90 };
    SafeWriteModule(0xE5D34D, xorEaxEax, 6);
  }

  // 2021 update moved DLC into main depot, skip steam checks since it's always available now
  // Steam checks will interfere if you disable the DLC depots in steam (DLC depots still contain 2017 files, need to disable them to make sure they don't overwrite 2021)
  // Game itself will check for file availability after this, so no worries
  if (GameAddress(DlcCheckPatch1_Addr))
    SafeWrite(GameAddress(DlcCheckPatch1_Addr), uint16_t(0x9090));
  if (GameAddress(DlcCheckPatch2_Addr))
    SafeWrite(GameAddress(DlcCheckPatch2_Addr), uint16_t(0x9090));

  if (Settings.CommunicationScreenResolution != 256)
  {
    SafeWrite(GameAddress(CommunicationScreenTexture_Init1_Addr), Settings.CommunicationScreenResolution);
    SafeWrite(GameAddress(CommunicationScreenTexture_Init2_Addr), Settings.CommunicationScreenResolution);
    if (version == GameVersion::Steam2017)
    {
      // special case for inlined CreateTextureBuffer
      SafeWrite(GameAddress(CommunicationScreenTexture_Init1_Addr) + 7, Settings.CommunicationScreenResolution);
      SafeWrite(GameAddress(CommunicationScreenTexture_Init2_Addr) + 7, Settings.CommunicationScreenResolution);
    }
  }

  dlog("\nLodMod init complete!\n\n");

  LoadExtraPlugins(L"LoadExtraPlugins", LoadedPlugins);
}

bool InitPlugin()
{
  printf("\nNieR Automata LodMod " MODULE_VERSION_STR " - by emoose\n");

  GameHModule = GetModuleHandleA("NieRAutomata.exe");

  if (!GameHModule)
    return false;

  mBaseAddress = reinterpret_cast<uintptr_t>(GameHModule);

  // Check if we actually support this version...
  bool foundVersion = false;
  for (int i = 0; i < int(GameVersion::MaxVersions); i++)
  {
    version = GameVersion(i);
    if (*GameAddress<uint32_t*>(TimestampAddr) == Timestamp[int(version)])
    {
      foundVersion = true;
      break;
    }
  }

  // wrong EXE?
  if (!foundVersion)
    return false;

  // Try loading config INI:
  memset(IniDir, 0, 4096 * sizeof(WCHAR));
  memset(IniPath, 0, 4096 * sizeof(WCHAR));
  memset(LogPath, 0, 4096 * sizeof(WCHAR));

  GotGameDir = GetModuleFolder(GameHModule, GameDir, 4096);

  // Check for INI inside LodMod DLLs folder first
  if (GetModuleFolder(DllHModule, IniDir, 4096))
    swprintf_s(IniPath, L"%sLodMod.ini", IniDir);

  // Doesn't exist in DLL folder? try game EXE folder
  if (!FileExists(IniPath) && GotGameDir)
  {
    wcscpy_s(IniDir, GameDir);
    swprintf_s(IniPath, L"%sLodMod.ini", GameDir);
  }

  if (!FileExists(IniPath))
  {
    // Doesn't exist in DLL/EXE folder, try checking games save folder
    // Win7/Win10: Documents\My Games\NieR_Automata
    // UWP: Documents\My Games\NieR_Automata_PC

    if (GetSaveFolder(IniDir, 4096))
      swprintf_s(IniPath, L"%sLodMod.ini", IniDir);
  }

  swprintf_s(LogPath, L"%sLodMod.log", IniDir);

  Settings_ReadINI(IniPath);

  // Load any extra INIs inside game folders
  Settings_LoadAllFromPath(GameDir);

  if (Settings.DebugLog)
  {
    dlog("\nSettings:\n");
    dlog(" LODMultiplier: %f\n", Settings.LODMultiplier);
    dlog(" AOMultiplierWidth: %f\n", Settings.AOMultiplierWidth);
    dlog(" AOMultiplierHeight: %f\n", Settings.AOMultiplierHeight);
    dlog(" ShadowDistanceMultiplier: %f\n", Settings.ShadowDistanceMultiplier);
    dlog(" ShadowDistanceMinimum: %f\n", Settings.ShadowDistanceMinimum);
    dlog(" ShadowDistanceMaximum: %f\n", Settings.ShadowDistanceMaximum);
    dlog(" ShadowResolution: %d\n", Settings.ShadowResolution);
    dlog(" ShadowFilterStrengthBias: %f\n", Settings.ShadowFilterStrengthBias);
    dlog(" ShadowFilterStrengthMinimum: %f\n", Settings.ShadowFilterStrengthMinimum);
    dlog(" ShadowFilterStrengthMaximum: %f\n", Settings.ShadowFilterStrengthMaximum);
    dlog(" DisableManualCulling: %s\n", Settings.DisableManualCulling ? "true" : "false");
    dlog(" CommunicationScreenResolution: %d\n", Settings.CommunicationScreenResolution);
    dlog(" HQMapSlots: %d\n", Settings.HQMapSlots);
    dlog(" BuffersExtendTo2021: %s\n", Settings.BuffersExtendTo2021 ? "true" : "false");
    dlog(" BuffersMovieMultiplier: %f\n", Settings.BuffersMovieMultiplier);
    dlog(" MoviesEnableH264: %d\n", Settings.MoviesEnableH264);
    dlog(" MoviesEncryptionKey: 0x%llX\n", Settings.MoviesEncryptionKey);
    dlog("\n");
  }

  return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    DllHModule = hModule;

    if (InitPlugin())
      Proxy_InitSteamStub();

    Proxy_Attach();

    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    Proxy_Detach();
    break;
  }
  return TRUE;
}
