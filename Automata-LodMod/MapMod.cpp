#include "pch.h"

#include <vector>
#include <cstdint>
#include "SDK.h"
#include "MinHook/MinHook.h"
#include <string>
#include <sstream>

const uint32_t MovieFileHeapSize_Addr[] = { 0x86BA27 + 1, 0x8632C7 + 1, 0x8927E7 + 1, 0x644852 + 2, 0xA0F86A + 1 };
const uint32_t MovieVramHeapSize_Addr[] = { 0x86BC8F + 1, 0x86352F + 1, 0x892A4F + 1, 0x644B85 + 2, 0xA0FAD2 + 1 };
const uint32_t cMemoryDeviceBase__createHeap_Addr[] = { 0x2622B0, 0x25B3B0, 0x2452D0, 0x91FCC0, 0xE1BA10 };

#pragma pack(push, 1)
struct cLodSlot
{
  cVec4<int> coords;
  cVec4<float> position;
};
static_assert(sizeof(cLodSlot) == 0x20);

struct cMapSlot
{
  cVec4<float> AreaPosition;
  int unk10;
  int AreaId;
  int unk18;
  int unk1C;
};
static_assert(sizeof(cMapSlot) == 0x20);

// from NA debug
enum class cMapLoadState : uint32_t
{
  Standby = 0,
  WaitReadData = 1,
  CreateInstance = 2,
  Stable = 3,
  InstanceRelease = 4,
  StartDataRelease = 5,
  WaitDataReleased = 6
};

struct cMapModelData
{
  /* 0x00 */ BYTE Unk0[0x48];

  // shift right by 10 for 7 digit num
  /* 0x48 */ uint64_t dat;
  /* 0x50 */ uint64_t dtt;
  /* 0x58 */ uint64_t texture;
  /* 0x60 */ uint64_t model;
};
struct cMapSection
{
  /* 0x00 */ cMapModelData* Unk0;
  /* 0x08 */ int32_t CurrentAreaId;
  /* 0x0C */ int32_t DesiredAreaId;
  /* 0x10 */ cVec4<float> Position;
  /* 0x20 */ uint32_t LoadNewArea; // bool?
  /* 0x24 */ uint32_t Unk24;
  /* 0x28 */ BYTE* Unk28;
  /* 0x30 */ BYTE* Unk30;
  /* 0x38 */ cMapLoadState LoadState;
  /* 0x3C */ uint32_t Unk3C;

  // haven't seen these set:
  /* 0x40 */ uint32_t Unk40[4];
};
static_assert(sizeof(cMapSection) == 0x50);

struct cHighMapController
{
  BYTE* vfTable;
  cMapSection* map_sections;
  int max_count;
  int is_active;
  int unk18;
  int unk1C;
  BYTE unk20[96];
  //cLodSlot slots[7];
};

#pragma pack(pop)

#define MAX_LOD_SLOTS 19

// Game stores areas in a flat hexagonal grid using axial coordinates
// https://www.redblobgames.com/grids/hexagons/#conversions-axial for more info
cLodSlot LodInfo[22] =
{
  // Delta coordinates & positions for those coords
  // Calculated by game at 0x7C7433, that only affects the old cLodSlots with max count of 7 though
  // Just copied the results after running that code into here
  /*  0 */ {{0,0,0,0}, {0,0,0,1}},

  /*  1 */ {{0,-1,0,0}, {0,0,-173.205078f,0}},
  /*  2 */ {{-1,0,0,0}, {-150,0,-86.602539f,0}},
  /*  3 */ {{-1,1,0,0}, {-150,0,86.602539f,0}},
  /*  4 */ {{0,1,0,0}, {0,0,173.205078f,0}},
  /*  5 */ {{1,0,0,0}, {150,0,86.602539f,0}},
  /*  6 */ {{1,-1,0,0}, {150,0,-86.602539f,0}},

  // New slots
  // Games position-calculation func doesn't work properly for distance = 2, so had to find these manually...
  /*  7 */ {{1,-2,0,0}, {150,0,-259.807617f,0}},
  /* 14 */ {{0,2,0,0},   {0,0,346.410156f,0}}, // swapped 14 & 8

  /*  9 */ {{-1,-1,0,0}, {-150,0,-259.807617f,0}},
  /* 10 */ {{-2,0,0,0},  {-300,0,-173.205078f,0}},
  /* 11 */ {{-2,1,0,0},  {-300,0,0,0}},
  /* 12 */ {{-2,2,0,0},  {-300,0,173.205078f,0}},
  /* 13 */ {{-1,2,0,0},  {-150,0,259.807617f,0}},
  /*  8 */ {{0,-2,0,0},  {0,0,-346.410156f,0}},

  // Seems to be another limit for 14 slots in the LowMapController stuff & other related crap, wew
  // If enabled these will break something and cause low-LOD to be stuck loaded with no way to load the HQ ver...

  /* 15 */ {{1,1,0,0},   {150,0,259.807617f,0}},
  /* 16 */ {{2,0,0,0},   {300,0,173.205078f,0}},
  /* 17 */ {{2,-1,0,0},  {300,0,0,0}},
  /* 18 */ {{2,-2,0,0},  {300,0,-173.205078f,0}},
};

std::string BuildReport(cHighMapController* ctl)
{
  std::stringstream ss;
  for (int i = 0; i < Settings.HQMapSlots; i++)
  {
    auto& mapData = ctl->map_sections[i];

    ss << i << ": " << (int)mapData.LoadState << " (" << std::hex << mapData.CurrentAreaId << ":" << mapData.DesiredAreaId << std::dec << ")";
    ss << " - " << mapData.Position.value[0] << " " << mapData.Position.value[1] << " " << mapData.Position.value[2] << " " << mapData.Position.value[3];
    ss << "\r\n";
  }
  return ss.str();
}

bool g11420IsLoaded = false;

void cHighMapController__UpdateSlots_Hook(cHighMapController* a1, __int64 a2, BYTE* a3)
{
  typedef int(*GetAreaIdForCoords_Fn)(int, int, int);
  GetAreaIdForCoords_Fn GetAreaIdForCoords = GameAddress<GetAreaIdForCoords_Fn>(0x7C2E60);
  fn_3args Vect4Add = GameAddress<fn_3args>(0x265F00);
  fn_2args Vect4Copy = GameAddress<fn_2args>(0x263E80);

  int prevSlotForSection[MAX_LOD_SLOTS];
  int curSlotAreaIds[MAX_LOD_SLOTS * 2];
  cMapSlot mapSections[MAX_LOD_SLOTS];

  std::memset(prevSlotForSection, 0xFF, MAX_LOD_SLOTS * sizeof(int));

  int x_pos = *(int*)(a3 + 16);
  int y_pos = *(int*)(a3 + 20);

#ifdef _DEBUG
  auto report = BuildReport(a1);
  int playerArea = GetAreaIdForCoords(x_pos, y_pos, 1);
  playerArea = playerArea;
#endif

  g11420IsLoaded = false;

  int i = 0;
  do
  {
    mapSections[i].AreaId = GetAreaIdForCoords(
      LodInfo[i].coords.value[0] + x_pos,
      LodInfo[i].coords.value[1] + y_pos,
      1);

    if ((mapSections[i].AreaId & 0xFFFF) == 0x1420)
      g11420IsLoaded = true;

    Vect4Add(&mapSections[i], a3, &LodInfo[i].position);

    mapSections[i].AreaPosition.value[3] = 1.f;

    if (i < 0 || i >= a1->max_count)
    {
      curSlotAreaIds[(i * 2)] = -1;
      curSlotAreaIds[(i * 2) + 1] = -1;
    }
    else
    {
      curSlotAreaIds[(i * 2)] = a1->map_sections[i].DesiredAreaId;
      curSlotAreaIds[(i * 2) + 1] = a1->map_sections[i].CurrentAreaId;
    }

    ++i;
  } while (i < Settings.HQMapSlots);

  i = 0;
  do
  {
    if (curSlotAreaIds[(i * 2)] != -1 || curSlotAreaIds[(i * 2) + 1] != -1)
    {
      int j = 0;
      do
      {
        int area_id = mapSections[j].AreaId;
        if (curSlotAreaIds[(i * 2)] == area_id || curSlotAreaIds[(i * 2) + 1] == area_id)
        {
          prevSlotForSection[j] = i;
          goto LABEL_25;
        }
        ++j;
      } while (j < Settings.HQMapSlots);

      if (a1->is_active && i >= 0 && i < a1->max_count)
      {
        a1->map_sections[i].DesiredAreaId = -1;
        a1->unk18 = 0;
      }

      curSlotAreaIds[(i * 2)] = -1;
      curSlotAreaIds[(i * 2) + 1] = -1;
    }
  LABEL_25:
    ++i;
  } while (i < Settings.HQMapSlots);

  i = 0;
  do
  {
    int prevSlot = prevSlotForSection[i];
    if (prevSlot == -1)
    {
      int j = 0;
      while (curSlotAreaIds[j * 2] != -1 && curSlotAreaIds[(j * 2) + 1] != -1 || !a1->is_active || j < 0 || j >= a1->max_count)
      {
        ++j;
        if (j >= Settings.HQMapSlots)
          goto LABEL_44;
      }

      a1->map_sections[j].DesiredAreaId = mapSections[i].AreaId;
      Vect4Copy(&a1->map_sections[j].Position, mapSections[i].AreaPosition.value);
      a1->map_sections[j].LoadNewArea = 1;

      curSlotAreaIds[j * 2] = mapSections[i].AreaId;
      curSlotAreaIds[(j * 2) + 1] = mapSections[i].AreaId;
      prevSlotForSection[i] = j;

      a1->unk18 = 0;
    }
    else
    {
      int area_id = mapSections[i].AreaId;
      if ((curSlotAreaIds[2 * prevSlot] != area_id || curSlotAreaIds[2 * prevSlot + 1] != area_id)
        && a1->is_active
        && prevSlot >= 0
        && prevSlot < a1->max_count)
      {
        a1->map_sections[prevSlot].DesiredAreaId = area_id;
        Vect4Copy(&a1->map_sections[prevSlot].Position, mapSections[i].AreaPosition.value);
        a1->map_sections[prevSlot].LoadNewArea = 1;

        a1->unk18 = 0;
      }
    }

  LABEL_44:
    ++i;
  } while (i < Settings.HQMapSlots);

#ifdef _DEBUG
  auto report2 = BuildReport(a1);
  report = report;
#endif
}

uint32_t PrevNumLods = 7;
uint32_t NewNumLods = 0;

uint32_t aMovieFileOld = 0;
uint32_t aMovieVramOld = 0;

// 4k is 4x 1080p, so quad the movie buffer sizes
const uint32_t aMovieFileNew = 0x3FC0000 * 4; 
const uint32_t aMovieVramNew = 0x1E00000 * 4;

void UpdateRootHeap(char* heapName, uint64_t* heapSize)
{
  if (!strcmp(heapName, "TEXTURE ROOT"))
  {
    if (aMovieVramOld)
    {
      *heapSize -= aMovieVramOld;
      *heapSize += aMovieVramNew;
    }

    if (NewNumLods)
    {
      uint32_t aHighMapVramOld = uint32_t(0x6400000 * PrevNumLods);
      uint32_t aHighMapVramNew = uint32_t(0x6400000 * NewNumLods);

      *heapSize -= aHighMapVramOld;
      *heapSize += aHighMapVramNew;
    }
  }
  else if (!strcmp(heapName, "FILE ROOT"))
  {
    if (aMovieFileOld)
    {
      *heapSize -= aMovieFileOld;
      *heapSize += aMovieFileNew;
    }

    if (NewNumLods)
    {
      uint32_t aHighMapFileOld = uint32_t(0x1600000 * PrevNumLods);
      uint32_t aHighMapFileNew = uint32_t(0x1600000 * NewNumLods);

      *heapSize -= aHighMapFileOld;
      *heapSize += aHighMapFileNew;
    }
  }
}

typedef void* (*cMemoryDeviceBase__createHeap_2017_Fn)(void* a1, void* destHeap, uint64_t heapSize, void* a3, char* heapName);
cMemoryDeviceBase__createHeap_2017_Fn cMemoryDeviceBase__createHeap_2017_Orig;
void* cMemoryDeviceBase__createHeap_2017_Hook(void* a1, void* destHeap, uint64_t heapSize, void* a3, char* heapName)
{
  UpdateRootHeap(heapName, &heapSize);
  return cMemoryDeviceBase__createHeap_2017_Orig(a1, destHeap, heapSize, a3, heapName);
}

typedef void* (*cMemoryDeviceBase__createHeap_Fn)(void* destHeap, uint64_t heapSize, void* a3, char* heapName);
cMemoryDeviceBase__createHeap_Fn cMemoryDeviceBase__createHeap_Orig;
void* cMemoryDeviceBase__createHeap_Hook(void* destHeap, uint64_t heapSize, void* a3, char* heapName)
{
  UpdateRootHeap(heapName, &heapSize);
  return cMemoryDeviceBase__createHeap_Orig(destHeap, heapSize, a3, heapName);
}

void LoadListSetup_Hook(BYTE* a1)
{
  for (uint64_t i = 0; i < (MAX_LOD_SLOTS * 2); i++)
  {
    if (*(DWORD*)(a1 + (i*0xC)) != 0xFFFFFFFF)
    {
      if (*(DWORD*)(a1 + (i * 0xC) + 8) || !*(DWORD*)(a1 + ((MAX_LOD_SLOTS * 2) * 0xC)))
      {
        auto v2 = *(float*)(a1 + (i * 0xC) + 4) - 0.05f;
        *(float*)(a1 + (i * 0xC) + 4) = v2;
        if (v2 <= 0.0)
          *(DWORD*)(a1 + (i * 0xC)) = 0xFFFFFFFF;
      }
      else
      {
        auto v1 = *(float*)(a1 + (i * 0xC) + 4) + 0.05f;
        *(float*)(a1 + (i * 0xC) + 4) = v1;
        if (v1 >= 1.0)
          *(float*)(a1 + (i * 0xC) + 4) = 1.f;
      }
    }
  }
}

void MapMod_Init()
{
  if (Settings.HQMapSlots > MAX_LOD_SLOTS)
    Settings.HQMapSlots = MAX_LOD_SLOTS;

  // Hook root heap init func so we can increase buffers past 32-bits
  if(version == GameVersion::Steam2017 || version == GameVersion::Debug2017)
    MH_CreateHook(GameAddress<LPVOID>(cMemoryDeviceBase__createHeap_Addr), cMemoryDeviceBase__createHeap_2017_Hook, (LPVOID*)&cMemoryDeviceBase__createHeap_2017_Orig);
  else
    MH_CreateHook(GameAddress<LPVOID>(cMemoryDeviceBase__createHeap_Addr), cMemoryDeviceBase__createHeap_Hook, (LPVOID*)&cMemoryDeviceBase__createHeap_Orig);

  // Expand MovieFile/MovieVram buffers, the root heaps for them will be updated in hook above
  // (allows videos above 1080 to play reliably)
  uint32_t size_MovieFile = *GameAddress<uint32_t*>(MovieFileHeapSize_Addr);
  if (aMovieFileNew > size_MovieFile)
  {
    aMovieFileOld = size_MovieFile;
    // new size is larger, extend child & parent heaps...
    SafeWriteModule(0x86BA27 + 1, aMovieFileNew);
  }
  uint32_t size_MovieVram = *GameAddress<uint32_t*>(MovieVramHeapSize_Addr);
  if (aMovieVramNew > size_MovieVram)
  {
    aMovieVramOld = size_MovieVram;
    // new size is larger, extend child & parent heaps...
    SafeWriteModule(0x86BC8F + 1, aMovieVramNew);
  }

  if (version == GameVersion::Steam2017)
  {
    // Expand buffers in 2017 to match 2021, allows using 2021 data files with 2017 EXE
    // (recommend grabbing shader.dat from 2017 data000.cpk & ui folder from 2017 data009.cpk, else you'll encounter some weirdness!)
    // (extract shader.dat into games data\ folder, and extract ui folder to data\ui\)
    // (note that CriPakTools doesn't fully extract data009.cpk, use cpk.bms instead)
    const uint32_t new_GraphicWork = 0x6400000;
    const uint32_t new_ShaderFile = 0x500000;
    const uint32_t new_UIVram = 0x6000000;

    uint32_t size_WorkRoot = *GameAddress<uint32_t*>(0x6441F3 + 2);
    uint32_t size_WorkRoot_GraphicWork = *GameAddress<uint32_t*>(0x64430C + 2);
    if (new_GraphicWork > size_WorkRoot_GraphicWork)
    {
      auto tweak_GraphicWork = new_GraphicWork - size_WorkRoot_GraphicWork;
      size_WorkRoot += tweak_GraphicWork;
      size_WorkRoot_GraphicWork += tweak_GraphicWork;

      SafeWriteModule(0x6441F3 + 2, size_WorkRoot);
      SafeWriteModule(0x64430C + 2, size_WorkRoot_GraphicWork);
    }

    uint32_t size_FileRoot = *GameAddress<uint32_t*>(0x64427D + 2);
    uint32_t size_FileRoot_ShaderFile = *GameAddress<uint32_t*>(0x644786 + 2);
    if (new_ShaderFile > size_FileRoot_ShaderFile)
    {
      auto tweak_ShaderFile = new_ShaderFile - size_FileRoot_ShaderFile;
      size_FileRoot += tweak_ShaderFile;
      size_FileRoot_ShaderFile += tweak_ShaderFile;
      SafeWriteModule(0x64427D + 2, size_FileRoot);
      SafeWriteModule(0x644786 + 2, size_FileRoot_ShaderFile);
    }

    uint32_t size_TextureRoot = *GameAddress<uint32_t*>(0x6442A5 + 2);
    uint32_t size_TextureRoot_UIVram = *GameAddress<uint32_t*>(0x644A8A + 2);
    if (new_UIVram > size_TextureRoot_UIVram)
    {
      auto tweak_UIVram = new_UIVram - size_TextureRoot_UIVram;
      size_TextureRoot += tweak_UIVram;
      size_TextureRoot_UIVram += tweak_UIVram;
      SafeWriteModule(0x6442A5 + 2, size_TextureRoot);
      SafeWriteModule(0x644A8A + 2, size_TextureRoot_UIVram);
    }
  }

  if (version != GameVersion::Win10)
    return;

  PrevNumLods = *GameAddress<uint8_t*>(0x7C72C7 + 2);
  NewNumLods = Settings.HQMapSlots;

  // Increase memory buffers to handle the increased LOD slots...
  uint32_t aHighMapVramOld = uint32_t(0x6400000 * PrevNumLods);
  uint32_t aHighMapVramNew = uint32_t(0x6400000 * Settings.HQMapSlots);
  SafeWriteModule(0x86BB38 + 1, aHighMapVramNew);

  uint32_t aHighMapFileOld = uint32_t(0x1600000 * PrevNumLods);
  uint32_t aHighMapFileNew = uint32_t(0x1600000 * Settings.HQMapSlots);
  SafeWriteModule(0x86B8A1 + 1, aHighMapFileNew);

  // Set cHighMapController constructor to use our LOD slot count
  SafeWriteModule(0x7C72C7 + 2, uint8_t(Settings.HQMapSlots));

  // Hook cHighMapController::UpdateSlots func to use our reimplementation instead
  // (reimplemented ver allows variable number of slots, and can read from NewSlots instead)
  MH_CreateHook(GameAddress<LPVOID>(0x815420), cHighMapController__UpdateSlots_Hook, NULL);

  // Update load-list ptr to block of unused space
  SafeWriteModule(0x108734 + 3, uint32_t(0x114d005 + 0x5566C0));
  SafeWriteModule(0x7C2DF4 + 3, uint32_t(0xa92945 + 0x5566C0));
  SafeWriteModule(0x7C4A61 + 3, uint32_t(0xa90cd8 + 0x5566C0));
  SafeWriteModule(0x7C8141 + 3, uint32_t(0xa8d5f8 + 0x5566C0));
  SafeWriteModule(0x7C8548 + 3, uint32_t(0xa8d1f1 + 0x5566C0));
  SafeWriteModule(0x7C85E8 + 3, uint32_t(0xa8d151 + 0x5566C0));
  SafeWriteModule(0x7C86DB + 3, uint32_t(0xa8d05e + 0x5566C0));

  // Some code writes to an int at end of load-list, update addr/pointers of it
  uint32_t loadSlotCount = (MAX_LOD_SLOTS * 2);
  uint32_t loadSlotSize = loadSlotCount * 0xC;
  SafeWriteModule(0x7C4A6C + 3, uint32_t(0xa90d75 + 0x5566C0 + loadSlotSize));
  SafeWriteModule(0x7D2AD3 + 3, loadSlotSize);
  SafeWriteModule(0x8224E3 + 2, loadSlotSize);

  // Update load-list init func to write double entry count
  SafeWriteModule(0x7D2ACA + 2, uint8_t(loadSlotCount));
  SafeWriteModule(0x8224DA + 2, uint8_t(loadSlotCount));

  // Patch load-list count checks
  SafeWriteModule(0x8317BB + 3, uint8_t(loadSlotCount));
  SafeWriteModule(0x8361CC + 3, uint8_t(loadSlotCount));
  SafeWriteModule(0x83182F + 3, uint8_t(loadSlotCount));

  MH_CreateHook(GameAddress<LPVOID>(0x830D40), LoadListSetup_Hook, NULL);

  // Uncomment this to let game set up the LOD slot coordinates
  // It does this incorrectly for the distance = 2 slots though ;_;
  //SafeWriteModule(0x7C743E + 2, uint8_t(NUM_SLOTS_TO_UPDATE - 1));

  // Disable LowMapController
  //SafeWriteModule(0x7C74C2 + 1, uint32_t(1));
  //SafeWriteModule(0x815C61 + 3, uint8_t(0x10)); // alternate
  //SafeWriteModule(0x815D20 + 3, uint8_t(0x10)); // alternate
}
