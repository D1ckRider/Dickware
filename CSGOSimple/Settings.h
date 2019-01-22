#include <string>
#include "json.hpp"
#include <deque>
#include <fstream>
#include <istream>
#pragma once

using json = nlohmann::json;


namespace Settings
{
	extern std::deque<std::string> Configs;
	extern std::string AppDataFolder;
	void Initialize();
	void CreateConfig(std::string fileName);
	void RefreshConfigList();
	void SaveSettings(std::string fileName);
	void LoadSettings(std::string fileName);

	namespace Aimbot
	{
		enum WeaponTypes
		{
			WEAPON_PISTOL = 0,
			WEAPON_DEAGLE = 1,
			WEAPON_REVOLVER = 2,
			WEAPON_SMG = 3,
			WEAPON_SHOTGUN = 4,
			WEAPON_MACHINEGUN = 5,
			WEAPON_RIFLE = 6,
			WEAPON_SSG08 = 7,
			WEAPON_SNIPER = 8
		};

		struct WeaponSetting
		{
			// Temporary settings
			float FOV;
			float Smooth;
			float Randomize;
			float Delay;
			bool RCS;
			float RCS_X;
			float RCS_Y;
			bool HitboxHead;
			bool HitboxNeck;
			bool HitboxChest;
			bool HitboxPelvis;
			bool HitboxStomach;
			bool HitboxArm;
			bool HitboxLeg;
			bool HitboxFoot;
		};
		extern bool Enabled;
		extern WeaponSetting WeaponAimSetting[9];
		extern bool Backtrack;
		extern bool BacktrackAtAim;
		extern float BacktrackTick;
	}
}