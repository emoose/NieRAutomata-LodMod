#pragma once

namespace Lang
{
  constexpr uint32_t HQ_GetAreaIdForCoords = 0x7C3260; // was 0x7C2E60 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListCheck1 = 0x831BDE; // was 0x8317BE in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListCheck2 = 0x8365EF; // was 0x8361CF in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListCheck3 = 0x831C52; // was 0x831832 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListCount1 = 0x7D2ECC; // was 0x7D2ACC in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListCount2 = 0x8228FC; // was 0x8224DC in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListDisp1 = 0x108737; // was 0x108737 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListDisp2 = 0x7C31F7; // was 0x7C2DF7 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListDisp3 = 0x7C4E64; // was 0x7C4A64 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListDisp4 = 0x7C8544; // was 0x7C8144 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListDisp5 = 0x7C894B; // was 0x7C854B in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListDisp6 = 0x7C89EB; // was 0x7C85EB in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListDisp7 = 0x7C8ADE; // was 0x7C86DE in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListEndIntDisp = 0x7C4E6F; // was 0x7C4A6F in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListSetup = 0x831160; // was 0x830D40 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListSize1 = 0x7D2ED6; // was 0x7D2AD6 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_LoadListSize2 = 0x822905; // was 0x8224E5 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_MapFileSize = 0x86BCC2; // was 0x86B8A2 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_MapVramSize = 0x86BF59; // was 0x86BB39 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_NumLods = 0x7C76C9; // was 0x7C72C9 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_UpdateSlots = 0x815840; // was 0x815420 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_Vect4Add = 0x266000; // was 0x265F00 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t HQ_Vect4Copy = 0x263F80; // was 0x263E80 in the original Win10 build (MapMod.cpp)
  constexpr uint32_t FakeHDR = 0x25E879; // was 0x25E779 in the original Win10 build (dllmain.cpp)
  constexpr uint32_t Intro_LoadMode = 0x990E4F; // was 0x99076F in the original Win10 build (dllmain.cpp)
  constexpr uint32_t Intro_MenuState = 0x990FDB; // was 0x9908FB in the original Win10 build (dllmain.cpp)
  constexpr uint32_t Intro_SkipCheck = 0x990FD6; // was 0x9908F6 in the original Win10 build (dllmain.cpp)
  constexpr uint32_t Intro_WaitState = 0x990E5F; // was 0x99077F in the original Win10 build (dllmain.cpp)
}

// RVA to use for a Win10-only patch site: the original offset, or the matching one in the Win10 Lang build
inline uint32_t RVA_For(uint32_t win10, uint32_t lang)
{
  return version == GameVersion::Win10Lang ? lang : win10;
}
