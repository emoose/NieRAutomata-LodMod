#include "pch.h"

HMODULE DllHModule;
HMODULE GameHModule;
uintptr_t mBaseAddress;

#define LODMOD_VERSION "0.76.2"

const char* GameVersionName[] = { "Steam/Win10", "Steam/Win7", "UWP/MS Store", "Steam/2017", "Debug/2017" };

// Address of PE header timestamp, so we can check what EXE is being used
const uint32_t TimestampAddr[] = { 0x178, 0x1A8, 0x180, 0xD8, 0xE0 };
const uint32_t Timestamp[] = { 1624484050, 1624484031, 1624482254, 1493111701, 1490166596 };

// Addresses of game functions/vars
const uint32_t GetSaveFolder_Addr[] = { 0x7A5790, 0x79D570, 0x7CB040, 0x0, 0x88DB10 }; // inlined in Steam/2017 ;_;

// Others
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
  .DisableManualCulling = false,
  .ShadowFilterStrengthBias = 0,
  .ShadowFilterStrengthMinimum = 0,
  .ShadowFilterStrengthMaximum = 0,
  .ShadowModelHQ = false,
  .ShadowModelForceAll = false,
  .CommunicationScreenResolution = 256,
  .HQMapSlots = 7,
  .WrapperLoadLibrary = { 0 }
};

int version = 0; // which GameVersion we're injected into

WCHAR ModuleName[4096];
WCHAR IniDir[4096];
WCHAR IniPath[4096];
char IniPathA[4096];
WCHAR LogPath[4096];

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

void Settings_ReadINI()
{
  // Try loading config INI:
  memset(IniDir, 0, 4096 * sizeof(WCHAR));
  memset(IniPath, 0, 4096 * sizeof(WCHAR));
  memset(LogPath, 0, 4096 * sizeof(WCHAR));

  // Check for INI inside LodMod DLLs folder first
  if (GetModuleFolder(DllHModule, IniDir, 4096))
    swprintf_s(IniPath, L"%sLodMod.ini", IniDir);

  if (!FileExists(IniPath))
  {
    // Doesn't exist in DLL folder, try game EXE folder
    if (GetModuleFolder(GameHModule, IniDir, 4096))
      swprintf_s(IniPath, L"%sLodMod.ini", IniDir);
  }

  if (!FileExists(IniPath))
  {
    // Doesn't exist in DLL/EXE folder, try checking games save folder
    // Win7/Win10: Documents\My Games\NieR_Automata
    // UWP: Documents\My Games\NieR_Automata_PC
    
    // TODO: Steam2017 inlined the GetSaveFolder func, so this needs a reimpl...
    if (GetSaveFolder_Addr[version] != 0)
    {
      typedef BOOL(*GetSaveFolder_Fn)(char* DstBuf, size_t SizeInBytes);
      GetSaveFolder_Fn GetSaveFolder_Orig = (GetSaveFolder_Fn)(mBaseAddress + GetSaveFolder_Addr[version]);
      if (GetSaveFolder_Orig(IniPathA, 4096))
      {
        swprintf_s(IniPath, L"%SLodMod.ini", IniPathA);
        swprintf_s(IniDir, L"%S", IniPathA);
      }
    }
  }

  if (!FileExists(IniPath))
    return;

  origModulePath[0] = '\0';

  GetPrivateProfileStringW(L"Wrapper", L"LoadLibrary", L"", origModulePath, 4096, IniPath);

  Settings.DebugLog = INI_GetBool(IniPath, L"LodMod", L"DebugLog", false);
  Settings.LODMultiplier = INI_GetFloat(IniPath, L"LodMod", L"LODMultiplier", 0);
  Settings.AOMultiplierWidth = INI_GetFloat(IniPath, L"LodMod", L"AOMultiplierWidth", 1);
  Settings.AOMultiplierHeight = INI_GetFloat(IniPath, L"LodMod", L"AOMultiplierHeight", 1);
  Settings.ShadowDistanceMultiplier = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMultiplier", 1);
  Settings.ShadowDistanceMinimum = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMinimum", 0);
  Settings.ShadowDistanceMaximum = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMaximum", 0);
  Settings.ShadowResolution = GetPrivateProfileIntW(L"LodMod", L"ShadowResolution", 2048, IniPath);
  Settings.ShadowFilterStrengthBias = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthBias", 0);
  Settings.ShadowFilterStrengthMinimum = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthMinimum", 0);
  Settings.ShadowFilterStrengthMaximum = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthMaximum", 0);
  Settings.DisableManualCulling = INI_GetBool(IniPath, L"LodMod", L"DisableManualCulling", false);
  Settings.CommunicationScreenResolution = GetPrivateProfileIntW(L"LodMod", L"CommunicationScreenResolution", 256, IniPath);
  Settings.HQMapSlots = GetPrivateProfileIntW(L"LodMod", L"HQMapSlots", 7, IniPath);
  Settings.ShadowModelHQ = INI_GetBool(IniPath, L"LodMod", L"ShadowModelHQ", false);
  Settings.ShadowModelForceAll = INI_GetBool(IniPath, L"LodMod", L"ShadowModelForceAll", false);

  // Old INI keynames...
  {
    if (INI_GetBool(IniPath, L"LodMod", L"DisableLODs", false))
      Settings.LODMultiplier = 0;

    if (INI_GetBool(IniPath, L"LodMod", L"FullResAO", false))
      Settings.AOMultiplierWidth = Settings.AOMultiplierHeight = 2;

    auto old_aomultiplier = INI_GetFloat(IniPath, L"LodMod", L"AOMultiplier", 0);
    if (old_aomultiplier != 0)
      Settings.AOMultiplierWidth = Settings.AOMultiplierHeight = old_aomultiplier;

    auto old_dist = INI_GetFloat(IniPath, L"LodMod", L"ShadowMinimumDistance", 0);
    if (old_dist != 0)
      Settings.ShadowDistanceMinimum = old_dist;
    old_dist = INI_GetFloat(IniPath, L"LodMod", L"ShadowMaximumDistance", 0);
    if (old_dist != 0)
      Settings.ShadowDistanceMaximum = old_dist;
  }

  // Only allow AO multiplier from 0.1-2 (higher than 2 adds artifacts...)
  Settings.AOMultiplierWidth = fmaxf(fminf(Settings.AOMultiplierWidth, 2), 0.1f);
  Settings.AOMultiplierHeight = fmaxf(fminf(Settings.AOMultiplierHeight, 2), 0.1f);

  if (Settings.HQMapSlots < 7)
    Settings.HQMapSlots = 7;

  if (Settings.DebugLog)
  {
    swprintf_s(LogPath, L"%sLodMod.log", IniDir);

    dlog("\nNieR Automata LodMod " LODMOD_VERSION " - by emoose\n");
    if (GetModuleName(DllHModule, ModuleName, 4096))
      dlog("LodMod module name: %S\n", ModuleName);
    dlog("Detected game type: %s\n", GameVersionName[version]);
    dlog("Wrapping DLL from %S\n", wcslen(origModulePath) > 0 ? origModulePath : L"system folder");
    dlog("Loaded INI from %S\n\nSettings:\n", IniPath);
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
    dlog(" HQMapSlots: %d\n\n", Settings.HQMapSlots);
  }

#ifdef _DEBUG
  hIniUpdateThread = CreateThread(
    NULL,                   // default security attributes
    0,                      // use default stack size  
    IniUpdateThread,       // thread function name
    nullptr,          // argument to thread function 
    0,                      // use default creation flags 
    &dwIniUpdateThread);   // returns the thread identifier 
#endif
}

bool injected = false;
void LodMod_Init()
{
  if (injected)
    return;

  injected = true;

  Settings_ReadINI();

  version = GameVersion::Win10;
  if (*(uint32_t*)(mBaseAddress + TimestampAddr[version]) != Timestamp[version])
  {
    version = GameVersion::Win7;
    if (*(uint32_t*)(mBaseAddress + TimestampAddr[version]) != Timestamp[version])
    {
      version = GameVersion::UWP;
      if (*(uint32_t*)(mBaseAddress + TimestampAddr[version]) != Timestamp[version])
      {
        version = GameVersion::Steam2017;
        if (*(uint32_t*)(mBaseAddress + TimestampAddr[version]) != Timestamp[version])
        {
          version = GameVersion::Debug2017;
          if (*(uint32_t*)(mBaseAddress + TimestampAddr[version]) != Timestamp[version])
          {
            // wrong EXE?
            return;
          }
        }
      }
    }
  }

  MH_Initialize();

  ShadowFixes_Init();
  AOFixes_Init();
  Rebug_Init();
  MapMod_Init();

  MH_EnableHook(MH_ALL_HOOKS);

  // Change SystemData.dat filename in the 2017 builds, since it seems to be slightly different format to 2021
  // (switching between 2017/2021 builds usually ends up changing settings due to these format differences...)
  char s2017[] = "2017";
  if (version == GameVersion::Steam2017)
    SafeWrite(mBaseAddress + 0xEA1CB0 + 8, s2017, 4);
  else if (version == GameVersion::Debug2017)
    SafeWrite(mBaseAddress + 0x1AB8F30 + 8, s2017, 4);

  // 2021 update moved DLC into main depot, skip steam checks since it's always available now
  // Steam checks will interfere if you disable the DLC depots in steam (DLC depots still contain 2017 files, need to disable them to make sure they don't overwrite 2021)
  // Game itself will check for file availability after this, so no worries
  if (DlcCheckPatch1_Addr[version])
    SafeWrite(mBaseAddress + DlcCheckPatch1_Addr[version], uint16_t(0x9090));
  if (DlcCheckPatch2_Addr[version])
    SafeWrite(mBaseAddress + DlcCheckPatch2_Addr[version], uint16_t(0x9090));

  if (Settings.CommunicationScreenResolution != 256)
  {
    SafeWrite(mBaseAddress + CommunicationScreenTexture_Init1_Addr[version], Settings.CommunicationScreenResolution);
    SafeWrite(mBaseAddress + CommunicationScreenTexture_Init2_Addr[version], Settings.CommunicationScreenResolution);
    if (version == GameVersion::Steam2017)
    {
      // special case for inlined CreateTextureBuffer
      SafeWrite(mBaseAddress + CommunicationScreenTexture_Init1_Addr[version] + 7, Settings.CommunicationScreenResolution);
      SafeWrite(mBaseAddress + CommunicationScreenTexture_Init2_Addr[version] + 7, Settings.CommunicationScreenResolution);
    }
  }

  dlog("\nLodMod init complete!\n\n");
}

void InitPlugin()
{
  printf("\nNieR Automata LodMod " LODMOD_VERSION " - by emoose\n");

  GameHModule = GetModuleHandleA("NieRAutomata.exe");

  if (!GameHModule)
    return;

  mBaseAddress = reinterpret_cast<uintptr_t>(GameHModule);
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

    InitPlugin();

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
