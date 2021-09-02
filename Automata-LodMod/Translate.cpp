﻿#include "pch.h"

const uint32_t Game__ShowMessageBoxVA_Addr[] = { 0x26B890, 0x265FD0, 0x26BB20, 0x9204F0, 0xE48770 };

// Game includes some msgs in japanese, mostly error msgs but debug has some interesting stuff
// We have the ability to translate them, so why not?

const char* messageBoxCaption = "NieR: Automata Error"; // originally "HWライブラリエラー" / "HW Library Error"
const WCHAR* messageBoxCaption_wide = L"NieR: Automata Error";

#pragma pack(push, 1)
struct Debug_PlayerMenu
{
	struct
	{
		uint32_t category_index;
		uint64_t flag_index;
		char name[0x100];
		char description[0x200];
	} Flags[36];
};
#pragma pack(pop)

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
	{u8"タスクマネージャーの初期化に失敗しました。", "Failed to initialize the Task Manager."},
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
	{u8"現在P%03X:%s", "Currently P%03X:%s"},
	{u8"G%05x:R%03x  シームレスマップ:%s", "G%05x:R%03x Seamless map:%s"},

	// Guessing Muteki means I-Frames, not 100% certain though
	{u8"T[%5.2f, %5.2f, %5.2f], R[%5.2f(%5.2f)], Rno[%d,%d,%d,%d], HP[ %d, %d ], Muteki[%d], Lv[%d], Beat[%d], 時速[%.2f]", 
	   "T[%5.2f, %5.2f, %5.2f], R[%5.2f(%5.2f)], Rno[%d,%d,%d,%d], HP[ %d, %d ], I-Frames[%d], Lv[%d], Beat[%d], Speed[%.2f]"},
	{u8"水に入っている", "On Water"}, // standing on water, draws on top of above string tho...
	{u8"オイルに入っている", "On Oil"}, // standing on oil

	// 14091CAC0
	{u8"---イベント状況---", "---Event Status---"},
	{u8"読み込み中", "Loading"},
	{u8"再生開始待ち", "Waiting for playback"},
	{u8"再生中", "Now Playing"},
	{u8"フェーズイベント再生中：p%03x ev%04x cut%02d", "Playing phase event: p%03x ev%04x cut%02d"},
	{u8"ムービー再生中：ev%04x cut%02d", "Playing movie: ev%04x cut%02d"},
	{u8"通常イベント再生中：ev%04x cut%02d", "Playing normal event: ev%04x cut%02d"},
	{u8"開放中", "Opening"},
	{u8"エラー", "Error"},
	{u8"スキップ可能", "Skippable"},

	// next are from 1407C3620
	{u8"総モデル数　:%d\n", "Model count:%d\n"},
	{u8"総メッシュ数:%d\n", "Mesh count: %d\n"},
	{u8"総関節数　　:%d\n", "Joint count:%d\n"},
	{u8"総ポリゴン数:%d(カメラ内：%d)\n", "Polygon count: %d (in camera: %d)\n"},
	{u8"総BAT数 :%d(カメラ内：%d)\n", "BAT count: %d (in camera: %d)\n"},
	{u8"カテゴリ  モデル数  メッシュ数  BAT数    ポリゴン数  関節数  表示モデル数  表示メッシュ数  表示BAT数  表示ポリゴン数  影落とし数\n",
		 "Categ. Models Meshes BATs Polygons Joints VisModls VisMesh VisBATs VisPolys  ShadowsDropped\n"},
	{u8"SC・BA・BHモデルが多すぎます(合計%d)\n", "Too many SC, BA, and BH models (total %d)\n"},
	{u8"SC・BA・BHモデルが多いです(合計%d)\n", "Too many SC, BA, BH models (total %d)\n"},
	{u8"総ポリゴン数が多すぎます(%d)\n", "Total polygon count is too high (%d)\n"},
	{u8"総ポリゴン数が多いです(%d)\n", "Total polygon count is too high (%d)\n"},
	{u8"表示マテリアル数が多すぎます(%d)\n", "Number of display materials is too large (%d)\n"},
	{u8"表示マテリアル数が多いです(%d)\n", "Too many materials to display (%d)\n"},
	{u8"敵の数が多いです(%d)\n", "Too many enemies (%d)\n"},
	{u8"総ポリゴン数:%d\n", "Total polygons:%d\n"},
	{u8"[%s 詳細表示]\n", "[%s Detail View]\n"},
	{u8"[R%03x %s 詳細表示]\n", "[R%03x %s Detail View]\n"},

	{u8"カーソルキー↑↓：選択\n", "\x81\xAA\x81\xAB: Select\n"},
	{u8"カーソルキー←→：部屋選択\n", "\x81\xA9\x81\xA8: Select room\n"},
	{u8"カーソルキー→　：選択項目の詳細表示\n", "\x81\xA8: Display details of selected item\n"},
	{u8"↑↓　　　　　　：選択\n", "\x81\xAA\x81\xAB: Select\n"},
	{u8"ＳＨＩＦＴ＋↑↓：加速\n", "SHIFT+\x81\xAA\x81\xAB: Accelerate\n"},
	{u8"ＲＥＴＵＲＮ　　：モデル位置に移動\n", "RETURN: Go to model position\n"},
	{u8"ＲＥＴＵＲＮ　　：次のモデルへ\n", "RETURN: Go to next model\n"},
	{u8"ＳＰＡＣＥ　　　：一覧表示モードへ\n", "SPACE: Go to list view mode\n"},
	{u8"ＳＰＡＣＥ　　　：ポリゴン数詳細モードへ\n", "SPACE: Go to polygon number detail mode\n"},
	{u8"←→　　　　　　：部屋選択\n", "\x81\xA9\x81\xA8: Select room\n"},
	{u8"←　　　　　　　：トップに戻る\n", "\x81\xA9: Return to the top menu\n"},
	{u8"ＨＯＭＥ　　　　：カメラをプレイヤーに戻す\n", "HOME: Return camera to the player\n"},

	{u8"十字キー,↓↑:カーソル移動\n", "\x81\xAB\x81\xAA: Move Cursor\n"},
	{u8"十字キー,←→:ツリー解除／展開\n", "\x81\xA9\x81\xA8: Expand/Retract\n"},

	{u8"　　モデル名　　　　　　　　　　 モデル数　 メッシュ数 表示ポリ数　　LOD0[数]    LOD1 　　   LOD2　　   LOD3　　　影     影描画数\n",
		 "  Model name      Models Meshes Plys LOD0[num]LOD1     LOD2     LOD3   Shadow    DrawnShadows\n"},
	{u8"  モデル名            モデル数  メッシュ数  BAT数    ポリゴン数  関節数  表示モデル数  表示メッシュ数  表示BAT数  表示ポリゴン数\n",
		 "  Model name       Models Meshes BATs   Polys Joints VisModls VisMeshs VisBATs VisiPolygons\n"}
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
		auto wide_ptr = std::make_unique<WCHAR[]>(wide_length + 1);
		if (MultiByteToWideChar(CP_UTF8, 0, text, 2048, wide_ptr.get(), wide_length) == wide_length)
		{
			wide_ptr.get()[wide_length] = 0; // null-terminate because MBTWC doesn't always do it?
			MessageBoxW(0, wide_ptr.get(), messageBoxCaption_wide, MB_TOPMOST);
			return;
		}
	}

	// Something failed, fall back to MessageBoxA
	MessageBoxA(0, text, messageBoxCaption, MB_TOPMOST);

	// Game normally writes the error to message_box.txt after this too, but that's kinda pointless when you can CTRL+C messageboxes, so meh...
}

typedef void(*Debug_PrintToConsole_Fn)(const char* Format, ...);
Debug_PrintToConsole_Fn Debug_PrintToConsole_Orig;
void Debug_PrintToConsole_Hook(const char* Format, ...)
{
	va_list va;
	va_start(va, Format);

	uint32_t* shouldPrintToConsole = GameAddress<uint32_t*>(0x20104D0);
	if (!*shouldPrintToConsole)
		return;

	// First convert input from SJIS to UTF8, for comparison with our map...
	auto converted = sj2utf8(Format);
	
	const char* format = Format;
	if(translations_fixed.count(converted.c_str()))
		format = translations_fixed[converted].c_str();

	char text_buf[2048] = { 0 };
	memset(text_buf, 0, 2048);
	vsprintf_s(text_buf, format, va);

	// Convert the vsprintf'd output, since they might have used shift-jis in a parameter
	auto converted2 = sj2utf8(text_buf);
	if (translations_fixed.count(converted2.c_str()))
		converted2 = translations_fixed[converted2].c_str();

	// Then convert UTF8 to UCS-2 so OutputDebugStringW can display it for us
	int wide_length = MultiByteToWideChar(CP_UTF8, 0, converted2.c_str(), converted2.length(), NULL, 0);
	if (wide_length)
	{
		auto wide_ptr = std::make_unique<WCHAR[]>(wide_length + 1);
		if (MultiByteToWideChar(CP_UTF8, 0, converted2.c_str(), converted2.length(), wide_ptr.get(), wide_length) == wide_length)
		{
			wide_ptr.get()[wide_length] = 0; // null-terminate because MBTWC doesn't always do it?
			OutputDebugStringW(wide_ptr.get());
			OutputDebugStringW(L"\n");
			return;
		}
	}

	// Something failed, fall back to OutputDebugStringA
	OutputDebugStringA(converted2.c_str());
	OutputDebugStringA("\n");
}

typedef void(*Debug_PlayerMenuPopulate_Fn)();
Debug_PlayerMenuPopulate_Fn Debug_PlayerMenuPopulate_Orig;
void Debug_PlayerMenuPopulate_Hook()
{
	// Let game set it up for us...
	Debug_PlayerMenuPopulate_Orig();

	if (version != GameVersion::Debug2017)
		return; // lolwut

	std::unordered_map<int, std::tuple<const char*, const char*>> replacements = {
		{0,  {"Invincible", "No attacks can hit the player"}}, // DBG_PLAYER_INVINCIBLE
		{1,  {"No Damage", "Player takes 0 damage"}}, // DBG_PLAYER_NO_DAMAGE
		{2,  {"Infinite Jump", "Player can jump infinitely"}}, // DBG_PL_MUGEN_JUMP
		{3,  {"Fast Move", "Player Fast Move"}}, // DBG_PL_FAST_MOVE
		{4,  {"HP1", "Set player's HP to 1"}}, // DBG_PL_HP_1
		{5,  {"Death on DEL", "DEL key kills the player"}}, // DBG_PL_DIE
		{6,  {"Fix Level", "Fix player's level"}}, // DBG_PL_LV_FIX
		{7,  {"Level Change", "KB left/right to change player's level"}}, // DBG_PL_LV_CHG
		{8,  {"Maximum Weapon Level", "Fix player's weapon level to maximum"}}, // DBG_PL_WEP_LV_MAX
		{9,  {"Special Wait OFF", "Disable wait when player flares skirt"}}, // DBG_PL_SP_WAIT_IGNORE
		{10, {"Combo Continuity Debug", "Combo Continuity Test"}}, // DBG_COUTINUOUS_COMBO
		{11, {"Stop Player Update", "Stop Player Update"}}, // DBSTP_PL
		{12, {"Stop Pad Vibration", "Stop Pad Vibration"}}, // DBSTP_PAD_VIB
		{13, {"Enemy Invincible", "Enemy takes 0 damage"}}, // DBG_EM_NO_DAMAGE
		{14, {"Enemy Standby", "Enemy always standby"}}, // DBG_EM_WAIT_MODE
		{15, {"Enemy Instant Death", "Enemy dies instantly"}}, // DBG_EM_INSTANT_DEATH
		{16, {"Maximum Enemy Stun Value", "Maximum enemy stun value"}}, // DBG_EM_STUN_MAX
		{17, {"Increased Enemy Damage", "Increased enemy damage"}}, // DBG_EM_MANY_DAMAGE
		{18, {"Enemy Stun Value Disable", "Enemy Stun Value Disable"}}, // DBG_EM_NO_STUN
		{19, {"Enemy Does Not Attack", "Enemy does not take attack action"}}, // DBG_EM_NO_ATTACK
		{20, {"Stop Enemy Refresh", "Stop Enemy Refresh"}}, // DBSTP_EM
		{21, {"Enemy Attack Stop", "Enemy attack stop"}}, // STP_EM_ATK
		{22, {"Display Enemy HP", "Display enemy HP"}}, // DBG_EM_HP_DISP
		{23, {"Player 9S", "Change Player (requires loading)"}}, // GAME_PLAYER_9S
		{24, {"Player A2", "Change Player (requires loading)"}}, // GAME_PLAYER_A2
		{25, {"Player Pascal", "[Unimplemented] Change Player (requires loading)"}}, // GAME_PLAYER_PASCAL
		{26, {"2B Dying", "2B Dying Behavior"}}, // GAME_2B_BRINK
		{27, {"Free-Flight Enabled", "Free-Flight Enabled"}}, // GAME_FREE_FLIGHT
		{28, {"Buddy Pascal", "Pascal accompaniment"}}, // GAME_NPC_PASCAL
		{29, {"Buddy Sedation", "Buddy will no longer take offensive actions"}}, // DBG_NPC_SEDACTIVE
		{30, {"Buddy Disable", "Buddy disappear"}}, // DBG_BUDDY_OFF
		{31, {"Fast Fishing", "Makes you catch fish quickly"}}, // DBG_FISH_TEST
		{32, {"9S Takeover", "Enable Mode B"}}, // DBG_9SCTRL_MODE_B
		{33, {"Autoplay", "Enable debug autoplay"}}, // DBG_PL_AUTO_PLAY
		{34, {"Hair Color Test", "Enable"}}, // DBG_PL_HAIR_COLOR_TEST
	};

	// Now translate each entry!
	Debug_PlayerMenu* flags = GameAddress<Debug_PlayerMenu*>(0x1F36A60);
	for (auto& kvp : replacements)
	{
		strcpy_s(flags->Flags[kvp.first].name, std::get<0>(kvp.second));
		strcpy_s(flags->Flags[kvp.first].description, std::get<1>(kvp.second));
	}

	// Shift the description text over to the right so name won't be obscured
	// (need to change pointer for a float to use a different float in the game EXE >.>)
	// (changes 380 -> 600)
	SafeWriteModule(0x7BA2E4 + 5, uint32_t(0x18E87DC - (0x7BA2E4 + 9)));
}

void DebugMenu_Update(dbMenu_FlagCategory* category, const std::unordered_map<int, const char*>& replacements)
{
	DWORD oldProtect = 0;
	VirtualProtect(category->FlagNames, sizeof(dbFlag) * category->NumFlags, PAGE_READWRITE, &oldProtect);
	for (auto& kvp : replacements)
	{
		if (kvp.first >= category->NumFlags)
			continue;

		category->FlagNames[kvp.first].Desc = kvp.second;
	}

	// Remove any "reservation" flags since they're pointless
	for (int i = 0; i < category->NumFlags; i++)
	{
		auto& flagName = category->FlagNames[i];
		if (!flagName.Name || !strlen(flagName.Name))
			flagName.Desc = flagName.Name;
	}

	VirtualProtect(category->FlagNames, sizeof(dbFlag) * category->NumFlags, oldProtect, &oldProtect);
}

extern std::unordered_map<int, const char*> replacements_DBG;
extern std::unordered_map<int, const char*> replacements_DBSTP;
extern std::unordered_map<int, const char*> replacements_DBDISP;
extern std::unordered_map<int, const char*> replacements_DBGRAPHIC;
extern std::unordered_map<int, const char*> replacements_STA;
extern std::unordered_map<int, const char*> replacements_STOP;
extern std::unordered_map<int, const char*> replacements_GRAPHIC;
extern std::unordered_map<int, const char*> replacements_DISP;
extern std::unordered_map<int, const char*> replacements_GAME;

typedef void(*Debug_PrintToScreen_E4AA70_Fn)(void* a1, void* a2, void* a3, const char* Format, void* va_list);
Debug_PrintToScreen_E4AA70_Fn Debug_PrintToScreen_E4AA70_Orig;
void Debug_PrintToScreen_E4AA70_Hook(void* a1, void* a2, void* a3, const char* Format, void* va_list)
{
	// First convert input from SJIS to UTF8, for comparison with our map...
	auto converted = sj2utf8(Format);

	// Check if we have any translation for the Format param, swap it out if so
	const char* format = Format;
	if (translations_fixed.count(converted.c_str()))
		format = translations_fixed[converted].c_str();

	return Debug_PrintToScreen_E4AA70_Orig(a1, a2, a3, format, va_list);
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

	if (version == GameVersion::Debug2017)
	{
		MH_CreateHook(GameAddress<LPVOID>(0xE491E0), Debug_PrintToConsole_Hook, (LPVOID*)&Debug_PrintToConsole_Orig);

		// Translate text writes to debug-menu pages
		MH_CreateHook(GameAddress<LPVOID>(0xE4AA70), Debug_PrintToScreen_E4AA70_Hook, (LPVOID*)&Debug_PrintToScreen_E4AA70_Orig);

		// Model category "その他" -> "MISC"
		const char* sMiscCategory = "MISC\0";
		SafeWriteModule(0x19CD7E0, (char*)sMiscCategory, 5);

		// Translate "debug player menu" entries
		MH_CreateHook(GameAddress<LPVOID>(0x1864060), Debug_PlayerMenuPopulate_Hook, (LPVOID*)&Debug_PlayerMenuPopulate_Orig);

		// Translate debug menu flag-editor descriptions
		dbMenu_Flags* flags = GameAddress<dbMenu_Flags*>(0x1F33A50);
		DebugMenu_Update(&flags->category[0], replacements_DBG);
		DebugMenu_Update(&flags->category[1], replacements_DBSTP);
		DebugMenu_Update(&flags->category[2], replacements_DBDISP);
		DebugMenu_Update(&flags->category[3], replacements_DBGRAPHIC);
		DebugMenu_Update(&flags->category[4], replacements_STA);
		DebugMenu_Update(&flags->category[5], replacements_STOP);
		DebugMenu_Update(&flags->category[6], replacements_GRAPHIC);
		DebugMenu_Update(&flags->category[7], replacements_DISP);
		DebugMenu_Update(&flags->category[8], replacements_GAME);
	}
}
