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

const uint32_t HookAddr_Win10 = 0x853800;
const uint32_t HookAddr_Win7 = 0x84B0A0;

typedef void* (*sub_853800_Fn)(void* thisptr, void* rdx, void* r8, void* r9);
sub_853800_Fn sub_853800_Orig;
void sub_853800_Hook(BYTE* a1, void* rdx, void* r8, void* r9)
{
  sub_853800_Orig(a1, rdx, r8, r9);

  float* dists = *(float**)(a1 + 0x3F0);
  if (dists) {
    dists[0] = 0;
    dists[1] = 0;
    dists[2] = 0;
    dists[3] = 0;
  }
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

  if (*(uint32_t*)(mBaseAddress + TimestampAddr_Win10) != Timestamp_Win10) {
    HookAddr = HookAddr_Win7;
    if (*(uint32_t*)(mBaseAddress + TimestampAddr_Win7) != Timestamp_Win7) {
      // wrong EXE?
      return;
    }
  }

  MH_CreateHook((LPVOID)(mBaseAddress + HookAddr), sub_853800_Hook, (LPVOID*)&sub_853800_Orig);
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
  printf("NieR Automata LodMod - by emoose\n");

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
