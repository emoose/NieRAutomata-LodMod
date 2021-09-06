#include "pch.h"

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s)
{
  return rtrim(ltrim(s));
}

std::string utf8_encode(const std::wstring& wstr)
{
  if (wstr.empty()) return std::string();
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
  return strTo;
}

bool INI_GetBool(const WCHAR* IniPath, const WCHAR* Section, const WCHAR* Key, bool DefaultValue)
{
  WCHAR IniData[256];
  bool retVal = false;
  if (GetPrivateProfileString(Section, Key, DefaultValue ? L"true" : L"false", IniData, 256, IniPath) > 0)
    retVal = (wcscmp(IniData, L"true") == 0 || wcscmp(IniData, L"1") == 0 || wcscmp(IniData, L"yes") == 0);
  return retVal;
}

float INI_GetFloat(const WCHAR* IniPath, const WCHAR* Section, const WCHAR* Key, float DefaultValue)
{
  WCHAR IniData[256];
  float retVal = false;
  if (GetPrivateProfileString(Section, Key, std::to_wstring(DefaultValue).c_str(), IniData, 256, IniPath) > 0)
    retVal = std::stof(IniData);
  return retVal;
}

bool FileExists(const WCHAR* Filename)
{
  if (wcslen(Filename) <= 0)
    return false;

  DWORD dwAttrib = GetFileAttributesW(Filename);
  return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool DirExists(const WCHAR* DirPath)
{
  DWORD dwAttrib = GetFileAttributesW(DirPath);
  return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

bool GetModuleName(HMODULE module, WCHAR* destBuf, int bufLength)
{
  // Get folder path of currently running EXE
  GetModuleFileName(module, destBuf, bufLength);
  size_t len = wcslen(destBuf);
  size_t lastPathSep = 0;
  for (int64_t i = len - 2; i >= 0; i--)
  {
    if (destBuf[i] == '\\' || destBuf[i] == '/')
    {
      lastPathSep = i;
      break;
    }
  }

  if (lastPathSep > 0)
  {
    wcscpy_s(destBuf, bufLength, destBuf + lastPathSep + 1);
    return true;
  }
  return false;
}

bool GetModuleFolder(HMODULE module, WCHAR* destBuf, int bufLength)
{
  // Get folder path of currently running EXE
  GetModuleFileName(module, destBuf, bufLength);
  size_t len = wcslen(destBuf);
  size_t lastPathSep = 0;
  for (int64_t i = len - 2; i >= 0; i--)
  {
    if (destBuf[i] == '\\' || destBuf[i] == '/')
    {
      lastPathSep = i;
      break;
    }
  }

  if (lastPathSep > 0)
  {
    destBuf[lastPathSep + 1] = 0;
    return true;
  }
  return false;
}

// Code to get main window HWND from https://stackoverflow.com/questions/1888863/how-to-get-main-window-handle-from-process-id
struct handle_data {
  DWORD process_id;
  HWND window_handle;
};

BOOL is_main_window(HWND handle)
{
  return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
  handle_data& data = *(handle_data*)lParam;
  if (data.window_handle)
    return TRUE;

  DWORD process_id = 0;
  GetWindowThreadProcessId(handle, &process_id);
  if (data.process_id != process_id || !is_main_window(handle))
    return TRUE;

  data.window_handle = handle;
  return FALSE;
}

HWND FindMainWindow(DWORD process_id)
{
  handle_data data;
  data.process_id = process_id;
  data.window_handle = 0;

  EnumWindows(enum_windows_callback, (LPARAM)&data);
  return data.window_handle;
}

void dlog(const char* Format, ...)
{
  if (!Settings.DebugLog)
    return;

  char* str = new char[4096];
  va_list ap;
  va_start(ap, Format);

  vsnprintf(str, 4096, Format, ap);
  va_end(ap);

  std::ofstream file;
  file.open(LogPath, std::ofstream::out | std::ofstream::app);
  if (!file.is_open())
    return; // wtf

  file << str;

  file.close();
}

// https://stackoverflow.com/questions/33165171/c-shiftjis-to-utf8-conversion
#include "third_party/sjis_table.h"
std::string sj2utf8(const std::string& input)
{
  std::string output(3 * input.length(), ' '); //ShiftJis won't give 4byte UTF8, so max. 3 byte per input char are needed
  size_t indexInput = 0, indexOutput = 0;

  while (indexInput < input.length())
  {
    char arraySection = (uint8_t)(input[indexInput]) >> 4;

    size_t arrayOffset;
    if (arraySection == 0x8) arrayOffset = 0x100; //these are two-byte shiftjis
    else if (arraySection == 0x9) arrayOffset = 0x1100;
    else if (arraySection == 0xE) arrayOffset = 0x2100;
    else arrayOffset = 0; //this is one byte shiftjis

    //determining real array offset
    if (arrayOffset)
    {
      arrayOffset += size_t(uint8_t(input[indexInput]) & 0xf) << 8;
      indexInput++;
      if (indexInput >= input.length()) break;
    }
    arrayOffset += uint8_t(input[indexInput++]);
    arrayOffset <<= 1;

    //unicode number is...
    uint16_t unicodeValue = (convTable[arrayOffset] << 8) | convTable[arrayOffset + 1];

    //converting to UTF8
    if (unicodeValue < 0x80)
    {
      output[indexOutput++] = char(unicodeValue);
    }
    else if (unicodeValue < 0x800)
    {
      output[indexOutput++] = 0xC0 | (unicodeValue >> 6);
      output[indexOutput++] = 0x80 | (unicodeValue & 0x3f);
    }
    else
    {
      output[indexOutput++] = 0xE0 | (unicodeValue >> 12);
      output[indexOutput++] = 0x80 | ((unicodeValue & 0xfff) >> 6);
      output[indexOutput++] = 0x80 | (unicodeValue & 0x3f);
    }
  }

  output.resize(indexOutput); //remove the unnecessary bytes
  return output;
}
