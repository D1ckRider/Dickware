#include <string>
#include "json.hpp"
#include <deque>
#include <fstream>
#include <istream>
#include "valve_sdk\csgostructs.hpp"
#include "valve_sdk\misc\Color.hpp"
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
	void SaveColorValue(json &j, std::string name, const Color& value);
	void LoadColorValue(json &j, std::string name, Color& value);
	std::string LoadGameCfg();

	enum WeaponType
	{
		WEAPON_PISTOL = 0,
		WEAPON_DEAGLE = 1,
		WEAPON_REVOLVER = 2,
		WEAPON_SMG = 3,
		WEAPON_SHOTGUN = 4,
		WEAPON_MACHINEGUN = 5,
		WEAPON_RIFLE = 6,
		WEAPON_SSG08 = 7,
		WEAPON_AWP = 8,
		WEAPON_AUTO = 9,
		WEAPON_SNIPER = 10
	};

	enum HitboxType
	{
		HEAD,
		NECK,
		PELVIS,
		CHEST,
		STOMACH,
		ARM,
		LEG,
		FOOT
	};

	namespace RageBot
	{
		enum AntiAimType
		{
			STANDING = 0,
			MOVING = 1,
			AIR = 3
		};

		struct RBotWeaponSetting
		{
			float Hitchance;
			float MinDamage;
			int BAimAfterShots;
			int ForceBAimAfterShots;
			bool BAimWhileMoving;
		};

		struct RBotHitbox
		{
			bool Enabled;
			float Scale;
		};

		struct AntiAim
		{
			int Pitch;
			int Yaw;
			float YawCustom;
			int YawAdd;
			float YawAddCustom;
			float Range;
			int FakelagMode;
			int FakelagTicks;
		};

		extern bool Enabled;
		extern RBotWeaponSetting WeaponSettings[11];
		extern int BAimMode;
		extern bool AirBAim;
		extern int BAimHotkey;

		extern RBotHitbox Hitboxes[7];

		extern bool EnabledAA;
		extern AntiAim AntiAimSettings[3];
		extern float SpinBotSpeed;
		extern bool SlideWalk;
		extern bool Desync;
		extern int ManualAAState;
		extern int ManualAALeftKey;
		extern int ManualAARightKey;
		extern int ManualAABackKey;

		extern bool AutoScope;
		extern bool AutoStop;
		extern bool AutoCrouch;
		extern bool SlowWalk;
		extern int SlowWalkHotkey;

		extern bool FakelagPrediction;
		extern int ShootingMode;
		extern bool ForceUnlag;
		extern bool Resolver;

		int GetWeaponType(C_BaseCombatWeapon* weapon);
	}

	namespace Aimbot
	{
		struct LBotWeaponSetting
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
		extern int Hotkey;
		extern LBotWeaponSetting WeaponAimSetting[11];
		extern bool Backtrack;
		extern bool BacktrackAtAim;
		extern float BacktrackTick;

		int GetWeaponType(C_BaseCombatWeapon* weapon);
	}

	namespace Visual
	{
		struct Chams
		{
			bool Enabled;
			int Mode;
			Color Visible;
			Color Invisible;
		};

		struct PlayerESP
		{
			bool Enabled;
			bool BoxEnabled;
			Color BoxColor;
			int BoxType;
			bool NameEnabled;
			Color NameColor;
			bool HealthEnabled;
			bool ArmorEnabled;
			Color ArmorColor;
			bool WeaponEnabled;
			Color WeaponColor;
			bool SnaplineEnabled;
			Color SnaplineColor;
		};
		struct OtherESP
		{
			bool Enabled;
			bool GrenadeEnabled;
			bool BombEnabled;
			Color BombColor;
			bool DZEnabled;
			float DZRange;
		};

		extern Chams LocalChams;
		extern PlayerESP LocalESP;
		extern Chams EnemyChams;
		extern PlayerESP EnemyESP;
		extern Chams TeamChams;
		extern PlayerESP TeamESP;

		extern bool ThirdPersonEnabled;
		extern int ThirdPersonHotkey;

		extern OtherESP GlobalESP;

		extern bool NoScopeOverlay;
		extern bool BulletTracers;
		extern bool NoFlash;
		extern bool SpreadCircleEnabled;
		extern Color SpreadCircleColor;
		extern bool DisableScopeZoom;
		extern int ViewModelFOV;
		extern int FOV;
		extern bool NoSmoke;
		extern bool Hitmarker;
		extern int RagdollForce;
	}

	namespace Misc
	{
		extern bool BHop;
		extern bool AutoStrafe;
		extern bool RankReveal;
		extern bool NoCrouchCooldown;
		extern bool Clantag;
		/*extern bool BuyBot;
		extern int BuyBotPistol;
		extern int */

	}
}