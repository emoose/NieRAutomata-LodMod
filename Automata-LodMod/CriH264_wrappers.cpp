#include "pch.h"

// Wrappers for the funcs called by the CRI H264 obj files
// Just redirect them to equivalent funcs in game EXE
// (this was part of CriH264.cpp, but for some reason this code doesn't like being optimized)
// (the #pragma at end of the file seems to fix it though)

enum CriError;
namespace CriMv {
  CriError ErrorContainer;
};

#ifdef __cplusplus
extern "C"
{
#endif

void* criCrw_MemClear(void* a1, void* a2)
{
  const uint32_t addr[] = { 0x98530, 0x98530, 0x985A0, 0xC64648, 0x11AA314 };

  return GameAddress<fn_2args>(addr)(a1, a2);
}

void* criErr_Notify(void* a1, void* a2)
{
  const uint32_t addr[] = { 0x98330, 0x98330, 0x983A0, 0xC64448, 0x11A9FA0 };

  return GameAddress<fn_2args>(addr)(a1, a2);
}

void* criErr_Notify1(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0x9834C, 0x9834C, 0x983BC, 0xC64464, 0x11A9FBC };

  return GameAddress<fn_3args>(addr)(a1, a2, a3);
}

void* criErr_NotifyGeneric(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0x98378, 0x98378, 0x983E8, 0xC64490, 0x11AA00C };

  return GameAddress<fn_3args>(addr)(a1, a2, a3);
}

void* criFixedAllocator_Allocate(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0x9976C, 0x9976C, 0x997DC, 0xC658CC, 0x15154A8 };

  return GameAddress<fn_3args>(addr)(a1, a2, a3);
}

void* criFixedAllocator_Create(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0x9978C, 0x9978C, 0x997FC, 0xC658EC, 0x15154C8 };

  return GameAddress<fn_3args>(addr)(a1, a2, a3);
}

void* criFixedAllocator_Destroy()
{ // nullsub
  const uint32_t addr[] = { 0x8640, 0x8640, 0x86B0, 0xC646E8, 0x15154D4 };

  return GameAddress<fn_0args>(addr)();
}

void* criHeap_AllocFix(void* a1, void* a2, void* a3, void* a4)
{
  const uint32_t addr[] = { 0xA1C00, 0xA1C00, 0xA1C70, 0xC6DD70, 0x152176C };

  return GameAddress<fn_4args>(addr)(a1, a2, a3, a4);
}

void* criHeap_Free(void* a1)
{
  const uint32_t addr[] = { 0xA1F08, 0xA1F08, 0xA1F78, 0xC6E078, 0x1521D1C };

  return GameAddress<fn_1args>(addr)(a1);
}

void* criMvPly_AttachCodec(void* a1)
{
  const uint32_t addr[] = { 0x6A9F0, 0x6A9F0, 0x6AA60, 0xB041B0, 0x1245EC0 };

  return GameAddress<fn_1args>(addr)(a1);
}

void* criSjUni_CalculateWorkSize(void* a1)
{
  const uint32_t addr[] = { 0xA35D4, 0xA35D4, 0xA3644, 0xC6F744, 0x1523C24 };

  auto ret = GameAddress<fn_1args>(addr)(a1);

  return ret;
}

void* criSjUni_Create(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0xA35F4, 0xA35F4, 0xA3664, 0xC6F764, 0x1523C44 };

  return GameAddress<fn_3args>(addr)(a1, a2, a3);
}

void* criSjUni_GetRequiredMemorySizeWithConfig(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0xA2DCC, 0xA2DCC, 0xA2E3C, 0xC6EF3C, 0x1523350 };

  auto ret = GameAddress<fn_3args>(addr)(a1, a2, a3);
  return ret;
}

void* criSj_Destroy(void* a1)
{
  const uint32_t addr[] = { 0xA2AA0, 0xA2AA0, 0xA2B10, 0xC6EC10, 0x1522E30 };

  return GameAddress<fn_1args>(addr)(a1);
}

void* criSj_GetChunk(void* a1)
{
  const uint32_t addr[] = { 0xA2AB4, 0xA2AB4, 0xA2B24, 0xC6EC24, 0x1522E44 };

  return GameAddress<fn_1args>(addr)(a1);
}

void* criSj_GetTotalSize(void* a1)
{
  const uint32_t addr[] = { 0x47C68, 0x47C68, 0x47CD8, 0xAE13C4, 0x1522E88 };

  return GameAddress<fn_1args>(addr)(a1);
}

void* criSj_PutChunk(void* a1)
{
  const uint32_t addr[] = { 0xA2ACC, 0xA2ACC, 0xA2B3C, 0xC6EC3C, 0x1522E90 };

  return GameAddress<fn_1args>(addr)(a1);
}

void* criSj_Reset(void* a1)
{
  const uint32_t addr[] = { 0xA2B30, 0xA2B30, 0xA2BA0, 0xC6ECA0, 0x1522FAC };

  return GameAddress<fn_1args>(addr)(a1);
}

void* criSj_UngetChunk(void* a1)
{
  const uint32_t addr[] = { 0xA2B6C, 0xA2B6C, 0xA2BDC, 0xC6ECDC, 0x1522FE8 };

  return GameAddress<fn_1args>(addr)(a1);
}

void* criUsfCmn_GetContents(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0xA28C8, 0xA28C8, 0xA2938, 0xC6EA38, 0x1522B08 };

  return GameAddress<fn_3args>(addr)(a1, a2, a3);
}

// Not included in game EXE... luckily pretty easy to reimplement
int criUsfCmn_GetHeaderSize(uint8_t* a1, uint32_t a2, uint8_t* a3)
{
  if (a2 >= 0x10)
  {
    *a3 = *(a1 + 9);
    return 1;
  }
  *a3 = 0;
  return 0;
}

int criUsfCmn_GetPaddingSize(uint8_t* a1, uint32_t a2, uint16_t* a3)
{
  if (a2 >= 0x10)
  {
    *a3 = _byteswap_ushort(*(uint16_t*)(a1 + 0xA));
    return 1;
  }
  *a3 = 0;
  return 0;
}

void* criUsfCmn_ParseUsfHeaderStd(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0xA2938, 0xA2938, 0xA29A8, 0xC6EAA8, 0x1522CC8 };

  return GameAddress<fn_2args>(addr)(a1, a2);
}

#ifdef __cplusplus
}
#endif

#pragma optimize("g", off)
