#include "pch.h"
#include <fstream>
#include <sstream>
#include <Shlobj.h>
#include <filesystem>

HMODULE GameHModule;
uintptr_t mBaseAddress;

// Address of PE header timestamp, so we can check what EXE is being used
const uint32_t TimestampAddr_Win10 = 0x178;
const uint32_t TimestampAddr_Win7 = 0x1A8;

const uint32_t Timestamp_Win10 = 1624484050;
const uint32_t Timestamp_Win7 = 1624484031;

// Addresses of game functions/vars

const uint32_t HookAddr_Win10 = 0x84CD60;
const uint32_t HookAddr_Win7 = 0x844680;

const uint32_t Hook2Addr_Win10 = 0x84D070;
const uint32_t Hook2Addr_Win7 = 0x844990;

const uint32_t SettingAddr_AOEnabled_Win10 = 0x1421F58;
const uint32_t SettingAddr_AOEnabled_Win7 = 0x1414E48;

const uint32_t IsAOAllowedAddr_Win10 = 0x78BC20;
const uint32_t IsAOAllowedAddr_Win7 = 0x783AF0;

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

  void DisableLODs() {
    // Set all DistRates to 0
    if (DistRates) {
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
};
static_assert(sizeof(NA_Mesh) == 0x5EC); // not proper size lol
#pragma pack(pop)

typedef void* (*sub_84CD60_Fn)(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8);
sub_84CD60_Fn sub_84CD60_Orig;
void* sub_84CD60_Hook(NA_Mesh* a1, BYTE* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
{
  // In case something in orig function depends on LOD details, disable them first
  a1->DisableLODs();

  auto ret = sub_84CD60_Orig(a1, a2, a3, a4, a5, a6, a7, a8);

  // Make sure LOD data is disabled
  a1->DisableLODs();

  return ret;
}

sub_84CD60_Fn sub_84D070_Orig;
void* sub_84D070_Hook(NA_Mesh* a1, BYTE* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
{
  // In case something in orig function depends on LOD details, disable them first
  a1->DisableLODs();

  auto ret = sub_84D070_Orig(a1, a2, a3, a4, a5, a6, a7, a8);

  // Make sure LOD data is disabled
  a1->DisableLODs();

  return ret;
}

typedef uint32_t(*IsAOAllowed_Fn)(void* a1);
IsAOAllowed_Fn IsAOAllowed_Orig;

uint32_t SettingAddr_AOEnabled = SettingAddr_AOEnabled_Win10;
uint32_t IsAOAllowed_Hook(void* a1)
{
  if (!IsAOAllowed_Orig(a1)) {
    return false;
  }

  auto result = *(uint32_t*)(mBaseAddress + SettingAddr_AOEnabled) != 0;
  return result;
}

bool injected = false;
void Injector_InitHooks()
{
  if (injected) {
    return;
  }
  injected = true;

  MH_Initialize();

  uint32_t HookAddr = HookAddr_Win10;
  uint32_t Hook2Addr = Hook2Addr_Win10;
  uint32_t IsAOAllowedAddr = IsAOAllowedAddr_Win10;
  SettingAddr_AOEnabled = SettingAddr_AOEnabled_Win10;
  if (*(uint32_t*)(mBaseAddress + TimestampAddr_Win10) != Timestamp_Win10) {
    HookAddr = HookAddr_Win7;
    Hook2Addr = Hook2Addr_Win7;
    IsAOAllowedAddr = IsAOAllowedAddr_Win7;
    SettingAddr_AOEnabled = SettingAddr_AOEnabled_Win7;
    if (*(uint32_t*)(mBaseAddress + TimestampAddr_Win7) != Timestamp_Win7) {
      // wrong EXE?
      return;
    }
  }

  MH_CreateHook((LPVOID)(mBaseAddress + HookAddr), sub_84CD60_Hook, (LPVOID*)&sub_84CD60_Orig);
  MH_CreateHook((LPVOID)(mBaseAddress + Hook2Addr), sub_84D070_Hook, (LPVOID*)&sub_84D070_Orig);
  MH_CreateHook((LPVOID)(mBaseAddress + IsAOAllowedAddr), IsAOAllowed_Hook, (LPVOID*)&IsAOAllowed_Orig);


  MH_EnableHook(MH_ALL_HOOKS);
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
  printf("NieR Automata LodMod 0.2a - by emoose\n");

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
