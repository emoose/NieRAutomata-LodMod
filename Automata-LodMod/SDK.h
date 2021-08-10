#pragma once
#include "SDKEnums.h"

// Most of the stuff in this file is unused (cShadowParam & BehaviorScr are the only structs LodMod actually make use of here)
// Just felt it was worth documenting the other structs that use the same BinaryXML stuff cShadowParam used :)
// (these same BinaryXML structs show up in a few PlatinumGames titles - maybe this can help with those too)

enum GameVersion { // release date follows
  Win10 = 0, // 15 July 2021
  Win7,      // 15 July 2021
  UWP,       // ? 2021
  Steam2017, // 28 April 2017
  Debug2017  // 22 March 2017
};

typedef void* (*fn_3args)(void* a1, void* a2, void* a3);
typedef void* (*fn_2args)(void* a1, void* a2);
typedef void* (*fn_1args)(void* a1);
typedef void* (*fn_0args)();

#pragma pack(push, 1)

struct dbFlag
{
  char* Name;
  char* Desc;
};

struct dbMenu_FlagCategory
{
  char* Name;
#ifdef NA_DEBUG
  // debug EXE only :(
  dbFlag* FlagNames;
#endif
  uint32_t* FlagPtr;
  uint64_t NumBits;
};

struct dbMenu_Flag
{
  /* 0x00 */ char* Name;
  /* 0x08 */ char* Desc; // japanese?
  /* 0x10 */ uint32_t Category;
  /* 0x14 */ uint32_t Index;
  /* 0x18 */ uint32_t LangIndex; // DBG_FLAG_0x{}, DBG_FLAG_HELP_0x{}
  /* 0x1C */ uint32_t Unk1C;
};

template<typename T>
struct cVec4
{
  T value[4];
};
static_assert(sizeof(cVec4<float>) == 0x10);

template<typename T>
struct cVec3
{
  T value[3];
};
static_assert(sizeof(cVec3<float>) == 0xC);

class BehaviorScr // name from game EXE
{
  virtual ~BehaviorScr() = 0; // for vftable
public:
  /* 0x008 */ uint8_t Unk0[0x388];
  /* 0x390 */ void* ShadowArray; // some kind of array/vector related with shadows, "ShadowCast" flag affects something in the entries
  /* 0x398 */ uint8_t Unk398[0x58];
  /* 0x3F0 */ float* DistRates; // pointer to "DistRate0"-"DistRate3"
  /* 0x3F8 */ uint32_t NumDistRates;
  /* 0x3FC */ float Unk3FC;
  /* 0x400 */ uint32_t Unk400; // "UseLostLOD", gets set if using dist rates?
  /* 0x404 */ uint32_t Unk404; // if set, Unk3FC = 0.05 ?
  /* 0x408 */ uint8_t Unk408[0x118];
  /* 0x520 */ uint32_t Unk520; // "UseCullAABB" sets/removes 0x10 flag
  /* 0x524 */ float Unk524;
  /* 0x528 */ uint8_t Unk528[0x48];
  /* 0x570 */ uint32_t Unk570; // "CamAlpha" sets to 1 or 0
  /* 0x574 */ float Unk574;
  /* 0x578 */ float Unk578; // "CamAlpha"
  /* 0x57C */ float Unk57C;
  /* 0x580 */ uint32_t Unk580;
  /* 0x584 */ uint32_t AmbientOcclusionAllowed; // "AO_OFF" sets to 0
  /* 0x588 */ uint32_t Unk588;
  /* 0x58C */ uint32_t Unk58C;
  /* 0x590 */ uint8_t Unk590[0x58];
  /* 0x5E8 */ float BloomStrength; // always 0 or 1 ?
  /* 0x5EC */ uint32_t Unk5EC;
  /* 0x5F0 */ uint8_t Unk5F0[0x300];

  // other possible fields:
  // - EV_LIGHT_NO (NA debug exe)
  // - NoBackCull (vanquish 2010)
  // - ShadowLOD (vanquish 2010)
  // - LostDistRate (vanquish 2010)

  void DisableLODs()
  {
    // Debug build adds 0x30 bytes to class members somewhere, it's before the ones we touch though, so just add 0x30 if needed
    int members_offset = version == GameVersion::Debug2017 ? 0x30 : 0;

    float* distRates = *(float**)(((uint8_t*)&DistRates) + members_offset);
    // Set all DistRates to 0
    if (distRates)
      memset(distRates, 0, sizeof(float) * 4);

    uint32_t* numDistRates = (uint32_t*)(((uint8_t*)&NumDistRates) + members_offset);
    // Set number of DistRates to 1 (0 causes weird issues)
    *numDistRates = 1;

    // Disable UseLostLOD
    uint32_t* unk400 = (uint32_t*)(((uint8_t*)&Unk400) + members_offset);
    uint32_t* unk404 = (uint32_t*)(((uint8_t*)&Unk404) + members_offset);
    *unk400 = 0;
    *unk404 = 0;

    // Remove UseCullAABB flag
    uint32_t* unk520 = (uint32_t*)(((uint8_t*)&Unk520) + members_offset);
    *unk520 &= 0xFFFFFFEF;
  }

  void MultiplyLODs(float multiplier)
  {
    // Debug build adds 0x30 bytes to class members somewhere, it's before the ones we touch though, so just add 0x30 if needed
    int members_offset = version == GameVersion::Debug2017 ? 0x30 : 0;

    float* distRates = *(float**)(((uint8_t*)&DistRates) + members_offset);
    if (!distRates || multiplier <= 0)
      return;

    uint32_t numDistRates = *(uint32_t*)(((uint8_t*)&NumDistRates) + members_offset);

    for (uint32_t i = 0; i < numDistRates; i++)
    {
      // DistRate needs to be made smaller to go further, idk how it works exactly
      distRates[i] /= multiplier;
    }
  }
};
static_assert(sizeof(BehaviorScr) == 0x8F0); // should be correct size

struct cObject_Data
{
  char* name;
  uint64_t unk8;
  uint64_t unk10;
  uint64_t unk18;
  uint64_t unk20;
};

class cObject
{
public:
  virtual cObject_Data* GetData() = 0;
};

// cObject type indexes:
// 0 - uint8_t
// 1 - ? (uint16_t?)
// 2 - uint32_t
// 3 - ?
// 4 - uint8_t again? (int8_t?)
// 5 - ? (int16_t?)
// 6 - float/uint32?? (int32_t?)
// 7 - float
// 8 - ?
// 9 - ?
// A - cVec3<float> (or sys::Angle sometimes?)
// B - cVec4<float>
// C - ?
// D - ?
// E - ?
// F - struct

class cShadowParam : public cObject // name from EXE
{
public:
  /* 0x008 */ uint32_t UnkDword8;
  /* 0x00C */ uint32_t UnkDwordC; // possibly "m_resolutionMode", from vanquish (2010) - unlikely though
  /* 0x010 */ float m_shadowCamOffset; // 7
  /* 0x014 */ float m_splitPoint[4]; // 7
  /* 0x024 */ float m_marginOffset[4]; // 7 - something to do with shadow-caster position/rotation?
  /* 0x034 */ float m_bokeStrength[4]; // 7 - first float = filter strength
  /* 0x044 */ float m_nkiShadowBase; // 7
};
static_assert(sizeof(cShadowParam) == 0x48); // should be correct size

class cLightApplyScale : public cObject
{
public:
  cObject_Data* GetData() { return 0; }

  /* 0x008 */ float m_scale[6]; // 7
};
static_assert(sizeof(cLightApplyScale) == 0x20); // should be correct size

class cLightSaveWork : public cObject
{
public:
  cObject_Data* GetData() { return 0; }

  /* 0x008 */ uint32_t m_flag; // 2
  /* 0x00C */ float m_priority; // 6 - could be int32_t

  /* 0x010 */ float* Unk10; // sometimes points to floats
  /* 0x018 */ uint8_t* Unk18; // sometimes points to something in graphics driver?

  /* 0x020 */ cVec4<float> m_pos; // B
  /* 0x030 */ cVec4<float> m_color; // B
  /* 0x040 */ cVec4<float> m_vleColorRate; // B
  /* 0x050 */ float m_distance; // 7

  /* 0x054 */ int32_t Unk54; // sometimes set to -1
  /* 0x058 */ uint8_t Unk58[0x8];

  /* 0x060 */ float m_vleDistance; // 7
  /* 0x064 */ cVec3<float> m_DirAng; // A - could be sys::Angle?

  /* 0x070 */ uint8_t Unk70[0xC];

  /* 0x07C */ uint8_t m_group; // 4
  /* 0x07D */ uint8_t m_applyFlag; // 0
  /* 0x07E */ uint8_t m_lightType; // 0

  /* 0x07F */ uint8_t Unk7F; // padding?

  /* 0x080 */ uint32_t m_lightOptionFlag; // 2
  /* 0x084 */ float m_effectiveDist; // 7
  /* 0x088 */ cLightApplyScale m_applyScale; // F

  /* 0x0A8 */ float m_anmParam_m_cycle; // 7
  /* 0x0AC */ float m_anmParam_m_range; // 7
};
static_assert(sizeof(cLightSaveWork) == 0xB0); // should be correct size

class cLightDataMinimum : public cObject
{
public:
  /* 0x008 */ uint64_t Unk8;
  /* 0x010 */ uint32_t m_ShadowMove; // 2

  /* 0x014 */ uint8_t unk14[0xC];

  /* 0x020 */ cVec4<float> m_SSSSS_Param1; // B
  /* 0x030 */ cVec4<float> m_windParam; // B
  /* 0x040 */ cVec4<float> m_windVec; // B
  /* 0x050 */ cVec4<float> m_VleParam; // B
  /* 0x060 */ cVec4<float> m_VleColor; // B
  /* 0x070 */ cVec4<float> m_VleParticleColors_0; // B
  /* 0x080 */ cVec4<float> m_VleParticleColors_1; // B
  /* 0x090 */ cVec4<float> m_VleParticleColors_2; // B
  /* 0x0A0 */ cVec4<float> m_VleParticleColors_3; // B
  /* 0x0B0 */ cVec4<float> m_VleStepSize; // B
  /* 0x0C0 */ cVec4<float> m_VelocityParam; // B
  /* 0x0D0 */ float m_ReferenceLuminance; // 7
  /* 0x0D4 */ float m_EyeAddaptionSpeed; // 7

  /* 0x0D8 */ uint8_t unkD8[8];

  /* 0x0E0 */ cVec4<float> m_SAO_Param1; // B
  /* 0x0F0 */ cVec4<float> m_SAO_Param2; // B

  /* 0x100 */ float m_DryReady; // 7
  /* 0x104 */ float m_DrySpeed; // 7
  /* 0x108 */ float m_VignettoIntensity; // 7
  /* 0x10C */ float m_VignettoGradation; // 7

  /* 0x110 */ cVec4<float> m_diffusionParam; // B
  /* 0x120 */ cVec4<float> m_WtrUvAnim; // B
  /* 0x130 */ cVec4<float> m_WtrNormalAnim; // B
  /* 0x140 */ cVec4<float> m_WtrNormalTexSeed; // B
  /* 0x150 */ cVec4<float> m_Wtr02Param_Tile1; // B
  /* 0x160 */ cVec4<float> m_Wtr02Param_Tile2; // B
  /* 0x170 */ cVec4<float> m_Wtr02Param_UvAnim; // B
  /* 0x180 */ cVec4<float> m_Wtr02Param_NormalAnim; // B
  /* 0x190 */ cVec4<float> m_Wtr02Param_NormalTexSeed; // B
  /* 0x1A0 */ cVec4<float> m_Wtr02Param_Tide1; // B
  /* 0x1B0 */ cVec4<float> m_Wtr02Param_Tide2; // B
  /* 0x1C0 */ cVec4<float> m_Wtr02Param_FogNearFar; // B
  /* 0x1D0 */ cVec4<float> m_Wtr02Param_FogColor1; // B
  /* 0x1E0 */ cVec4<float> m_Wtr02Param_FogColor2; // B
  /* 0x1F0 */ cVec4<float> m_Wtr02Param_FogColor3; // B
  /* 0x200 */ cVec4<float> m_Wtr02Param_AlbedoColor; // B
  /* 0x210 */ cVec4<float> m_Wtr02Param_Translucent; // B
  /* 0x220 */ cVec4<float> m_Wtr02Param_Refract; // B
  /* 0x230 */ cVec4<float> m_Wtr02Param_OtherParam; // B
  /* 0x240 */ cVec4<float> m_Wtr02Param_ParamVS_0; // B
  /* 0x250 */ cVec4<float> m_Wtr02Param_ParamVS_1; // B
  /* 0x260 */ cVec4<float> m_Wtr02Param_ParamVS_2; // B
  /* 0x270 */ cVec4<float> m_Wtr02Param_ParamVS_3; // B
  /* 0x280 */ cVec4<float> m_Wtr02Param_ParamVS_4; // B
  /* 0x290 */ float m_DirAng[8]; // 9

  /* 0x2B0 */ uint8_t unk2B0[0x20];

  /* 0x2D0 */ cVec4<float> m_DirCol[8]; // B
  /* 0x350 */ uint32_t m_DirApplyFlag[8]; // 2
  /* 0x370 */ uint8_t m_DirApplyGroup[8]; // 0
  /* 0x378 */ cLightApplyScale m_DirApplyScale[8]; // F

  /* 0x478 */ uint8_t unk478[8];

  /* 0x480 */ cVec4<float> m_CamFrustumParam; // B
  /* 0x490 */ float m_AlphaShadowLV; // 6
  /* 0x494 */ uint32_t m_dofType; // 2
  /* 0x498 */ uint8_t m_ShadowDirIdx; // 0
  /* 0x499 */ uint8_t m_EnlightenLightIdx; // 0

  /* 0x49A */ uint8_t unk49A[2];

  /* 0x49C */ float m_backLightRate; // 7
  /* 0x4A0 */ cVec3<float> m_LightMapShadowPow; // A (vec3?)
  /* 0x4AC */ cVec3<float> m_LightMapShadowPow2; // A
  /* 0x4B8 */ cVec3<float> m_SkyLight; // A

  /* 0x4C4 */ uint8_t unk4C4[0xC];

  /* 0x4D0 */ cVec4<float> m_EnlDummyCubeColor; // B

  /* 0x4E0 */ cLightSaveWork m_RoomLightWork[0x40]; // F

  /* 0x30E0 */ uint8_t unk30E0[0x20];
};
static_assert(sizeof(cLightDataMinimum) == 0x3100); // size from the EXE

class cLightDataMinimumEv : public cLightDataMinimum
{
};
static_assert(sizeof(cLightDataMinimumEv) == 0x3100); // size from the EXE

class cDofWork : public cObject
{
public:
  /* 0x008 */ uint32_t Unk8;
  /* 0x00C */ uint32_t UnkC;
  /* 0x010 */ uint32_t Unk10;
  /* 0x014 */ uint32_t Unk14;
  /* 0x018 */ uint32_t Unk18;
  /* 0x01C */ float m_FrontFarZ[3]; // 7
  /* 0x028 */ float m_BackNearZ[3]; // 7
  /* 0x034 */ float m_BackFarZ[3]; // 7
  /* 0x040 */ float m_StartFrame[3]; // 7
  /* 0x04C */ float m_EndFrame[3]; // 7
  /* 0x058 */ float m_BokeStrength[3]; // 7
  /* 0x064 */ float m_AlphaStrength[3]; // 7
};
static_assert(sizeof(cDofWork) == 0x70);

#pragma pack(pop)
