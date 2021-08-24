#include "pch.h"

const uint32_t cBinaryXml__Read_Addr[] = { 0x14C700, 0x14C530, 0x14C860, 0x816880, 0xC5C130 };

const uint32_t ShadowDistanceReaderAddr[] = { 0x77FEA0, 0x777D70, 0x78E8A0, 0x5513C0, 0x848D30 };

const uint32_t ShadowQualityPatchAddr[] = { 0x772484, 0x76A354, 0x780E84, 0x53A576, 0x835C0F };

const uint32_t ShadowBufferSizePatch1Addr[] = { 0x77F7C5, 0x777695, 0x78E1C5, 0x52FAD3, 0x8474A5 };
const uint32_t ShadowBufferSizePatch2Addr[] = { 0x77F7CB, 0x77769B, 0x78E1CB, 0x52FADB, 0x8474AB };
const uint32_t ShadowBufferSizePatch3Addr[] = { 0x77F7E7, 0x7776B7, 0x78E1E7, 0x52FAEB, 0x8474C7 };
const uint32_t ShadowBufferSizePatch4Addr[] = { 0x77F7ED, 0x7776BD, 0x78E1ED, 0x52FAF3, 0x8474CD };

const uint32_t ShadowBufferSizePatch1Addr2[] = { 0x77F5F7, 0x7774C7, 0x78DFF7, 0x52F3C9, 0x8471F7 };
const uint32_t ShadowBufferSizePatch2Addr2[] = { 0x77F5FD, 0x7774CD, 0x78DFFD, 0x52F3D1, 0x8471FD };
const uint32_t ShadowBufferSizePatch3Addr2[] = { 0x77F619, 0x7774E9, 0x78E019, 0x52F3E1, 0x847219 };
const uint32_t ShadowBufferSizePatch4Addr2[] = { 0x77F61F, 0x7774EF, 0x78E01F, 0x52F3E9, 0x84721F };
// addt patches for Steam/2017
const uint32_t ShadowBufferSizePatch5Addr2[] = { 0, 0, 0, 0x52F412, 0 };
const uint32_t ShadowBufferSizePatch6Addr2[] = { 0, 0, 0, 0x52F41A, 0 };

const uint32_t ShadowModel_HQPatch1Addr[] = { 0x847EDA, 0x83F82A, 0x86EC0A, 0x61AC7D, 0x9CD8B6 };
const uint32_t ShadowModel_HQPatch2Addr[] = { 0x847EF9, 0x83F849, 0x86EC29, 0x61AC9C, 0x9CD8D5 };
const uint32_t ShadowModel_DisableLQPatch1_Addr[] = { 0x847E0D, 0x83F75D, 0x86EB3D, 0x61AB83, 0x9CD830 };

const uint32_t g_HalfShadowMap_SizeAddr[] = { 0x774A21, 0x76C8F1, 0x783421, 0x53BC53, 0x837A28 };

// For validating that game set ShadowBuffSizeBits to what we wanted...
const uint32_t ShadowBuffSizeBits_Addr[] = { 0xF513D0, 0xF443C4, 0xFCF3E4, 0x10517BC, 0x1F4A8A8 };

int ExpectedShadowBuffSizeBits = 1; // to check against ShadowBuffSizeBits_Addr later on

#ifdef _DEBUG
std::vector<std::string> SeenShadowModels;
std::mutex SeenShadowModelsMutex;
bool LogShadowModels = false;
char ShadowModelsToSkip[16384] = { 0 };
#endif

std::unordered_map<int, std::vector<std::string>> ShadowForceAllFilters;

void BehaviorScr::SetCastShadows(bool castsShadows)
{
  // Debug build adds 0x30 bytes to class members somewhere, it's before the ones we touch though, so just add 0x30 if needed
  int members_offset = version == GameVersion::Debug2017 ? 0x30 : 0;

  auto* shadowList = (cModel*)(((uint8_t*)&this->ModelInfo) + members_offset);
  if (!shadowList->EntryCount)
    return;

  for (int i = 0; i < shadowList->EntryCount; i++)
  {
    auto flags = shadowList->Entries[i].Flags;
    if (castsShadows)
      flags = flags | 1;
    else
      flags = flags & ~1;
    shadowList->Entries[i].Flags = flags;
  }

  if (!castsShadows)
    return;

  int areaid_offset = version == GameVersion::Debug2017 ? 0x40 : 0;
  uint32_t areaId = *(uint32_t*)(((uint8_t*)&this->AreaId) + areaid_offset) & 0xFFFF;

  // Make sure we don't enable ShadowCast on "ENKEI" model
  // Otherwise shadow covers almost entire city ruins area in route C...

#ifndef _DEBUG
  // release ver optimization, only run loops if this is correct area
  if (ShadowForceAllFilters.count(areaId))
#endif
  {
    // Loop through the Model/ShadowModel pairs, check if any should be filtered
    if (shadowList->Unk8 && shadowList->Unk8->NumUnkA0)
    {
      for (int i = 0; i < shadowList->Unk8->NumUnkA0; i++)
      {
        auto* unkA0 = &shadowList->Unk8->UnkA0[i];
        if (unkA0 && unkA0->NumShadowModelPairs)
        {
          for (int y = 0; y < unkA0->NumShadowModelPairs; y++)
          {
            auto* pair = &unkA0->ShadowModelPairs[y];
            if (pair->ModelIndex == -1 || pair->ShadowModelIndex == -1)
              continue;
            if (pair->ModelIndex >= shadowList->ModelEntryCount)
              continue;

            auto* model = &shadowList->ModelEntries[pair->ModelIndex];
            auto* modelName = &model->ModelName;

            // debug2017 build adds 0x10 bytes to ModelEntry struct, so add that offset to our ptr...
            if (version == GameVersion::Debug2017)
              modelName = (const char**)((uint8_t*)modelName + (pair->ModelIndex * 0x10));

            auto* shadowInfo = &shadowList->Entries[pair->ShadowModelIndex];

#ifdef _DEBUG
            std::string name = *modelName;
            std::string lower_name = name;
            std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
              [](unsigned char c) { return std::tolower(c); });

            if (LogShadowModels)
            {
              std::lock_guard<std::mutex> lock(SeenShadowModelsMutex);
              if (std::find(SeenShadowModels.cbegin(), SeenShadowModels.cend(), lower_name) == SeenShadowModels.cend())
              {
                OutputDebugStringA((lower_name + "\n").c_str());
                SeenShadowModels.push_back(lower_name);
              }
            }

            if (strstr(ShadowModelsToSkip, lower_name.c_str()))
              shadowInfo->Flags &= ~1;
#endif

            auto& filters = ShadowForceAllFilters[areaId];
            for(auto& filter : filters)
              if(!_stricmp(filter.c_str(), *modelName))
                shadowInfo->Flags &= ~1;

            //if (!strcmp(*modelName, "ENKEI") || (areaIdCoords == 0x1118 && !strcmp(*modelName, "GROUND_g11118jimen")))
          }
        }
      }
    }
  }
}

void UpdateShadowResolution(int resolution)
{
  // game uses shifts by 0xB/0xA of a value patched below to determine resolution used to setup shadow texture buffers
  // normally is always set to 1 (so 1 << 0xB / 2048), but there does seem to be some resolution checks nearby that can change it to 4 (8192)
  // that only seems to be for 4k resolutions though, and is maybe incomplete since shadow quadrant sizes don't get updated for it, leaving the increased resolution space unused(?)
  int value = resolution >> 0xB;

  // shadows can't go any lower than 2048
  if (value <= 0)
    return;

  uint8_t ShadowQualityPatch[] = { 0xB9, 0x04, 0x00, 0x00, 0x00, 0x90 };
  *(uint32_t*)(&ShadowQualityPatch[1]) = value;
  SafeWrite(GameAddress(ShadowQualityPatchAddr), ShadowQualityPatch, 6);

  // Patch out the 3840x2160/3200x1800 ShadowQuality = 4 code
  if (version == GameVersion::Steam2017)
  {
    uint8_t MovR8dEcx[] = { 0x41, 0x89, 0xC8, 0x90, 0x90, 0x90 };
    SafeWrite(GameAddress(ShadowQualityPatchAddr + 10), MovR8dEcx, 6);
  }
  else if (version == GameVersion::Debug2017)
  {
    uint8_t MovEdxEcx[] = { 0x89, 0xCA, 0x90, 0x90, 0x90 };
    SafeWrite(GameAddress(ShadowQualityPatchAddr + 10), MovEdxEcx, 5);
  }
  else
  {
    uint8_t MovEaxEcx[] = { 0x89, 0xC8, 0x90, 0x90, 0x90 };
    SafeWrite(GameAddress(ShadowQualityPatchAddr + 10), MovEaxEcx, 5);
  }

  ExpectedShadowBuffSizeBits = value;

  // Size of each quadrant in shadowmap
  int shadowQuadSize = resolution / 2;

  dlog("Shadow quadrant size: %d\n", shadowQuadSize);

  dlog("ShadowBuffSizeBits: %d\n", ExpectedShadowBuffSizeBits);

  // Poor mans lzcnt...
  int tempSize = shadowQuadSize;
  int shadowNumBits = 0;
  while (tempSize)
  {
    tempSize /= 2;
    shadowNumBits++;
  }
  shadowNumBits--;

  dlog("ShadowResNumBits: %d\n", shadowNumBits);

  // Update shadow quadrant sizes
  SafeWrite(GameAddress(ShadowBufferSizePatch1Addr), uint8_t(shadowNumBits));
  SafeWrite(GameAddress(ShadowBufferSizePatch2Addr), uint8_t(shadowNumBits));
  SafeWrite(GameAddress(ShadowBufferSizePatch3Addr), uint32_t(shadowQuadSize));
  SafeWrite(GameAddress(ShadowBufferSizePatch4Addr), uint32_t(shadowQuadSize));

  SafeWrite(GameAddress(ShadowBufferSizePatch1Addr2), uint8_t(shadowNumBits));
  SafeWrite(GameAddress(ShadowBufferSizePatch2Addr2), uint8_t(shadowNumBits));
  SafeWrite(GameAddress(ShadowBufferSizePatch3Addr2), uint32_t(shadowQuadSize));
  SafeWrite(GameAddress(ShadowBufferSizePatch4Addr2), uint32_t(shadowQuadSize));

  // g_HalfShadowMap size needs to be half of shadow buffer size too, else god rays will break
  SafeWrite(GameAddress(g_HalfShadowMap_SizeAddr), uint32_t(shadowQuadSize));
  if (version == GameVersion::Steam2017)
  {
    // special case for inlined CreateTextureBuffer
    SafeWrite(GameAddress(g_HalfShadowMap_SizeAddr + 7), uint32_t(shadowQuadSize));

    SafeWrite(GameAddress(ShadowBufferSizePatch5Addr2), uint32_t(shadowQuadSize));
    SafeWrite(GameAddress(ShadowBufferSizePatch6Addr2), uint32_t(shadowQuadSize));
  }

  dlog("Shadow resolution patched to %dx%d.\n", resolution, resolution);
}

bool CheckedShadowBuffSizeBits = false;
void UpdateShadowParams(cShadowParam* shadowParam)
{
  // Use this opportunity to check that game set the ShadowBuffSizeBits to what we asked (ExpectedShadowBuffSizeBits)
  // If it's not, that likely means we were injected into the game after the shadow-init code has been ran....
  if (!CheckedShadowBuffSizeBits && Settings.DebugLog && ExpectedShadowBuffSizeBits > 0)
  {
    auto ShadowBuffSizeBits = *GameAddress<uint32_t*>(ShadowBuffSizeBits_Addr);
    dlog("ShadowDistanceReader_Hook: ShadowBuffSizeBits = %d, ExpectedShadowBuffSizeBits = %d\n", ShadowBuffSizeBits, ExpectedShadowBuffSizeBits);
    if (ExpectedShadowBuffSizeBits != ShadowBuffSizeBits)
    {
      dlog("\nError: games current ShadowBuffSizeBits (%d) doesn't match the value we tried to set (%d)!\n", ShadowBuffSizeBits, ExpectedShadowBuffSizeBits);
      dlog("This will likely mean shadow resolution won't be updated properly, probably resulting in strange artifacts!\n");
      dlog("(this might be caused by LodMod being injected into the game _after_ shadow-init code has been ran - maybe try a different inject method)\n");
      dlog("If using SpecialK's Import feature to load in LodMod maybe using 'When=Lazy' can help.\n\n");

      // Revert shadow resolution changes, since the updated shadow-quadrant code will mess up shadows in this state
      // (even though this state is mostly caused by user-error, it's probably not a good idea for LM to break games in any case)
      dlog("Reverting shadow resolution to game default:\n");
      UpdateShadowResolution(ShadowBuffSizeBits << 0xB);
    }

    CheckedShadowBuffSizeBits = true;
  }

  float new_strength = shadowParam->m_bokeStrength[0];
  if (Settings.ShadowFilterStrengthBias != 0)
  {
    new_strength = shadowParam->m_bokeStrength[0] + Settings.ShadowFilterStrengthBias;
    if (new_strength < 0)
      new_strength = 0;
  }

  if (new_strength < Settings.ShadowFilterStrengthMinimum)
    new_strength = Settings.ShadowFilterStrengthMinimum;

  if (Settings.ShadowFilterStrengthMaximum != 0 && new_strength > Settings.ShadowFilterStrengthMaximum)
    new_strength = Settings.ShadowFilterStrengthMaximum;

  if (new_strength != shadowParam->m_bokeStrength[0])
  {
    if (Settings.DebugLog)
    {
      dlog("UpdateShadowParams: Updated shadow m_bokeStrength from %f to %f\n", shadowParam->m_bokeStrength[0], new_strength);
    }
    shadowParam->m_bokeStrength[0] = new_strength;
  }

  auto& distances = shadowParam->m_splitPoint;
  float new_distances[4] = { distances[0], 0, 0, 0 };

  if (Settings.ShadowDistanceMultiplier != 1)
    new_distances[0] = distances[0] * Settings.ShadowDistanceMultiplier;

  if (Settings.ShadowDistanceMinimum > 0 && new_distances[0] < Settings.ShadowDistanceMinimum)
    new_distances[0] = Settings.ShadowDistanceMinimum;

  if (Settings.ShadowDistanceMaximum > 0 && new_distances[0] > Settings.ShadowDistanceMaximum)
    new_distances[0] = Settings.ShadowDistanceMaximum;

  if (new_distances[0] != distances[0])
  {
    // figure out the old cascade ratios
    // (this is only run when distance is being updated first time for this area)

    float ratios[] = {
      distances[1] / distances[0],
      distances[2] / distances[0],
      distances[3] / distances[0]
    };

    new_distances[1] = new_distances[0] * ratios[0];
    new_distances[2] = new_distances[0] * ratios[1];
    new_distances[3] = new_distances[0] * ratios[2];

    if (Settings.DebugLog)
    {
      dlog("UpdateShadowParams: Updating shadow m_splitPoint\n 0: %f -> %f\n 1: %f -> %f\n 2: %f -> %f\n 3: %f -> %f\n",
        distances[0], new_distances[0],
        distances[1], new_distances[1],
        distances[2], new_distances[2],
        distances[3], new_distances[3]);
    }

    std::copy_n(new_distances, 4, distances);
  }
}

#ifdef _DEBUG
std::unordered_map<uintptr_t, cShadowParam> origShadowParams;
#endif

// likely not proper fn name
typedef void* (*cBinaryXml__Read_Fn)(struct cBinaryXml* thisptr, uint32_t a2, cObject* output);
cBinaryXml__Read_Fn cBinaryXml__Read_Orig;
void* cBinaryXml__Read_Hook(struct cBinaryXml* thisptr, uint32_t a2, cObject* output)
{
  auto result = cBinaryXml__Read_Orig(thisptr, a2, output);

  // TODO: is there a better way than string compare?
  if (strcmp(output->GetData()->name, "cShadowParam"))
    return result;

  auto* shadowParam = reinterpret_cast<cShadowParam*>(output);

#ifndef _DEBUG
  UpdateShadowParams(shadowParam);
#endif

  return result;
}

#ifdef _DEBUG
// Hook per-frame shadow update func so we can capture the current shadow params being used

typedef void* (*ShadowDistanceReader_Fn)(cShadowParam* thisptr, void* a2, void* a3, void* a4);
ShadowDistanceReader_Fn ShadowDistanceReader_Orig;

cShadowParam* g_curShadowParam = nullptr;
void* ShadowDistanceReader_Hook(cShadowParam* thisptr, void* a2, void* a3, void* a4)
{
  if (g_curShadowParam != thisptr)
    g_curShadowParam = thisptr;

  // Restore original shadow params if we have them, else make a copy of them
  if (origShadowParams.count(uintptr_t(thisptr)))
    *thisptr = origShadowParams[uintptr_t(thisptr)];
  else
    origShadowParams[uintptr_t(thisptr)] = *thisptr;

  UpdateShadowParams(thisptr);

  return ShadowDistanceReader_Orig(thisptr, a2, a3, a4);
}
#endif

void ShadowFixes_Init()
{
  MH_CreateHook(GameAddress<LPVOID>(cBinaryXml__Read_Addr), cBinaryXml__Read_Hook, (LPVOID*)&cBinaryXml__Read_Orig);

#ifdef _DEBUG
  if(version != GameVersion::Steam2017) // ShadowDistanceReader func is weird in steam2017
    MH_CreateHook(GameAddress<LPVOID>(ShadowDistanceReaderAddr), ShadowDistanceReader_Hook, (LPVOID*)&ShadowDistanceReader_Orig);
#endif

  UpdateShadowResolution(Settings.ShadowResolution);

  if (Settings.ShadowModelForceAll)
  {
    // Patch out checks inside cModelShaderModule so more models can cast shadows
    // (not totally sure what the code this patches is checking, either something to do with LOD level, or maybe a "this->ShadowsDisabled" check of some kind)
    SafeWrite(GameAddress(ShadowModel_HQPatch1Addr), uint16_t(0x9090));

    // Load in shadow filters...
    int LoadINIFilterList(const wchar_t* listName, std::unordered_map<int, std::vector<std::string>>&list); // Rebug.cpp
    int count = LoadINIFilterList(L"ShadowForceAllFilters", ShadowForceAllFilters);
    dlog(" ShadowForceAllFilters: %d filters\n", count);
  }

  if (Settings.ShadowModelHQ)
  {
    // Patching this seems to allow moving objects like trees to cast updated shadows (from swaying around)
    // Unfortunately the shadow-LOD version of these objects will also still get rendered, so we need the patch after this to disable those
    SafeWrite(GameAddress(ShadowModel_HQPatch2Addr), uint16_t(0x9090));

    // Disable LQ shadow model from being rendered, since we're now using HQ version above
    SafeWrite(GameAddress(ShadowModel_DisableLQPatch1_Addr), uint16_t(0xE990));
    // alternate method:
    //SafeWrite(GameAddress(ShadowModel_DisableLQPatch1_Addr), uint16_t(0x840F));
    //SafeWrite(GameAddress(ShadowModel_DisableLQPatch1_Addr + 0x10), uint16_t(0x850F));
  }
}
