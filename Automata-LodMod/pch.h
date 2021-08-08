#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include "MinHook/MinHook.h"

extern uintptr_t mBaseAddress;

// dllmain.cpp
extern HMODULE DllHModule;
extern HMODULE GameHModule;
extern uintptr_t mBaseAddress;
void LodMod_Init();

void Rebug_Init(); // Rebug.cpp

// Configurables
struct LodModSettings
{
  bool DebugLog;
  float LODMultiplier;
  float AOMultiplierWidth;
  float AOMultiplierHeight;
  int ShadowResolution;
  float ShadowDistanceMinimum;
  float ShadowDistanceMaximum;
  float ShadowDistanceMultiplier;
  bool DisableManualCulling;
  float ShadowFilterStrengthBias;
  float ShadowFilterStrengthMinimum;
  float ShadowFilterStrengthMaximum;
  int CommunicationScreenResolution;
  int HQMapSlots;
  WCHAR WrapperLoadLibrary[4096];
};
extern LodModSettings Settings;
extern uintptr_t mBaseAddress;
extern int version;
extern WCHAR LogPath[4096];
extern WCHAR IniPath[4096];

// MapMod.cpp
extern bool g11420IsLoaded;
void MapMod_Init();

// proxy.cpp
extern WCHAR origModulePath[4096];
bool Proxy_Attach();
void Proxy_Detach();
void Proxy_InitSteamStub();

// Utility.cpp
bool FileExists(const WCHAR* Filename);
bool DirExists(const WCHAR* DirPath);
bool GetModuleName(HMODULE module, WCHAR* destBuf, int bufLength);
bool GetModuleFolder(HMODULE module, WCHAR* destBuf, int bufLength);

HWND FindMainWindow(DWORD process_id);

bool INI_GetBool(const WCHAR* IniPath, const WCHAR* Section, const WCHAR* Key, bool DefaultValue);
float INI_GetFloat(const WCHAR* IniPath, const WCHAR* Section, const WCHAR* Key, float DefaultValue);

void dlog(const char* Format, ...);

template <typename T>
void SafeWrite(uintptr_t address, T value)
{
  DWORD oldProtect = 0;
  VirtualProtect((LPVOID)address, sizeof(T), PAGE_READWRITE, &oldProtect);
  *reinterpret_cast<T*>(address) = value;
  VirtualProtect((LPVOID)address, sizeof(T), oldProtect, &oldProtect);
}

template <typename T>
void SafeWrite(uintptr_t address, T value, int count)
{
  DWORD oldProtect = 0;
  VirtualProtect((LPVOID)address, sizeof(T) * count, PAGE_READWRITE, &oldProtect);
  for(int i = 0; i < count; i++)
    *reinterpret_cast<T*>(address + (sizeof(T) * i)) = value;
  VirtualProtect((LPVOID)address, sizeof(T) * count, oldProtect, &oldProtect);
}

template <typename T>
void SafeWrite(uintptr_t address, T* value, int count)
{
  DWORD oldProtect = 0;
  VirtualProtect((LPVOID)address, sizeof(T) * count, PAGE_READWRITE, &oldProtect);
  for (int i = 0; i < count; i++)
    *reinterpret_cast<T*>(address + (sizeof(T) * i)) = value[i];
  VirtualProtect((LPVOID)address, sizeof(T) * count, oldProtect, &oldProtect);
}

template <typename T>
void SafeWriteModule(uintptr_t offset, T value)
{
  SafeWrite<T>(mBaseAddress + offset, value);
}

template <typename T>
void SafeWriteModule(uintptr_t offset, T value, int count)
{
  SafeWrite<T>(mBaseAddress + offset, value, count);
}

template <typename T>
void SafeWriteModule(uintptr_t offset, T* value, int count)
{
  SafeWrite<T>(mBaseAddress + offset, value, count);
}

// Same as SafeWrite but doesn't VirtualProtect first, more efficient if you already know the memory is writable!
template <typename T>
inline void UnsafeWriteModule(uintptr_t offset, T value)
{
  *reinterpret_cast<T*>(mBaseAddress + offset) = value;
}
