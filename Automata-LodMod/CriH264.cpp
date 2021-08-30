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


#ifdef __cplusplus
extern "C"
{
#endif
void criMana_SetupMediaFoundationH264Decoder_PC(void* config, void* work, void* work_size);
#ifdef __cplusplus
}
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
