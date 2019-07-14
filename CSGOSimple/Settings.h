#include <string>
#include "json.hpp"
#include <deque>
#include <fstream>
#include <istream>
#include "valve_sdk\csgostructs.hpp"
#include "valve_sdk\misc\Color.hpp"
#include "options.hpp"
#pragma once

using json = nlohmann::json;


namespace Settings
{
	extern std::deque<std::string> Configs;
	extern std::string AppDataFolder;
	void Initialize();
	void CreateConfig(std::string fileName);
	void RefreshConfigList(); 
	/* Settings Save section */
	void SaveSettings(std::string fileName);
	void SaveRageBot(json& j);
	void SaveLegitBot(json& j);
	void SaveVisual(json& j);
	void SaveMisc(json& j);
	void SaveSkinsSettings();
	/* Settings Load Section */
	void LoadSettings(std::string fileName);
	void LoadRageBot(json& j);
	void LoadLegitBot(json& j);
	void LoadVisual(json& j);
	void LoadMisc(json& j);
	void LoadSkinsSettings();
	std::string LoadGameCfg();
	/* Settings reset section */
	void ResetConfig();
	void ResetRagebot();
	void ResetAimbot();
	void ResetTriggerbot();
	void ResetVisuals();
	void ResetMisc();
	/* Values section */
	template <typename T>
	void SaveValue(json &j, std::string name, const T& value);
	template <typename T>
	void SaveNestedValue(json &j, std::string name1, std::string name2, const T& value);
	template <typename T>
	void LoadValue(json &j, std::string name, T& value);
	template <typename T>
	void LoadNestedValue(json &j, std::string name1, std::string name2, T& value);
	void SaveColorValue(json &j, std::string name, const Color& value);
	void LoadColorValue(json &j, std::string name, Color& value);
	

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
			AIR = 2
		};

		enum DesyncAAType
		{
			NONE = 0,
			STATIC = 1,
			BALANCE = 2,
			STRETCH = 3,
			JITTER = 4
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

		extern RBotHitbox Hitboxes[8];

		extern bool EnabledAA;
		extern AntiAim AntiAimSettings[3];
		extern float SpinBotSpeed;
		extern bool SlideWalk;
		extern bool Desync;
		extern int DesyncType;
		extern int ManualAAState;
		extern int ManualAALeftKey;
		extern int ManualAARightKey;
		extern int ManualAABackKey;
		extern int DesyncFlipHotkey;

		extern bool AutoScope;
		extern bool AutoStop;
		extern bool AutoCrouch;
		extern bool SlowWalk;
		extern int SlowWalkHotkey;
		extern float SlowWalkMod;
		extern bool FakeDuck;
		extern int FakeDuckHotkey;
		extern float SlowWalkMod;

		extern bool FakelagPrediction;
		extern int ShootingMode;
		extern bool ForceUnlag;
		extern bool LagComp;
		extern int LagCompType;
		extern bool Backtrack;
		extern bool Resolver;

		extern bool AimStepEnabled;
		extern int AimStepValue;

		int GetWeaponType(C_BaseCombatWeapon* weapon);
	}

	namespace Aimbot
	{
		struct LBotWeaponSetting
		{
			float FOV;
			bool Silent;
			float SilentFOV;
			int AimType;
			int FOVType;
			int SmoothType;
			float Smooth;
			float Randomize;
			int Delay;
			int KillDelay;
			bool Autopistol;
			bool RCS;
			int RCSType;
			bool RCS_SmoothEnabled;
			bool RCS_FOVEnabled;
			int RCS_X;
			int RCS_Y;
			float RCS_Smooth;
			float RCS_FOV;
			int RCS_Bullet;
			int Hitbox;
			bool AutowallEnabled;
			int MinDamage;
			bool Autofire;
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
		extern int AutofireHotkey;
		extern bool SmokeCheck;
		extern bool JumpCheck;
		extern bool FlashCheck;
		extern bool DeathmatchMode;
		extern int LegitAA;
		extern float AaSide;
		extern int AAFlipHotkey;
		extern bool DrawFov;
		
		
		extern int Priority;
		extern LBotWeaponSetting WeaponAimSetting[11];
		extern bool Backtrack;
		extern bool BacktrackAtAim;
		extern float BacktrackTick;

		int GetWeaponType(C_BaseCombatWeapon* weapon);
	}

	namespace TriggerBot
	{
		extern bool Enabled;
		extern int Key;
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

		struct Glow
		{
			bool Enabled;
			Color Visible;
			Color Invisible;
			int Type;
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
			bool DropedWeaponsEnabled;
			Color BombColor;
			bool DZEnabled;
			float DZRange;
			int RadarType;
			float RadarRange;
			int RadarAlpha;
			bool SoundESPEnabled;
		};

		extern Chams LocalChams;
		extern bool GhostEnabled;
		extern Color GhostColor;
		extern PlayerESP LocalESP;
		extern Chams EnemyChams;
		extern Glow EnemyGlow;
		extern PlayerESP EnemyESP;
		extern bool OffscreenESPEnabled;
		extern Color OffscreenESPColor;
		extern Chams TeamChams;
		extern Glow TeamGlow;
		extern PlayerESP TeamESP;

		extern bool ThirdPersonEnabled;
		extern int ThirdPersonHotkey;

		extern OtherESP GlobalESP;

		extern bool NoScopeOverlay;
		extern bool BulletTracers;
		extern bool NoFlash;
		extern bool NadeTracerEnabled;
		extern Color NadeTracerColor;
		extern bool SpreadCircleEnabled;
		extern Color SpreadCircleColor;
		extern bool DamageIndicator;
		extern Color DamageIndicatorColor;
		extern bool DisableScopeZoom;
		extern bool NightMode;
		extern float NightModeBrighthness;
		extern bool DisablePP;
		extern int ViewModelFOV;
		extern int FOV;
		extern bool NoSmoke;
		extern bool Hitmarker;
		extern bool HitmarkerSound;
		extern bool DebugInfoEnabled;
		extern int RagdollForce;

		extern int HealthPos;
		extern int ArmorPos;
	}


	namespace Skins
	{
		extern std::map<int, item_setting> m_items;
		extern std::unordered_map<std::string, std::string> m_icon_overrides;

		/*auto get_icon_override(const std::string original) const -> const char*
		{
			return m_icon_overrides.count(original) ? m_icon_overrides.at(original).data() : nullptr;
		}*/
	}

	namespace Misc
	{
		struct BuyBotNades
		{
			bool NadeMolotov;
			bool NadeDecoy;
			bool NadeFlash;
			bool NadeHE;
			bool NadeSmoke;
		};

		enum ClantagsType
		{
			None,
			Static,
			Dynamic,
			CustomStatic,
			CustomSlide
		};

		extern bool BHop;
		extern bool AutoStrafe;
		extern float Retrack;
		extern bool RankReveal;
		extern bool NoCrouchCooldown;
		extern bool Clantag;
		extern int ClantagType;
		extern bool SpectatorsEnabled;
		extern bool NoVisualRecoil;
		extern bool AutoAccept;
		extern bool BuyBot;
		extern int BuyBotPistol;
		extern int BuyBotWeapon;
		//extern BuyBotNades BuyNades;
		extern bool BuyBotArmor;
		extern bool BuyBotZeus;
		extern bool BuyBotDefuser;

		extern bool RadioEnabled;
		extern int RadioSelected;
		extern int RadioVolume;
		extern int RadioPauseHotkey;
		extern bool WatermarkEnabled;
		extern bool EventLogEnabled;

		extern bool SkinchangerEnabled;
	}

	template<typename T>
	void SaveValue(json & j, std::string name, const T & value)
	{
		if (!value)
			return;
		j[name] = value;
	}
	template<typename T>
	void SaveNestedValue(json & j, std::string name1, std::string name2, const T & value)
	{
		if (!value)
			return;
		j[name1][name2] = value;
	}
	template<typename T>
	void LoadValue(json & j, std::string name, T & value)
	{
		try
		{
			value = j[name];
		}
		catch (json::exception& ex)
		{
#ifdef _DEBUG
			//g_Logger.Warning("CONFIG", "Loading error: " + std::to_string(ex.what()));
			g_Logger.Warning("CONFIG", ex.what());
#endif
		}
	}

	template<typename T>
	void LoadNestedValue(json & j, std::string name1, std::string name2, T & value)
	{
		try
		{
			value = j[name1][name2];
		}
		catch (json::exception& ex)
		{
#ifdef _DEBUG
			g_Logger.Warning("CONFIG", ex.what());
#endif
			value = 0;
			//g_Logger.Warning( "CONFIG", "Loading error: " + std::to_string(ex.what()) );
		}
	}
}