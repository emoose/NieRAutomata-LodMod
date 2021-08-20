#include "pch.h"

#include "SDK.h"
#include "MinHook/MinHook.h"

// Rebug: hooks to add some debug-flag checks back into functions, based on NA debug exe

// Configurables:
std::unordered_map<int, std::vector<std::string>> SoftFilteredModels;
std::unordered_map<int, std::vector<std::string>> HardFilteredModels;

const uint32_t Flag_DBG_Addr[] = { 0x1029840, 0x101C750, 0x10AC3E0, 0x1415B90, 0x20D9E38 };
const uint32_t Flag_DBSTP_Addr[] = { 0x102987C, 0x101C78C, 0x10AC41C, 0x1415BCC, 0x20D9E74 };
const uint32_t Flag_DBDISP_Addr[] = { 0x1029860, 0x101C770, 0x10AC400, 0x1415BB0, 0x20D9E58 };
const uint32_t Flag_DBGRAPHIC_Addr[] = { 0x102988C, 0x101C79C, 0x10AC42C, 0x1415BDC, 0x20D9E84 };

const uint32_t Flag_STA_Addr[] = { 0x10297C0, 0x101C6D0, 0x10AC360, 0x1415B10, 0x20D9D30 };
const uint32_t Flag_STOP_Addr[] = { 0x10297D0, 0x101C6E0, 0x10AC370, 0x1415B20, 0x20D9D40 };
const uint32_t Flag_GRAPHIC_Addr[] = { 0x10297E0, 0x101C6F0, 0x10AC380, 0x1415B30, 0x20D9D50 };
const uint32_t Flag_DISP_Addr[] = { 0x10297F8, 0x101C708, 0x10AC398, 0x1415B48, 0x20D9D68 };
const uint32_t Flag_GAME_Addr[] = { 0x10297F0, 0x101C700, 0x10AC390, 0x1415B40, 0x20D9D60 };

const uint32_t Model_ShouldBeCulled_Addr[] = { 0x7F40F0, 0x7EBC20, 0x81A960, 0x5BD210, 0x9543E0 };
const uint32_t Model_ShouldBeCulled_ValueAddr[] = { 0x12500D0, 0x11D6D28, 0x12CA0A0, 0x147BEC0, 0x237F290 };

const uint32_t Global_PlayerCoords_Addr[] = { 0xF568B0, 0xF498B0, 0xFD4620, 0x1082DF0, 0x1F51020 };
const uint32_t ModelManager__Update_Addr[] = { 0x846260, 0x83DBB0, 0x86CF90, 0x61D440, 0x9CAB50 }; // sets up global/per-model LOD?

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

#ifdef _DEBUG
bool LogModels = false;
bool LogPassedModels = false;
std::vector<std::string> CulledModels;
std::vector<std::string> PassedModels;
std::mutex CulledModelsMutex;

char ModelsToSkip[16384] = { 0 };
char ModelsToCull[16384] = { 0 };
#endif

template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1) {
  static const char* digits = "0123456789ABCDEF";
  std::string rc(hex_len+3, '0');
  rc[0] = '0';
  rc[1] = 'x';
  for (size_t i = 2, j = (hex_len - 1) * 4; i < hex_len+2; ++i, j -= 4)
    rc[i] = digits[(w >> j) & 0x0f];
  rc[hex_len + 2] = '_';
  return rc;
}

int GetPlayerAreaId()
{
  // TODO: find a better method than this!
  int* position_ptr = reinterpret_cast<int32_t*>(mBaseAddress + Global_PlayerCoords_Addr[version]);

  uint32_t x_pos = (uint8_t)position_ptr[0];
  uint32_t y_pos = (uint8_t)position_ptr[1];

  return (x_pos << 8) | y_pos;
}

inline std::string GetFullModelName(int area_id, const std::string& model_name)
{
  return n2hexstr(area_id, 4) + model_name;
}

bool ShouldForceCull(int area_id, const std::string& lower_model_name)
{
  // x1319 has an ugly dummy LOD for the resistance camp building, kill it if resistance camp is loaded
  if (area_id == 0x1319 && lower_model_name == "g11420_dummybuild" && g11420IsLoaded)
    return (void*)1;

  auto player_area = GetPlayerAreaId();
  if (HardFilteredModels.count(player_area))
  {
    auto testName = GetFullModelName(area_id, lower_model_name);
    auto& areasToFilterInside = HardFilteredModels[player_area];
    for (auto modelName : areasToFilterInside)
      if (modelName == testName)
        return true;
  }
#ifdef _DEBUG
  return strstr(ModelsToCull, lower_model_name.c_str()) != NULL;
#endif
  return false;
}

fn_2args Model_ShouldBeCulled_Orig;
void* Model_ShouldBeCulled_Hook(uint64_t area_id_full, char* model_name)
{
  // NA debug seems to set this value before returning...
  *reinterpret_cast<uint32_t*>(mBaseAddress + Model_ShouldBeCulled_ValueAddr[version]) = 1;

  // Reimplement DBG_MANUAL_CULLING_DISABLE
  // TODO: there's another check for this in NA debug (0x9A42BD), might be worth reimplementing (at ~0x827C77)
  if (CheckFlag(DBG_FLAG::DBG_MANUAL_CULLING_DISABLE))
    return (void*)0;

  // remove game-stage (beginning/middle/end) from area id
  int area_id = area_id_full & 0xFFFF;

  std::string lower_name = model_name;
  std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
    [](unsigned char c) { return std::tolower(c); });

  // checks if we should forcibly cull this model (return true) instead of passing it to Model_ShouldBeCulled_Orig
#ifdef _DEBUG
  if(lower_name != "ground")
#endif
    if (ShouldForceCull(area_id, lower_name))
      return (void*)1;

  if (Settings.DisableManualCulling)
  {
    // exclude "low"/"lod"/"dummy" models, we want to keep those cullable
    bool lowModel =
      lower_name.find("low") != std::string::npos ||
      lower_name.find("lod") != std::string::npos ||
      lower_name.find("dummy") != std::string::npos ||
      lower_name.find("distant") != std::string::npos ||
      lower_name.find("far") != std::string::npos;

    if (lowModel)
    {
      // "nolow" is used for things they want culled from LQ, force enable them
      if (lower_name.find("nolow") != std::string::npos)
        // except amusement park has "nolowmap_far_01" which is an ugly lod near save point
        // (park also has a weird pop-in issue near where the goliath tank is, the connecting bit 
        // between save-point & that area has some kind of trigger which swaps model sets, 
        // haven't found a way to change that yet)
        if (lower_name.find("far") == std::string::npos)
          lowModel = false;
    }

    if (!lowModel && SoftFilteredModels.count(area_id))
    {
      auto& area_filters = SoftFilteredModels[area_id];
      lowModel = std::find(area_filters.begin(), area_filters.end(), lower_name) != area_filters.end();
    }

    // if area_id isn't known, use 0 for this to filter it:
    if (!lowModel && SoftFilteredModels.count(0))
    {
      auto& area_filters = SoftFilteredModels[0];
      lowModel = std::find(area_filters.begin(), area_filters.end(), lower_name) != area_filters.end();
      if (lowModel)
        lowModel = lowModel;
    }

#ifdef _DEBUG
    if (!lowModel && strstr(ModelsToSkip, lower_name.c_str()))
      lowModel = true;

    bool clearCulled = false;
    if (clearCulled)
      CulledModels.clear();
#endif

    // Not a low model, return false to disable culling on this
    if (!lowModel)
    {
#ifdef _DEBUG
      if (LogModels)
      {
        std::lock_guard<std::mutex> lock(CulledModelsMutex);
        if (std::find(CulledModels.cbegin(), CulledModels.cend(), lower_name) == CulledModels.cend())
        {
          OutputDebugStringA((lower_name + "\n").c_str());
          CulledModels.push_back(lower_name);
        }
      }
#endif
      return (void*)0;
    }
  }


#ifdef _DEBUG
  if (LogPassedModels)
  {
    std::lock_guard<std::mutex> lock(CulledModelsMutex);
    if (std::find(CulledModels.cbegin(), CulledModels.cend(), lower_name) == CulledModels.cend())
    {
      OutputDebugStringA((GetFullModelName(area_id, lower_name.c_str()) + std::string("\n")).c_str());
      CulledModels.push_back(lower_name);
    }
  }
#endif
  return Model_ShouldBeCulled_Orig((void*)area_id_full, model_name);
}

fn_0args ModelManager__Update_Orig;
void* ModelManager__Update_Hook()
{
  if (!CheckFlag(DBGRAPHIC_FLAG::DBGRAPHIC_HIGH_LOD_FIXED))
    return ModelManager__Update_Orig();

  // Model_LodSetup_Orig checks value of STA flags, and if set it does what we need for DBGRAPHIC_HIGH_LOD_FIXED
  // so we'll temporarily set the flag for that fn to read
  // TODO: sadly nothing like this for LOW_LOD_FIXED to use, might need to reimpl the orig function...
  auto* STA = reinterpret_cast<uint32_t*>(mBaseAddress + Flag_STA_Addr[version]);

  auto origSTA = *STA;
  *STA |= 0x40000000;
  auto ret = ModelManager__Update_Orig();
  *STA = origSTA;

  return ret;
}

TCHAR	szBuffer[65535];
int LoadINIFilterList(const wchar_t* listName, std::unordered_map<int, std::vector<std::string>>& list)
{
  szBuffer[0] = 0;

  int count = 0;

  GetPrivateProfileSection(listName, szBuffer, 65535, IniPath);
  TCHAR* curString = szBuffer;
  for (TCHAR* cp = szBuffer; ; cp++)
  {
    if (!cp[0])
    {
      // end of line
      auto* sep = wcsstr(curString, L"=");
      if (!sep)
        break;
      *sep = L'\0';
      sep++;
      auto* key = curString;
      auto* value = sep;

      std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
      std::string model_name = converter.to_bytes(key);

      std::transform(model_name.begin(), model_name.end(), model_name.begin(),
        [](unsigned char c) { return std::tolower(c); });

      auto area_id = std::stol(std::wstring(value), nullptr, 0) & 0xFFFF;

      if (!list.count(area_id))
        list[area_id] = std::vector<std::string>();

      list[area_id].push_back(model_name);
      count++;

      curString = &cp[1];

      if (!curString[0])
      {
        // end of file
        break;
      }
    }
  }
  return count;
}

void Rebug_Init()
{
  // Load in our filter lists if we have any...
  int numSoft = LoadINIFilterList(L"SoftFilteredModels", SoftFilteredModels);
  int numHard = LoadINIFilterList(L"HardFilteredModels", HardFilteredModels);

  if (Settings.DebugLog)
  {
    dlog(" SoftFilteredModels: %d filters\n", numSoft);
    dlog(" HardFilteredModels: %d filters\n", numHard);
  }

  MH_CreateHook((LPVOID)(mBaseAddress + Model_ShouldBeCulled_Addr[version]), Model_ShouldBeCulled_Hook, (LPVOID*)&Model_ShouldBeCulled_Orig);
  MH_CreateHook((LPVOID)(mBaseAddress + ModelManager__Update_Addr[version]), ModelManager__Update_Hook, (LPVOID*)&ModelManager__Update_Orig);
}
