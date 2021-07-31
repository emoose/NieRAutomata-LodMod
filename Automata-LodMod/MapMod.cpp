#include "pch.h"

#include <vector>
#include <cstdint>
#include "SDK.h"
#include "MinHook/MinHook.h"

extern uintptr_t mBaseAddress;
extern int version;
extern int NumHQMapSlots;

#pragma pack(push, 1)

struct cLodSlot
{
  cVec4<float> unk0;
  cVec4<float> unk10;
};

struct cHighMapController
{
  BYTE* vfTable;
  BYTE* unk8;
  DWORD max_count;
  DWORD unk14;
  DWORD unk18;
  DWORD unk1C;
  BYTE unk20[96];
  //cLodSlot slots[7];
};

#pragma pack(pop)

typedef DWORD(*sub_14000F196_Fn)(DWORD, DWORD, DWORD);

#define MAX_LOD_SLOTS 17

// this LOD slot stuff is a quick hacky job, badly needs to be cleaned up
cLodSlot* newSlots;

void add_slot(int i, int x, int y)
{
  *(int*)&newSlots[i + 0].unk0.value[0] = x;
  *(int*)&newSlots[i + 0].unk0.value[1] = y;
  newSlots[i + 0].unk10.value[0] = (float)x * 150.f;
  newSlots[i + 0].unk10.value[2] = (float)y * 86.60253143f;
  i++;
}

void add_slot(int i, int x, int y, float xpos, float ypos)
{
  *(int*)&newSlots[i].unk0.value[0] = x;
  *(int*)&newSlots[i].unk0.value[1] = y;
  newSlots[i].unk10.value[0] = xpos;
  newSlots[i].unk10.value[2] = ypos;
}

void InitLodSlots()
{

  newSlots = (cLodSlot*)(mBaseAddress + 0x17ABE00);

  // Game would normally init the cHighMapController instance slots with writes to hardcoded addrs
  // Since we moved the slots to 0x17ABE00, we could either patch all those writes, or just handle the init ourselves:
  int i = 0;
  *(int*)&newSlots[i + 0].unk0.value[0] = 0;
  *(int*)&newSlots[i++].unk0.value[1] = 0;

  *(int*)&newSlots[i + 0].unk0.value[0] = 0;
  *(int*)&newSlots[i++].unk0.value[1] = -1;

  *(int*)&newSlots[i + 0].unk0.value[0] = -1;
  *(int*)&newSlots[i++].unk0.value[1] = 0;

  *(int*)&newSlots[i + 0].unk0.value[0] = -1;
  *(int*)&newSlots[i++].unk0.value[1] = 1;

  *(int*)&newSlots[i + 0].unk0.value[0] = 0;
  *(int*)&newSlots[i++].unk0.value[1] = 1;

  *(int*)&newSlots[i + 0].unk0.value[0] = 1;
  *(int*)&newSlots[i++].unk0.value[1] = 0;

  *(int*)&newSlots[i + 0].unk0.value[0] = 1;
  *(int*)&newSlots[i++].unk0.value[1] = -1;

  // Setup LOD slot coordinates
  // Code at 0x7C7433 should calculate these coordinates for these
  // Sadly that only works properly for distance = 1 though... had to manually find these coords out myself
  if (true) {
    /*   8 */ add_slot(i++, 0, -2, 0, -346.4101562f);
    /*   9 */ add_slot(i++, 0, 2, 0, 346.4101562f);
    /*  10 */ add_slot(i++, -2, 0, -300, -173.2050781f);
    /*  11 */ add_slot(i++, 2, 0, 300, 173.2050781f);
    /*  12 */ add_slot(i++, -1, -1, -150, -259.8076172f);
    /*  13 */ add_slot(i++, 1, 1, 150, 259.8076172f);
    /*  14 */ add_slot(i++, -2, 2, -300, 173.2050781f);
    /*  15 */ add_slot(i++, 2, -2, 300, -173.2050781f);
    /*  16 */ add_slot(i++, 2, 2, 300, 519.6152344f);
    /*  17 */ add_slot(i++, -2, -2, -346.4101562f, -346.4101562f);
  }

  // TODO:
  // the other 8 level2 slots  ;_;

  // nasty hooking method means we clobbed xmm registers, using this should set them back to 0 like the hooked code block needs:
  add_slot(0, 0, 0, 0, 0);
}

uint64_t __fastcall cHighMapController_Update(cHighMapController* a1, __int64 a2, BYTE* a3)
{
  int v3; // edx
  int v5; // ecx
  int v6; // ebx
  __int64 v7; // rdi
  int v12; // ecx
  int v13; // ecx
  DWORD* v14; // r8
  int v15; // er10
  __int64 v16; // rbx
  int v17; // er11
  int v18; // eax
  __int64 v19; // r9
  DWORD* v20; // rcx
  DWORD* v21; // r15
  cLodSlot* v22; // r14
  __int64 v23; // r12
  uint64_t result; // rax
  DWORD v25; // edx
  BYTE* v26; // rbx
  int v27; // ebp
  __int64 v28; // rdi
  BYTE* v29; // rbx
  DWORD v32[MAX_LOD_SLOTS]; // [rsp+30h] [rbp-188h] ext this
  DWORD v33[MAX_LOD_SLOTS * 2]; // [rsp+50h] [rbp-168h]
  cLodSlot v34[MAX_LOD_SLOTS]; // [rsp+90h] [rbp-128h] ext this

  sub_14000F196_Fn sub_14000F196 = (sub_14000F196_Fn)(mBaseAddress + 0x7C2E60);
  fn_3args sub_140E32CC0 = (fn_3args)(mBaseAddress + 0x265F00);
  fn_3args sub_140E32D20 = (fn_3args)(mBaseAddress + 0x263E80);

  cLodSlot* newSlots = (cLodSlot*)(mBaseAddress + 0x17ABE00);
  v3 = *(DWORD*)(a3 + 20);
  v5 = *(DWORD*)(a3 + 16);
  v6 = 0;
  v7 = 0i64;
  std::memset(v32, 0xFF, MAX_LOD_SLOTS * sizeof(DWORD));
  do
  {
    *(DWORD*)&v34[v6].unk10.value[1] = sub_14000F196(
      *(int*)&newSlots[v6].unk0.value[0] + v5,
      *(int*)&newSlots[v6].unk0.value[1] + v3,
      1i64);

    sub_140E32CC0(&v34[v6], a3, &newSlots[v6].unk10);

    v34[v6].unk0.value[3] = 1;

    if (v6 < 0 || v6 >= (signed int)a1->max_count)
      v12 = -1;
    else
      v12 = *(DWORD*)(a1->unk8 + (v7 + 12));

    v33[(v6 * 2)] = v12;

    if (v6 < 0 || v6 >= (signed int)a1->max_count)
      v13 = -1;
    else
      v13 = *(DWORD*)(a1->unk8 + (v7 + 8));

    v33[(v6 * 2) + 1] = v12;

    ++v6;
    v7 += 0x50i64;
  } while (v6 < NumHQMapSlots);

  v14 = &v33[1];
  v15 = 0;
  v16 = 0i64;
  do
  {
    v17 = *(v14 - 1);
    if (v17 != -1 || *v14 != -1)
    {
      v18 = 0;
      v19 = 0i64;
      v20 = (DWORD*)&v34[0].unk10.value[1];
      do
      {
        if (v17 == *v20 || *v14 == *v20)
        {
          v32[v19] = v15;
          goto LABEL_25;
        }
        ++v18;
        ++v19;
        v20 += 8;
      } while (v18 < NumHQMapSlots);

      if (a1->unk14 && v15 >= 0 && v15 < (signed int)a1->max_count)
      {
        *(DWORD*)&a1->unk8[v16 + 12] = -1;
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
  v23 = 0i64;
  do
  {
    result = *v21;
    if ((int)result == -1)
    {
      v27 = 0;
      v28 = 0i64;
      while (v33[2 * v28] != -1 && v33[2 * v28 + 1] != -1 || !a1->unk14 || v28 < 0 || v27 >= (signed int)a1->max_count)
      {
        ++v28;
        ++v27;
        if (v28 >= NumHQMapSlots)
          goto LABEL_44;
      }

      v29 = (BYTE*)&a1->unk8[0x50 * v28];
      *(float*)(v29 + 0xC) = v22->unk10.value[1];
      sub_140E32D20(v29 + 0x10, v22, v14);
      *(DWORD*)(v29 + 0x20) = 1;

      DWORD result2 = *(DWORD*)&v22->unk10.value[1];
      v33[2 * v28] = result2;
      v33[2 * v28 + 1] = result2;
      *v21 = v27;

      a1->unk18 = 0;
    }
    else
    {
      v25 = *(DWORD*)&v22->unk10.value[1];
      if ((v33[2 * result] != v25 || v33[2 * result + 1] != v25)
        && a1->unk14
        && (int)result >= 0
        && (int)result < (signed int)a1->max_count)
      {
        v26 = (BYTE*)&a1->unk8[0x50 * result];
        *(DWORD*)(v26 + 0xC) = v25;
        result = (uint64_t)sub_140E32D20(v26 + 0x10, v22, v14);
        *(DWORD*)(v26 + 0x20) = 1;
        a1->unk18 = 0;
      }
    }

  LABEL_44:
    ++v22;
    ++v21;
    ++v23;
  } while (v23 < NumHQMapSlots);

  return result;
}

void MapMod_Init()
{
#ifdef _DEBUG
  NumHQMapSlots = MAX_LOD_SLOTS;
#endif

  uint8_t trampoline[] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xD0 };
  uint8_t setRcx[] = { 0x48, 0xB9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x90 };

  uint32_t NewSlotsAddr = 0x17ABE00;

  cLodSlot* newSlots = (cLodSlot*)(mBaseAddress + NewSlotsAddr);

  *(uintptr_t*)&trampoline[2] = (uintptr_t)&InitLodSlots;
  *(uintptr_t*)&setRcx[2] = (uintptr_t)&newSlots[0].unk10;

  SafeWrite(mBaseAddress + 0x7C7378, trampoline, 12);
  SafeWrite(mBaseAddress + 0x7C7378 + 0xC, setRcx, 11);

  const uint32_t oldNumLods = *(uint8_t*)(mBaseAddress + 0x7C72C7 + 2);

  if (NumHQMapSlots > MAX_LOD_SLOTS)
    NumHQMapSlots = MAX_LOD_SLOTS;

  // Increase memory buffers to handle the increased LOD slots...
  uint32_t aHighMapVramOld = (uint32_t)(0x6400000 * oldNumLods);
  uint32_t aHighMapVramNew = (uint32_t)(0x6400000 * NumHQMapSlots);
  SafeWrite(mBaseAddress + 0x86BB38 + 1, aHighMapVramNew);

  uint32_t aTextureRootOld = *(uint32_t*)(mBaseAddress + 0x86B60B + 1);
  uint32_t aTextureRootNew = (aTextureRootOld - aHighMapVramOld) + aHighMapVramNew;
  if (aTextureRootOld > aTextureRootNew)
    aTextureRootNew = 0xFFFFFFFF; // todo: need to rewrite rdx...
  SafeWrite(mBaseAddress + 0x86B60B + 1, aTextureRootNew);

  uint32_t aHighMapFileOld = (uint32_t)(0x1600000 * oldNumLods);
  uint32_t aHighMapFileNew = (uint32_t)(0x1600000 * NumHQMapSlots);
  SafeWrite(mBaseAddress + 0x86B8A1 + 1, aHighMapFileNew);

  uint32_t aFileRootOld = *(uint32_t*)(mBaseAddress + 0x86B5E5 + 1);
  uint32_t aFileRootNew = (aFileRootOld - aHighMapFileOld) + aHighMapFileNew;
  if (aFileRootOld > aFileRootNew)
    aFileRootNew = 0xFFFFFFFF; // todo: need to rewrite rdx...
  SafeWrite(mBaseAddress + 0x86B5E5 + 1, aFileRootNew);

  // Set cHighMapController constructor to use our LOD slot count
  SafeWrite(mBaseAddress + 0x7C72C7 + 2, (uint8_t)NumHQMapSlots);

  // Uncomment this to let game set up the LOD slot coordinates
  // It does this incorrectly for the distance = 2 slots though ;_;
  //SafeWrite(mBaseAddress + 0x7C743E + 2, (uint8_t)(NUM_SLOTS_TO_UPDATE - 1));

  // Rewrite LodSlots accesses to use newSlots instead
  // (we have pretty much unlimited space there afaik)
  uint32_t BufferRedirect1Addr = 0x7C7352;
  uint32_t BufferRedirect1Size = 0x7;
  // Redirect to NewSlots+0x10
  SafeWrite(mBaseAddress + 0x7C7352 + 3, ((NewSlotsAddr + 0x10) - (BufferRedirect1Addr + BufferRedirect1Size)));

  uint32_t BufferRedirect2Addr = 0x7C7410;
  uint32_t BufferRedirect2Size = 0x7;
  // Redirect to NewSlots+0x30
  SafeWrite(mBaseAddress + 0x7C7410 + 3, ((NewSlotsAddr + 0x30) - (BufferRedirect2Addr + BufferRedirect2Size)));

  // Hook cHighMapController::Update func to use our reimplementation instead
  // (reimplemented ver allows variable number of slots, and can read from NewSlots instead)
  MH_CreateHook((LPVOID)(mBaseAddress + 0x815420), cHighMapController_Update, NULL);
}
