#include "pch.h"

#include <vector>
#include <cstdint>
#include "SDK.h"
#include "MinHook/MinHook.h"
#include <string>
#include <sstream>

extern uintptr_t mBaseAddress;
extern int version;
extern int NumHQMapSlots;

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
  DWORD max_count;
  DWORD unk14;
  DWORD unk18;
  DWORD unk1C;
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
  /*  8 */ {{0,-2,0,0},  {0,0,-346.410156f,0}},
  /*  9 */ {{-1,-1,0,0}, {-150,0,-259.807617f,0}},
  /* 10 */ {{-2,0,0,0},  {-300,0,-173.205078f,0}},
  /* 11 */ {{-2,1,0,0},  {-300,0,0,0}},
  /* 12 */ {{-2,2,0,0},  {-300,0,173.205078f,0}},
  /* 13 */ {{-1,2,0,0},  {-150,0,259.807617f,0}},
  /* 14 */ {{0,2,0,0},   {0,0,346.410156f,0}},

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
  for (int i = 0; i < NumHQMapSlots; i++)
  {
    auto& mapData = ctl->map_sections[i];

    ss << i << ": " << (int)mapData.LoadState << " (" << std::hex << mapData.CurrentAreaId << ":" << mapData.DesiredAreaId << std::dec << ")";
    ss << " - " << mapData.Position.value[0] << " " << mapData.Position.value[1] << " " << mapData.Position.value[2] << " " << mapData.Position.value[3];
    ss << "\r\n";
  }
  return ss.str();
}

bool g11420IsLoaded = false;

uint64_t __fastcall cHighMapController_Update(cHighMapController* a1, __int64 a2, BYTE* a3)
{
  int v3; // edx
  int v5; // ecx
  int v6; // ebx
  int v12; // ecx
  int v13; // ecx
  int* v14; // r8
  int v15; // er10
  int v16; // rbx
  int v17; // er11
  int v18; // eax
  int v19; // r9
  int v20; // rcx
  int* v21; // r15
  cMapSlot* v22; // r14
  int v23; // r12
  uint64_t result; // rax
  int v25; // edx
  int v27; // ebp
  int v28; // rdi
  int v32[MAX_LOD_SLOTS]; // [rsp+30h] [rbp-188h] ext this
  int v33[MAX_LOD_SLOTS * 2]; // [rsp+50h] [rbp-168h]
  cMapSlot v34[MAX_LOD_SLOTS]; // [rsp+90h] [rbp-128h] ext this

  typedef int(*GetAreaIdForCoords_Fn)(int, int, int);
  GetAreaIdForCoords_Fn GetAreaIdForCoords = (GetAreaIdForCoords_Fn)(mBaseAddress + 0x7C2E60);
  fn_3args AddVects = (fn_3args)(mBaseAddress + 0x265F00);
  fn_2args CopyVect4 = (fn_2args)(mBaseAddress + 0x263E80);

  v3 = *(int*)(a3 + 20);
  v5 = *(int*)(a3 + 16);
  v6 = 0;
  std::memset(v32, 0xFF, MAX_LOD_SLOTS * sizeof(int));

#ifdef _DEBUG
  auto report = BuildReport(a1);
  int playerArea = GetAreaIdForCoords(v5, v3, 1);
  playerArea = playerArea;
#endif

  g11420IsLoaded = false;

  do
  {
    v34[v6].AreaId = GetAreaIdForCoords(
      LodInfo[v6].coords.value[0] + v5,
      LodInfo[v6].coords.value[1] + v3,
      1);

    if ((v34[v6].AreaId & 0xFFFF) == 0x1420)
      g11420IsLoaded = true;

    AddVects(&v34[v6], a3, &LodInfo[v6].position);

    v34[v6].AreaPosition.value[3] = 1.f;

    if (v6 < 0 || v6 >= (signed int)a1->max_count)
      v12 = -1;
    else
      v12 = a1->map_sections[v6].DesiredAreaId;

    v33[(v6 * 2)] = v12;

    if (v6 < 0 || v6 >= (signed int)a1->max_count)
      v13 = -1;
    else
      v13 = a1->map_sections[v6].CurrentAreaId;

    v33[(v6 * 2) + 1] = v13;

    ++v6;
  } while (v6 < NumHQMapSlots);

  v14 = &v33[1];
  v15 = 0;
  v16 = 0;
  do
  {
    v17 = *(v14 - 1);
    if (v17 != -1 || *v14 != -1)
    {
      v18 = 0;
      v19 = 0;
      do
      {
        v20 = v34[v18].AreaId;
        if (v17 == v20 || *v14 == v20)
        {
          v32[v19] = v15;
          goto LABEL_25;
        }
        ++v18;
        ++v19;
      } while (v18 < NumHQMapSlots);

      if (a1->unk14 && v15 >= 0 && v15 < (signed int)a1->max_count)
      {
        a1->map_sections[v15].DesiredAreaId = -1;
        a1->unk18 = 0;
      }

      *(int64_t*)(v14 - 1) = -1i64;
    }
  LABEL_25:
    ++v15;
    v16 += 0x50;
    v14 += 2;
  } while (v15 < NumHQMapSlots);

  v21 = v32;
  v22 = v34;
  v23 = 0;
  do
  {
    result = *v21;
    if ((int)result == -1)
    {
      v27 = 0;
      v28 = 0;
      while (v33[2 * v28] != -1 && v33[2 * v28 + 1] != -1 || !a1->unk14 || v28 < 0 || v27 >= (signed int)a1->max_count)
      {
        ++v28;
        ++v27;
        if (v28 >= NumHQMapSlots)
          goto LABEL_44;
      }


      a1->map_sections[v28].DesiredAreaId = v22->AreaId;
      CopyVect4(&a1->map_sections[v28].Position, v22->AreaPosition.value);
      a1->map_sections[v28].LoadNewArea = 1;

      v33[2 * v28] = v22->AreaId;
      v33[2 * v28 + 1] = v22->AreaId;
      *v21 = v27;

      a1->unk18 = 0;
    }
    else
    {
      v25 = v22->AreaId;
      if ((v33[2 * result] != v25 || v33[2 * result + 1] != v25)
        && a1->unk14
        && (int)result >= 0
        && (int)result < (signed int)a1->max_count)
      {
        a1->map_sections[result].DesiredAreaId = v25;
        CopyVect4(&a1->map_sections[result].Position, v22->AreaPosition.value);
        a1->map_sections[result].LoadNewArea = 1;

        a1->unk18 = 0;
      }
    }

  LABEL_44:
    ++v22;
    ++v21;
    ++v23;
  } while (v23 < NumHQMapSlots);

#ifdef _DEBUG
  auto report2 = BuildReport(a1);
  report = report;
#endif

  return result;
}

uint32_t PrevNumLods = 7;

typedef void* (*MemorySystem__CreateRootHeap_Fn)(void* destHeap, uint64_t heapSize, void* a3, char* heapName);
MemorySystem__CreateRootHeap_Fn MemorySystem__CreateRootHeap_Orig;
void* MemorySystem__CreateRootHeap_Hook(void* destHeap, uint64_t heapSize, void* a3, char* heapName)
{
  if (!strcmp(heapName, "TEXTURE ROOT"))
  {
    uint32_t aHighMapVramOld = (uint32_t)(0x6400000 * PrevNumLods);
    uint32_t aHighMapVramNew = (uint32_t)(0x6400000 * NumHQMapSlots);

    heapSize -= aHighMapVramOld;
    heapSize += aHighMapVramNew;
  }
  else if (!strcmp(heapName, "FILE ROOT"))
  {
    uint32_t aHighMapFileOld = (uint32_t)(0x1600000 * PrevNumLods);
    uint32_t aHighMapFileNew = (uint32_t)(0x1600000 * NumHQMapSlots);

    heapSize -= aHighMapFileOld;
    heapSize += aHighMapFileNew;
  }

  return MemorySystem__CreateRootHeap_Orig(destHeap, heapSize, a3, heapName);
}

void LoadListSetup_Hook(BYTE* a1)
{
  for (int i = 0; i < (MAX_LOD_SLOTS * 2); i++)
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
#ifdef _DEBUG
  NumHQMapSlots = MAX_LOD_SLOTS;
#endif

  if (NumHQMapSlots == 7)
    return; // nothing to change;

  PrevNumLods = *(uint8_t*)(mBaseAddress + 0x7C72C7 + 2);

  if (NumHQMapSlots > MAX_LOD_SLOTS)
    NumHQMapSlots = MAX_LOD_SLOTS;

  // Increase memory buffers to handle the increased LOD slots...
  uint32_t aHighMapVramOld = (uint32_t)(0x6400000 * PrevNumLods);
  uint32_t aHighMapVramNew = (uint32_t)(0x6400000 * NumHQMapSlots);
  SafeWrite(mBaseAddress + 0x86BB38 + 1, aHighMapVramNew);

  uint32_t aHighMapFileOld = (uint32_t)(0x1600000 * PrevNumLods);
  uint32_t aHighMapFileNew = (uint32_t)(0x1600000 * NumHQMapSlots);
  SafeWrite(mBaseAddress + 0x86B8A1 + 1, aHighMapFileNew);

  // Hook root heap init func so we can increase buffers past 32-bits
  MH_CreateHook((LPVOID)(mBaseAddress + 0x2622B0), MemorySystem__CreateRootHeap_Hook, (LPVOID*)&MemorySystem__CreateRootHeap_Orig);

  // Set cHighMapController constructor to use our LOD slot count
  SafeWrite(mBaseAddress + 0x7C72C7 + 2, (uint8_t)NumHQMapSlots);

  // Update load-list ptr to block of unused space
  SafeWrite(mBaseAddress + 0x108734 + 3, (uint32_t)(0x114d005 + 0x5566C0));
  SafeWrite(mBaseAddress + 0x7C2DF4 + 3, (uint32_t)(0xa92945 + 0x5566C0));
  SafeWrite(mBaseAddress + 0x7C4A61 + 3, (uint32_t)(0xa90cd8 + 0x5566C0));
  SafeWrite(mBaseAddress + 0x7C8141 + 3, (uint32_t)(0xa8d5f8 + 0x5566C0));
  SafeWrite(mBaseAddress + 0x7C8548 + 3, (uint32_t)(0xa8d1f1 + 0x5566C0));
  SafeWrite(mBaseAddress + 0x7C85E8 + 3, (uint32_t)(0xa8d151 + 0x5566C0));
  SafeWrite(mBaseAddress + 0x7C86DB + 3, (uint32_t)(0xa8d05e + 0x5566C0));

  // Some code writes to an int at end of load-list, update addr/pointers of it
  uint32_t loadSlotCount = (MAX_LOD_SLOTS * 2);
  uint32_t loadSlotSize = loadSlotCount * 0xC;
  SafeWrite(mBaseAddress + 0x7C4A6C + 3, (uint32_t)(0xa90d75 + 0x5566C0 + loadSlotSize));
  SafeWrite(mBaseAddress + 0x7D2AD3 + 3, loadSlotSize);
  SafeWrite(mBaseAddress + 0x8224E3 + 2, loadSlotSize);

  // Update load-list init func to write double entry count
  SafeWrite(mBaseAddress + 0x7D2ACA + 2, (uint8_t)(loadSlotCount));
  SafeWrite(mBaseAddress + 0x8224DA + 2, (uint8_t)(loadSlotCount));

  // Patch load-list count checks
  SafeWrite(mBaseAddress + 0x8317BB + 3, (uint8_t)(loadSlotCount));
  SafeWrite(mBaseAddress + 0x8361CC + 3, (uint8_t)(loadSlotCount));
  SafeWrite(mBaseAddress + 0x83182F + 3, (uint8_t)(loadSlotCount));

  MH_CreateHook((LPVOID)(mBaseAddress + 0x830D40), LoadListSetup_Hook, NULL);
  // Uncomment this to let game set up the LOD slot coordinates
  // It does this incorrectly for the distance = 2 slots though ;_;
  //SafeWrite(mBaseAddress + 0x7C743E + 2, (uint8_t)(NUM_SLOTS_TO_UPDATE - 1));

  // Disable LowMapController
  //SafeWrite(mBaseAddress + 0x7C74C2 + 1, (uint32_t)1);
  //SafeWrite(mBaseAddress + 0x815C61 + 3, (uint8_t)0x10); // alternate
  //SafeWrite(mBaseAddress + 0x815D20 + 3, (uint8_t)0x10); // alternate

  // Hook cHighMapController::Update func to use our reimplementation instead
  // (reimplemented ver allows variable number of slots, and can read from NewSlots instead)
  MH_CreateHook((LPVOID)(mBaseAddress + 0x815420), cHighMapController_Update, NULL);
}
