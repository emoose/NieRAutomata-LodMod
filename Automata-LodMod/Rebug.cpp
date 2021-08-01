#include "pch.h"

#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>
#include "SDK.h"
#include "MinHook/MinHook.h"

// Rebug: hooks to add some debug-flag checks back into functions, based on NA debug exe

extern uintptr_t mBaseAddress;
extern int version;

const uint32_t Flag_DBG_Addr[] = { 0x1029840, 0x101C750, 0x10AC3E0 };
const uint32_t Flag_DBSTP_Addr[] = { 0x102987C, 0x101C78C, 0x10AC41C };
const uint32_t Flag_DBDISP_Addr[] = { 0x1029860, 0x101C770, 0x10AC400 };
const uint32_t Flag_DBGRAPHIC_Addr[] = { 0x102988C, 0x101C79C, 0x10AC42C };
const uint32_t Flag_STA_Addr[] = { 0x10297C0, 0x101C6D0, 0x10AC360 };
const uint32_t Flag_STOP_Addr[] = { 0x10297D0, 0x101C6E0, 0x10AC370 };
const uint32_t Flag_GRAPHIC_Addr[] = { 0x10297E0, 0x101C6F0, 0x10AC380 };
const uint32_t Flag_DISP_Addr[] = { 0x10297F8, 0x101C708, 0x10AC398 };
const uint32_t Flag_GAME_Addr[] = { 0x10297F0, 0x101C700, 0x10AC390 };

uint32_t GetFlagValue(DBG_FLAG flag, uint32_t& address)
{
  uint32_t offset = 0;
  uint32_t value = GetFlagValue(static_cast<uint32_t>(flag), offset);
  address = Flag_DBG_Addr[version] + offset;
  return value;
}

uint32_t GetFlagValue(DBGRAPHIC_FLAG flag, uint32_t& address)
{
  uint32_t offset = 0;
  uint32_t value = GetFlagValue(static_cast<uint32_t>(flag), offset);
  address = Flag_DBGRAPHIC_Addr[version] + offset;
  return value;
}

bool CheckFlag(DBG_FLAG flag)
{
  uint32_t address = 0;
  auto rawFlag = GetFlagValue(flag, address);

  return (*reinterpret_cast<uint32_t*>(mBaseAddress + address) & rawFlag) != 0;
}

bool CheckFlag(DBGRAPHIC_FLAG flag)
{
  uint32_t address = 0;
  auto rawFlag = GetFlagValue(flag, address);

  return (*reinterpret_cast<uint32_t*>(mBaseAddress + address) & rawFlag) != 0;
}

const uint32_t Model_ManualCull_Addr[] = { 0x7F40F0, 0x7EBC20, 0x81A960 };
const uint32_t Model_ManualCull_ValueAddr[] = { 0x12500D0, 0x11D6D28, 0x12CA0A0 };

extern bool DisableManualCulling;

#ifdef _DEBUG
bool LogModels = false;
std::vector<std::string> CulledModels;
std::vector<std::string> ForcedCulls;
#endif

fn_2args Model_ManualCull_Orig;
void* Model_ManualCull_Hook(uint64_t area_id, char* model_name)
{
  // NA debug seems to set this value before returning...
  *reinterpret_cast<uint32_t*>(mBaseAddress + Model_ManualCull_ValueAddr[version]) = 1;

  // Reimplement DBG_MANUAL_CULLING_DISABLE
  // TODO: there's another check for this in NA debug (0x9A42BD), might be worth reimplementing (at ~0x827C77)
  if (CheckFlag(DBG_FLAG::DBG_MANUAL_CULLING_DISABLE))
    return 0;

  if (DisableManualCulling)
  {
    std::string lower_name = model_name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
      [](unsigned char c) { return std::tolower(c); });

    // exclude "low"/"lod"/"dummy" models, we want to keep those cullable
    bool lowModel =
      lower_name.find("low") != std::string::npos ||
      lower_name.find("lod") != std::string::npos ||
      lower_name.find("dummy") != std::string::npos ||
      lower_name.find("distant") != std::string::npos ||
      lower_name.find("far") != std::string::npos ||

      // forest/shopping center LODs near desert entrance
      (area_id == 0x11316 &&
        (lower_name == "mall" ||
         lower_name == "cliff" ||
         lower_name == "maintree")) ||

      // factory LODs showing near desert start
      // theres 1 more factory LOD that quickly appears/disappears near desert start above an arch
      // but its not caused by DisableManualCulling, ugh...
      (area_id == 0x10921 && lower_name == "mtrobot5") ||
      (area_id == 0x11021 && lower_name == "mtrobot9") ||

      // misplaced LOD ground near desert housing
      (area_id == 0x11115 && lower_name == "g11015_ground");

#ifdef _DEBUG
    bool clearCulled = false;
    if (clearCulled)
      CulledModels.clear();

    if (!lowModel)
    {
      bool forceCullThis = false;
      if (forceCullThis)
        ForcedCulls.push_back(lower_name);

      if (std::find(ForcedCulls.cbegin(), ForcedCulls.cend(), lower_name) != ForcedCulls.cend())
        lowModel = true;
    }
#endif

    // Not a low model, return false to disable culling on this
    if (!lowModel)
    {
#ifdef _DEBUG
      if (LogModels)
      {
        if (std::find(CulledModels.begin(), CulledModels.end(), lower_name) == CulledModels.end())
        {
          OutputDebugStringA((lower_name + "\n").c_str());
          CulledModels.push_back(lower_name);
        }
      }
#endif
      return 0;
    }
  }

  
  return Model_ManualCull_Orig((void*)area_id, model_name);
}

const uint32_t Model_LodSetup_Addr[] = { 0x846260, 0x83DBB0, 0x86CF90 };
fn_0args Model_LodSetup_Orig;
void* Model_LodSetup_Hook()
{
  if (!CheckFlag(DBGRAPHIC_FLAG::DBGRAPHIC_HIGH_LOD_FIXED))
    return Model_LodSetup_Orig();

  // Model_LodSetup_Orig checks value of STA flags, and if set it does what we need for DBGRAPHIC_HIGH_LOD_FIXED
  // so we'll temporarily set the flag for that fn to read
  // TODO: sadly nothing like this for LOW_LOD_FIXED to use, might need to reimpl the orig function...
  auto* STA = reinterpret_cast<uint32_t*>(mBaseAddress + Flag_STA_Addr[version]);

  auto origSTA = *STA;
  *STA |= 0x40000000;
  auto ret = Model_LodSetup_Orig();
  *STA = origSTA;

  return ret;
}



void Rebug_Init()
{
  MH_CreateHook((LPVOID)(mBaseAddress + Model_ManualCull_Addr[version]), Model_ManualCull_Hook, (LPVOID*)&Model_ManualCull_Orig);
  MH_CreateHook((LPVOID)(mBaseAddress + Model_LodSetup_Addr[version]), Model_LodSetup_Hook, (LPVOID*)&Model_LodSetup_Orig);
}
