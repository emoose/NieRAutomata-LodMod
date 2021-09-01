#include "pch.h"

const uint32_t Game__ShowMessageBoxVA_Addr[] = { 0x26B890, 0x265FD0, 0x26BB20, 0x9204F0, 0xE48770 };

// Game includes some msgs in japanese, mostly error msgs but debug has some interesting stuff
// We have the ability to translate them, so why not?

const char* messageBoxCaption = "NieR: Automata Error"; // originally "HWライブラリエラー" / "HW Library Error"
const WCHAR* messageBoxCaption_wide = L"NieR: Automata Error";

std::unordered_map<std::u8string, std::string> translations =
{
	// Error messages
	{u8"メモリ不足が発生(%s)", "ERROR: Ran out of memory while loading %s."},
	{u8"ESP: 頂点シェーダーが存在しません。\n%s", "ESP: Vertex shader does not exist.\n%s"},
	{u8"ESP: ピクセルシェーダーが存在しません。\n%s", "ESP: Pixel shader does not exist.\n%s"},
	{u8"ESP: コンピュートシェーダーが存在しません。\n%s", "ESP: Compute shader does not exist.\n%s"},
	{u8"[HW::StartupWindow] 更新スレッドの作成に失敗しました。", "[HW::StartupWindow] Failed to create update thread."},
	{u8"[HW::StartupWindow] ウィンドウクラスの登録に失敗しました。", "[HW::StartupWindow] Failed to register the window class."},
	{u8"[HW::StartupWindow] ウィンドウの作成に失敗しました。", "[HW::StartupWindow] Failed to create window."},
	{u8"不正なタスク移譲が発生しました。", "An invalid task transfer occurred."},
	{u8"不正なタスク休止が発生しました。", "An invalid task pause occurred."},
	{u8"不正なタスク終了が発生しました。", "An invalid task termination occurred."},
	{u8"タスクマネージャーの初期化に失敗しました。", "Failed to initialize Task Manager."},
	{u8"不正なタスクムーブが発生しました。", "An invalid task move occurred."},
	{u8"不正なタスク消滅が発生しました。", "An invalid task disappearance."},
	{u8"不正なタスク起動が発生しました。", "An invalid task start occurred."},
	{u8"タスクのワークが不足しました。", "Insufficient work for the task."},
	{u8"ジョブマネージャーの初期化に失敗しました。", "Failed to initialize the Job Manager."},
	{u8"スレッド環境の初期化に失敗しました。", "Failed to initialize the thread environment."},
	{u8"スレッドの作成に失敗しました。", "Failed to create a thread."},
	{u8"[Hw::rofs::fst] ファイルサイズテーブルの作成に失敗しました", "[Hw::rofs::fst] Failed to create file size table."},
	{u8"初期化されていない列挙オブジェクトを実行しました。", "Executed an uninitialized enumeration object."},
	{u8"cStringDevice::create() : StringDeviceの初期化に失敗しました。", "cStringDevice::create() : Failed to initialize StringDevice."},
	{u8"cStringDevice::createCharSetManager() : 文字セットマネージャの生成に失敗しました。", "cStringDevice::createCharSetManager() : Failed to create the character set manager."},
	{u8"ERROR : 必要最低限のシェーダの初期化に失敗", "ERROR: Failed to initialize the minimum required shaders."},

	// Debug build error msgs
	{u8"[Hw::rofs::fst] コンテンツファイルサイズテーブルの作成に失敗しました。", "[Hw::rofs::fst] Failed to create contents file size table."},
	{u8"[Hw::Exception] 例外シンボルバッファの取得に失敗しました。", "[Hw::Exception] Failed to get exception symbol buffer."},
	{u8"Steamアカウントがゲームをプレイする権限を持っていません", "Your Steam account does not have permission to play the game."},
	{u8"ESP: シェーダーが存在しません。\n%s", "ESP: Shader does not exist.\n%s"},
	{u8"初期化されていない検索オブジェクトを実行しました。", "An uninitialized search object was executed."},

	// TODO: Debug menu

	// TODO: Debug log-messages?
};

std::unordered_map<std::string, std::string> translations_fixed;

std::string sj2utf8(const std::string& input);

typedef void(*Game__ShowMessageBoxVA_Fn)(const char* Format, va_list ArgList);

Game__ShowMessageBoxVA_Fn Game__ShowMessageBoxVA_Orig;
void Game__ShowMessageBoxVA_Hook(const char* Format, va_list ArgList)
{
	// Reimplement the function, because it uses untranslated msgbox caption which is hard to patch
	// (we can also fix it to display JP characters properly too :)

	// First convert input from SJIS to UTF8, for comparison with our map...
	auto converted = sj2utf8(Format);

	const char* format = converted.c_str();
	if (translations_fixed.count(converted.c_str()))
		format = translations_fixed[converted].c_str();

	char text[2048];
	vsprintf_s(text, format, ArgList);

	// Then convert UTF8 to UCS-2 so MessageBoxW can display it for us
	int wide_length = MultiByteToWideChar(CP_UTF8, 0, text, 2048, NULL, 0);
	if (wide_length)
	{
		auto wide_ptr = std::make_unique<WCHAR[]>(wide_length);
		if (MultiByteToWideChar(CP_UTF8, 0, text, 2048, wide_ptr.get(), wide_length) == wide_length)
		{
			MessageBoxW(0, wide_ptr.get(), messageBoxCaption_wide, 0x40000);
			return;
		}
	}

	// Something failed, fall back to MessageBoxA
	MessageBoxA(0, text, messageBoxCaption, 0x40000);
}

void Translate_Init()
{
	// Need to create a new map at runtime because of stupid C++20 restriction on initing std::string with u8...
	// (can also be solved by casting to char*, but that's needed for every u8 string inited >.>)
	for (auto& kvp : translations)
		translations_fixed[(char*)kvp.first.c_str()] = kvp.second;

	// Force error to display (win10)
	// SafeWriteModule(0x283E26, uint16_t(0x9090));

	MH_CreateHook(GameAddress<LPVOID>(Game__ShowMessageBoxVA_Addr), Game__ShowMessageBoxVA_Hook, (LPVOID*)&Game__ShowMessageBoxVA_Orig);
}

// https://stackoverflow.com/questions/33165171/c-shiftjis-to-utf8-conversion
#include "sjis_table.h"
std::string sj2utf8(const std::string& input)
{
	std::string output(3 * input.length(), ' '); //ShiftJis won't give 4byte UTF8, so max. 3 byte per input char are needed
	size_t indexInput = 0, indexOutput = 0;

	while (indexInput < input.length())
	{
		char arraySection = ((uint8_t)input[indexInput]) >> 4;

		size_t arrayOffset;
		if (arraySection == 0x8) arrayOffset = 0x100; //these are two-byte shiftjis
		else if (arraySection == 0x9) arrayOffset = 0x1100;
		else if (arraySection == 0xE) arrayOffset = 0x2100;
		else arrayOffset = 0; //this is one byte shiftjis

		//determining real array offset
		if (arrayOffset)
		{
			arrayOffset += (((uint8_t)input[indexInput]) & 0xf) << 8;
			indexInput++;
			if (indexInput >= input.length()) break;
		}
		arrayOffset += (uint8_t)input[indexInput++];
		arrayOffset <<= 1;

		//unicode number is...
		uint16_t unicodeValue = (convTable[arrayOffset] << 8) | convTable[arrayOffset + 1];

		//converting to UTF8
		if (unicodeValue < 0x80)
		{
			output[indexOutput++] = unicodeValue;
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
