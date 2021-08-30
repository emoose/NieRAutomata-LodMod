#include "pch.h"

// Requires the following .obj files from cri_mana_pcx64.lib:
// crimana_decoder_h264_mft.obj
// crimv_decoder_v264mft.obj
// crimvply_decoder_v264mft.obj
// criv264mft_lib.obj

// uncomment the line below once you have them:
// #define CRI_H264_SUPPORT 1

const uint32_t criMana_Initialize_Addr[] = { 0x5D2A0, 0x5D2A0, 0x5D310, 0xAF6A60, 0x1233F00 };
const uint32_t s_default_decryptionkey_Addr[] = { 0x14C86A8, 0x14BB598, 0x154B468, 0x1171908, 0x273F178 };

// Wrappers for the funcs called by those obj files, make them call equivalent funcs in game EXE

#ifdef CRI_H264_SUPPORT
typedef enum CriError;
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
  const uint32_t addr[] = { 0xA35D4, 0xA35D4, 0xA3644, 0xC6F2DC, 0x1523768 };

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
  else
  {
    *a3 = 0;
    return 0;
  }
}

int criUsfCmn_GetPaddingSize(uint8_t* a1, uint32_t a2, uint16_t* a3)
{
  __int64 result; // rax

  if (a2 >= 0x10)
  {
    *a3 = _byteswap_ushort(*(uint16_t*)(a1 + 0xA));
    return 1;
  }
  else
  {
    *a3 = 0;
    return 0;
  }
  return result;
}

void* criUsfCmn_ParseUsfHeaderStd(void* a1, void* a2, void* a3)
{
  const uint32_t addr[] = { 0xA2938, 0xA2938, 0xA29A8, 0xC6EAA8, 0x1522CC8 };

  return GameAddress<fn_2args>(addr)(a1, a2);
}

// The main function we're calling in the obj files
void criMana_SetupMediaFoundationH264Decoder_PC(void* config, void* work, void* work_size);

#ifdef __cplusplus
}
#endif

#endif

// Sets up CriMana to use a given encryption key
// (code from USM Demux Tool v1.01-mod)
void CriMana_InitMask(uint64_t key)
{
  uint32_t key1 = (uint32_t)(key & 0xFFFFFFFF);
  uint32_t key2 = (uint32_t)((key >> 32) & 0xFFFFFFFF);

  unsigned char t[0x20];
  t[0x00] = ((unsigned char*)&key1)[0];
  t[0x01] = ((unsigned char*)&key1)[1];
  t[0x02] = ((unsigned char*)&key1)[2];
  t[0x03] = ((unsigned char*)&key1)[3] - 0x34;
  t[0x04] = ((unsigned char*)&key2)[0] + 0xF9;
  t[0x05] = ((unsigned char*)&key2)[1] ^ 0x13;
  t[0x06] = ((unsigned char*)&key2)[2] + 0x61;
  t[0x07] = t[0x00] ^ 0xFF;
  t[0x08] = t[0x02] + t[0x01];
  t[0x09] = t[0x01] - t[0x07];
  t[0x0A] = t[0x02] ^ 0xFF;
  t[0x0B] = t[0x01] ^ 0xFF;
  t[0x0C] = t[0x0B] + t[0x09];
  t[0x0D] = t[0x08] - t[0x03];
  t[0x0E] = t[0x0D] ^ 0xFF;
  t[0x0F] = t[0x0A] - t[0x0B];
  t[0x10] = t[0x08] - t[0x0F];
  t[0x11] = t[0x10] ^ t[0x07];
  t[0x12] = t[0x0F] ^ 0xFF;
  t[0x13] = t[0x03] ^ 0x10;
  t[0x14] = t[0x04] - 0x32;
  t[0x15] = t[0x05] + 0xED;
  t[0x16] = t[0x06] ^ 0xF3;
  t[0x17] = t[0x13] - t[0x0F];
  t[0x18] = t[0x15] + t[0x07];
  t[0x19] = 0x21 - t[0x13];
  t[0x1A] = t[0x14] ^ t[0x17];
  t[0x1B] = t[0x16] + t[0x16];
  t[0x1C] = t[0x17] + 0x44;
  t[0x1D] = t[0x03] + t[0x04];
  t[0x1E] = t[0x05] - t[0x16];
  t[0x1F] = t[0x1D] ^ t[0x13];

  uint64_t* s_default_decryptionkey = GameAddress<uint64_t*>(s_default_decryptionkey_Addr);
  *s_default_decryptionkey = key;

  unsigned char* _videoMask1 = ((uint8_t*)s_default_decryptionkey) + 8;
  unsigned char* _videoMask2 = _videoMask1 + 0x20;
  unsigned char* _audioMask = _videoMask2 + 0x20;

  unsigned char t2[4] = { 'U','R','U','C' };
  for (int i = 0; i < 0x20; i++) {
    _videoMask1[i] = t[i];
    _videoMask2[i] = t[i] ^ 0xFF;
    _audioMask[i] = (i & 1) ? t2[(i >> 1) & 3] : t[i] ^ 0xFF;
  }
}

fn_3args criMana_Initialize_Orig;
void* criMana_Initialize_Hook(void* a1, void* a2, void* a3)
{
  // 0x0000450D608C479F is key for uma musume USM
  if (Settings.MoviesEncryptionKey)
    CriMana_InitMask(Settings.MoviesEncryptionKey);

#ifdef CRI_H264_SUPPORT
  // enable H264 support in CriMana
  if (Settings.MoviesEnableH264)
    criMana_SetupMediaFoundationH264Decoder_PC(0, NULL, NULL);
#endif

  return criMana_Initialize_Orig(a1, a2, a3);
}

void CriH264_Init()
{
  // Hook criMana_Initialize so we can run SetupMediaFoundationH264Decoder before it:
  MH_CreateHook(GameAddress<LPVOID>(criMana_Initialize_Addr), criMana_Initialize_Hook, (LPVOID*)&criMana_Initialize_Orig);
}
