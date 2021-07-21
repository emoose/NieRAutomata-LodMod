﻿#include "pch.h"
#include <filesystem>

HMODULE GameHModule;
uintptr_t mBaseAddress;

enum GameVersion {
  Win10 = 0,
  Win7,
  UWP
};

// Arrays below are [Win10 exe, Win7 exe]
// Address of PE header timestamp, so we can check what EXE is being used
const uint32_t TimestampAddr[] = { 0x178, 0x1A8, 0x180 };
const uint32_t Timestamp[] = { 1624484050, 1624484031, 1624482254 };

// Addresses of game functions/vars

const uint32_t GetSaveFolder_Addr[] = { 0x7A5790, 0x79D570, 0x7CB040 };

const uint32_t LodHook1Addr[] = { 0x84CD60, 0x844680, 0x873A90 };
const uint32_t LodHook2Addr[] = { 0x84D070, 0x844990, 0x873DA0 };

const uint32_t var_SettingAddr_AOEnabled[] = { 0x1421F58, 0x1414E48, 0x14A4B08 };

const uint32_t IsAOAllowedAddr[] = { 0x78BC20, 0x783AF0, 0x79A620 };

const uint32_t ShadowDistanceReaderAddr[] = { 0x77FEA0, 0x777D70, 0x78E8A0 };

uint32_t ShadowQualityPatchAddr[] = { 0x772484, 0x76A354, 0x780E84 };

uint32_t ShadowBufferSizePatch1Addr[] = { 0x77F7C5, 0x777695, 0x78E1C5 };
uint32_t ShadowBufferSizePatch2Addr[] = { 0x77F7CB, 0x77769B, 0x78E1CB };
uint32_t ShadowBufferSizePatch3Addr[] = { 0x77F7E7, 0x7776B7, 0x78E1E7 };
uint32_t ShadowBufferSizePatch4Addr[] = { 0x77F7ED, 0x7776BD, 0x78E1ED };

uint32_t ShadowBufferSizePatch1Addr2[] = { 0x77F5F7, 0x7774C7, 0x78DFF7 };
uint32_t ShadowBufferSizePatch2Addr2[] = { 0x77F5FD, 0x7774CD, 0x78DFFD };
uint32_t ShadowBufferSizePatch3Addr2[] = { 0x77F619, 0x7774E9, 0x78E019 };
uint32_t ShadowBufferSizePatch4Addr2[] = { 0x77F61F, 0x7774EF, 0x78E01F };

uint32_t g_HalfShadowMap_SizeAddr[] = { 0x774A21, 0x76C8F1, 0x783421 };

// SAO CreateTextureBuffer call hooks:
uint32_t CreateTextureBuffer_Addr[] = { 0x248060, 0x2415D0, 0x24A870 };
uint32_t CreateTextureBuffer_TrampolineAddr[] = { 0x7879D2, 0x77F8A2, 0x7963D2 };

uint32_t AO_CreateTextureBufferCall1_Addr[] = { 0x77439A, 0x76C26A, 0x782D9A };
uint32_t AO_CreateTextureBufferCall2_Addr[] = { 0x774446, 0x76C316, 0x782E46 };
uint32_t AO_CreateTextureBufferCall3_Addr[] = { 0x7744B4, 0x76C384, 0x782EB4 };

// Configurables
float LODMultiplier = 0; // if set to 0 will disable LODs
float AOMultiplier = 1;
float ShadowMinimumDistance = 0;
float ShadowMaximumDistance = 0;
int ShadowBufferSize = 2048; // can be set to 2048+

#pragma pack(push, 1)
struct NA_Mesh
{
  /* 0x000 */ uint8_t Unk0[0x390];
  /* 0x390 */ void* ShadowArray; // some kind of array/vector related with shadows, "ShadowCast" flag affects something in the entries
  /* 0x398 */ uint8_t Unk398[0x58];
  /* 0x3F0 */ float* DistRates; // pointer to "DistRate0"-"DistRate3"
  /* 0x3F8 */ uint32_t NumDistRates;
  /* 0x3FC */ float Unk3FC;
  /* 0x400 */ uint32_t Unk400; // "UseLostLOD", gets set if using dist rates?
  /* 0x404 */ uint32_t Unk404; // if set, Unk3FC = 0.05 ?
  /* 0x408 */ uint8_t Unk408[0x118];
  /* 0x520 */ uint32_t Unk520; // "UseCullAABB" sets/removes 0x10 flag
  /* 0x524 */ float Unk524;
  /* 0x528 */ uint8_t Unk528[0x48];
  /* 0x570 */ uint32_t Unk570; // "CamAlpha" sets to 1 or 0
  /* 0x574 */ float Unk574;
  /* 0x578 */ float Unk578; // "CamAlpha"
  /* 0x57C */ float Unk57C;
  /* 0x580 */ uint32_t Unk580;
  /* 0x584 */ uint32_t AmbientOcclusionAllowed; // "AO_OFF" sets to 0
  /* 0x588 */ uint32_t Unk588;
  /* 0x58C */ uint32_t Unk58C;
  /* 0x590 */ uint8_t Unk590[0x58];
  /* 0x5E8 */ float BloomStrength; // always 0 or 1 ?

  void DisableLODs()
  {
    // Set all DistRates to 0
    if (DistRates)
    {
      memset(DistRates, 0, sizeof(float) * 4);
    }

    // Set number of DistRates to 1 (0 causes weird issues)
    NumDistRates = 1;

    // Disable UseLostLOD
    Unk400 = 0;
    Unk404 = 0;

    // Remove UseCullAABB flag
    Unk520 &= 0xFFFFFFEF;
  }

  void MultiplyLODs(float multiplier)
  {
    if (!DistRates || multiplier <= 0)
      return;

    for (uint32_t i = 0; i < NumDistRates; i++)
    {
      // DistRate needs to be made smaller to go further, idk how it works exactly
      DistRates[i] /= multiplier;
    }
  }
};
static_assert(sizeof(NA_Mesh) == 0x5EC); // not proper size lol
#pragma pack(pop)

typedef void* (*sub_84CD60_Fn)(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8);
sub_84CD60_Fn sub_84CD60_Orig;
void* sub_84CD60_Hook(NA_Mesh* a1, BYTE* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
{
  if (LODMultiplier <= 0)
  {
    // In case something in orig function depends on LOD details, disable them first
    a1->DisableLODs();
  }

  auto ret = sub_84CD60_Orig(a1, a2, a3, a4, a5, a6, a7, a8);

  if (LODMultiplier <= 0)
  {
    // Make sure LOD data is disabled
    a1->DisableLODs();
  }
  else
  {
    a1->MultiplyLODs(LODMultiplier);
  }

  return ret;
}

sub_84CD60_Fn sub_84D070_Orig;
void* sub_84D070_Hook(NA_Mesh* a1, BYTE* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
{
  if (LODMultiplier <= 0)
  {
    // In case something in orig function depends on LOD details, disable them first
    a1->DisableLODs();
  }

  auto ret = sub_84D070_Orig(a1, a2, a3, a4, a5, a6, a7, a8);

  if (LODMultiplier <= 0)
  {
    // Make sure LOD data is disabled
    a1->DisableLODs();
  }
  else
  {
    a1->MultiplyLODs(LODMultiplier);
  }

  return ret;
}

typedef uint32_t(*IsAOAllowed_Fn)(void* a1);
IsAOAllowed_Fn IsAOAllowed_Orig;

uint32_t SettingAddr_AOEnabled = var_SettingAddr_AOEnabled[0];
uint32_t IsAOAllowed_Hook(void* a1)
{
  if (!IsAOAllowed_Orig(a1)) {
    return false;
  }

  auto result = *(uint32_t*)(mBaseAddress + SettingAddr_AOEnabled) != 0;
  return result;
}

typedef void* (*CreateTextureBuffer_Fn)(void* texture, uint32_t width, uint32_t height, void* a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10, void* a11, void* a12);
CreateTextureBuffer_Fn CreateTextureBuffer_Orig;

void* AO_CreateTextureBuffer_Hook(void* texture, uint32_t width, uint32_t height, void* a4, void* a5, void* a6, void* a7, void* a8, void* a9, void* a10, void* a11, void* a12)
{
  float width_new = (float)width * AOMultiplier;
  float height_new = (float)height * AOMultiplier;

  return CreateTextureBuffer_Orig(texture, (uint32_t)width_new, (uint32_t)height_new, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

void PatchCall(uintptr_t callAddr, uintptr_t callDest)
{
  uint8_t callBuf[] = { 0xE8, 0x00, 0x00, 0x00, 0x00 };
  uint32_t diff = callDest - (callAddr + 5);
  *(uint32_t*)&callBuf[1] = diff;

  SafeWrite(callAddr, callBuf, 5);
}

// TODO: need to find where the shadow distance is set originally and hook there instead
// That way we could double/half/etc instead of needing to set to a static value
// (atm this is just hooking the function that reads it/handles setting up shadow stuff from it, which is ran every frame...)
typedef void*(*ShadowDistanceReader_Fn)(void* a1, void* a2, void* a3, void* a4);
ShadowDistanceReader_Fn ShadowDistanceReader_Orig;
void* ShadowDistanceReader_Hook(BYTE* a1, void* a2, void* a3, void* a4)
{
  float* distance = (float*)(a1 + 0x14);
  if (ShadowMinimumDistance > 0 && *distance < ShadowMinimumDistance)
    *distance = ShadowMinimumDistance;
  if (ShadowMaximumDistance > 0 && *distance > ShadowMaximumDistance)
    *distance = ShadowMaximumDistance;

  return ShadowDistanceReader_Orig(a1, a2, a3, a4);
}

bool injected = false;
WCHAR IniPath[4096];
char IniPathA[4096];

void Injector_InitHooks()
{
  if (injected) {
    return;
  }
  injected = true;

  MH_Initialize();

  int version = GameVersion::Win10;
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

  // Get folder path of currently running EXE
  GetModuleFileName(GameHModule, IniPath, 4096);
  int len = wcslen(IniPath);
  int lastPathSep = -1;
  for (int i = len - 2; i >= 0; i--)
  {
    if (IniPath[i] == '\\' || IniPath[i] == '/')
    {
      lastPathSep = i;
      break;
    }
  }

  if (lastPathSep >= 0)
  {
    IniPath[lastPathSep + 1] = 0;
    swprintf_s(IniPath, L"%s/LodMod.ini", IniPath);

    if (!FileExists(IniPath))
    {
      // Try checking games save folder
      // Win7/Win10: Documents\My Games\NieR_Automata
      // UWP: Documents\My Games\NieR_Automata_PC

      typedef BOOL(*GetSaveFolder_Fn)(char* DstBuf, size_t SizeInBytes);
      GetSaveFolder_Fn GetSaveFolder_Orig = (GetSaveFolder_Fn)(mBaseAddress + GetSaveFolder_Addr[version]);
      if (GetSaveFolder_Orig(IniPathA, 4096))
      {
        swprintf_s(IniPath, L"%S/LodMod.ini", IniPathA);
      }
    }

    if (FileExists(IniPath))
    {
      LODMultiplier = INI_GetFloat(IniPath, L"LodMod", L"LODMultiplier", 0);
      AOMultiplier = INI_GetFloat(IniPath, L"LodMod", L"AOMultiplier", 1);
      ShadowMinimumDistance = INI_GetFloat(IniPath, L"LodMod", L"ShadowMinimumDistance", 0);
      ShadowMaximumDistance = INI_GetFloat(IniPath, L"LodMod", L"ShadowMaximumDistance", 0);
      ShadowBufferSize = GetPrivateProfileIntW(L"LodMod", L"ShadowResolution", 2048, IniPath);

      // Old INI keynames...
      if (INI_GetBool(IniPath, L"LodMod", L"DisableLODs", false))
        LODMultiplier = 0;

      if (INI_GetBool(IniPath, L"LodMod", L"FullResAO", false))
        AOMultiplier = 2;

      // Only allow AO multiplier from 0.1-2 (higher than 2 adds artifacts...)
      AOMultiplier = fmaxf(fminf(AOMultiplier, 2), 0.1f);
    }
  }

  SettingAddr_AOEnabled = var_SettingAddr_AOEnabled[version];

  if (LODMultiplier != 1)
  {
    MH_CreateHook((LPVOID)(mBaseAddress + LodHook1Addr[version]), sub_84CD60_Hook, (LPVOID*)&sub_84CD60_Orig);
    MH_CreateHook((LPVOID)(mBaseAddress + LodHook2Addr[version]), sub_84D070_Hook, (LPVOID*)&sub_84D070_Orig);
  }

  MH_CreateHook((LPVOID)(mBaseAddress + IsAOAllowedAddr[version]), IsAOAllowed_Hook, (LPVOID*)&IsAOAllowed_Orig);
  MH_CreateHook((LPVOID)(mBaseAddress + ShadowDistanceReaderAddr[version]), ShadowDistanceReader_Hook, (LPVOID*)&ShadowDistanceReader_Orig);

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

  // Shadow quality patch:
  // 
  // Code at this address sets a global var that's used to size different buffers based on it
  // Seems to always be set to 1 normally, but the code around it seems to be checking game render resolution
  // and sets it to 4 depending on some unknown resolution being detected, guess it was left incomplete?
  // Buffer size = value << 0xB
  int value = ShadowBufferSize >> 11;
  if (value <= 0)
    return; // can't go any lower than 2048

  uint8_t ShadowQualityPatch[] = { 0xB9, 0x04, 0x00, 0x00, 0x00, 0x90 };
  *(uint32_t*)(&ShadowQualityPatch[1]) = value;
  SafeWrite(mBaseAddress + ShadowQualityPatchAddr[version], ShadowQualityPatch, 6);

  // Size of each quadrant in shadowmap
  ShadowBufferSize /= 2;

  // Poor mans lzcnt...
  int tempSize = ShadowBufferSize;
  int shadowNumBits = 0;
  while (tempSize)
  {
    tempSize /= 2;
    shadowNumBits++;
  }
  shadowNumBits--;

  // Update shadow buffer sizes (should be half of the above buffer size?)
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
  printf("NieR Automata LodMod 0.54 - by emoose\n");

  GameHModule = GetModuleHandleA("NieRAutomata.exe");

  if (!GameHModule)
    return;

  mBaseAddress = reinterpret_cast<uintptr_t>(GameHModule);

  Injector_InitSteamStub();
}

HMODULE ourModule;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    ourModule = hModule;

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
