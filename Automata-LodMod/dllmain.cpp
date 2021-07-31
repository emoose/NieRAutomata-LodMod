#include "pch.h"
#include <filesystem>
#include "SDK.h"

HMODULE DllHModule;
HMODULE GameHModule;
uintptr_t mBaseAddress;

#define LODMOD_VERSION "0.7"

// Arrays below are [Win10 exe, Win7 exe, UWP/MSStore exe]

// Address of PE header timestamp, so we can check what EXE is being used
const uint32_t TimestampAddr[] = { 0x178, 0x1A8, 0x180 };
const uint32_t Timestamp[] = { 1624484050, 1624484031, 1624482254 };

// Addresses of game functions/vars
const uint32_t GetSaveFolder_Addr[] = { 0x7A5790, 0x79D570, 0x7CB040 };

const uint32_t LodHook1Addr[] = { 0x84CD60, 0x844680, 0x873A90 };
const uint32_t LodHook2Addr[] = { 0x84D070, 0x844990, 0x873DA0 };

const uint32_t Setting_AOEnabled_Addr[] = { 0x1421F58, 0x1414E48, 0x14A4B08 };

const uint32_t IsAOAllowedAddr[] = { 0x78BC20, 0x783AF0, 0x79A620 };

const uint32_t cBinaryXml__Read_Addr[] = { 0x14C700, 0x14C530, 0x14C860 };

const uint32_t ShadowDistanceReaderAddr[] = { 0x77FEA0, 0x777D70, 0x78E8A0 };

const uint32_t ShadowQualityPatchAddr[] = { 0x772484, 0x76A354, 0x780E84 };

const uint32_t ShadowBufferSizePatch1Addr[] = { 0x77F7C5, 0x777695, 0x78E1C5 };
const uint32_t ShadowBufferSizePatch2Addr[] = { 0x77F7CB, 0x77769B, 0x78E1CB };
const uint32_t ShadowBufferSizePatch3Addr[] = { 0x77F7E7, 0x7776B7, 0x78E1E7 };
const uint32_t ShadowBufferSizePatch4Addr[] = { 0x77F7ED, 0x7776BD, 0x78E1ED };

const uint32_t ShadowBufferSizePatch1Addr2[] = { 0x77F5F7, 0x7774C7, 0x78DFF7 };
const uint32_t ShadowBufferSizePatch2Addr2[] = { 0x77F5FD, 0x7774CD, 0x78DFFD };
const uint32_t ShadowBufferSizePatch3Addr2[] = { 0x77F619, 0x7774E9, 0x78E019 };
const uint32_t ShadowBufferSizePatch4Addr2[] = { 0x77F61F, 0x7774EF, 0x78E01F };

const uint32_t g_HalfShadowMap_SizeAddr[] = { 0x774A21, 0x76C8F1, 0x783421 };

// For validating that game set ShadowBuffSizeBits to what we wanted...
const uint32_t ShadowBuffSizeBits_Addr[] = { 0xF513D0, 0xF443C4, 0xFCF3E4 };

// SAO CreateTextureBuffer call hooks:
const uint32_t CreateTextureBuffer_Addr[] = { 0x248060, 0x2415D0, 0x24A870 };
const uint32_t CreateTextureBuffer_TrampolineAddr[] = { 0x7879D2, 0x77F8A2, 0x7963D2 };

const uint32_t AO_CreateTextureBufferCall1_Addr[] = { 0x77439A, 0x76C26A, 0x782D9A };
const uint32_t AO_CreateTextureBufferCall2_Addr[] = { 0x774446, 0x76C316, 0x782E46 };
const uint32_t AO_CreateTextureBufferCall3_Addr[] = { 0x7744B4, 0x76C384, 0x782EB4 };

// Others
const uint32_t CommunicationScreenTexture_Init1_Addr[] = { 0x772658, 0x76A528, 0x781058 };
const uint32_t CommunicationScreenTexture_Init2_Addr[] = { 0x7750DC, 0x76CFAC, 0x783ADC };

// Configurables
bool DebugLog = false;
float LODMultiplier = 0; // if set to 0 will disable LODs
float AOMultiplier = 1;
float ShadowDistanceMultiplier = 1;
float ShadowDistanceMinimum = 0;
float ShadowDistanceMaximum = 0;
float ShadowFilterStrengthBias = 0;
float ShadowFilterStrengthMinimum = 0;
float ShadowFilterStrengthMaximum = 0;
int ShadowBufferSize = 2048; // can be set to 2048+
bool DisableManualCulling = false;
int CommunicationScreenResolution = 256;
int NumHQMapSlots = 7;

// Calculated stuff
int version = 0; // which GameVersion we're injected into
int ExpectedShadowBuffSizeBits = 1; // to check against ShadowBuffSizeBits_Addr

typedef void* (*cBinaryXml__Read_Fn)(struct cBinaryXml* thisptr, uint32_t a2, cObject* output);
cBinaryXml__Read_Fn cBinaryXml__Read_Orig;

bool CheckedShadowBuffSizeBits = false;
// likely not proper fn name
void* cBinaryXml__Read_Hook(struct cBinaryXml* thisptr, uint32_t a2, cObject* output)
{
  auto result = cBinaryXml__Read_Orig(thisptr, a2, output);

  auto* data = output->GetData();

  // TODO: is there a better way than string compare?
  if (strcmp(data->name, "cShadowParam"))
    return result; // not cShadowParam

  // Use this opportunity to check that game set the ShadowBuffSizeBits to what we asked (ExpectedShadowBuffSizeBits)
  // If it's not, that likely means we were injected into the game after the shadow-init code has been ran....
  if (!CheckedShadowBuffSizeBits && DebugLog && ExpectedShadowBuffSizeBits > 0)
  {
    auto ShadowBuffSizeBits = *(uint32_t*)(mBaseAddress + ShadowBuffSizeBits_Addr[version]);
    dlog("ShadowDistanceReader_Hook: ShadowBuffSizeBits = %d, ExpectedShadowBuffSizeBits = %d\n", ShadowBuffSizeBits, ExpectedShadowBuffSizeBits);
    if (ExpectedShadowBuffSizeBits != ShadowBuffSizeBits)
    {
      dlog("\nError: games current ShadowBuffSizeBits (%d) doesn't match the value we tried to set (%d)!\n", ShadowBuffSizeBits, ExpectedShadowBuffSizeBits);
      dlog("This will likely mean shadow resolution won't be updated properly, probably resulting in strange artifacts!\n");
      dlog("(this might be caused by LodMod being injected into the game _after_ shadow-init code has been ran - maybe try a different inject method)\n");
      dlog("If using SpecialK's Import feature to load in LodMod maybe using 'When=Lazy' can help.\n\n");
    }

    CheckedShadowBuffSizeBits = true;
  }

  auto* shadowParam = reinterpret_cast<cShadowParam*>(output);

  float new_strength = shadowParam->m_bokeStrength[0];
  if (ShadowFilterStrengthBias != 0)
  {
    new_strength = shadowParam->m_bokeStrength[0] + ShadowFilterStrengthBias;
    if (new_strength < 0)
      new_strength = 0;
  }

  if (new_strength < ShadowFilterStrengthMinimum)
    new_strength = ShadowFilterStrengthMinimum;

  if (ShadowFilterStrengthMaximum != 0 && new_strength > ShadowFilterStrengthMaximum)
    new_strength = ShadowFilterStrengthMaximum;

  if (new_strength != shadowParam->m_bokeStrength[0])
  {
    if (DebugLog)
    {
      dlog("cBinaryXml::Read: Updated shadow m_bokeStrength from %f to %f\n", shadowParam->m_bokeStrength[0], new_strength);
    }
    shadowParam->m_bokeStrength[0] = new_strength;
  }

  auto& distances = shadowParam->m_splitPoint;
  float new_distances[4] = { distances[0], 0, 0, 0 };

  if (ShadowDistanceMultiplier != 1)
    new_distances[0] = distances[0] * ShadowDistanceMultiplier;

  if (ShadowDistanceMinimum > 0 && new_distances[0] < ShadowDistanceMinimum)
    new_distances[0] = ShadowDistanceMinimum;

  if (ShadowDistanceMaximum > 0 && new_distances[0] > ShadowDistanceMaximum)
    new_distances[0] = ShadowDistanceMaximum;

  if (new_distances[0] != distances[0])
  {
    // figure out the old cascade ratios
    // (this is only run when distance is being updated first time for this area)

    float ratios[] = {
      distances[1] / distances[0],
      distances[2] / distances[0],
      distances[3] / distances[0]
    };

    new_distances[1] = new_distances[0] * ratios[0];
    new_distances[2] = new_distances[0] * ratios[1];
    new_distances[3] = new_distances[0] * ratios[2];

    if (DebugLog)
    {
      dlog("cBinaryXml::Read: Updating shadow m_splitPoint\n 0: %f -> %f\n 1: %f -> %f\n 2: %f -> %f\n 3: %f -> %f\n",
        distances[0], new_distances[0],
        distances[1], new_distances[1],
        distances[2], new_distances[2],
        distances[3], new_distances[3]);
    }

    std::copy_n(new_distances, 4, distances);
  }

  return result;
}

#ifdef _DEBUG
// Hook per-frame shadow update func so we can capture the current shadow params being used

typedef void* (*ShadowDistanceReader_Fn)(cShadowParam* thisptr, void* a2, void* a3, void* a4);
ShadowDistanceReader_Fn ShadowDistanceReader_Orig;

cShadowParam* g_curShadowParam = nullptr;

void* ShadowDistanceReader_Hook(cShadowParam* thisptr, void* a2, void* a3, void* a4)
{
  if (g_curShadowParam != thisptr)
    g_curShadowParam = thisptr;
  return ShadowDistanceReader_Orig(thisptr, a2, a3, a4);
}
#endif

typedef void* (*sub_84CD60_Fn)(BehaviorScr* thisptr, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8);
sub_84CD60_Fn sub_84CD60_Orig;
void* sub_84CD60_Hook(BehaviorScr* thisptr, BYTE* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
{
  if (LODMultiplier <= 0)
  {
    // In case something in orig function depends on LOD details, disable them first
    thisptr->DisableLODs();
  }

  auto ret = sub_84CD60_Orig(thisptr, a2, a3, a4, a5, a6, a7, a8);

  if (LODMultiplier <= 0)
  {
    // Make sure LOD data is disabled
    thisptr->DisableLODs();
  }
  else
  {
    thisptr->MultiplyLODs(LODMultiplier);
  }

  return ret;
}

sub_84CD60_Fn sub_84D070_Orig;
void* sub_84D070_Hook(BehaviorScr* thisptr, BYTE* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
{
  if (LODMultiplier <= 0)
  {
    // In case something in orig function depends on LOD details, disable them first
    thisptr->DisableLODs();
  }

  auto ret = sub_84D070_Orig(thisptr, a2, a3, a4, a5, a6, a7, a8);

  if (LODMultiplier <= 0)
  {
    // Make sure LOD data is disabled
    thisptr->DisableLODs();
  }
  else
  {
    thisptr->MultiplyLODs(LODMultiplier);
  }

  return ret;
}

typedef uint32_t(*IsAOAllowed_Fn)(void* a1);
IsAOAllowed_Fn IsAOAllowed_Orig;

uint32_t IsAOAllowed_Hook(void* a1)
{
  if (!IsAOAllowed_Orig(a1))
    return false;

  auto result = *(uint32_t*)(mBaseAddress + Setting_AOEnabled_Addr[version]) != 0;
  return result;
}

typedef void* (*CreateTextureBuffer_Fn)(void* texture, uint32_t width, uint32_t height, void* a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10, void* a11, void* a12);
CreateTextureBuffer_Fn CreateTextureBuffer_Orig;

void* AO_CreateTextureBuffer_Hook(void* texture, uint32_t width, uint32_t height, void* a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10, void* a11, void* a12)
{
  float width_new = (float)width * AOMultiplier;
  float height_new = (float)height * AOMultiplier;

  // This hook is only called 3 times, so lets log it if we can

  if (DebugLog)
    dlog("AO_CreateTextureBuffer_Hook(%dx%d) - setting resolution to AOMultiplier (%fx) = %dx%d (will be scaled with game render resolution)\n", width, height, AOMultiplier, (uint32_t)width_new, (uint32_t)height_new);

  return CreateTextureBuffer_Orig(texture, (uint32_t)width_new, (uint32_t)height_new, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

void PatchCall(uintptr_t callAddr, uintptr_t callDest)
{
  uint8_t callBuf[] = { 0xE8, 0x00, 0x00, 0x00, 0x00 };
  uint32_t diff = uint32_t(callDest - (callAddr + 5));
  *(uint32_t*)&callBuf[1] = diff;

  SafeWrite(callAddr, callBuf, 5);
}

WCHAR ModuleName[4096];
WCHAR IniDir[4096];
WCHAR IniPath[4096];
char IniPathA[4096];
WCHAR LogPath[4096];

bool injected = false;
void Injector_InitHooks()
{
  if (injected)
    return;

  injected = true;

  version = GameVersion::Win10;
  if (*(uint32_t*)(mBaseAddress + TimestampAddr[0]) != Timestamp[0])
  {
    version = GameVersion::Win7;
    if (*(uint32_t*)(mBaseAddress + TimestampAddr[1]) != Timestamp[1])
    {
      version = GameVersion::UWP;
      if (*(uint32_t*)(mBaseAddress + TimestampAddr[2]) != Timestamp[2])
      {
        // wrong EXE?
        return;
      }
    }
  }

  MH_Initialize();

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

    typedef BOOL(*GetSaveFolder_Fn)(char* DstBuf, size_t SizeInBytes);
    GetSaveFolder_Fn GetSaveFolder_Orig = (GetSaveFolder_Fn)(mBaseAddress + GetSaveFolder_Addr[version]);
    if (GetSaveFolder_Orig(IniPathA, 4096))
    {
      swprintf_s(IniPath, L"%SLodMod.ini", IniPathA);
      swprintf_s(IniDir, L"%S", IniPathA);
    }
  }

  if (FileExists(IniPath))
  {
    DebugLog = INI_GetBool(IniPath, L"LodMod", L"DebugLog", false);
    LODMultiplier = INI_GetFloat(IniPath, L"LodMod", L"LODMultiplier", 0);
    AOMultiplier = INI_GetFloat(IniPath, L"LodMod", L"AOMultiplier", 1);
    ShadowDistanceMultiplier = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMultiplier", 1);
    ShadowDistanceMinimum = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMinimum", 0);
    ShadowDistanceMaximum = INI_GetFloat(IniPath, L"LodMod", L"ShadowDistanceMaximum", 0);
    ShadowBufferSize = GetPrivateProfileIntW(L"LodMod", L"ShadowResolution", 2048, IniPath);
    ShadowFilterStrengthBias = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthBias", 0);
    ShadowFilterStrengthMinimum = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthMinimum", 0);
    ShadowFilterStrengthMaximum = INI_GetFloat(IniPath, L"LodMod", L"ShadowFilterStrengthMaximum", 0);
    DisableManualCulling = INI_GetBool(IniPath, L"LodMod", L"DisableManualCulling", false);
    CommunicationScreenResolution = GetPrivateProfileIntW(L"LodMod", L"CommunicationScreenResolution", 256, IniPath);
    NumHQMapSlots = GetPrivateProfileIntW(L"LodMod", L"HQMapSlots", 7, IniPath);
    // Old INI keynames...
    if (INI_GetBool(IniPath, L"LodMod", L"DisableLODs", false))
      LODMultiplier = 0;

    if (INI_GetBool(IniPath, L"LodMod", L"FullResAO", false))
      AOMultiplier = 2;

    auto old_dist = INI_GetFloat(IniPath, L"LodMod", L"ShadowMinimumDistance", 0);
    if (old_dist != 0)
      ShadowDistanceMinimum = old_dist;
    old_dist = INI_GetFloat(IniPath, L"LodMod", L"ShadowMaximumDistance", 0);
    if (old_dist != 0)
      ShadowDistanceMaximum = old_dist;

    // Only allow AO multiplier from 0.1-2 (higher than 2 adds artifacts...)
    AOMultiplier = fmaxf(fminf(AOMultiplier, 2), 0.1f);

    if (NumHQMapSlots < 7)
      NumHQMapSlots = 7;

    if (DebugLog)
    {
      swprintf_s(LogPath, L"%sLodMod.log", IniDir);

      dlog("NieR Automata LodMod " LODMOD_VERSION " - by emoose\n");
      if (GetModuleName(DllHModule, ModuleName, 4096))
        dlog("LodMod module name: %S\n", ModuleName);
      dlog("Detected game type: %s\n", version == GameVersion::Win10 ? "Steam/Win10" : (version == GameVersion::Win7 ? "Steam/Win7" : "UWP/MS Store"));
      dlog("Loaded INI from %S\n\nSettings:\n", IniPath);
      dlog(" LODMultiplier: %f\n", LODMultiplier);
      dlog(" AOMultiplier: %f\n", AOMultiplier);
      dlog(" ShadowDistanceMultiplier: %f\n", ShadowDistanceMultiplier);
      dlog(" ShadowDistanceMinimum: %f\n", ShadowDistanceMinimum);
      dlog(" ShadowDistanceMaximum: %f\n", ShadowDistanceMaximum);
      dlog(" ShadowResolution: %d\n", ShadowBufferSize);
      dlog(" ShadowFilterStrengthBias: %f\n", ShadowFilterStrengthBias);
      dlog(" ShadowFilterStrengthMinimum: %f\n", ShadowFilterStrengthMinimum);
      dlog(" ShadowFilterStrengthMaximum: %f\n", ShadowFilterStrengthMaximum);
      dlog(" DisableManualCulling: %s\n", DisableManualCulling ? "true" : "false");
      dlog(" CommunicationScreenResolution: %d\n\n", CommunicationScreenResolution);
    }
  }

  if (LODMultiplier != 1)
  {
    MH_CreateHook((LPVOID)(mBaseAddress + LodHook1Addr[version]), sub_84CD60_Hook, (LPVOID*)&sub_84CD60_Orig);
    MH_CreateHook((LPVOID)(mBaseAddress + LodHook2Addr[version]), sub_84D070_Hook, (LPVOID*)&sub_84D070_Orig);
  }

  MH_CreateHook((LPVOID)(mBaseAddress + IsAOAllowedAddr[version]), IsAOAllowed_Hook, (LPVOID*)&IsAOAllowed_Orig);
  MH_CreateHook((LPVOID)(mBaseAddress + cBinaryXml__Read_Addr[version]), cBinaryXml__Read_Hook, (LPVOID*)&cBinaryXml__Read_Orig);

#ifdef _DEBUG
  MH_CreateHook((LPVOID)(mBaseAddress + ShadowDistanceReaderAddr[version]), ShadowDistanceReader_Hook, (LPVOID*)&ShadowDistanceReader_Orig);
#endif

  void Rebug_Init();
  Rebug_Init();

  void MapMod_Init();
  MapMod_Init();

  MH_EnableHook(MH_ALL_HOOKS);

  if (AOMultiplier != 1)
  {
    CreateTextureBuffer_Orig = (CreateTextureBuffer_Fn)(mBaseAddress + CreateTextureBuffer_Addr[version]);

    // Have to write a trampoline somewhere within 2GiB of the hooked call, needs 12 bytes...
    uint8_t trampoline[] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xE0 };

    *(uintptr_t*)&trampoline[2] = (uintptr_t)&AO_CreateTextureBuffer_Hook;

    SafeWrite(mBaseAddress + CreateTextureBuffer_TrampolineAddr[version], trampoline, 12);

    // Hook SAO-related CreateTextureBuffer calls to call the trampoline we patched in

    PatchCall(mBaseAddress + AO_CreateTextureBufferCall1_Addr[version], mBaseAddress + CreateTextureBuffer_TrampolineAddr[version]);
    PatchCall(mBaseAddress + AO_CreateTextureBufferCall2_Addr[version], mBaseAddress + CreateTextureBuffer_TrampolineAddr[version]);
    PatchCall(mBaseAddress + AO_CreateTextureBufferCall3_Addr[version], mBaseAddress + CreateTextureBuffer_TrampolineAddr[version]);
  }

  dlog("Hooks complete!\n");

  if (CommunicationScreenResolution != 256)
  {
    SafeWrite(mBaseAddress + CommunicationScreenTexture_Init1_Addr[version], CommunicationScreenResolution);
    SafeWrite(mBaseAddress + CommunicationScreenTexture_Init2_Addr[version], CommunicationScreenResolution);
  }

  // Shadow quality patch:
  // 
  // Code at this address sets a global var that's used to size different buffers based on it
  // Seems to always be set to 1 normally, but the code around it seems to be checking game render resolution
  // and sets it to 4 depending on some unknown resolution being detected, guess it was left incomplete?
  // (update: seems its maybe set to 4 for 3840x2160 & 3200x1800, but I'm not sure how that would even work without the "Update shadow quadrant sizes" stuff below...)
  // Buffer size = value << 0xB
  int value = ShadowBufferSize >> 11;

  // shadows can't go any lower than 2048
  if (value > 0)
  {
    dlog("\nPatching shadow buffer code...\n");
    uint8_t ShadowQualityPatch[] = { 0xB9, 0x04, 0x00, 0x00, 0x00, 0x90 };
    *(uint32_t*)(&ShadowQualityPatch[1]) = value;
    SafeWrite(mBaseAddress + ShadowQualityPatchAddr[version], ShadowQualityPatch, 6);

    // Patch out the 3840x2160/3200x1800 ShadowQuality = 4 code
    uint8_t MovEaxEcx[] = { 0x89, 0xC8, 0x90, 0x90, 0x90 };
    SafeWrite(mBaseAddress + ShadowQualityPatchAddr[version] + 10, MovEaxEcx, 5);

    ExpectedShadowBuffSizeBits = value;

    // Size of each quadrant in shadowmap
    ShadowBufferSize /= 2;

    dlog("Shadow quadrant size: %d\n", ShadowBufferSize);

    dlog("ShadowBuffSizeBits: %d\n", ExpectedShadowBuffSizeBits);

    // Poor mans lzcnt...
    int tempSize = ShadowBufferSize;
    int shadowNumBits = 0;
    while (tempSize)
    {
      tempSize /= 2;
      shadowNumBits++;
    }
    shadowNumBits--;

    dlog("ShadowResNumBits: %d\n", shadowNumBits);

    // Update shadow quadrant sizes
    SafeWrite(mBaseAddress + ShadowBufferSizePatch1Addr[version], uint8_t(shadowNumBits));
    SafeWrite(mBaseAddress + ShadowBufferSizePatch2Addr[version], uint8_t(shadowNumBits));
    SafeWrite(mBaseAddress + ShadowBufferSizePatch3Addr[version], uint32_t(ShadowBufferSize));
    SafeWrite(mBaseAddress + ShadowBufferSizePatch4Addr[version], uint32_t(ShadowBufferSize));

    SafeWrite(mBaseAddress + ShadowBufferSizePatch1Addr2[version], uint8_t(shadowNumBits));
    SafeWrite(mBaseAddress + ShadowBufferSizePatch2Addr2[version], uint8_t(shadowNumBits));
    SafeWrite(mBaseAddress + ShadowBufferSizePatch3Addr2[version], uint32_t(ShadowBufferSize));
    SafeWrite(mBaseAddress + ShadowBufferSizePatch4Addr2[version], uint32_t(ShadowBufferSize));

    // g_HalfShadowMap size needs to be half of shadow buffer size too, else god rays will break
    SafeWrite(mBaseAddress + g_HalfShadowMap_SizeAddr[version], uint32_t(ShadowBufferSize));
  }

  dlog("\nLodMod init complete!\n\n");
}


// IAT hooks for getting around SteamStub, bleh

typedef void(*GetSystemTimeAsFileTime_ptr)(LPFILETIME lpSystemTimeAsFileTime);
GetSystemTimeAsFileTime_ptr GetSystemTimeAsFileTime_orig = NULL;
GetSystemTimeAsFileTime_ptr* GetSystemTimeAsFileTime_iat = NULL;

typedef BOOL(*QueryPerformanceCounter_ptr)(LARGE_INTEGER* lpPerformanceCount);
QueryPerformanceCounter_ptr QueryPerformanceCounter_orig = NULL;
QueryPerformanceCounter_ptr* QueryPerformanceCounter_iat = NULL;

static void GetSystemTimeAsFileTime_Hook(LPFILETIME lpSystemTimeAsFileTime)
{
  // call original hooked func
  GetSystemTimeAsFileTime_orig(lpSystemTimeAsFileTime);

  SafeWrite((uintptr_t)GetSystemTimeAsFileTime_iat, GetSystemTimeAsFileTime_orig);
  SafeWrite((uintptr_t)QueryPerformanceCounter_iat, QueryPerformanceCounter_orig);

  // run our code :)
  Injector_InitHooks();
}

static BOOL QueryPerformanceCounter_hook(LARGE_INTEGER* lpPerformanceCount)
{
  // patch iats back to original
  SafeWrite((uintptr_t)GetSystemTimeAsFileTime_iat, GetSystemTimeAsFileTime_orig);
  SafeWrite((uintptr_t)QueryPerformanceCounter_iat, QueryPerformanceCounter_orig);

  // run our code :)
  Injector_InitHooks();

  // call original hooked func
  return QueryPerformanceCounter_orig(lpPerformanceCount);
}

void* ModuleDirectoryEntryData(void* Module, int DirectoryEntry, int* EntrySize = nullptr)
{
  auto* base = (BYTE*)Module;
  auto* dosHeader = (IMAGE_DOS_HEADER*)base;
  if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    return nullptr; // invalid header :(

  auto* ntHeader = (IMAGE_NT_HEADERS*)(base + dosHeader->e_lfanew);
  if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
    return nullptr; // invalid header :(

  auto entryAddr = ntHeader->OptionalHeader.DataDirectory[DirectoryEntry].VirtualAddress;
  if (!entryAddr)
    return nullptr;

  if (EntrySize)
    *EntrySize = ntHeader->OptionalHeader.DataDirectory[DirectoryEntry].Size;

  return base + entryAddr;
}

FARPROC* GetIATPointer(void* Module, const char* LibraryName, const char* ImportName)
{
  auto* base = (BYTE*)Module;
  auto* importTable = (IMAGE_IMPORT_DESCRIPTOR*)ModuleDirectoryEntryData(Module, IMAGE_DIRECTORY_ENTRY_IMPORT);
  if (!importTable)
    return nullptr;

  for (; importTable->Characteristics; ++importTable)
  {
    auto* dllName = (const char*)(base + importTable->Name);

    if (!_stricmp(dllName, LibraryName))
    {
      // found the dll

      auto* thunkData = (IMAGE_THUNK_DATA*)(base + importTable->OriginalFirstThunk);
      auto* iat = (FARPROC*)(base + importTable->FirstThunk);

      for (; thunkData->u1.Ordinal; ++thunkData, ++iat)
      {
        if (!IMAGE_SNAP_BY_ORDINAL(thunkData->u1.Ordinal))
        {
          auto* importInfo = (IMAGE_IMPORT_BY_NAME*)(base + thunkData->u1.AddressOfData);

          if (!_stricmp((char*)importInfo->Name, ImportName))
          {
            // found the import
            return iat;
          }
        }
      }

      return nullptr;
    }
  }

  return nullptr;
}

void Injector_InitSteamStub()
{
  // Hook the GetSystemTimeAsFileTime function, in most games this seems to be one of the first imports called once SteamStub has finished.
  bool hooked = false;
  GetSystemTimeAsFileTime_iat = (GetSystemTimeAsFileTime_ptr*)GetIATPointer(GameHModule, "KERNEL32.DLL", "GetSystemTimeAsFileTime");
  if (GetSystemTimeAsFileTime_iat)
  {
    // Found IAT address, hook the function to run our own code instead

    GetSystemTimeAsFileTime_orig = *GetSystemTimeAsFileTime_iat;
    SafeWrite((uintptr_t)GetSystemTimeAsFileTime_iat, GetSystemTimeAsFileTime_Hook);

    hooked = true;
  }

  // As a backup we'll also hook QueryPerformanceCounter, almost every game makes use of this
  QueryPerformanceCounter_iat = (QueryPerformanceCounter_ptr*)GetIATPointer(GameHModule, "KERNEL32.DLL", "QueryPerformanceCounter");
  if (QueryPerformanceCounter_iat)
  {
    // Found IAT address, hook the function to run our own code instead

    QueryPerformanceCounter_orig = *QueryPerformanceCounter_iat;
    SafeWrite((uintptr_t)QueryPerformanceCounter_iat, QueryPerformanceCounter_hook);

    hooked = true;
  }

  // If we failed to hook, try running our code directly
  if (!hooked)
  {
    Injector_InitHooks();
  }
}

void InitPlugin()
{
  printf("\nNieR Automata LodMod " LODMOD_VERSION " - by emoose\n");

  GameHModule = GetModuleHandleA("NieRAutomata.exe");

  if (!GameHModule)
    return;

  mBaseAddress = reinterpret_cast<uintptr_t>(GameHModule);

  Injector_InitSteamStub();
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

    bool Proxy_Attach(); // proxy.cpp
    Proxy_Attach();

    InitPlugin();
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    void Proxy_Detach();
    Proxy_Detach();
    break;
  }
  return TRUE;
}
