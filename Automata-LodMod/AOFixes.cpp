#include "pch.h"

const uint32_t LodHook1Addr[] = { 0x84CD60, 0x844680, 0x873A90, 0x627DE0, 0x9D7B80 };

const uint32_t Setting_AOEnabled_Addr[] = { 0x1421F58, 0x1414E48, 0x14A4B08, 0x0, 0x0 };

const uint32_t IsAOAllowedAddr[] = { 0x78BC20, 0x783AF0, 0x79A620, 0x0, 0x85C230 }; // inlined in Steam/2017

// SAO CreateTextureBuffer call hooks:
const uint32_t CreateTextureBuffer_Addr[] = { 0x248060, 0x2415D0, 0x24A870, 0x9348F0, 0xE75EA0 };
const uint32_t CreateTextureBuffer_TrampolineAddr[] = { 0x7879D2, 0x77F8A2, 0x7963D2, 0x53EE71, 0xE76313 };

const uint32_t AO_CreateTextureBufferCall1_Addr[] = { 0x77439A, 0x76C26A, 0x782D9A, 0x53B56A, 0x837462 };
const uint32_t AO_CreateTextureBufferCall2_Addr[] = { 0x774446, 0x76C316, 0x782E46, 0x53B5D0, 0x8374CA };
const uint32_t AO_CreateTextureBufferCall3_Addr[] = { 0x7744B4, 0x76C384, 0x782EB4, 0x53B620, 0x837502 };

typedef void* (*sub_84CD60_Fn)(BehaviorScr* thisptr, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8);
sub_84CD60_Fn sub_84CD60_Orig;
void* sub_84CD60_Hook(BehaviorScr* thisptr, BYTE* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8)
{
  if (thisptr->ObjectId == 0xC0200) // fix engels appearing in sea during intro flying section
    return sub_84CD60_Orig(thisptr, a2, a3, a4, a5, a6, a7, a8);

  // In case something in orig function makes use of LOD details, disable them before we call orig code
  if (Settings.LODMultiplier <= 0)
    thisptr->DisableLODs();

  auto ret = sub_84CD60_Orig(thisptr, a2, a3, a4, a5, a6, a7, a8);

  if (Settings.LODMultiplier > 0 && Settings.LODMultiplier != 1)
    thisptr->MultiplyLODs(Settings.LODMultiplier);
  else if (Settings.LODMultiplier == 0)
    thisptr->DisableLODs();

  if (Settings.ShadowModelForceAll)
    thisptr->SetCastShadows(true);

  return ret;
}

typedef uint32_t(*IsAOAllowed_Fn)(void* a1);
IsAOAllowed_Fn IsAOAllowed_Orig;
uint32_t IsAOAllowed_Hook(void* a1)
{
  if (!IsAOAllowed_Orig(a1))
    return false;

  if (!GameAddress(Setting_AOEnabled_Addr))
    return true;

  auto result = *GameAddress<uint32_t*>(Setting_AOEnabled_Addr) != 0;
  return result;
}

typedef void* (*CreateTextureBuffer_Fn)(void* texture, uint32_t width, uint32_t height, void* a4, void* a5, uint64_t textureFormat, void* a7, void* a8, void* a9, void* a10, void* a11, void* a12);
CreateTextureBuffer_Fn CreateTextureBuffer_Orig;

void* AO_CreateTextureBuffer_Hook(void* texture, uint32_t width, uint32_t height, void* a4, void* a5, uint64_t textureFormat, void* a7, void* a8, void* a9, void* a10, void* a11, void* a12)
{
  // 2017's SAO textureFormat is R8G8B8A8_UNORM (0)
  // 2021 format is R8_UNORM (9)
  // changing this sadly makes no difference though ;-;

  uint32_t width_new = uint32_t(float(width) * Settings.AOMultiplierWidth);
  uint32_t height_new = uint32_t(float(height) * Settings.AOMultiplierHeight);

  // This hook is only called 3 times, so lets log it if we can

  if (Settings.DebugLog)
    dlog("AO_CreateTextureBuffer_Hook(%dx%d) - setting resolution %dx%d%s\n", width, height, width_new, height_new,
      (version != GameVersion::Debug2017 ? " (will be scaled with game render resolution)" : "") // 2017 doesn't scale
    );

  return CreateTextureBuffer_Orig(texture, width_new, height_new, a4, a5, textureFormat, a7, a8, a9, a10, a11, a12);
}

#pragma pack(push, 1)
struct cCreateTextureInfo
{
  /* 0x00 */ uint64_t unk0;
  /* 0x08 */ uint32_t width;
  /* 0x0C */ uint32_t height;
  /* 0x10 */ uint32_t unk10;
  /* 0x14 */ uint64_t unk14;
};
static_assert(sizeof(cCreateTextureInfo) == 0x1C);
#pragma pack(pop)

typedef void* (*CreateTextureBuffer_Fn_2017)(void* unk, void* texture, cCreateTextureInfo* texture_info);
// Steam2017 inlined the actual AO_CreateTextureBuffer code, so we have to hook the function call that'd normally be inside CreateTextureBuffer
// (after width/height/etc have been set up in a struct)
void* AO_CreateTextureBuffer_Hook_2017(void* unk, void* texture, cCreateTextureInfo* texture_info)
{
  uint32_t width_new = uint32_t(float(texture_info->width) * Settings.AOMultiplierWidth);
  uint32_t height_new = uint32_t(float(texture_info->height) * Settings.AOMultiplierHeight);

  // This hook is only called 3 times, so lets log it if we can

  if (Settings.DebugLog)
    dlog("AO_CreateTextureBuffer_Hook_2017(%dx%d) - setting resolution %dx%d\n", texture_info->width, texture_info->height, width_new, height_new);

  texture_info->width = width_new;
  texture_info->height = height_new;

  CreateTextureBuffer_Fn_2017 origFn = (CreateTextureBuffer_Fn_2017)CreateTextureBuffer_Orig;

  return origFn(unk, texture, texture_info);
}

void PatchCall(uintptr_t callAddr, uintptr_t callDest)
{
  uint8_t callBuf[] = { 0xE8, 0x00, 0x00, 0x00, 0x00 };
  uint32_t diff = uint32_t(callDest - (callAddr + 5));
  *(uint32_t*)&callBuf[1] = diff;

  SafeWrite(callAddr, callBuf, 5);
}

void AOFixes_Init()
{
  if (Settings.LODMultiplier != 1 || Settings.ShadowModelForceAll)
    MH_CreateHook(GameAddress<LPVOID>(LodHook1Addr), sub_84CD60_Hook, (LPVOID*)&sub_84CD60_Orig);

  if (GameAddress(IsAOAllowedAddr) != 0)
    MH_CreateHook(GameAddress<LPVOID>(IsAOAllowedAddr), IsAOAllowed_Hook, (LPVOID*)&IsAOAllowed_Orig);

  if (Settings.AOMultiplierWidth != 1 || Settings.AOMultiplierHeight != 1)
  {
    CreateTextureBuffer_Orig = GameAddress<CreateTextureBuffer_Fn>(CreateTextureBuffer_Addr);

    // Have to write a trampoline somewhere near the hooked addr, needs 12 bytes...
    uint8_t trampoline[] = { 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xE0 };

    if (version != GameVersion::Steam2017)
      *(uintptr_t*)&trampoline[2] = (uintptr_t)&AO_CreateTextureBuffer_Hook;
    else
      *(uintptr_t*)&trampoline[2] = (uintptr_t)&AO_CreateTextureBuffer_Hook_2017;

    SafeWrite(GameAddress(CreateTextureBuffer_TrampolineAddr), trampoline, 12);

    // Hook SAO-related CreateTextureBuffer calls to call the trampoline we patched in

    PatchCall(GameAddress(AO_CreateTextureBufferCall1_Addr), GameAddress(CreateTextureBuffer_TrampolineAddr));
    PatchCall(GameAddress(AO_CreateTextureBufferCall2_Addr), GameAddress(CreateTextureBuffer_TrampolineAddr));
    PatchCall(GameAddress(AO_CreateTextureBufferCall3_Addr), GameAddress(CreateTextureBuffer_TrampolineAddr));
  }
}
