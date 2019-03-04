#include "definitions.h"
#include "valve_sdk\csgostructs.hpp"

std::map<int, wchar_t> g_WeaponIcons =
{
	{ ItemDefinitionIndex::WEAPON_MP9, 'O' },
	{ ItemDefinitionIndex::WEAPON_UMP45, 'L' },
	{ ItemDefinitionIndex::WEAPON_TASER, 'h' },
	{ ItemDefinitionIndex::WEAPON_NOVA, 'e' },
	{ ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL, 'E'},
	{ ItemDefinitionIndex::WEAPON_P90, 'P'},
	{ ItemDefinitionIndex::WEAPON_HKP2000, 'E'},
	{ ItemDefinitionIndex::WEAPON_REVOLVER, 'J'},
	{ ItemDefinitionIndex::WEAPON_MAG7, 'd'},
	{ ItemDefinitionIndex::WEAPON_SAWEDOFF, 'c'},
	{ ItemDefinitionIndex::WEAPON_SCAR20, 'Y'},
	{ ItemDefinitionIndex::WEAPON_SG556, 'V'},
	{ ItemDefinitionIndex::WEAPON_XM1014, 'b'},
	{ ItemDefinitionIndex::WEAPON_USP_SILENCER, 'G'},
	{ ItemDefinitionIndex::WEAPON_MOLOTOV, 'l'},
	{ ItemDefinitionIndex::WEAPON_MP7, 'N'},
	{ ItemDefinitionIndex::WEAPON_KNIFE_PUSH, '9'},
	{ ItemDefinitionIndex::WEAPON_M4A1, 'S'},
	{ ItemDefinitionIndex::WEAPON_NEGEV, 'f'},
	{ ItemDefinitionIndex::WEAPON_M4A1_SILENCER, 'T'},
	{ ItemDefinitionIndex::WEAPON_MAC10, 'K'},
	{ ItemDefinitionIndex::WEAPON_TEC9, 'H'},
	{ ItemDefinitionIndex::WEAPON_KNIFE_T, '['},
	{ ItemDefinitionIndex::WEAPON_SSG08, 'a'},
	{ ItemDefinitionIndex::WEAPON_M249, 'g'},
	{ ItemDefinitionIndex::WEAPON_SMOKEGRENADE, 'k'},
	{ ItemDefinitionIndex::WEAPON_FAMAS, 'R'},
	{ ItemDefinitionIndex::WEAPON_KNIFE_GUT, '3'},
	{ ItemDefinitionIndex::WEAPON_KNIFE_FALCHION, '2'},
	{ ItemDefinitionIndex::WEAPON_FLASHBANG, 'i'},
	{ ItemDefinitionIndex::WEAPON_AWP, 'Z'},
	{ ItemDefinitionIndex::WEAPON_G3SG1, 'X'},
	{ ItemDefinitionIndex::WEAPON_GALILAR, 'Q'},
	{ ItemDefinitionIndex::WEAPON_GLOCK, 'D'},
	{ ItemDefinitionIndex::WEAPON_DEAGLE, 'A'},
	{ ItemDefinitionIndex::WEAPON_HEGRENADE, 'j'},
	{ ItemDefinitionIndex::WEAPON_P250,'F'},
	{ ItemDefinitionIndex::WEAPON_INCGRENADE, 'n'},
	{ ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET, '5'},
	{ ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT, '4'},
	{ ItemDefinitionIndex::WEAPON_DECOY, 'M'},
	{ ItemDefinitionIndex::WEAPON_ELITE, 'B'},
	{ ItemDefinitionIndex::WEAPON_AK47, 'W'},
	//{ItemDefinitionIndex::WEAPON_KNIFE_BAY, 'r'},
	{ ItemDefinitionIndex::WEAPON_FIVESEVEN, 'C'},
	{ ItemDefinitionIndex::WEAPON_BIZON, 'M'},
	{ ItemDefinitionIndex::WEAPON_CZ75A, 'I'},
	{ ItemDefinitionIndex::WEAPON_KNIFE_FLIP, 'v'},
	{ ItemDefinitionIndex::WEAPON_AUG, 'U'},
	{ ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY, '8'},
	{ ItemDefinitionIndex::WEAPON_C4, 'o'},
	{ ItemDefinitionIndex::WEAPON_KNIFE, ']'}
};

std::vector<std::string> radio_name = {
	"VRX",
	"Anison.fm",
	"Chillsky",
	"DKFM Shoegaze Radio",
	"Primal Radio",
	"Synthetic FM"
};

std::wstring stations[] = {
	L"http://vrx.piro.moe:8000/stream-256",
	L"http://pool.anison.fm:9000/AniSonFM(320)?nocache=0.752104723294601",
	L"http://hyades.shoutca.st:8043/autodj",
	L"http://bb32.sonixcast.com:8080/8724",
	L"http://199.101.51.168:8058/stream",
	L"http://stream.syntheticfm.com:8040/stream"
};