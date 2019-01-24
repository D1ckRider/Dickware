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

	/* Setting up variables */
	/* Init weapon settings for RBot */
	for (size_t i = 0; i < 11; i++)
	{
		RageBot::WeaponSettings[i].Hitchance = 0.f;
		RageBot::WeaponSettings[i].MinDamage = 0.f;
		RageBot::WeaponSettings[i].BAimAfterShots = 0;
		RageBot::WeaponSettings[i].ForceBAimAfterShots = 0;
		RageBot::WeaponSettings[i].BAimWhileMoving = false;
	}
	/* Init Hitbox settings */
	for (size_t i = 0; i < 7; i++)
	{
		RageBot::Hitboxes[i].Enabled = false;
		RageBot::Hitboxes[i].Scale = 0.f;

	}
	/* Init AA settings */
	for (size_t i = 0; i < 3; i++)
	{
		RageBot::AntiAimSettings[i].Pitch = 0;
		RageBot::AntiAimSettings[i].Yaw = 0;
		RageBot::AntiAimSettings[i].YawCustom = 0.f;
		RageBot::AntiAimSettings[i].YawAdd = 0;
		RageBot::AntiAimSettings[i].YawAddCustom = 0.f;
		RageBot::AntiAimSettings[i].FakelagTicks = 0;
		RageBot::AntiAimSettings[i].FakelagMode = 0;
	}
	/* Init weapon settings for LBot */
	for (size_t i = 0; i < 11; i++)
	{
		Aimbot::WeaponAimSetting[i].FOV = 0.f;
		Aimbot::WeaponAimSetting[i].Smooth = 1.f;
		Aimbot::WeaponAimSetting[i].Randomize = 0.f;
		Aimbot::WeaponAimSetting[i].Delay = 0.f;

		Aimbot::WeaponAimSetting[i].RCS = false;
		Aimbot::WeaponAimSetting[i].RCS_X = 0.f;
		Aimbot::WeaponAimSetting[i].RCS_Y = 0.f;
		
		Aimbot::WeaponAimSetting[i].HitboxHead = false;
		Aimbot::WeaponAimSetting[i].HitboxNeck = false;
		Aimbot::WeaponAimSetting[i].HitboxChest = false;
		Aimbot::WeaponAimSetting[i].HitboxPelvis = false;
		Aimbot::WeaponAimSetting[i].HitboxStomach = false;
		Aimbot::WeaponAimSetting[i].HitboxArm = false;
		Aimbot::WeaponAimSetting[i].HitboxLeg = false;
		Aimbot::WeaponAimSetting[i].HitboxFoot = false;
	}
	Visual::LocalChams.Enabled = false;
	Visual::LocalChams.Mode = 0;
	Visual::LocalChams.Visible = Color::Red;
	Visual::LocalChams.Invisible = Color::Black;
	Visual::LocalESP.Enabled = false;
	Visual::LocalESP.BoxEnabled = false;
	Visual::LocalESP.BoxType = 0;
	Visual::LocalESP.BoxColor = Color::Black;
	Visual::LocalESP.NameEnabled = false;
	Visual::LocalESP.NameColor = Color::White;
	Visual::LocalESP.HealthEnabled = false;
	Visual::LocalESP.ArmorEnabled = false;
	Visual::LocalESP.ArmorColor = Color::White;
	Visual::LocalESP.WeaponEnabled = false;
	Visual::LocalESP.WeaponColor = Color::White;
	Visual::LocalESP.SnaplineEnabled = false;
	Visual::LocalESP.SnaplineColor = Color::White;

	Visual::TeamChams.Enabled = false;
	Visual::TeamChams.Mode = 0;
	Visual::TeamChams.Visible = Color::Red;
	Visual::TeamChams.Invisible = Color::Black;
	Visual::TeamESP.Enabled = false;
	Visual::TeamESP.BoxEnabled = false;
	Visual::TeamESP.BoxType = 0;
	Visual::TeamESP.BoxColor = Color::Black;
	Visual::TeamESP.NameEnabled = false;
	Visual::TeamESP.NameColor = Color::White;
	Visual::TeamESP.HealthEnabled = false;
	Visual::TeamESP.ArmorEnabled = false;
	Visual::TeamESP.ArmorColor = Color::White;
	Visual::TeamESP.WeaponEnabled = false;
	Visual::TeamESP.WeaponColor = Color::White;
	Visual::TeamESP.SnaplineEnabled = false;
	Visual::TeamESP.SnaplineColor = Color::White;

	Visual::EnemyChams.Enabled = false;
	Visual::EnemyChams.Mode = 0;
	Visual::EnemyChams.Visible = Color::Red;
	Visual::EnemyChams.Invisible = Color::Black;
	Visual::EnemyESP.Enabled = false;
	Visual::EnemyESP.BoxEnabled = false;
	Visual::EnemyESP.BoxType = 0;
	Visual::EnemyESP.BoxColor = Color::Black;
	Visual::EnemyESP.NameEnabled = false;
	Visual::EnemyESP.NameColor = Color::White;
	Visual::EnemyESP.HealthEnabled = false;
	Visual::EnemyESP.ArmorEnabled = false;
	Visual::EnemyESP.ArmorColor = Color::White;
	Visual::EnemyESP.WeaponEnabled = false;
	Visual::EnemyESP.WeaponColor = Color::White;
	Visual::EnemyESP.SnaplineEnabled = false;
	Visual::EnemyESP.SnaplineColor = Color::White;

	Visual::GlobalESP.Enabled = false;
	Visual::GlobalESP.GrenadeEnabled = false;
	Visual::GlobalESP.BombEnabled = false;
	Visual::GlobalESP.BombColor = Color::Green;
	Visual::GlobalESP.DZEnabled = false;
	Visual::GlobalESP.DZRange = 100.f;
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

	/* Write ragebot settings */
	j["rbot_enabled"] = RageBot::Enabled;
	for (size_t i = 0; i < 11; i++)
	{
		j["rbot_weapon" + std::to_string(i)]["Hitchance"] = RageBot::WeaponSettings[i].Hitchance;
		j["rbot_weapon" + std::to_string(i)]["MinDamage"] = RageBot::WeaponSettings[i].MinDamage;
		j["rbot_weapon" + std::to_string(i)]["BAimAfterShots"] = RageBot::WeaponSettings[i].BAimAfterShots;
		j["rbot_weapon" + std::to_string(i)]["ForceBAimAfterShots"] = RageBot::WeaponSettings[i].ForceBAimAfterShots;
		j["rbot_weapon" + std::to_string(i)]["MovingBAim"] = RageBot::WeaponSettings[i].BAimWhileMoving;
	}
	j["rbot_baim_mode"] = RageBot::BAimMode;
	j["rbot_air_baim"] = RageBot::AirBAim;
	j["rbot_baim_hotkey"] = RageBot::BAimHotkey;

	for (size_t i = 0; i < 7; i++)
	{
		j["rbot_hitbox" + std::to_string(i)]["Enabled"] = RageBot::Hitboxes[i].Enabled;
		j["rbot_hitbox" + std::to_string(i)]["Scale"] = RageBot::Hitboxes[i].Scale;
	}

	j["rbot_aa_enabled"] = RageBot::EnabledAA;
	for (size_t i = 0; i < 3; i++)
	{
		j["rbot_aa" + std::to_string(i)]["Pitch"] = RageBot::AntiAimSettings[i].Pitch;
		j["rbot_aa" + std::to_string(i)]["Yaw"] = RageBot::AntiAimSettings[i].Yaw;
		j["rbot_aa" + std::to_string(i)]["YawAdd"] = RageBot::AntiAimSettings[i].YawAdd;
		j["rbot_aa" + std::to_string(i)]["Range"] = RageBot::AntiAimSettings[i].Range;
		j["rbot_aa" + std::to_string(i)]["FakelagMode"] = RageBot::AntiAimSettings[i].FakelagMode;
		j["rbot_aa" + std::to_string(i)]["FakelagTicks"] = RageBot::AntiAimSettings[i].FakelagTicks;
	}
	j["rbot_spinbot_speed"] = RageBot::SpinBotSpeed;
	j["rbot_slidewalk"] = RageBot::SlideWalk;
	j["rbot_desync"] = RageBot::Desync;
	j["rbot_manual_leftkey"] = RageBot::ManualAALeftKey;
	j["rbot_manual_rightkey"] = RageBot::ManualAARightKey;
	j["rbot_manual_backkey"] = RageBot::ManualAABackKey;

	j["rbot_autostop"] = RageBot::AutoStop;
	j["rbot_autoscope"] = RageBot::AutoScope;
	j["rbot_autocrouch"] = RageBot::AutoCrouch;
	j["rbot_slowwalk"] = RageBot::SlowWalk;
	j["rbot_slowwalk_hotkey"] = RageBot::SlowWalkHotkey;

	j["rbot_fakelag_prediction"] = RageBot::FakelagPrediction;
	j["rbot_shooting_mode"] = RageBot::ShootingMode;
	j["rbot_force_unlag"] = RageBot::ForceUnlag;
	j["rbot_resolver"] = RageBot::Resolver;

	/* Write legitbot settings */
	j["lbot_enabled"] = Aimbot::Enabled;
	j["lbot_key"] = Aimbot::Hotkey;
	for ( int i = 0; i < 11; i++ )
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
	/* Visuals */
	// Chams
	j["vis_chams_local_enabled"] = Visual::LocalChams.Enabled;
	j["vis_chams_local_mode"] = Visual::LocalChams.Mode;
	SaveColorValue(j, "vis_chams_local_invisible", Visual::LocalChams.Invisible);
	SaveColorValue(j, "vis_chams_local_visible", Visual::LocalChams.Visible);
	j["vis_chams_enemy_enabled"] = Visual::EnemyChams.Enabled;
	j["vis_chams_enemy_mode"] = Visual::EnemyChams.Mode;
	SaveColorValue(j, "vis_chams_enemy_invisible", Visual::EnemyChams.Invisible);
	SaveColorValue(j, "vis_chams_enemy_visible", Visual::EnemyChams.Visible);
	j["vis_chams_team_enabled"] = Visual::TeamChams.Enabled;
	j["vis_chams_team_mode"] = Visual::TeamChams.Mode;
	SaveColorValue(j, "vis_chams_team_invisible", Visual::TeamChams.Invisible);
	SaveColorValue(j, "vis_chams_team_visible", Visual::TeamChams.Visible);
	// Player ESP
	j["vis_esp_local_enabled"] = Visual::LocalESP.Enabled;
	j["vis_esp_local_box_enabled"] = Visual::LocalESP.BoxEnabled;
	j["vis_esp_local_box_type"] = Visual::LocalESP.BoxType;
	SaveColorValue(j, "vis_esp_local_box_color", Visual::LocalESP.BoxColor);
	j["vis_esp_local_name_enabled"] = Visual::LocalESP.NameEnabled;
	SaveColorValue(j, "vis_esp_local_name_color", Visual::LocalESP.NameColor);
	j["vis_esp_local_health_enabled"] = Visual::LocalESP.HealthEnabled;
	j["vis_esp_local_armor_enabled"] = Visual::LocalESP.ArmorEnabled;
	SaveColorValue(j, "vis_esp_local_armor_color", Visual::LocalESP.ArmorColor);
	j["vis_esp_local_weapon_enabled"] = Visual::LocalESP.WeaponEnabled;
	SaveColorValue(j, "vis_esp_local_weapon_color", Visual::LocalESP.WeaponColor);
	// Enemy ESP
	j["vis_esp_enemy_enabled"] = Visual::EnemyESP.Enabled;
	j["vis_esp_enemy_box_enabled"] = Visual::EnemyESP.BoxEnabled;
	j["vis_esp_enemy_box_type"] = Visual::EnemyESP.BoxType;
	SaveColorValue(j, "vis_esp_enemy_box_color", Visual::EnemyESP.BoxColor);
	j["vis_esp_enemy_name_enabled"] = Visual::EnemyESP.NameEnabled;
	SaveColorValue(j, "vis_esp_enemy_name_color", Visual::EnemyESP.NameColor);
	j["vis_esp_enemy_health_enabled"] = Visual::EnemyESP.HealthEnabled;
	j["vis_esp_enemy_armor_enabled"] = Visual::EnemyESP.ArmorEnabled;
	SaveColorValue(j, "vis_esp_enemy_armor_color", Visual::EnemyESP.ArmorColor);
	j["vis_esp_enemy_weapon_enabled"] = Visual::EnemyESP.WeaponEnabled;
	SaveColorValue(j, "vis_esp_enemy_weapon_color", Visual::EnemyESP.WeaponColor);
	j["vis_esp_enemy_snapline_enabled"] = Visual::EnemyESP.SnaplineEnabled;
	SaveColorValue(j, "vis_esp_enemy_snapline_color", Visual::EnemyESP.SnaplineColor);
	// Team ESP
	j["vis_esp_team_enabled"] = Visual::TeamESP.Enabled;
	j["vis_esp_team_box_enabled"] = Visual::TeamESP.BoxEnabled;
	j["vis_esp_team_box_type"] = Visual::TeamESP.BoxType;
	SaveColorValue(j, "vis_esp_team_box_color", Visual::TeamESP.BoxColor);
	j["vis_esp_team_name_enabled"] = Visual::TeamESP.NameEnabled;
	SaveColorValue(j, "vis_esp_team_name_color", Visual::TeamESP.NameColor);
	j["vis_esp_team_health_enabled"] = Visual::TeamESP.HealthEnabled;
	j["vis_esp_team_armor_enabled"] = Visual::TeamESP.ArmorEnabled;
	SaveColorValue(j, "vis_esp_team_armor_color", Visual::TeamESP.ArmorColor);
	j["vis_esp_team_weapon_enabled"] = Visual::TeamESP.WeaponEnabled;
	SaveColorValue(j, "vis_esp_team_weapon_color", Visual::TeamESP.WeaponColor);
	j["vis_esp_team_snapline_enabled"] = Visual::TeamESP.SnaplineEnabled;
	SaveColorValue(j, "vis_esp_team_snapline_color", Visual::TeamESP.SnaplineColor);
	// Other
	j["vis_thirdperson"] = Visual::ThirdPersonEnabled;
	j["vis_thirdperson_hotkey"] = Visual::ThirdPersonHotkey;
	j["vis_esp_other_enabled"] = Visual::GlobalESP.Enabled;
	j["vis_esp_other_grenade_enable"] = Visual::GlobalESP.GrenadeEnabled;
	j["vis_esp_other_bomb_enabled"] = Visual::GlobalESP.BombEnabled;
	SaveColorValue(j, "vis_esp_other_bomb_color", Visual::GlobalESP.BombColor);
	j["vis_esp_other_dz_item"] = Visual::GlobalESP.DZEnabled;
	j["vis_esp_other_dz_range"] = Visual::GlobalESP.DZRange;
	j["vis_noscope_overlay"] = Visual::NoScopeOverlay;
	j["vis_bullet_tracers"] = Visual::BulletTracers;
	j["vis_noflash"] = Visual::NoFlash;
	j["vis_spread_cirlce_enabled"] = Visual::SpreadCircleEnabled;
	SaveColorValue(j, "vis_spread_circle_color", Visual::SpreadCircleColor);
	j["vis_disable_scoop_zoom"] = Visual::DisableScopeZoom;
	j["vis_fov"] = Visual::FOV;
	j["vis_viewmodel_fov"] = Visual::ViewModelFOV;
	j["vis_nosmoke"] = Visual::NoSmoke;
	j["vis_hitmarker"] = Visual::Hitmarker;
	j["vis_ragdoll_force"] = Visual::RagdollForce;
	/* Misc */
	j["misc_bhop"] = Misc::BHop;
	j["misc_autostrafe"] = Misc::AutoStrafe;
	j["misc_rank_reveal"] = Misc::RankReveal;
	j["misc_no_crouch_cooldown"] = Misc::NoCrouchCooldown;
	j["misc_clantag"] = Misc::Clantag;
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
	
	/* Read RageBot Settings */
	RageBot::Enabled = j["rbot_enabled"];
	for (size_t i = 0; i < 11; i++)
	{
		RageBot::WeaponSettings[i].Hitchance = j["rbot_weapon" + std::to_string(i)]["Hitchance"];
		RageBot::WeaponSettings[i].MinDamage = j["rbot_weapon" + std::to_string(i)]["MinDamage"];
		RageBot::WeaponSettings[i].BAimAfterShots = j["rbot_weapon" + std::to_string(i)]["BAimAfterShots"];
		RageBot::WeaponSettings[i].ForceBAimAfterShots = j["rbot_weapon" + std::to_string(i)]["ForceBAimAfterShots"];
		RageBot::WeaponSettings[i].BAimWhileMoving = j["rbot_weapon" + std::to_string(i)]["MovingBAim"];
	}
	RageBot::BAimMode = j["rbot_baim_mode"];
	RageBot::AirBAim = j["rbot_air_baim"];
	RageBot::BAimHotkey = j["rbot_baim_hotkey"];

	for (size_t i = 0; i < 7; i++)
	{
		RageBot::Hitboxes[i].Enabled = j["rbot_hitbox" + std::to_string(i)]["Enabled"];
		RageBot::Hitboxes[i].Scale = j["rbot_hitbox" + std::to_string(i)]["Scale"];
	}

	RageBot::EnabledAA = j["rbot_aa_enabled"];
	for (size_t i = 0; i < 3; i++)
	{
		RageBot::AntiAimSettings[i].Pitch = j["rbot_aa" + std::to_string(i)]["Pitch"];
		RageBot::AntiAimSettings[i].Yaw = j["rbot_aa" + std::to_string(i)]["Yaw"];
		RageBot::AntiAimSettings[i].YawAdd = j["rbot_aa" + std::to_string(i)]["YawAdd"];
		RageBot::AntiAimSettings[i].Range =  j["rbot_aa" + std::to_string(i)]["Range"];
		RageBot::AntiAimSettings[i].FakelagMode = j["rbot_aa" + std::to_string(i)]["FakelagMode"];
		RageBot::AntiAimSettings[i].FakelagTicks = j["rbot_aa" + std::to_string(i)]["FakelagTicks"];
	}
	RageBot::SpinBotSpeed = j["rbot_spinbot_speed"];
	RageBot::SlideWalk = j["rbot_slidewalk"];
	RageBot::Desync = j["rbot_desync"];
	RageBot::ManualAALeftKey = j["rbot_manual_leftkey"];
	RageBot::ManualAARightKey = j["rbot_manual_rightkey"];
	RageBot::ManualAABackKey = j["rbot_manual_backkey"];

	RageBot::AutoStop = j["rbot_autostop"];
	RageBot::AutoScope = j["rbot_autoscope"];
	RageBot::AutoCrouch = j["rbot_autocrouch"];
	RageBot::SlowWalk = j["rbot_slowwalk"];
	RageBot::SlowWalkHotkey = j["rbot_slowwalk_hotkey"];

	RageBot::FakelagPrediction = j["rbot_fakelag_prediction"];
	RageBot::ShootingMode = j["rbot_shooting_mode"];
	RageBot::ForceUnlag = j["rbot_force_unlag"];
	RageBot::Resolver = j["rbot_resolver"];

	/* Read LBot Settings */
	Aimbot::Enabled = j["lbot_enabled"];
	Aimbot::Hotkey = j["lbot_key"];
	for (int i = 0; i < 11; i++)
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
	/* Visuals */
	// Chams
	Visual::LocalChams.Enabled = j["vis_chams_local_enabled"];
	Visual::LocalChams.Mode = j["vis_chams_local_mode"];
	LoadColorValue(j, "vis_chams_local_invisible", Visual::LocalChams.Invisible);
	LoadColorValue(j, "vis_chams_local_visible", Visual::LocalChams.Visible);
	Visual::EnemyChams.Enabled = j["vis_chams_enemy_enabled"];
	j["vis_chams_enemy_mode"] = Visual::EnemyChams.Mode;
	LoadColorValue(j, "vis_chams_enemy_invisible", Visual::EnemyChams.Invisible);
	LoadColorValue(j, "vis_chams_enemy_visible", Visual::EnemyChams.Visible);
	Visual::TeamChams.Enabled = j["vis_chams_team_enabled"];
	Visual::TeamChams.Mode = j["vis_chams_team_mode"];
	LoadColorValue(j, "vis_chams_team_invisible", Visual::TeamChams.Invisible);
	LoadColorValue(j, "vis_chams_team_visible", Visual::TeamChams.Visible);
	// Player ESP
	Visual::LocalESP.Enabled = j["vis_esp_local_enabled"];
	Visual::LocalESP.BoxEnabled = j["vis_esp_local_box_enabled"];
	Visual::LocalESP.BoxType = j["vis_esp_local_box_type"];
	LoadColorValue(j, "vis_esp_local_box_color", Visual::LocalESP.BoxColor);
	Visual::LocalESP.NameEnabled = j["vis_esp_local_name_enabled"];
	LoadColorValue(j, "vis_esp_local_name_color", Visual::LocalESP.NameColor);
	Visual::LocalESP.HealthEnabled = j["vis_esp_local_health_enabled"];
	Visual::LocalESP.ArmorEnabled = j["vis_esp_local_armor_enabled"];
	LoadColorValue(j, "vis_esp_local_armor_color", Visual::LocalESP.ArmorColor);
	Visual::LocalESP.WeaponEnabled = j["vis_esp_local_weapon_enabled"];
	LoadColorValue(j, "vis_esp_local_weapon_color", Visual::LocalESP.WeaponColor);
	// Enemy ESP
	Visual::EnemyESP.Enabled = j["vis_esp_enemy_enabled"];
	Visual::EnemyESP.BoxEnabled = j["vis_esp_enemy_box_enabled"];
	Visual::EnemyESP.BoxType = 	j["vis_esp_enemy_box_type"];
	LoadColorValue(j, "vis_esp_enemy_box_color", Visual::EnemyESP.BoxColor);
	Visual::EnemyESP.NameEnabled = 	j["vis_esp_enemy_name_enabled"];
	LoadColorValue(j, "vis_esp_enemy_name_color", Visual::EnemyESP.NameColor);
	Visual::EnemyESP.HealthEnabled = 	j["vis_esp_enemy_health_enabled"];
	Visual::EnemyESP.ArmorEnabled = 	j["vis_esp_enemy_armor_enabled"];
	LoadColorValue(j, "vis_esp_enemy_armor_color", Visual::EnemyESP.ArmorColor);
	Visual::EnemyESP.WeaponEnabled = 	j["vis_esp_enemy_weapon_enabled"];
	LoadColorValue(j, "vis_esp_enemy_weapon_color", Visual::EnemyESP.WeaponColor);
	Visual::EnemyESP.SnaplineEnabled = 	j["vis_esp_enemy_snapline_enabled"];
	LoadColorValue(j, "vis_esp_enemy_snapline_color", Visual::EnemyESP.SnaplineColor);
	// Team ESP
	Visual::TeamESP.Enabled = j["vis_esp_team_enabled"];
	Visual::TeamESP.BoxEnabled = j["vis_esp_team_box_enabled"];
	Visual::TeamESP.BoxType = j["vis_esp_team_box_type"];
	LoadColorValue(j, "vis_esp_team_box_color", Visual::TeamESP.BoxColor);
	Visual::TeamESP.NameEnabled = 	j["vis_esp_team_name_enabled"];
	LoadColorValue(j, "vis_esp_team_name_color", Visual::TeamESP.NameColor);
	Visual::TeamESP.HealthEnabled = 	j["vis_esp_team_health_enabled"];
	Visual::TeamESP.ArmorEnabled = 	j["vis_esp_team_armor_enabled"];
	LoadColorValue(j, "vis_esp_team_armor_color", Visual::TeamESP.ArmorColor);
	Visual::TeamESP.WeaponEnabled = 	j["vis_esp_team_weapon_enabled"];
	LoadColorValue(j, "vis_esp_team_weapon_color", Visual::TeamESP.WeaponColor);
	Visual::TeamESP.SnaplineEnabled = 	j["vis_esp_team_snapline_enabled"];
	LoadColorValue(j, "vis_esp_team_snapline_color", Visual::TeamESP.SnaplineColor);
	// Other
	Visual::ThirdPersonEnabled = 	j["vis_thirdperson"];
	Visual::ThirdPersonHotkey = 	j["vis_thirdperson_hotkey"];
	Visual::GlobalESP.Enabled = 	j["vis_esp_other_enabled"];
	Visual::GlobalESP.GrenadeEnabled = 	j["vis_esp_other_grenade_enable"];
	Visual::GlobalESP.BombEnabled = 	j["vis_esp_other_bomb_enabled"];
	LoadColorValue(j, "vis_esp_other_bomb_color", Visual::GlobalESP.BombColor);
	Visual::GlobalESP.DZEnabled = 	j["vis_esp_other_dz_item"];
	Visual::GlobalESP.DZRange = 	j["vis_esp_other_dz_range"];
	Visual::NoScopeOverlay = 	j["vis_noscope_overlay"];
	Visual::BulletTracers = 	j["vis_bullet_tracers"];
	Visual::NoFlash = 	j["vis_noflash"];
	Visual::SpreadCircleEnabled = 	j["vis_spread_cirlce_enabled"];
	LoadColorValue(j, "vis_spread_circle_color", Visual::SpreadCircleColor);
	Visual::DisableScopeZoom = 	j["vis_disable_scoop_zoom"];
	Visual::FOV = 	j["vis_fov"];
	Visual::ViewModelFOV = 	j["vis_viewmodel_fov"];
	Visual::NoSmoke = 	j["vis_nosmoke"];
	Visual::Hitmarker = 	j["vis_hitmarker"];
	Visual::RagdollForce = 	j["vis_ragdoll_force"];
	/* Misc */
	Misc::BHop = j["misc_bhop"];
	Misc::AutoStrafe = j["misc_autostrafe"];
	Misc::RankReveal = j["misc_rank_reveal"];
	Misc::NoCrouchCooldown = j["misc_no_crouch_cooldown"];
	Misc::Clantag = j["misc_clantag"];
}

void Settings::SaveColorValue(json & j, std::string name, const Color & value)
{
	j[name]["R"] = value.r();
	j[name]["G"] = value.g();
	j[name]["B"] = value.b();
	j[name]["A"] = value.a();
}

void Settings::LoadColorValue(json & j, std::string name, Color & value)
{
	value.SetColor(
		(int)j[name]["R"],
		(int)j[name]["G"],
		(int)j[name]["B"],
		(int)j[name]["A"]
	);
	/*value.r() = j[name]["R"];
	value.g() = j[name]["G"];
	value.b() = j[name]["B"];
	value.a() = j[name]["A"];*/
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
namespace Settings::RageBot
{
	bool Enabled = false;
	RBotWeaponSetting WeaponSettings[11] = { 0 };
	int BAimMode = 0;
	bool AirBAim = false;
	int BAimHotkey = 0x0;

	RBotHitbox Hitboxes[7] = { 0 };

	bool EnabledAA = false;
	AntiAim AntiAimSettings[3] = { 0 };
	float SpinBotSpeed= 0.f;
	bool SlideWalk= false;
	bool Desync = false;
	int ManualAAState = 0;
	int ManualAALeftKey = 0x0;
	int ManualAARightKey = 0x0;
	int ManualAABackKey = 0x0;

	bool AutoScope = false;
	bool AutoStop = false;
	bool AutoCrouch = false;
	bool SlowWalk = false;
	int SlowWalkHotkey = 0x0;

	bool FakelagPrediction = false;
	int ShootingMode = 0;
	bool ForceUnlag = false;
	bool Resolver = false;

	int GetWeaponType(C_BaseCombatWeapon * weapon)
	{
		if (weapon->GetItemDefinitionIndex() == WEAPON_DEAGLE)
			return WeaponType::WEAPON_DEAGLE;
		else if (weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
			return WeaponType::WEAPON_REVOLVER;
		else if (weapon->GetItemDefinitionIndex() == WEAPON_SSG08)
			return WeaponType::WEAPON_SSG08;
		else if (weapon->GetItemDefinitionIndex() == WEAPON_AWP)
			return WeaponType::WEAPON_AWP;
		else if (weapon->GetItemDefinitionIndex() == WEAPON_SCAR20 || weapon->GetItemDefinitionIndex() == WEAPON_G3SG1)
			return WeaponType::WEAPON_AUTO;
		else if (weapon->IsPistol())
			return WeaponType::WEAPON_PISTOL;
		else if (weapon->IsRifle())
			return WeaponType::WEAPON_RIFLE;
		else
			return WeaponType::WEAPON_SHOTGUN;
	}
}

namespace Settings::Aimbot
{
	bool Enabled = false;
	int Hotkey = 0;
	LBotWeaponSetting WeaponAimSetting[11] = { 0 };
	bool Backtrack = false;
	bool BacktrackAtAim = false;
	float BacktrackTick = false;

	int GetWeaponType(C_BaseCombatWeapon * weapon)
	{
		if ( weapon->GetItemDefinitionIndex() == WEAPON_DEAGLE )
			return WeaponType::WEAPON_DEAGLE;
		else if ( weapon->GetItemDefinitionIndex() == WEAPON_SSG08 )
			return WeaponType::WEAPON_SSG08;
		else if ( weapon->IsPistol() )
			return WeaponType::WEAPON_PISTOL;
		else if ( weapon->IsSubmachinegun() )
			return WeaponType::WEAPON_SMG;
		else if ( weapon->IsMachinegun() )
			return WeaponType::WEAPON_MACHINEGUN;
		else if ( weapon->IsShotgun() )
			return WeaponType::WEAPON_SHOTGUN;
		else if ( weapon->IsSniper() )
			return WeaponType::WEAPON_SNIPER;
		else
			return WeaponType::WEAPON_RIFLE;
	}
}

namespace Settings::Visual
{
	Chams LocalChams;
	PlayerESP LocalESP;
	Chams EnemyChams;
	PlayerESP EnemyESP;
	Chams TeamChams;
	PlayerESP TeamESP;

	bool ThirdPersonEnabled = false;
	int ThirdPersonHotkey = 0x0;

	OtherESP GlobalESP;

	bool NoScopeOverlay = false;
	bool BulletTracers = false;
	bool NoFlash = false;
	bool SpreadCircleEnabled = false;
	Color SpreadCircleColor = Color::Blue;
	bool DisableScopeZoom = false;
	int ViewModelFOV = 80;
	int FOV = 80;
	bool NoSmoke = false;
	bool Hitmarker = false;
	int RagdollForce = 0;
}

namespace Settings::Misc
{
	bool BHop = false;
	bool AutoStrafe = false;
	bool RankReveal = false;
	bool NoCrouchCooldown = false;
	bool Clantag = false;
}