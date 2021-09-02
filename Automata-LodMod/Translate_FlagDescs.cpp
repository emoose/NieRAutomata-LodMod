#include "pch.h"

std::unordered_map<int, const char*> replacements_DBG = {
	{0,   "Use debug camera"}, //CAMERA_MODE
	{1,   "Camera mode: for OBJ"}, //CAM_MODE_OBJ
	{2,   "Camera mode: for SCR: not yet supported: default camera operation"}, //CAM_MODE_SCR
	{3,   "Trailer Capture Mode"}, //TRAILER_CAPTURE_MODE
	{4,   "for judging effect capture"}, //DBG_EFFECT_SCRCAPMODE
	{5,   "Camera information display for debugging"}, //DBG_CAM_INFO_DISP
	{6,   "Display camera information for debugging"}, //DBG_DBCAM_INFO_DISP
	{7,   "tool mode"}, //TOOL_MODE
	{8,   "in effect tool"}, //DBG_ESP_TOOL
	{9,   "Used to process the sound played in the tool on exit"}, //SOUND_TOOL
	{10,  "GA tool in progress"}, //DBG_GA_TOOL
	{11,  "During tool, the value changes without pressing A"}, //DBG_WINDOWCTL_NO_ABTN
	{12,  "Do not display debug string" }, //DBG_DBGSTRING_NODISP
	{13,  "Do not display system page" }, //DBG_ERASE_SYSTEM_PAGE
	{14,  "Do not display the memory map in the lower right corner of the screen" }, //DBG_ERASE_MEMORY_MAP
	{15,  "step mode" }, //STEP_MODE
	{16,  "step forward" }, //STEP_ADVANCE
	{17,  "screen capture activation" }, //DBG_SCREEN_CAPTURE
	{18,  "Room Jump with Debug Function" }, //DBG_DEBUG_ROOM_JUMP
	{19,  "Open debug menu without pressing R1" }, //DEBUG_MENU_BTN_NO_R1
	{20,  "Turn on the reload system" }, //DBG_RELOAD_ON
	{21,  "Display processing time" }, //PROCESS_BAR_DISP
	{22,  "Display processing time without averaging" }, //PROCESS_BAR_NO_AVE
	{23,  "Hide debug menu bar" }, //DBG_NOUSE_MENU_BAR
	{24,  "Show Effects Debug ON" }, //DBG_ESP_DEBUG_DISP
	{25,  "Do not draw screen effects" }, //DBG_ESP_SCREEN_NODISP
	{26,  "MOVE multi-threading of effects OFF" }, //DBG_ESP_MULTI_THREAD_OFF
	{27,  "TRANS multithreading of effects OFF" }, //DBG_ESP_T_MULTI_THREAD_OFF
	{28,  "PL low poly and cube map check" }, //DBG_PL_LOW_CUBE_MAP_CHECK
	{29,  "motion camera off flag" }, //DBG_MOT_CAM_OFF
	{30,  "INURN multithreading OFF for effects" }, //DBG_ESP_I_MULTI_THREAD_OFF
	{31,  "Display in order of effect generation OFF" }, //DBG_ESP_TRANS_CREATE_ORDER_OFF
	{32,  "Floor collision check just below gazing point" }, //DBG_CAM_VIEW_UNDER_COL_CK
	{33,  "Camera correction process OFF" }, //DBG_CAM_ADJUST_OFF
	{34,  "make strong attack a special attack" }, //DBG_STRONG_ATK_SP
	{35,  "capturing cubemap" }, //DBG_IN_CUBEMAP_CAPTURE_MODE
	{36,  "pad replacement" }, //DBG_PAD_REPLACE
	{37,  "insert keyboard input on pad" }, //DBG_PAD_KEYBOARD
	{38,  "single-thread the BEHAVIOR" }, //DBG_BEHAVIOR_SINGLE
	{39,  "single-thread transformer" }, //DBG_OBJ_TRANS_SINGLE
	{41,  "emulate DVD reading time" }, //DBG_DVD_TIME
	{42,  "Fast loading at room initialization" }, //DBG_DVD_FAST_STARTUP
	{43,  "read time record" }, //DBG_DVD_TIME_RECORD
	{44,  "HAP Development Mode" }, //DBG_HAP_DEVELOPMENT_MODE
	{45,  "HAP Test Mode" }, //DBG_HAP_TEST
	{46,  "Enable memoization even for HAP tools" }, //DBG_HAP_TOOL_USE_MEMO
	{47,  "Change weapon level" }, //DBG_PL_WEP_LV_CHG
	{48,  "no player HP reduction" }, //DBG_PLAYER_NO_DAMAGE
	{49,  "ignore player damage" }, //DBG_PLAYER_INVINCIBLE
	{50,  "player infinite slow" }, //DBG_PL_SLOW_MUGEN
	{51,  "player gauge infinite" }, //DBG_PL_GAUGE_MUGEN
	{52,  "Player Fast Move" }, //DBG_PL_FAST_MOVE
	{53,  "Player HP1" }, //DBG_PL_HP_1
	{54,  "Player DEL key for instant death" }, //DBG_PL_DIE
	{55,  "player infinite jump" }, //DBG_PL_MUGEN_JUMP
	{56,  "Player Level Fixed" }, //DBG_PL_LV_FIX
	{57,  "Player KB left/right to change level" }, //DBG_PL_LV_CHG
	{58,  "9s hacking debug (maximum companion/control level)" }, //DBG_PL_9S_HACKING
	{59,  "turn off variable frame rate" }, //DBG_VARIABLE_FRAME_DISABLE
	{60,  "30 frame mode" }, //DBG_SLOW_RATE_30
	{61,  "combo continue debug" }, //DBG_COUTINUOUS_COMBO
	{62,  "player flare skirt wait disabled" }, //DBG_PL_SP_WAIT_IGNORE
	{63,  "Turn on player camera background bite" }, //DBG_PL_CAM_CHECK_HIT_ON
	{64,  "enemy never takes damage" }, //DBG_EM_NO_DAMAGE
	{65,  "enemy always wait" }, //DBG_EM_WAIT_MODE
	{66,  "Enemy One-Hit Death" }, //DBG_EM_INSTANT_DEATH
	{67,  "enemy action selection" }, //DBG_EM_SELECT
	{68,  "maximum enemy stun value" }, //DBG_EM_STUN_MAX
	{69,  "more enemy damage" }, //DBG_EM_MANY_DAMAGE
	{70,  "enemy stun value invalid" }, //DBG_EM_NO_STUN
	{71,  "no enemy attack" }, //DBG_EM_NO_ATTACK
	{72,  "enemy brand test" }, //DBG_EM_BRAND
	{73,  "Enemy Takeover Test" }, //DBG_EM_CONTROLHACK
	{74,  "Enemy Team Test" }, //DBG_EM_TEAM
	{75,  "Enemy Level Up Effect Debugging (Keyboard 0)" }, //DBG_EM_LEVELUP_ESP
	{76,  "enemy hacking reaction test" }, //DBG_EM_HACKING_REACTION_TEST
	{77,  "stop enemy reaction change" }, //DBG_EM_NO_CHANGE_REACTION
	{78,  "NPC Sedation" }, //DBG_NPC_SEDACTIVE
	{79,  "Player Damage Test" }, //DBG_PL_DAMAGE_TEST
	{80,  "Forcing SLOW ON during tool" }, //DBG_TOOL_SLOW_MODE
	{81,  "cubemap shooting tool" }, //DBG_SCR_SHOT_TOOL
	{82,  "Constant Shooting" }, //DBG_SCR_SHOT_CONSTANT
	{83,  "Effect Light Shooting" }, //DBG_SCR_SHOT_ESP_LIGHT
	{84,  "Distant View Shooting" }, //DBG_SCR_SHOT_DISTANTVIEW
	{85,  "Stop reading back textures" }, //DBG_STOP_TEX_READ
	{86,  "disable mouse movement during debug camera" }, //DBG_NO_CAMERA_MOUSE
	{88,  "For testing enemy EMP attacks" }, //DBG_EM_EMP_TEST
	{89,  "GI tool in use" }, //DBG_GI_TOOL
	{90,  "Capsule check in front of the player" }, //DBG_PL_FRONT_CAPSULE_CK
	{91,  "for event drawing system experiment" }, //DBG_EVENT_TEST_DRAW
	{92,  "for event capture" }, //DBG_SCENE_CAPTURE
	{93,  "Start Event Capture Preparation" }, //DBG_SCENE_CAPTURE_READY
	{94,  "weapon level max" }, //DBG_PL_WEP_LV_MAX
	{95,  "Allow step mode even when window system starts"}, //DBG_PERMIT_STEP_MODE_USE_WND
	{96,  "Onishi debugging" }, //DBG_ONISHI
	{97,  "WAGATSUMA debugging" }, //DBG_WAGATSUMA
	{98,  "Flight Mode Test" }, //DBG_WAGATSUMA_FLIGHT_UNIT
	{99,  "028 debug camera" }, //DBG_CAM_MODE_028
	{100, "Buddy disabled" }, //DBG_BUDDY_OFF
	{101, "hacking acceleration" }, //DBG_HACKING_ACCELERATION
	{102, "Consumable Item Test" }, //DBG_ITEM_TEST
	{103, "Delayed Virus Notification Test" }, //DBG_EM_DELAYED_VIRUS_TEST
	{104, "Jump Height Test" }, //DBG_WAGATSUMA_JUMPHEIGHT_TEST
	{105, "Only sequence layer 2 valid" }, //DBG_WAGATSUMA_ONLYSEQ_2
	{106, "Flight mode parameters" }, //DBG_WAGATSUMA_FLIGHTMODE_SPEED
	{107, "motion name display" }, //DBG_VIEW_MOT
	{108, "2B dying check" }, //DBG_2B_BREAK
	{109, "ignore effect raycast" }, //DBG_ESPRAYCAST_IGNORE
	{110, "Hacking Game: Auto Shoot" }, //DBG_YASUI_HACK_AUTO_SHOOT
	{111, "Okabe Debugging" }, //DBG_OKABE
	{113, "Capture UI together" }, //DBG_CAPTURE_WITH_UI
	{114, "turn off mouse mode" }, //DBG_MOUSEMODE_OFF
	{115, "Enable player's keyboard operation" }, //DBG_KEYBOARDPAD_ON
	{116, "output log of file reading" }, //DBG_FILE_READ_LOG
	{117, "For Playgo testing" }, //DBG_PLAYGO_TEST
	{118, "automatic test mode" }, //DBG_AUTO_JUMP_TEST_MODE
	{119, "Honda Debug" }, //DBG_HONDA
	{120, "minimap test" }, //DBG_YASUI_MINIMAP
	{121, "No UI Data Readback" }, //DBG_UI_NO_MODE_CHANGE
	{122, "Passive hacking during F2 hacking" }, //DBG_HACK_PASSIVE
	{123, "broken expression black and white filter ON" }, //DBG_UI_FILTER_BREAK
	{124, "autoplay execution flag" }, //DBG_AUTO_PLAY
	{125, "local file autoplay execution flag" }, //DBG_AUTO_PLAY_LOCAL
	{126, "Stop updating g_Pad. Don't even clear it." }, //DBG_STP_UPDATE_PAD
	{127, "EMP filter ON" }, //DBG_UI_FILTER_EMP
	{128, "Do not execute effect Move in SPU" }, //DBG_NOUSESPU_ESPJOBMOVESPU
	{129, "Do not execute effect PreTrans in SPU" }, //DBG_NOUSESPU_ESPJOBPRETRANSSPU
	{130, "Do not perform model joint calculations in SPU" }, //DBG_NOUSESPU_MODELUPDATEPARTS_MTX
	{131, "Do not run model transformations in SPU" }, //DBG_NOUSESPU_MODELTRANS
	{132, "BitonicSort GpuBullet with Cpu" }, //DBG_GPUBULLET_BITONICSORTCPU
	{133, "texture data address check" }, //DBG_WASADADEBUG_0
	{134, "always generate ef005b" }, //DBG_WASADADEBUG_1
	{135, "Lock with SetEffectResource" }, //DBG_WASADADEBUG_2
	{136, "force change to experimental shader" }, //DBG_EXPERRIENTSHADER_CHANGE
	{137, "DBG_WASADADEBUG_4" }, //DBG_WASADADEBUG_4
	{138, "DBG_WASADEBUG_5" }, //DBG_WASADEBUG_5
	{139, "DBG_WASADADEBUG_6" }, //DBG_WASADADEBUG_6
	{140, "DBG_WASADADEBUG_7" }, //DBG_WASADADEBUG_7
	{142, "PL's hair color test" }, //DBG_PL_HAIR_COLOR_TEST
	{143, "Don't sort GPU bullets" }, //DBG_EFFECT_GPUBULLET_NO_SORT
	{144, "Kamagami debug" }, //DBG_KAMAKAMI
	{145, "UI log in detail" }, //DBG_UI_DETAIL_LOG
	{146, "Name Entry Test" }, //DBG_UI_NAMEENTRY
	{147, "Show UI in wireframe" }, //DBG_UI_WIREFRAME
	{148, "Do not mask the UI" }, //DBG_UI_MASK_NODISP
	{149, "Do not perform UI Gaussian processing" }, //DBG_UI_GAUSS_NODISP
	{150, "Display experience gauge even if no skills are attached" }, //DBG_UI_DISP_EXPGAUGE
	{151, "pretend you have all HUD display chips" }, //DBG_UI_PASSIVE_SKILL_ON
	{152, "fish list complete" }, //DBG_UI_FISHLIST_COMPLETE
	{153, "Enemy List Complete" }, //DBG_UI_EMLIST_COMPLETE
	{154, "open all tutorials in pause menu" }, //DBG_UI_TUTORIAL_OPEN
	{155, "release all novels in pause menu" }, //DBG_UI_NOVEL_OPEN
	{156, "Show HEX on the minimap" }, //DBG_UI_VIEW_HEX
	{157, "force pad use mode" }, //DBG_UI_PADMODE_FORCE
	{158, "turn UI update single" }, //DBG_UI_SINGLE_UPDATE
	{159, "Turn UI trans on single" }, //DBG_UI_SINGLE_TRANS
	{160, "Dummy data for the download result of the support mesage" }, //DBG_STAFFROLL_DUMMY_DOWNLOAD
	{161, "Reduce the time it takes for a corpse to change state" }, //DBG_TIME_CRUNCH
	{162, "change to a tainted corpse" }, //DBG_DECORPSE_BODY
	{163, "9S hacking control mode B" }, //DBG_9SCTRL_MODE_B
	{164, "Enable log display in debug" }, //DBG_UI_LOG
	{165, "Unmask all 3D maps" }, //DBG_3DMAP_VISIBLE
	{166, "Enable to open debug menu after clearing" }, //DBG_AFTERCLEARANCE_DBMENU_ENABLE
	{167, "Add 15 minutes to play time" }, //DBG_ADD_PLAYTIME_15MINUTES
	{169, "player autoplay" }, //DBG_PL_AUTO_PLAY
	{170, "player shooting plane off" }, //DBG_PL_SHOT_PLANE_OFF
	{170, "Player Firing Plane OFF" }, //DBG_PL_SHOT_PLANE_OFF {171, "Player Firing Plane Display"}, //DBG_PL_SHOT_PLANE_DISP
	{172, "Player Firing Plane Background B" }, //DBG_PL_SHOT_PLANE_SCR_B
	{173, "Player Shooting Plane Battle B" }, //DBG_PL_SHOT_PLANE_BATTLE_B
	{174, "Enemy HP Display" }, //DBG_EM_HP_DISP
	{175, "hacking test" }, //DBG_PL_HACKING_TEST
	{177, "force event skip" }, //DBG_EVENTSKIP_FORCE
	{178, "play record display ON" }, //DBG_PLAYRECORD_OPEN
	{179, "Effect check mode (\"Not registered with manager \", etc. will stop with assert)" }, //DBG_ESP_CHECK_MODE
	{180, "Remove limit of artificial bodies that can appear" }, //DBG_NOT_LIMIT_CORPSE_NUMBER
	{181, "No time limit on granting passive skill chip effect" }, //DBG_NO_TIME_LIMIT_CHIP_EFFECT
	{182, "display prosthetic parameters" }, //DBG_DISPLAY_CORPSE_PARAMETER
	{183, "Pseudo-occurrence of the process when silent prayer is received" }, //DBG_SUPPLY_CHECK_ON
	{184, "Speed up the timing for checking relief supplies (1 minute). Be aware that this may cause excessive server access." }, //DBG_TIME_CRUNCH_SUPPLY
	{185, "Enable level bind" }, //DBG_HAP_LEVEL_BIND
	{186, "Show cheering message in Traditional Chinese" }, //DBG_SUPPORT_MSG_TW
	{187, "Show support message in Korean" }, //DBG_SUPPORT_MSG_KR
	{188, "Network menu is always displayed" }, //DBG_NETWORK_MENU_ON
	{189, "release extras after clearing"}, //DBG_CLEARFLAG_ON
	{190, "make it possible to catch fish quickly" }, //DBG_FISH_TEST
	{191, "specify screen fill color" }, //DBG_CLEAR_COLOR_CUSTOM
	{192, "Wwise Debug Communication" }, //DBG_SOUND_WWISE_COMM
	{193, "Mute all sounds" }, //DBG_SOUND_MUTE_MASTER
	{194, "Mute SE" }, //DBG_SOUND_MUTE_SE
	{195, "mute background music" }, //DBG_SOUND_MUTE_BGM
	{196, "Mute VOICE" }, //DBG_SOUND_MUTE_VOICE
	{197, "mute ambient sound" }, //DBG_SOUND_MUTE_ENV
	{198, "disable area control" }, //DBG_SOUND_DISABLE_AREA
	{199, "disable reflection control" }, //DBG_SOUND_DISABLE_REFLECTION
	{200, "Turn off SE culling" }, //DBG_SOUND_OFF_SE_CULL
	{201, "make all SEs non-volatile" }, //DBG_SOUND_NON_VOLATILE
	{207, "destabilize slow rate" }, //DBG_UNSTABLE_SLOW_RATE
	{208, "enable automatic death check for effects" }, //DBG_EFFECT_AUTODIECHECK
	{209, "effect auto-calling enabled" }, //DBG_EFFECT_HITEFFECT_AUTO
	{210, "Behave as if the colosseum data is mounted (only when non-cpk)" }, //DBG_DLC00_MOUNT
	{211, "behave as if the white book data is mounted (only when non-cpk)" }, //DBG_DLC01_MOUNT
	{212, "Behave as if the mechanical life form head is mounted (only when non-cpk)" }, //DBG_DLC02_MOUNT
	{213, "behave as if valve data is mounted (only when non-cpk)" }, //DBG_DLC03_MOUNT
	{214, "behave as if bs1 data is mounted (only when non-cpk)" }, //DBG_DLC04_MOUNT
	{215, "Behave as if Amazoso data is mounted (only when non-cpk)" }, //DBG_DLC05_MOUNT
	{216, "behave as if loanso data is mounted (only when non-cpk)" }, //DBG_DLC06_MOUNT
	{217, "behave as if retro red data is mounted (only when non-cpk)" }, //DBG_DLC07_MOUNT
	{218, "behave as if retro gray data were mounted (only when non-cpk)" }, //DBG_DLC08_MOUNT
	{219, "behave as if retro black data were mounted (only when non-cpk)" }, //DBG_DLC09_MOUNT
	{220, "behave as if retro purple data is mounted (only when non-cpk)" }, //DBG_DLC10_MOUNT
	{221, "behave as if retro white data is mounted (only when non-cpk)" }, //DBG_DLC11_MOUNT
	{222, "behave as if eagle-head data were mounted (only when non-cpk)" }, //DBG_DLC12_MOUNT
	{224, "disable distant view" }, //DBG_LOW_MAP_DISABLE
	{225, "disable manual culling" }, //DBG_MANUAL_CULLING_DISABLE
	{226, "Show distant view only" }, //DB_LOW_MAP_ONLY
	{228, "Use leaderboard for QA (Steam version only)" }, //DBG_USE_QA_LEADERBORAD
	{229, "Set upload date to 7 days ago" }, //DBG_SET_UPLOAD_7DAY_IN_AVANCE
	{230, "Set upload date to 60 days ago" }, //DBG_SET_UPLOAD_60DAY_IN_AVANCE
	{231, "Force 2B launch game time limit to 1 minute remaining" }, //DBG_2B_SETUP_1MIN
	{232, "for fast travel test" }, //DBG_FASTTRAVEL_TEST
	{233, "carry all weapons" }, //DBG_SET_ALL_WEAPON
	{234, "possess all active skills" }, //DBG_SET_ALL_ACTIVESKILL
	{235, "for passive skill testing" }, //DBG_PASSIVESKILL_TEST
	{236, "old fast travel" }, //DBG_FASTTRAVEL_OLD
	{237, "for mailbox testing" }, //DBG_MAILBOX_TEST
	{238, "for quest test" }, //DBG_QUEST_TEST
	{239, "for title test" }, //DBG_TITLE_TEST
	{240, "Reverberation processing (debug after clear)" }, //DBG_BGM_HOLY_EFX
	{241, "chorus effect (debug after clear)"}, //DBG_BGM_CHORUS_EFX
	{242, "raise pitch (debug after clear)"}, //DBG_BGM_PITCH_SHIFT_P
	{243, "lower the pitch (debug after clear)"}, //DBG_BGM_PITCH_SHIFT_M
	{244, "lower the tempo (debug after clear)"}, //DBG_BGM_TIME_STRETCH_SLOW
	{245, "Always 8-bit (debug after clear)"}, //DBG_BGM_8BIT_TEST
	{246, "record simulate (debug after clear)"}, //DBG_BGM_RECORD_EFX
	{247, "force play vocals if any (debug after clear)"}, //DBG_BGM_TRY_VOCAL_ON
	{248, "pod expansion and contraction according to tempo"}, //DBG_BGM_POD_TEMPO_SCALING
};

std::unordered_map<int, const char*> replacements_DBSTP = {
	{0, "all models"}, //DBSTP_OBJ
	{1, "player stop"}, //DBSTP_PL
	{2, "Stop Enemy"}, //DBSTP_EM
	{3, "Stop Other Models"}, //DBSTP_ETC
	{4, "Stop Effect"}, //DBSTP_ESP
	{5, "SCR Stop"}, //DBSTP_SCR
	{6, "UI stop"}, //DBSTP_UI
	{7, "filter stop"}, //DBSTP_FILTER
	{8, "shadow stop"}, //DBSTP_SHADOW
	{9, "Player Stop 2"}, //DBSTP_SUBPL
	{10, "pad vibration stop"}, //DBSTP_PAD_VIB
	{11, "stop layout placement"}, //DBSTP_LAYOUT_SET
	{16, "stop checking hit of attack/damage"}, //DBSTP_ATDMHIT
	{17, "Stop Checking for Object Hits"}, //DBSTP_OBJHIT
	{18, "Stop Scroll Hit Check"}, //DBSTP_SCRHIT
	{19, "Stop Scenario"}, //DBSTP_SCENARIO
	{20, "event stop"}, //DBSTP_EVENT
	{21, "cloth processing calculation stop"}, //DBSTP_CLOTH_CALC_OFF
	{22, "camera stop"}, //DBSTP_CAMERA
	{23, "Stop Light"}, //DBSTP_LIGHT
	{24, "stop checking per scenario"}, //DBSTP_SCE_AT
	{25, "HAP debug drawing stop"}, //DBSTP_HAP_DISP
	{26, "stop IK processing"}, //DBSTP_IK
	{27, "Stop GPU Bullet"}, //DBSTP_GPU_BULLET
	{32, "stop all hit checks"}, //DBSTP_ALLHIT
	{33, "all models"}, //DBSTP_ALL_MODEL
	{34, "stop scene"}, //DBSTP_SCENE
	{37, "Stop Effect Call"}, //DBSTP_ESPCALL
	{39, "Stop Keyboard Shortcut"}, //DBSTP_KEYBORED_SHORTCUT
	{48, "Stop Model WindReceive"}, //DBSTP_MODEL_WIND_RECEIVE 
	{49, "Stop foot IK related"}, //DBSTP_FOOT_IK
	{50, "stop debug log function"}, //DBSTP_DEBUG_LOG
	{53, "stop animation"}, //DBSTP_ANIM
	{54, "stop updating the sleep table"}, //DBSTP_UPDATE_VFCUL
	{56, "stop step mode"}, //DBSTP_STEP_MODE
	{57, "stop calculating the route check"}, //DBSTP_CALC_ROUTE
	{58, "HAP scenario stop"}, //DBSTP_HAP_SCENARIO
	{59, "HAP lazy read disable"}, //DBSTP_HAP_LAZY_MOUNT
	{64, "POD stop"}, //DBSTP_POD_SUSPEND
	{80, "Stop updating seamless map"}, //DBSTP_MAP_UPDATE
	{81, "Define model tree processing"}, //DBSTP_MODEL_TREE_OFF
};

std::unordered_map<int, const char*> replacements_DBDISP = {
	{0,   "Stop drawing objects"}, //DBDISP_OBJ_NO_DISP
	{1,   "Stop drawing the player"}, //DBDISP_PL_NO_DISP
	{2,   "Stop drawing enemies"}, //DBDISP_EM_NO_DISP
	{3,   "Stop drawing other models"}, //DBDISP_ETCOBJ_NO_DISP
	{4,   "Stop drawing effects"}, //DBDISP_ESP_NO_DISP
	{5,   "Stop drawing scroll"}, //DBDISP_SCR_NO_DISP
	{6,   "Stop drawing UI"}, //DBDISP_UI_NO_DISP
	{7,   "Stop drawing filter"}, //DBDISP_FILTER_NO_DISP
	{8,   "Stop drawing shadow"}, //DBDISP_SHADOW_NO_DISP
	{9,   "Stop drawing DBG primitive"}, //DBDISP_DBPRIM_NO_DISP
	{10,  "Stop displaying lights"}, //DBDISP_LIGHT_NO_DISP
	{11,  "Stop drawing weapon model"}, //DBDISP_WP_NO_DISP
	{12,  "Stop drawing BG model"}, //DBDISP_BGOBJ_NO_DISP
	{13,  "Stop drawing BA model"}, //DBDISP_BAOBJ_NO_DISP
	{14,  "Stop drawing BH model"}, //DBDISP_BHOBJ_NO_DISP
	{15,  "Stop drawing SC model"}, //DBDISP_SCOBJ_NO_DISP
	{16,  "Attack/damage hit display"}, //DBDISP_ATDM_HIT_DISP
	{17,  "Object Hit Display"}, //DBDISP_OBJ_HIT_DISP
	{18,  "background scroll hit display"}, //DBDISP_SCR_HIT_DISP
	{19,  "Draw per-scenario hit"}, //DBDISP_SCE_AT_DISP
	{20,  "Draw per-filter"}, //DBDISP_FILTER_ATDISP
	{21,  "draw per cubemap"}, //DBDISP_CUBEMAP_ATDISP
	{22,  "show instancing region"}, //DBDISP_INSTANCINGREGION
	{23,  "lighten drawing test"}, //DBDISP_DRAW_TEST
	{24,  "Use motion blur"}, //DBDISP_MOTION_BLUR
	{25,  "Display Behavior Information"}, //DBDISP_BEHAVIOR_INFO
	{26,  "Display per GA"}, //DBDISP_GA_ATARI_DISP
	{27,  "display of phase information"}, //DBDISP_PHASEINFO
	{28,  "game flag display"}, //DBDISP_GAMEFLAG
	{29,  "camera display"}, //DBDISP_CAMERA
	{30,  "character proxy display"}, //DBDISP_CHARAPROXY
	{31,  "Behavior Collision Check Display"}, //DBDISP_BEHAVIOR_SCR_HIT_DISP
	{32,  "Display volume for semi-transparency"}, //DBDISP_CAM_ALPHA_VOLUME
	{33,  "Hide things laid out in Hap"}, //DBDISP_NO_DISP_HAP_LAYOUT
	{34,  "Turn off 1bitMask related display"}, //DBDISP_1BM_OFF
	{35,  "Debug display of volume for LOD determination"}, //DBDISP_LOD_BOUNDING_VOULME
	{36,  "debug display of bounding volume"}, //DBDISP_BOUNDING_VOULME
	{37,  "distance debug display"}, //DBDISP_DIST_DISP
	{38,  "wet relation judgment"}, //DBDISP_WET_CHECK
	{39,  "hide player's dmg when displaying attack/damage"}, //DBDISP_NO_DISP_PL_DM
	{40,  "Hide enemy's dmg when showing attack/damage"}, //DBDISP_NO_DISP_EM_DM
	{41,  "Hide player's at when displaying attack/damage"}, //DBDISP_NO_DISP_PL_AT
	{42,  "Hide enemy's at when showing attack/damage"}, //DBDISP_NO_DISP_EM_AT
	{43,  "Display per shade"}, //DBDISP_SHADE_COL_DISP
	{44,  "Visualize blend relationship"}, //DBDISP_BLEND_CHECK
	{45,  "display animation name"}, //DBDISP_ANIMATIONNAME_DISP
	{46,  "display IK information"}, //DBGDISP_IK
	{47,  "display route information"}, //DBGDISP_ROUTE
	{48,  "display distance from camera to enemy"}, //DBDISP_EM_CAM_DIS
	{49,  "display player's sequence flags"}, //DBDISP_PL_SEQ_FLAG_DISP
	{50,  "Display distance from camera to other OBJ"}, //DBDISP_ETC_CAM_DIS
	{51,  "Display OBJID"}, //DBDISP_OBJ_ID
	{52,  "Display debug drawing information"}, //DBDISP_SHADER_SETTING_INFO
	{53,  "Debug display of drawing information (for list)"}, //DBDISP_SHADER_SETTING_LIST
	{54,  "Display Bezier curve used by the enemy"}, //DBDISP_EM_BEZIERCURVE
	{55,  "Display debug drawing information"}, //DBDISP_SHADER_SETTING_MODEL
	{56,  "Frame rate drop information highlighting"}, //DBDISP_FRAME_RATE_EMERGENCY
	{57,  "Fishing Area Display"}, //DBDISP_FISHING_AREA
	{58,  "Cloth Handling Display"}, //DBDISP_CLOTH
	{59,  "Light Display"}, //DBDISP_LIGHT
	{60,  "Marker Distance Display"}, //DBDISP_MAKER
	{61,  "part spring display"}, //DBDISP_PARTSSPRING
	{62,  "Parts Information Display"}, //DBDISP_PARTS_INFO
	{63,  "Display Special Parts Information"}, //DBDISP_PARTS_INFO_SPECIAL
	{64,  "Display Polygon Occluder"}, //DBDISP_POLYGON_OCCLUDER
	{65,  "Do not draw z-map"}, //DBDISP_NO_DRAW_ZMAP
	{66,  "hide DOF"}, //DBDISP_DOF_OFF
	{67,  "FOG hidden"}, //DBDISP_FOG_NO_DISP
	{68,  "show shadow model"}, //DBDISP_SHADOW_MODEL_DISP
	{69,  "SSLB function OFF"}, //DBDISP_SSLB_OFF
	{70,  "SSAO function OFF"}, //DBDISP_SSAO_OFF
	{71,  "Ambient hidden"}, //DBDISP_AMBIENT_OFF
	{72,  "Work memory amount display"}, //DBDISP_MEM_WORK
	{73,  "buffer memory amount display"}, //DBDISP_MEM_BUFFER
	{74,  "file memory amount display"}, //DBDISP_MEM_FILE
	{75,  "texture memory amount display"}, //DBDISP_MEM_TEXTURE
	{76,  "debug memory amount display"}, //DBDISP_MEM_DEBUG
	{77,  "GPURW memory display"}, //DBDISP_MEM_GPURWBUFFER
	{78,  "VLEOFF"}, //DBDISP_VLE_OFF
	{79,  "tone map exposure OFF"}, //DBDISP_TONEMAP_EXPOSURE
	{80,  "Effect Work Number Display"}, //DBDISP_ESP_WORK
	{81,  "Effect debug display"}, //DBDISP_ESP_DEBUG
	{82,  "Effect lens flare debug display ON"}, //DBDISP_ESP_LEDBG_EM_AUTO_REQUESTNSFLARE_DISP
	{83,  "Effect light debug display"}, //DBDISP_ESP_LIGHT_DISP
	{84,  "Effect light OFF"}, //DBDISP_ESP_LIGHT_OFF
	{85,  "Effect Model OFF"}, //DBDISP_ESP_MODEL_OFF
	{86,  "Effect Model OFF"}, //DBDISP_ESP_NO_MODEL_OFF
	{87,  "Effect Auto Extinction Test (at 120%)"}, //DBDISP_ESP_AUTOALPHA_MAX1
	{88,  "Effect Auto Extinction Test (at 150%)"}, //DBDISP_ESP_AUTOALPHA_MAX2
	{89,  "Show only high-priority effects"}, //DBDISP_ESP_HI_PRIORITY
	{90,  "effect lens flare display OFF"}, //DBDISP_ESP_LENSFLARE_OFF
	{91,  "effect debug display room effect only"}, //DBDISP_ESP_DEBUG_ROOM_ONLY
	{92,  "show per effect SST"}, //DBDISP_ESPSST_ATDISP
	{93,  "draw effect area"}, //DBDISP_EFFECT_SCRSET_AREA,
	{94,  "effect GPU particle OFF"}, //DBDISP_ESP_GPUPARTICLE_OFF
	{95,  "show event effect only"}, //DBDISP_ESP_EVENT_ONLY
	{96,  "show effect line check"}, //DBDISP_ESP_LINECHECK
	{97,  "show effect profile"}, //DBDISP_ESP_PROFILE
	{98,  "Effect 2D only OFF"}, //DBDISP_ESP_2D_OFF
	{99,  "Effect shell polygon only OFF"}, //DBDISP_ESP_SHELLPOLYGON_OFF
	{100, "Effect Resolve Only OFF" }, //DBDISP_ESP_RESOLVE_OFF
	{101, "effect blur only OFF" }, //DBDISP_ESP_BLUR_OFF
	{102, "show effect culling" }, //DBDISP_ESP_CULL_SPHERE
	{103, "show effect culling rate" }, //DBDISP_ESP_OCCLUSION_RATE
	{104, "Effect Strip Hide" }, //DBDISP_ESP_STRIP_OFF
	{105, "Hide Effect AutoShader" }, //DBDISP_ESP_AUTOSHADER_OFF
	{106, "Hide Effect CustomShader" }, //DBDISP_ESP_CUSTOMSHADER_OFF
	{107, "Effect Capture Hide ON" }, //DBDISP_ESP_CAPTURE_NO_DISP
	{108, "effect exposure compensation OFF" }, //DBDISP_ESP_EXPOSURE_OFF
	{109, "effect external factor debug display" }, //DBDISP_EFFECT_EXTERNALFACTOR_DEBUG
	{110, "debug display of effect shader buffer" }, //DBDISP_EFFECT_SHADERBUFFER_DDEBUG
	{111, "Camera marker display off" }, //DBDISP_CAMERA_MARKER_OFF
	{112, "environmental sound area display" }, //DBDISP_SOUND_AREA_SE
	{113, "Environment effect area display" }, //DBDISP_SOUND_AREA_EFFECT
	{114, "BGM area display" }, //DBDISP_SOUND_AREA_BGM
	{115, "Show SE source" }, //DBDISP_SOUND_SE_OBJECT
	{116, "Show SE distance" }, //DBDISP_SOUND_SE_DISTANCE
	{117, "Display sound debug information" }, //DBDISP_SOUND_INFO
	{118, "Anytime Level Meter" }, //DBDISP_SOUND_ANYTIME_METER
	{119, "show stream log" }, //DBDISP_SOUND_STREAM_LOG
	{123, "debug voice call" }, //DBDISP_SOUND_VOICE_CALL
	{124, "Character Type Debugging" }, //DBDISP_SOUND_CHARA_TYPE
	{125, "Show material collision rate for reflection system" }, //DBDISP_SOUND_MATERIAL_GRAPH
	{126, "Make map HEX boundaries easier to understand"}, //DBDISP_SOUND_MAP_HEX
	{127, "Sample Spectrum Viewer" }, //DBDISP_SOUND_SAMPLE_SPECTRUM
	{128, "Regional Change Area Display" }, //DBDISP_REGIONAL_AREA_DISP
	{129, "celestial display OFF" }, //DBDISP_CELESTIAL_OFF
	{142, "Point Light Source OFF" }, //DBDISP_POINTLIGHT_OFF
	{143, "Spotlight OFF" }, //DBDISP_SPOTLIGHT_OFF
	{160, "UI2.5D display OFF" }, //DBDISP_UI_HUD_25D_OFF
	{161, "UI filter display OFF" }, //DBDISP_UI_FILTER_OFF
	{192, "AABB display per background" }, //DBDISP_SCR_HIT_AABB_DISP
	{193, "Display work number per background" }, //DBDISP_SCR_HIT_COL_WNO_DISP
	{194, "Display current node per background" }, //DBDISP_SCR_HIT_CURRENT_DISP
	{195, "Display per background scroll" }, //DBDISP_SCR_HIT_COL_CK_DISP
	{196, "Map Hex Display" }, //DBDISP_DRAW_MAP_HEX
	{197, "Manual culling references player position" }, //DBDISP_MANUAL_CULLING_PL
	{198, "Display mask for event capture" }, //DBDISP_EVENT_CAPTURE
};

std::unordered_map<int, const char*> replacements_DBGRAPHIC = {
	{0,   "minimize the drawing area"}, //DBGRAPHIC_SIMPLE_GRAPHIC
	{1,   "use simple shaders"}, //DBGRAPHIC_SIMPLE_SHADER
	{2,   "show shader load"}, //DBGRAPHIC_SHADER_PERFORMANCE
	{3,   "Draw LOD model only at highest"}, //DBGRAPHIC_HIGH_LOD_FIXED
	{4,   "draw LOD model only at lowest"}, //DBGRAPHIC_LOW_LOD_FIXED
	{5,   "Check the resolution of the texture drawn on the screen"}, //DBGRAPHIC_TEXTURE_LV_CK
	{6,   "check texture density"}, //DBGRAPHIC_TEXTURE_DENSITY_CK
	{7,   "Show Glare Texture"}, //DBGRAPHIC_GLARE_DISP
	{8,   "Draw Frame (for Win32)"}, //DBGRAPHIC_SCREEN_FRAME
	{9,   "disable anisotropic filter"}, //DBGRAPHIC_NO_ANISO
	{10,  "Use anisotropic filter at intensity 4"}, //DBGRAPHIC_4_ANISO
	{11,  "Show texture mip at max level"}, //DBGRAPHIC_MIP_MAX_ON
	{12,  "Turn off shadow camera culling"}, //DBGRAPHIC_STP_SHADOWCAM_VFCUL
	{13,  "Show effect shader load"}, //DBGRAPHIC_EFFECTSHADER_PERFORMANCE
	{14,  "Visualization of a cut model written to a stencil"}, //DBGRAPHIC_CUTMODEL_STENCIL_VIEW
	{15,  "Color-coded display of LOD status"}, //DBGRAPHIC_LOD_DRAW
	{16,  "Draw only models using alpha texture"}, //DBGRAPHIC_DRAW_ALPHA_ONLY
	{17,  "Draw only models using punch texture"}, //DBGRAPHIC_DRAW_PUNCH_ONLY
	{18,  "Draw only the shadow drop model"}, //DBGRAPHIC_DRAW_SHADOW_CAST
	{19,  "Draw only the shadow cast model"}, //DBGRAPHIC_DRAW_SHADOW_REC
	{20,  "Draw only models with blur"}, //DBGRAPHIC_DRAW_BLUR_MODEL
	{21,  "draw wireframe"}, //DBGRAPHIC_WIRE_FRAME_MODE
	{32,  "shadow caster OT hidden"}, //DBGRAPHIC_NODRAW_SHADOW_CAST
	{33,  "shadow receiver OT hidden"}, //DBGRAPHIC_NODRAW_SHADOW_RCV
	{34,  "model OT hidden"}, //DBGRAPHIC_NODRAW_MODEL_OT
	{35,  "DEFERRED hidden"}, //DBGRAPHIC_NODRAW_DEFERRED
	{36,  "Presort3DOT hidden"}, //DBGRAPHIC_NODRAW_PRESORT3D
	{37,  "Resolve After 3DOT Hide"}, //DBGRAPHIC_NODRAW_RESOLVEAFTER3D
	{38,  "3D sort OT hide"}, //DBGRAPHIC_NODRAW_SORT3D
	{48,  "turn off deferred shader function"}, //DBGRAPHIC_NODRAW_SORT3D
	{49,  "Clear the G buffer"}, //DBGRAPHIC_DEFERRED_OFF
	{50,  "stop GA switching process"}, //DBGRAPHIC_G_BUFFER_CLEAR
	{51,  "draw deferrred light"}, //DBGRAPHIC_GA_AREA_OFF
	{52,  "Automatic resolution change OFF (fixed to LV1)"}, //DBGRAPHIC_VARIABLE_RESOLUTION_LV1
	{53,  "Automatic resolution change OFF (fixed to LV2)"}, //DBGRAPHIC_VARIABLE_RESOLUTION_LV2
	{54,  "Automatic resolution change OFF (fixed to LV3)"}, //DBGRAPHIC_VARIABLE_RESOLUTION_LV3
	{56,  "Turn off the model query function"}, //DBGRAPHIC_MODEL_QUERY_OFF
	{57,  "keep current query result"}, //DBGRAPHIC_MODEL_QUERY_KEEP
	{64,  "SSLBMAP drawing"}, //DBGRAPHIC_SSLBMAP_DISP
	{65,  "SSAOMAP draw"}, //DBGRAPHIC_SSAOMAP_DISP
	{66,  "GI function OFF"}, //DBGRAPHIC_GI_OFF
	{67,  "emit a sphere to visualize data for GI"}, //DBGRAPHIC_GI_SPHERE_DISP
	{68,  "draw GI only"}, //DBGRAPHIC_GI_ONLY
	{69,  "force GI update"}, //DBGRAPHIC_GI_KYOUSEI_UPDATE
	{70,  "Do not use atmospheric scattering"}, //DBGRAPHIC_AIR_SCATTERING_OFF
	{71,  "start safety net system"}, //DBGRAPHIC_SAFETYNET_START
	{73,  "Use Texture Motion Blur"}, //DBGRAPHIC_TEXTURE_BLUR
	{74,  "HDAO Force OFF"}, //DBGRAPHIC_HDAO_OFF
	{75,  "Camera Blur ON"}, //DBGRAPHIC_TEXTURE_BLUR_CAM_ON
	{76,  "Show Histogram"}, //DBGRAPHIC_HISTOGRAM_ON
	{77,  "Show Texture Size"}, //DBGRAPHIC_TEXTURE_SIZE
	{78,  "Start at a resolution without AA"}, //DBGRAPHIC_NOAA_RESOLUTION
	{79,  "test shader on"}, //DBGRAPHIC_SHADER_TEST
	{80,  "Show model albedo"}, //DBGRAPHIC_ALBEDO_SHADER
	{81,  "Show model normals (without normal map)"}, //DBGRAPHIC_NORMAL_SHADER
	{82,  "Show model normals (with normal map)"}, //DBGRAPHIC_NORMAL_TEX_SHADER
	{83,  "show vertex color"}, //DBGRAPHIC_COLOR_SHADER
	{84,  "show vertex alpha"}, //DBGRAPHIC_COLOR_ALPHA_SHADER
	{85,  "Show UV0"}, //DBGRAPHIC_UV_SHADER
	{86,  "Show Tangent"}, //DBGRAPHIC_TANGENT_SHADER
	{87,  "Change lightmap to white"}, //DBGRAPHIC_NO_LIGHTMAP
	{88,  "change albedo texture to white"}, //DBGRAPHIC_NO_ALBEDO
	{89,  "change cube map to gray"}, //DBGRAPHIC_CUBE_MAP_GLAY
	{90,  "show lightmap only"}, //DBGRAPHIC_LIGHTMAP_ONLY
	{96,  "no loss of distance"}, //DBGRAPHIC_DIST_LOST_OFF
	{97,  "Lost over 80M"}, //DBGRAPHIC_FAR_LOST_80M
	{98,  "Lost over 40M"}, //DBGRAPHIC_FAR_LOST_40M
	{99,  "do not draw occluder"}, //DBGRAPHIC_OCCLUDER_NO_DRAW
	{100, "Turn off GA-related switching functions"}, //DBGRAPHIC_GA_KEEP_EXIT
	{101, "High resolution mode for capturing events and such"}, //DBGRAPHIC_HI_RESOLUTION
	{102, "medium resolution mode for capturing events and such"}, //DBGRAPHIC_MID_RESOLUTION
	{103, "Don't do shadow new culling"}, //DBGRAPHIC_SHADOW_NEWCULL_OFF
	{104, "Hide SCR models over a certain number"}, //DBGRAPHIC_DISP_STOP_SCR
	{108, "force OBJ-based shadow receiver calculation to be turned off"}, //DBGRAPHIC_OBJ_SHADOW_RECEIVE_OFF
	{109, "make camera far 5 times"}, //DBGRAPHIC_CAM_FAR_LONG
	{112, "change max shader for LOD"}, //DBGRAPHIC_LOD_SHADER_UPDATE
	{113, "Change LOD MAX shader to one that does not calculate normals"}, //DBGRAPHIC_LOD_NORMAL_OFF
	{114, "Change LOD MAX shader to not calculate cubemaps"}, //DBGRAPHIC_LOD_CUBEMAP_OFF
	{115, "Change LOD MAX shader to not calculate specular"}, //DBGRAPHIC_LOD_SPECULAR_OFF
	{116, "LODLV2 is the target of shader change"}, //DBGRAPHIC_LOD_LEVEL2
	{117, "LODLV3 is the target of shader changes"}, //DBGRAPHIC_LOD_LEVEL3
	{122, "Change all Dynamic calculations to Cube method"}, //DBGRAPHIC_ENL_IRRADIANCE_CUBE
	{128, "Special processing for model parameter replacement"}, //DBGRAPHIC_TOOL_WRITE_OFF
	{129, "Processing for events (joint information is not updated when the cut is stopped)"}, //DBGRAPHIC_MTX_NO_UPDATE
	{130, "Texture shooting for Enlighten"}, //DBGARPHIC_ENLIGHTEN_TEXTURE_SHOT
	{131, "Force resolve PreEff texture every frame"}, //DBGRAPHIC_FORCE_PREEFF_RESOLVE
	{132, "force effect shader downgrade"}, //DBGRAPHIC_EFFECTSHADER_DOWNGRADE
	{133, "buffer clear color black"}, //DBGRAPHIC_BUFFERCLEAR_COLORBLACK
	{134, ""}, //DBGRAPHIC_LIGHT_MOVE
	{135, "Enlighten's Irradiance Processing OFF"}, //DBGRAPHIC_ENLIGHTEN_IRRADIANCE_OFF
	{136, "Enlighten update stop"}, //DBGARPHIC_ENLIGHTEN_SUSPEND
	{137, "Do not use EnlightenCube texture"}, //DBGARPHIC_ENLIGHTEN_CUBE_OFF
	{138, "Show only Irradiance"}, //DBGARPHIC_ENLIGHTEN_IRRADIANCE_DISP
	{139, "texture shoot jump only"}, //DBGARPHIC_TEXTURE_SHOT_JUMPONLY
	{140, "skylight off"}, //DBGARPHIC_SKY_LIGHT_OFF
	{141, "Take a picture of the map you are on"}, //DBGARPHIC_ENLIGHTEN_TEXTURE_SHOT2
	{142, "Display the center position of the Enlighten cube map"}, //DBGARPHIC_ENLIGHTEN_CUBEVOLUME_DISP
	{144, "Use UI simple shader"}, //DBGRAPHIC_UI_SIMPLE_SHADER
};

std::unordered_map<int, const char*> replacements_STA = {
	{0,  "Pausing"}, //STA_PAUSE
	{1,  "Event" }, //STA_EVENT
	{2,  "soft event in progress"}, //STA_SOFT_EVENT
	{3,  "Two update mode for event"}, //STA_EVENT_2MOVE
	{4,  "Draw skip in 2-time update mode for events"}, //STA_EVENT_2MOVE_DRAW_SKIP_FRAME
	{5,  "Suspending"}, //STA_SUSPEND
	{6,  "Continuing"}, //STA_CONTINUE
	{7,  "in credit not playable"}, //STA_CREDIT_NO_PLAYABLE
	{8,  "Multi-threaded BEHEVIOR processing"}, //STA_MT_BEHEVIOR
	{9,  "bunker mode"}, //STA_BUNKER
	{10, "kumite in progress"}, //STA_QTE
	{11, "Event mode (for script)"}, //STA_SCRIPT_EVENT_MODE
	{12, "Linking event camera" }, //STA_EVENT_CAMERA_LINK
	{16, "hacking transition"}, //STA_HACKING_TRANSITION
	{17, "Gallery in progress"}, //STA_GALLERY
	{18, "displaying subtitles"}, //STA_CAPTION
	{19, "Shooting"}, //STA_SHOOTING
	{20, "drawing model in sub screen"}, //STA_SUBSCREEN_DRAW_MODEL
	{21, "update pause-out animation capture"}, //STA_PAUSE_CAPTURE
	{22, "Local Area Action Invalid"}, //STA_LOCAL_AREA_INVALID
	{23, "Adam Network OFF on P200"}, //STA_P200_110_ADAM_NET_OFF
	{24, "fast travel in progress"}, //STA_FAST_TRAVEL
	{25, "debug mode after clear"}, //STA_FLAG_DBG_MODE
	{26, "during novel"}, //STA_NOVEL
	{27, "in screen-stop type dialog"}, //STA_DIALOG_PAUSE
	{28, "Screen stop single picture display"}, //STA_PAUSE_IMAGE_VIEW
	{29, "camera leaving player behind"}, //STA_CAMERA_LEAVE_PLAYER
	{30, "end roll shooting"}, //STA_ENDROLL_SHOOTING
	{31, "during movie"}, //STA_MOVIE
	{33, "Stopping screen to show tutorial"}, //STA_PAUSE_TUTORIAL
	{34, "Selecting item with playable shortcut"}, //STA_SHORTCUT_ITEM
	{35, "In the enemy set room"}, //STA_ENEMYSET_ROOM
	{36, "Playing storybook in data menu"}, //STA_PICTUREBOOK_PLAY
	{37, "event skip check only"}, //STA_PAUSE_SKIPCHECK_ONLY
	{38, "show mouse cursor"}, //STA_MOUSE_CURSOR_ON
	{39, "save ban for scenario reasons"}, //STA_SAVE_BAN
	{40, "in elevator"}, //STA_ELEV
	{41, "playing novel in data menu"}, //STA_NOVEL_PLAY
	{42, "Hap fade-out in progress"}, //STA_HAP_FADEOUT
	{42, "Hap fade out"}, //STA_HAP_FADEOUT {43, "Do not open debug menu after clear"}, //STA_ACDB_BAN
	{48, "Search Mode"}, //STA_SEARCH_MODE
	{49, "UI bug display"}, //STA_UI_BUG
	{50, "Game Center"}, //STA_GAMECENTER
	{51, "Enemies are not dropping anything"}, //STA_EM_NO_ITEM_DORP
	{52, "auto state"}, //STA_HAP_AUTO
	{53, "auto4,5 status"}, //STA_HAP_AUTO_HIGH
	{54, "save data delete event"}, //STA_DATA_DEL_EVENT
	{55, "make enemy wait in standby state"}, //STA_EM_WAIT
	{58, "Interceptor gun in progress"}, //STA_CIWS
	{59, "in bunker elevator"}, //STA_BUNKER_ELEV
	{60, "No PL Damage"}, //STA_PL_NO_DAMAGE
	{61, "window conversation display event"}, //STA_EVENT_WINDOW_TALK
	{62, "gimmick access in progress"}, //STA_ACCESS_OBJ
	{63, "During Kaiju STG"}, //STA_KAIJU_STG
};

std::unordered_map<int, const char*> replacements_STOP = {
	{0,  "object stop"}, //STP_OBJ
	{1,  "PL's move stop"}, //STP_PL
	{2,  "EM's move stop"}, //STP_EM
	{3,  "ETC move stop"}, //STP_ETCOBJ
	{4,  "Stop Effect"}, //STP_ESP
	{5,  "SCR stop"}, //STP_SCR
	{6,  "UI stop"}, //STP_UI
	{7,  "filter stop"}, //STP_FILTER
	{8,  "shadow stop"}, //STP_SHADOW
	{9,  "PL move stop 2"}, //STP_SUBPL
	{10, "Player keystroke stop"}, //STP_PL_KEY
	{11, "stop player's move key only"}, //STP_PL_MOVE_KEY
	{12, "Attack Key Stop"}, //STP_PL_ATTACK_KEY
	{13, "Stop IK for all objects"}, //STP_IK
	{14, "camera key stop"}, //STP_PL_CAM_KEY
	{15, "stop trigger processing"}, //STP_TRIGGER
	{16, "stop event skip"}, //STP_EVENT_SKIP
	{17, "stop scenario"}, //STP_SCENARIO
	{18, "stop game update"}, //STP_GAME_UPDATE
	{19, "stop enemy attack"}, //STP_EM_ATK
	{20, "Stop PL's face to the enemy"}, //STP_PL_NO_FACE_MOVE
	{32, "stop converting keyboard input to pad"}, //STP_KEYBOARD_PAD
	{33, "Do not limit mouse movement range"}, //STP_MOUSE_RESTRICTION
	{34, "Player keystroke stop (for HAP)"}, //STP_HAP_PL_KEY
	{48, "Stop the ComputeUpdate path of the effect"}, //STP_ESP_COMPUTE_UPADTE
	{49, "stop line check for effect"}, //STP_ESP_LINECHECK
};

std::unordered_map<int, const char*> replacements_GRAPHIC = {
	{2,  "Deferred mode"}, //GRAPHIC_DEFERRED
	{9,  "Disable DOF"}, //GRAPHIC_NO_DOF
	{12, "Disable blur. This is the type of thing you can optionally switch to."}, //GRAPHIC_BLUR_OFF_SYSTEM
	{13, "Disable perform blur processing. This is the type of thing you can optionally switch to."}, //GRAPHIC_BLUR_OFF_GAME
	{15, "Stop 3D-related drawing"}, //GRAPHIC_STOP_3D_DRAW
	{16, "slow filter for player"}, //GRAPHIC_SLOW_FILTER_PLAYER
	{17, "slow filter for enemies"}, //GRAPHIC_SLOW_FILTER_ENEMY
	{18, "Do not use dynamic exposure compensation"}, //GRAPHIC_NOUSE_REALTIME_TONEMAP
	{19, "Turn off filter gradient function"}, //GRAPHIC_NOUSE_FILTER_GRADATION
	{20, "Display texture mip in maximum steps"}, //GRAPHIC_MIP_MAX_ON
	{21, "SSSS use mode ON"}, //GRAPHIC_SSSSS_MODE
	{22, "Strictly register shadow casters (check distance and size)"}, //GRAPHIC_SHADOWCAST_ENTRY_CHECK
	{32, "stop shadowcast culling"}, //GRAPHIC_STP_CAM_VFCUL
	{33, "Do not cull shadow casters"}, //GRAPHIC_NO_SHW_CAST_CULL
	{34, "force update of cube map"}, //GRAPHIC_COMP_CUBE_UPDATE
	{35, "turn off camera translucency"}, //GRAPHIC_NO_CAM_ALPHA
	{36, "update preFogColor"}, //GRAPHIC_PREFOGCOLOR_UPDATE
	{37, "finish shader initialization"}, //GRAPHIC_SHADER_INIT_OVER
	{38, "DOF autofocus function ON"}, //GRAPHIC_CAM_AUTO_FOCUS
	{39, "Use SSAO version 2"}, //GRAPHIC_SSAO_VER2
	{40, "Change to forward rendering"}, //GRAPHIC_FOWARD_ON
	{41, "use CRAA"}, //GRAPHIC_CRAA_ON
	{42, "Use FXAA"}, //GRAPHIC_FXAA_ON
	{43, "use SMAA"}, //GRAPHIC_SMAA_ON
	{44, "Use SSAO"}, //GRAPHIC_SSAO_ON
	{45, "use mirror draw"}, //GRAPHIC_MIRROR_DRAW
	{46, "draw shadow off"}, //GRAPHIC_SHADOW_OFF
	{47, "rain wet shader mode"}, //GRAPHIC_RAIN_MODE
	{48, "force clear flag"}, //GRAPHIC_COMP_CLEAR_BUFF
	{49, "make UI fetch"}, //GRAPHIC_SET_UI_RESOLVE_PREVFRAME
	{50, "stop capturing"}, //GRAPHIC_STOP_RESOLVE_PREVFRAME
	{51, "Draw back PreveFrame"}, //GRAPHIC_DRAW_PREVFRAME
	{52, "Draw into buffer for UI"}, //GRAPHIC_UI_GAME_CAPTER
	{53, "Force clear flag for GBuffer"}, //GRAPHIC_COMP_CLEAR_GBUFFER
	{54, "Resolve PreEff"}, //GRAPHIC_RESOLVE_PREEFFTEXTURE
	{64, "drawing movie"}, //GRAPHIC_DRAW_MOVIE
	{80, "stop updating compute particle"}, //GRAPHIC_COMPUTE_PARTICLE_NO_UPDATE
	{81, "stop drawing compute particle"}, //GRAPHIC_COMPUTE_PARTICLE_NO_DRAW
	{82, "compute particle collision"}, //GRAPHIC_COMPUTE_PARTICLE_COLLISION
	{83, "compute particle vector field"}, //GRAPHIC_COMPUTE_PARTICLE_VECTORFIELD
	{84, "Do not GPUSORT"}, //GRAPHIC_COMPUTE_PARTICLE_NO_BITONICSORT
	{85, "Do not use fixed length sort"}, //GRAPHIC_COMPUTE_PARTICLE_NO_FIXEDSORT
	{86, "use radixsort"}, //GRAPHIC_COMPUTE_PARTICLE_RADIXSORT
	{94, "use asynchronous compute"}, //GRAPHIC_COMPUTE_ASYNC
	{95, "Use RadixSort"}, //GRAPHIC_COMPUTE_PARTICLE_RADIXSORT
};

std::unordered_map<int, const char*> replacements_DISP = {
	{0,  "Stop drawing objects"}, //DISP_NO_OBJ
	{1,  "Stop drawing the player"}, //DISP_NO_PL
	{2,  "Stop drawing enemies"}, //DISP_NO_EM
	{3,  "Stop drawing other models"}, //DISP_NO_ETCOBJ
	{4,  "Stop drawing effects"}, //DISP_NO_ESP
	{5,  "Stop drawing scroll"}, //DISP_NO_SCR
	{6,  "Stop drawing movie"}, //DISP_NO_MOVIE
	{7,  "Stop drawing seamless map"}, //DISP_NO_SEAMLESS_MAP
	{11, "Stop drawing weapon model"}, //DISP_NO_WP
	{12, "Stop drawing BM model"}, //DISP_NO_BMOBJ
	{13, "Stop drawing BA model"}, //DISP_NO_BAOBJ
	{14, "Stop drawing BH model"}, //DISP_NO_BHOBJ
};

std::unordered_map<int, const char*> replacements_GAME = {
	{0, "camera up/down reverse"}, //GAME_CAMERA_UD_REVERSE
	{1, "camera left/right reverse"}, //GAME_CAMERA_RL_REVERSE
	{6, "Reading 2B and A2"}, //GAME_PLAYER_2BA2
	{7, "Reading 9S and A2"}, //GAME_PLAYER_9SA2
	{8, "player weak attack prohibited"}, //GAME_STOP_PL_ATK_S
	{9, "player strong attack prohibited"}, //GAME_STOP_PL_ATK_L
	{10, "Player Jump Prohibited"}, //GAME_STOP_PL_JUMP
	{11, "Player Shooting Prohibited"}, //GAME_STOP_PL_GUN
	{12, "Player Skill A Prohibited"}, //GAME_STOP_PL_SKILL_A
	{13, "Player Skill B Prohibited"}, //GAME_STOP_PL_SKILL_B
	{14, "Player Evasion Prohibited"}, //GAME_STOP_PL_ESCAPE
	{15, "player movement prohibited"}, //GAME_STOP_PL_MOVE
	{16, "player funnel mode"}, //GAME_PLAYER_FUNNEL
	{17, "hacking"}, //GAME_HACKING
	{18, "set player to 9s"}, //GAME_PLAYER_9S
	{19, "set player to A2"}, //GAME_PLAYER_A2
	{20, "[Unimplemented] Make player Pascal"}, //GAME_PLAYER_PASCAL
	{21, "2B dying"}, //GAME_2B_BRINK
	{22, "Free-flight enabled"}, //GAME_FREE_FLIGHT
	{23, "Buddy pascal enabled"}, //GAME_NPC_PASCAL
	{24, "player walk talk only"}, //GAME_PL_WALK_TALK_ONLY
	{25, "make loading display long"}, //GAME_LOADING_LONG
	{26, "hacking game (humanoid)"}, //GAME_HACKING_HUMANOID
	{27, "stop the cyberspace"}, //GAME_CYBERSPACE_STOP
	{28, "stop attacking in cyberspace"}, //GAME_CYBERSPACE_ATK_STOP
	{29, "friendly fire limit"}, //GAME_FRIENDLY_FIRE_DISABLE
	{30, "start attacking in cyberspace"}, //GAME_CYBERSPACE_ATK_START
	{31, "short the loading display"}, //GAME_LOADING_SHORT
	{32, "long range hacking of loading display"}, //GAME_LOADING_HACKING
	{33, "Stop the cube around Adam's fight"}, //GAME_ADAM_QUBE_STOP
	{34, "During staff roll"}, //GAME_STAFFROLL
	{35, "Move towards the cube in the Adam battle"}, //GAME_ADAM_QUBE_MOVE
	{36, "saving is prohibited"}, //GAME_SAVE_BAN
	{37, "DLC has been unmounted"}, //GAME_DLC_UNMOUNT
	{38, "waiting for restart after DLC removed"}, //GAME_DLC_UNMOUNT_WAIT
};
