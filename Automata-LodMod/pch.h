#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <Shlobj.h>

#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <locale>
#include <codecvt>
#ifdef _DEBUG
#include <mutex>
#include <thread>
#endif

#include "third_party/MinHook/MinHook.h"

#include "SDK.h"
#include "CriUSM.h"

// dllmain.cpp
extern HMODULE DllHModule;
extern HMODULE GameHModule;
extern uintptr_t mBaseAddress;

inline uintptr_t GameAddress(const uint32_t* addr)
{
  if (!addr[int(version)])
    return 0;
  return mBaseAddress + addr[int(version)];
}

inline uintptr_t GameAddress(uint32_t addr)
{
  return mBaseAddress + addr;
}

template <typename T>
inline T GameAddress(const uint32_t* addr)
{
  if (!addr[int(version)])
    return 0;
  return reinterpret_cast<T>(mBaseAddress + addr[int(version)]);
}

template <typename T>
inline T GameAddress(uint32_t addr)
{
  return reinterpret_cast<T>(mBaseAddress + addr);
}

void LodMod_Init();

void ShadowFixes_Init(); // ShadowFixes.cpp
void AOFixes_Init(); // AOFixes.cpp
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
  float ShadowDistancePSS;
  bool DisableManualCulling;
  float ShadowFilterStrengthBias;
  float ShadowFilterStrengthMinimum;
  float ShadowFilterStrengthMaximum;
  bool ShadowModelHQ;
  bool ShadowModelForceAll;
  int CommunicationScreenResolution;
  int HQMapSlots;
  WCHAR WrapperLoadLibrary[4096];
  float BuffersMovieMultiplier;
  bool BuffersExtendTo2021;
  int MoviesEnableH264;
  uint64_t MoviesEncryptionKey;
  bool MiscFixJapaneseEncoding;
  bool MiscTranslateEnable;
  bool MiscSkipIntroMovies;
  bool MiscSkipBootingScreens;
  bool MiscMakeIntroScreenLoadGame;
  bool MiscDisableVignette;
  bool MiscDisableFakeHDR;
};
extern LodModSettings Settings;
extern uintptr_t mBaseAddress;
extern WCHAR LogPath[4096];
extern WCHAR IniPath[4096];

// CriH264.cpp
void CriH264_Init();

// MapMod.cpp
extern bool g11420IsLoaded;
void MapMod_Init();

// proxy.cpp
extern WCHAR origModulePath[4096];
bool Proxy_Attach();
void Proxy_Detach();
void Proxy_InitSteamStub();

// Translate.cpp
void Translate_Init();

// Utility.cpp
std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string trim(const std::string& s);
std::string utf8_encode(const std::wstring& wstr);

std::string sj2utf8(const std::string& input);
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
  for (int i = 0; i < count; i++)
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
inline void SafeWriteModule(uintptr_t offset, T value)
{
  SafeWrite<T>(mBaseAddress + offset, value);
}

template <typename T>
inline void SafeWriteModule(uintptr_t offset, T value, int count)
{
  SafeWrite<T>(mBaseAddress + offset, value, count);
}

template <typename T>
inline void SafeWriteModule(uintptr_t offset, T* value, int count)
{
  SafeWrite<T>(mBaseAddress + offset, value, count);
}

// Same as SafeWrite but doesn't VirtualProtect first, more efficient if you already know the memory is writable!
template <typename T>
inline void UnsafeWriteModule(uintptr_t offset, T value)
{
  *reinterpret_cast<T*>(mBaseAddress + offset) = value;
}

template<typename T>
inline std::istream& stream_read(std::ifstream& stream, T* dest)
{
  return stream.read((char*)dest, sizeof(T));
}

template<typename T>
inline std::istream& stream_read(std::ifstream& stream, T* dest, size_t count)
{
  return stream.read((char*)dest, sizeof(T) * count);
}
