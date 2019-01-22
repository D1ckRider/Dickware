#include "Settings.h"
#include <filesystem>
#include "Logger.h"

namespace fs = std::filesystem;

/* Configs Stuff */
std::deque<std::string> Settings::Configs;
std::string Settings::AppDataFolder = "";

void Settings::Initialize()
{
	AppDataFolder = std::string(getenv("APPDATA")) + "\\DickwareBeta\\";

	if (!fs::exists(AppDataFolder))
		fs::create_directory(AppDataFolder);

	if (!fs::exists(AppDataFolder + "configs\\"))
		fs::create_directory(AppDataFolder + "configs\\");

	RefreshConfigList();
}

void Settings::CreateConfig(std::string fileName)
{
	std::string orgfile = fileName;

	if (fs::exists(AppDataFolder + "configs\\" + fileName))
		return;

	std::ofstream ofs(AppDataFolder + "configs\\" + fileName);
	ofs << "";
	ofs.close();

	g_Logger.Success("CONFIG", "config " + orgfile + " created");

	RefreshConfigList();
}

void Settings::RefreshConfigList()
{
	Configs.clear();

	for (auto& p : fs::directory_iterator(AppDataFolder + "configs\\"))
	{
		fs::path path = fs::path(p);
		Configs.push_back(path.filename().string());
	}

	g_Logger.Info("CONFIG", "refreshed config list");
}

void Settings::SaveSettings(std::string fileName)
{
	std::string orgfile = fileName;
	fileName = AppDataFolder + "configs\\" + fileName;

	std::ofstream o(fileName);
	json j;

	/* Write legitbot settings */
	j["lbot_enabled"] = Aimbot::Enabled;
	j["lbot_key"] = Aimbot::Hotkey;
	for ( int i = 0; i < 9; i++ )
	{
		/* Base Settings */
		j["lbot_weapon" + std::to_string(i)]["FOV"] = Aimbot::WeaponAimSetting[i].FOV;
		j["lbot_weapon" + std::to_string(i)]["Smooth"] = Aimbot::WeaponAimSetting[i].Smooth;
		j["lbot_weapon" + std::to_string(i)]["Randomize"] = Aimbot::WeaponAimSetting[i].Randomize;
		j["lbot_weapon" + std::to_string(i)]["Delay"] = Aimbot::WeaponAimSetting[i].Delay;
		/* RCS */
		j["lbot_weapon" + std::to_string(i)]["RCS"] = Aimbot::WeaponAimSetting[i].RCS;
		j["lbot_weapon" + std::to_string(i)]["RCS_X"] = Aimbot::WeaponAimSetting[i].RCS_X;
		j["lbot_weapon" + std::to_string(i)]["RCS_Y"] = Aimbot::WeaponAimSetting[i].RCS_Y;
		/* Hitboxes */
		j["lbot_weapon" + std::to_string(i)]["Head"] = Aimbot::WeaponAimSetting[i].HitboxHead;
		j["lbot_weapon" + std::to_string(i)]["Neck"] = Aimbot::WeaponAimSetting[i].HitboxNeck;
		j["lbot_weapon" + std::to_string(i)]["Chest"] = Aimbot::WeaponAimSetting[i].HitboxChest;
		j["lbot_weapon" + std::to_string(i)]["Pelvis"] = Aimbot::WeaponAimSetting[i].HitboxPelvis;
		j["lbot_weapon" + std::to_string(i)]["Stomach"] = Aimbot::WeaponAimSetting[i].HitboxStomach;
		j["lbot_weapon" + std::to_string(i)]["Arm"] = Aimbot::WeaponAimSetting[i].HitboxArm;
		j["lbot_weapon" + std::to_string(i)]["Leg"] = Aimbot::WeaponAimSetting[i].HitboxLeg;
		j["lbot_weapon" + std::to_string(i)]["Foot"] = Aimbot::WeaponAimSetting[i].HitboxFoot;
	}
	j["lbot_backtrack"] = Aimbot::Backtrack;
	j["lbot_backtrack_aim"] = Aimbot::BacktrackAtAim;
	j["lbot_backtrack_tick"] = Aimbot::BacktrackTick;

	/* Write file */
	o << std::setw(4) << j << std::endl;
}

void Settings::LoadSettings(std::string fileName)
{
	std::string orgfile = fileName;
	fileName = AppDataFolder + "configs\\" + fileName;

	std::ifstream i(fileName);
	json j;
	i >> j;
	
	/* LBot Settings */
	Aimbot::Enabled = j["lbot_enabled"];
	Aimbot::Hotkey = j["lbot_key"];
	for (int i = 0; i < 9; i++)
	{
		/* Base Weapon Settings */
		Aimbot::WeaponAimSetting[i].FOV = j["lbot_weapon" + std::to_string(i)]["FOV"];
		Aimbot::WeaponAimSetting[i].Smooth = j["lbot_weapon" + std::to_string(i)]["Smooth"];
		Aimbot::WeaponAimSetting[i].Randomize = j["lbot_weapon" + std::to_string(i)]["Randomize"];
		Aimbot::WeaponAimSetting[i].Delay = j["lbot_weapon" + std::to_string(i)]["Delay"];
		/* RCS */
		Aimbot::WeaponAimSetting[i].RCS = j["lbot_weapon" + std::to_string(i)]["RCS"];
		Aimbot::WeaponAimSetting[i].RCS_X = j["lbot_weapon" + std::to_string(i)]["RCS_X"];
		Aimbot::WeaponAimSetting[i].RCS_Y = j["lbot_weapon" + std::to_string(i)]["RCS_Y"];
		/* Hitboxes */
		Aimbot::WeaponAimSetting[i].HitboxHead = j["lbot_weapon" + std::to_string(i)]["Head"];
		Aimbot::WeaponAimSetting[i].HitboxNeck = j["lbot_weapon" + std::to_string(i)]["Neck"];
		Aimbot::WeaponAimSetting[i].HitboxChest = j["lbot_weapon" + std::to_string(i)]["Chest"];
		Aimbot::WeaponAimSetting[i].HitboxPelvis = j["lbot_weapon" + std::to_string(i)]["Pelvis"];
		Aimbot::WeaponAimSetting[i].HitboxStomach = j["lbot_weapon" + std::to_string(i)]["Stomach"];
		Aimbot::WeaponAimSetting[i].HitboxArm = j["lbot_weapon" + std::to_string(i)]["Arm"];
		Aimbot::WeaponAimSetting[i].HitboxLeg = j["lbot_weapon" + std::to_string(i)]["Leg"];
		Aimbot::WeaponAimSetting[i].HitboxFoot = j["lbot_weapon" + std::to_string(i)]["Foot"];
	}
	Aimbot::Backtrack = j["lbot_backtrack"];
	Aimbot::BacktrackAtAim = j["lbot_backtrack_aim"];
	Aimbot::BacktrackTick  = j["lbot_backtrack_tick"];
}

std::string Settings::LoadGameCfg()
{
	if (!fs::exists(AppDataFolder + "GameConfig.txt"))
		return "";
	std::ifstream t(AppDataFolder + "GameConfig.txt");
	std::string str;

	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return str;
}


/* Init settings vars */
namespace Settings::Aimbot
{
	bool Enabled = false;
	int Hotkey = 0;
	WeaponSetting WeaponAimSetting[9] = { 0 };
	bool Backtrack = false;
	bool BacktrackAtAim = false;
	float BacktrackTick = false;

	int GetWeaponType(C_BaseCombatWeapon * weapon)
	{
		if ( weapon->GetItemDefinitionIndex() == WEAPON_DEAGLE )
			return WeaponTypes::WEAPON_DEAGLE;
		else if ( weapon->GetItemDefinitionIndex() == WEAPON_SSG08 )
			return WeaponTypes::WEAPON_SSG08;
		else if ( weapon->IsPistol() )
			return WeaponTypes::WEAPON_PISTOL;
		else if ( weapon->IsSubmachinegun() )
			return WeaponTypes::WEAPON_SMG;
		else if ( weapon->IsMachinegun() )
			return WeaponTypes::WEAPON_MACHINEGUN;
		else if ( weapon->IsShotgun() )
			return WeaponTypes::WEAPON_SHOTGUN;
		else if ( weapon->IsSniper() )
			return WeaponTypes::WEAPON_SNIPER;
		else
			return WeaponTypes::WEAPON_RIFLE;
	}
}

namespace Settings::Misc
{
	bool BHop = false;
	bool AutoStrafe = false;
	bool RankReveal = false;
	bool NoCrouchCooldown = false;
	bool Clantag = false;
}