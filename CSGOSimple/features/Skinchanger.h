#pragma once
#include "..\valve_sdk\sdk.hpp"
#include "..\valve_sdk\csgostructs.hpp"
#include "..\singleton.hpp"
#include "..\json.hpp"
#include "..\helpers\DynSkin.h"
#include <unordered_map>
#include <vector>
#include <string>

using json = nlohmann::json;

struct EconomyItemCfg
{
	int iItemDefinitionIndex = 0;
	int nFallbackPaintKit = 0;
	int nFallbackSeed = 0;
	int nFallbackStatTrak = -1;
	int iEntityQuality = 4;
	char* szCustomName = nullptr;
	float flFallbackWear = 0.1f;
};

struct SkinInfo_t
{
	unsigned int	_id;			// index u know for what

	std::string		_shortname;		// shortname
	std::string		_name;			// full skin name
};


extern std::unordered_map<int, EconomyItemCfg>  g_SkinchangerCfg;
extern std::unordered_map<int, const char*> g_ViewModelCfg;
extern std::unordered_map<const char*, const char*> g_KillIconCfg;

class Skinchanger : public Singleton<Skinchanger>
{
public:
	void OnFrameStageNotify();
	void LoadAllSkinsForWeapon();
	void SaveCustomSkins();
	void LoadCustomSkins();
private:
	bool ApplyCustomSkin(CHandle<C_BaseCombatWeapon> &WeaponHandle);
	bool ApplyCustomModel(C_BasePlayer* player, C_BaseAttributableItem* item);
};

