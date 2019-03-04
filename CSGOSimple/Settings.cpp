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
		Aimbot::WeaponAimSetting[i].Autopistol = false;

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
	Visual::GlobalESP.DropedWeaponsEnabled = false;
	Visual::GlobalESP.BombColor = Color::Green;
	Visual::GlobalESP.DZEnabled = false;
	Visual::GlobalESP.DZRange = 100.f;
	Visual::GlobalESP.SoundESPEnabled = false;
}

void Settings::CreateConfig(std::string fileName)
{
	std::string orgfile = fileName;

	if (fs::exists(AppDataFolder + "configs\\" + fileName))
		return;

	std::ofstream ofs(AppDataFolder + "configs\\" + fileName);
	ofs << "";
	ofs.close();
	SaveSettings(fileName);

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

void Settings::ResetConfig()
{
	ResetRagebot();
	ResetAimbot();
	ResetTriggerbot();
	ResetVisuals();
	ResetMisc();
}

void Settings::SaveSettings(std::string fileName)
{
	std::string orgfile = fileName;
	fileName = AppDataFolder + "configs\\" + fileName;

	std::ofstream o(fileName);
	json j;

	/* Write ragebot settings */
	SaveValue(j, "rbot_enabled", RageBot::Enabled);
	for (size_t i = 0; i < 11; i++)
	{
		SaveNestedValue(j, "rbot_weapon" + std::to_string(i), "Hitchance", RageBot::WeaponSettings[i].Hitchance);
		SaveNestedValue(j, "rbot_weapon" + std::to_string(i), "MinDamage", RageBot::WeaponSettings[i].MinDamage);
		SaveNestedValue(j, "rbot_weapon" + std::to_string(i), "BAimAfterShots", RageBot::WeaponSettings[i].BAimAfterShots);
		SaveNestedValue(j, "rbot_weapon" + std::to_string(i), "ForceBAimAfterShots", RageBot::WeaponSettings[i].ForceBAimAfterShots);
		SaveNestedValue(j, "rbot_weapon" + std::to_string(i), "MovingBAim", RageBot::WeaponSettings[i].BAimWhileMoving);
	}
	SaveValue(j, "rbot_baim_mode", RageBot::BAimMode);
	SaveValue(j, "rbot_air_baim", RageBot::AirBAim);
	SaveValue(j, "rbot_baim_hotkey", RageBot::BAimHotkey);

	for (size_t i = 0; i < 7; i++)
	{
		SaveNestedValue(j, "rbot_hitbox" + std::to_string(i), "Enabled", RageBot::Hitboxes[i].Enabled);
		SaveNestedValue(j, "rbot_hitbox" + std::to_string(i), "Scale", RageBot::Hitboxes[i].Scale);
	}

	SaveValue(j, "rbot_aa_enabled", RageBot::EnabledAA);
	for (size_t i = 0; i < 3; i++)
	{
		SaveNestedValue(j, "rbot_aa" + std::to_string(i), "Pitch", RageBot::AntiAimSettings[i].Pitch);
		SaveNestedValue(j, "rbot_aa" + std::to_string(i), "Yaw", RageBot::AntiAimSettings[i].Yaw);
		SaveNestedValue(j, "rbot_aa" + std::to_string(i), "YawAdd", RageBot::AntiAimSettings[i].YawAdd);
		SaveNestedValue(j, "rbot_aa" + std::to_string(i), "Range", RageBot::AntiAimSettings[i].Range);
		SaveNestedValue(j, "rbot_aa" + std::to_string(i), "FakelagMode", RageBot::AntiAimSettings[i].FakelagMode);
		SaveNestedValue(j, "rbot_aa" + std::to_string(i), "FakelagTicks", RageBot::AntiAimSettings[i].FakelagTicks);
	}
	SaveValue(j, "rbot_spinbot_speed", RageBot::SpinBotSpeed);
	SaveValue(j, "rbot_slidewalk", RageBot::SlideWalk);
	SaveValue(j, "rbot_desync", RageBot::Desync);
	SaveValue(j, "rbot_manual_leftkey", RageBot::ManualAALeftKey);
	SaveValue(j, "rbot_manual_rightkey", RageBot::ManualAARightKey);
	SaveValue(j, "rbot_manual_backkey", RageBot::ManualAABackKey);
	SaveValue(j, "rbot_desync_hotkey", RageBot::DesyncFlipHotkey);

	SaveValue(j, "rbot_autostop", RageBot::AutoStop);
	SaveValue(j, "rbot_autoscope", RageBot::AutoScope);
	SaveValue(j, "rbot_autocrouch", RageBot::AutoCrouch);
	SaveValue(j, "rbot_slowwalk", RageBot::SlowWalk);
	SaveValue(j, "rbot_slowwalk_hotkey", RageBot::SlowWalkHotkey);
	SaveValue(j, "rbot_fakeduck", RageBot::FakeDuck);
	SaveValue(j, "rbot_fakeduck_hotkey", RageBot::FakeDuckHotkey);
	SaveValue(j, "rbot_slowwalk_speed", RageBot::SlowWalkMod);

	SaveValue(j, "rbot_fakelag_prediction", RageBot::FakelagPrediction);
	SaveValue(j, "rbot_shooting_mode", RageBot::ShootingMode);
	SaveValue(j, "rbot_force_unlag", RageBot::ForceUnlag);
	SaveValue(j, "rbot_resolver", RageBot::Resolver);

	/* Write legitbot settings */
	SaveValue(j, "lbot_enabled", Aimbot::Enabled);
	SaveValue(j, "lbot_key", Aimbot::Hotkey);
	for ( int i = 0; i < 11; i++ )
	{
		/* Base Settings */
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "FOV", Aimbot::WeaponAimSetting[i].FOV);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Smooth", Aimbot::WeaponAimSetting[i].Smooth);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Randomize", Aimbot::WeaponAimSetting[i].Randomize);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Delay", Aimbot::WeaponAimSetting[i].Delay);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Autopistol", Aimbot::WeaponAimSetting[i].Autopistol);
		/* RCS */
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "RCS", Aimbot::WeaponAimSetting[i].RCS);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "RCS_X", Aimbot::WeaponAimSetting[i].RCS_X);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "RCS_Y", Aimbot::WeaponAimSetting[i].RCS_Y);
		/* Hitboxes */
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Head", Aimbot::WeaponAimSetting[i].HitboxHead);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Neck", Aimbot::WeaponAimSetting[i].HitboxNeck);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Chest", Aimbot::WeaponAimSetting[i].HitboxChest);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Pelvis", Aimbot::WeaponAimSetting[i].HitboxPelvis);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Stomach", Aimbot::WeaponAimSetting[i].HitboxStomach);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Arm", Aimbot::WeaponAimSetting[i].HitboxArm);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Leg", Aimbot::WeaponAimSetting[i].HitboxLeg);
		SaveNestedValue(j, "lbot_weapon" + std::to_string(i), "Foot", Aimbot::WeaponAimSetting[i].HitboxFoot);
	}
	SaveValue(j, "lbot_backtrack", Aimbot::Backtrack);
	SaveValue(j, "lbot_backtrack_aim", Aimbot::BacktrackAtAim);
	SaveValue(j, "lbot_backtrack_tick", Aimbot::BacktrackTick);
	/* Triggerbot */
	SaveValue(j, "lbot_trigger_enabled", TriggerBot::Enabled);
	SaveValue(j, "lbot_trigger_key", TriggerBot::Key);
	/* Visuals */
	// Chams
	SaveValue(j, "vis_chams_local_enabled", Visual::LocalChams.Enabled);
	SaveValue(j, "vis_chams_local_mode", Visual::LocalChams.Mode);
	SaveColorValue(j, "vis_chams_local_invisible", Visual::LocalChams.Invisible);
	SaveColorValue(j, "vis_chams_local_visible", Visual::LocalChams.Visible);
	SaveValue(j, "vis_ghost_enabled", Visual::GhostEnabled);
	SaveColorValue(j, "vis_ghost_color", Visual::GhostColor);
	SaveValue(j, "vis_chams_enemy_enabled", Visual::EnemyChams.Enabled);
	SaveValue(j, "vis_chams_enemy_mode", Visual::EnemyChams.Mode);
	SaveColorValue(j, "vis_chams_enemy_invisible", Visual::EnemyChams.Invisible);
	SaveColorValue(j, "vis_chams_enemy_visible", Visual::EnemyChams.Visible);
	SaveValue(j, "vis_chams_team_enabled", Visual::TeamChams.Enabled);
	SaveValue(j, "vis_chams_team_mode", Visual::TeamChams.Mode);
	SaveColorValue(j, "vis_chams_team_invisible", Visual::TeamChams.Invisible);
	SaveColorValue(j, "vis_chams_team_visible", Visual::TeamChams.Visible);
	// Player ESP
	SaveValue(j, "vis_esp_local_enabled", Visual::LocalESP.Enabled);
	SaveValue(j, "vis_esp_local_box_enabled", Visual::LocalESP.BoxEnabled);
	SaveValue(j, "vis_esp_local_box_type", Visual::LocalESP.BoxType);
	SaveColorValue(j, "vis_esp_local_box_color", Visual::LocalESP.BoxColor);
	SaveValue(j, "vis_esp_local_name_enabled", Visual::LocalESP.NameEnabled);
	SaveColorValue(j, "vis_esp_local_name_color", Visual::LocalESP.NameColor);
	SaveValue(j, "vis_esp_local_health_enabled", Visual::LocalESP.HealthEnabled);
	SaveValue(j, "vis_esp_local_armor_enabled", Visual::LocalESP.ArmorEnabled);
	SaveColorValue(j, "vis_esp_local_armor_color", Visual::LocalESP.ArmorColor);
	SaveValue(j, "vis_esp_local_weapon_enabled", Visual::LocalESP.WeaponEnabled);
	SaveColorValue(j, "vis_esp_local_weapon_color", Visual::LocalESP.WeaponColor);
	// Enemy ESP
	SaveValue(j, "vis_esp_enemy_enabled", Visual::EnemyESP.Enabled);
	SaveValue(j, "vis_esp_enemy_box_enabled", Visual::EnemyESP.BoxEnabled);
	SaveValue(j, "vis_esp_enemy_box_type", Visual::EnemyESP.BoxType);
	SaveColorValue(j, "vis_esp_enemy_box_color", Visual::EnemyESP.BoxColor);
	SaveValue(j, "vis_esp_enemy_name_enabled", Visual::EnemyESP.NameEnabled);
	SaveColorValue(j, "vis_esp_enemy_name_color", Visual::EnemyESP.NameColor);
	SaveValue(j, "vis_esp_enemy_health_enabled", Visual::EnemyESP.HealthEnabled);
	SaveValue(j, "vis_esp_enemy_armor_enabled", Visual::EnemyESP.ArmorEnabled);
	SaveColorValue(j, "vis_esp_enemy_armor_color", Visual::EnemyESP.ArmorColor);
	SaveValue(j, "vis_esp_enemy_weapon_enabled", Visual::EnemyESP.WeaponEnabled);
	SaveColorValue(j, "vis_esp_enemy_weapon_color", Visual::EnemyESP.WeaponColor);
	SaveValue(j, "vis_esp_enemy_snapline_enabled", Visual::EnemyESP.SnaplineEnabled);
	SaveColorValue(j, "vis_esp_enemy_snapline_color", Visual::EnemyESP.SnaplineColor);
	// Team ESP
	SaveValue(j, "vis_esp_team_enabled", Visual::TeamESP.Enabled);
	SaveValue(j, "vis_esp_team_box_enabled", Visual::TeamESP.BoxEnabled);
	SaveValue(j, "vis_esp_team_box_type", Visual::TeamESP.BoxType);
	SaveColorValue(j, "vis_esp_team_box_color", Visual::TeamESP.BoxColor);
	SaveValue(j, "vis_esp_team_name_enabled", Visual::TeamESP.NameEnabled);
	SaveColorValue(j, "vis_esp_team_name_color", Visual::TeamESP.NameColor);
	SaveValue(j, "vis_esp_team_health_enabled", Visual::TeamESP.HealthEnabled);
	SaveValue(j, "vis_esp_team_armor_enabled", Visual::TeamESP.ArmorEnabled);
	SaveColorValue(j, "vis_esp_team_armor_color", Visual::TeamESP.ArmorColor);
	SaveValue(j, "vis_esp_team_weapon_enabled", Visual::TeamESP.WeaponEnabled);
	SaveColorValue(j, "vis_esp_team_weapon_color", Visual::TeamESP.WeaponColor);
	SaveValue(j, "vis_esp_team_snapline_enabled", Visual::TeamESP.SnaplineEnabled);
	SaveColorValue(j, "vis_esp_team_snapline_color", Visual::TeamESP.SnaplineColor);
	// Other
	SaveValue(j, "vis_thirdperson", Visual::ThirdPersonEnabled);
	SaveValue(j, "vis_thirdperson_hotkey", Visual::ThirdPersonHotkey);
	SaveValue(j, "vis_esp_other_enabled", Visual::GlobalESP.Enabled);
	SaveValue(j, "vis_esp_other_grenade_enable", Visual::GlobalESP.GrenadeEnabled);
	SaveValue(j, "vis_esp_other_bomb_enabled", Visual::GlobalESP.BombEnabled);
	SaveColorValue(j, "vis_esp_other_bomb_color", Visual::GlobalESP.BombColor);
	SaveValue(j, "vis_esp_other_weapon_enabled", Visual::GlobalESP.DropedWeaponsEnabled);
	SaveValue(j, "vis_esp_other_dz_item", Visual::GlobalESP.DZEnabled);
	SaveValue(j, "vis_esp_other_dz_range", Visual::GlobalESP.DZRange);
	SaveValue(j, "vis_esp_other_sound_enabled", Visual::GlobalESP.SoundESPEnabled);
	SaveValue(j, "vis_noscope_overlay", Visual::NoScopeOverlay);
	SaveValue(j, "vis_bullet_tracers", Visual::BulletTracers);
	SaveValue(j, "vis_noflash", Visual::NoFlash);
	SaveValue(j, "vis_nightmode", Visual::NightMode);
	SaveValue(j, "vis_disabe_pp", Visual::DisablePP);
	SaveValue(j, "vis_nadetracer_enabled", Visual::NadeTracerEnabled);
	SaveColorValue(j, "vis_nadetracer_color", Visual::NadeTracerColor);
	SaveValue(j, "vis_spread_cirlce_enabled", Visual::SpreadCircleEnabled);
	SaveColorValue(j, "vis_spread_circle_color", Visual::SpreadCircleColor);
	SaveValue(j, "vis_damage_ind_enabled", Visual::DamageIndicator);
	SaveColorValue(j, "vis_damage_ind_color", Visual::DamageIndicatorColor);
	SaveValue(j, "vis_disable_scoop_zoom", Visual::DisableScopeZoom);
	SaveValue(j, "vis_fov", Visual::FOV);
	SaveValue(j, "vis_viewmodel_fov", Visual::ViewModelFOV);
	SaveValue(j, "vis_nosmoke", Visual::NoSmoke);
	SaveValue(j, "vis_hitmarker", Visual::Hitmarker);
	SaveValue(j, "vis_hitmarker_sound", Visual::HitmarkerSound);
	SaveValue(j, "vis_ragdoll_force", Visual::RagdollForce);
	/* Misc */
	SaveValue(j, "misc_bhop", Misc::BHop);
	SaveValue(j, "misc_autostrafe", Misc::AutoStrafe);
	SaveValue(j, "misc_rank_reveal", Misc::RankReveal);
	SaveValue(j, "misc_autoaccept", Misc::AutoAccept);
	SaveValue(j, "misc_no_crouch_cooldown", Misc::NoCrouchCooldown);
	SaveValue(j, "misc_clantag", Misc::Clantag);
	SaveValue(j, "misc_spectator_list", Misc::SpectatorsEnabled);
	SaveValue(j, "misc_buybot_enabled", Misc::BuyBot);
	SaveValue(j, "misc_buybot_pistol", Misc::BuyBotPistol);
	SaveValue(j, "misc_buybot_weapon", Misc::BuyBotWeapon);
	SaveValue(j, "misc_buybot_armor", Misc::BuyBotArmor);
	SaveValue(j, "misc_buybot_zeus", Misc::BuyBotZeus);
	SaveValue(j, "misc_buybot_defuser", Misc::BuyBotDefuser);
	SaveValue(j, "misc_skin_enabled", Misc::SkinchangerEnabled);
	/* Radio */
	SaveValue(j, "misc_radio_enabled", Misc::RadioEnabled);
	SaveValue(j, "misc_radio_selected", Misc::RadioSelected);
	SaveValue(j, "misc_radio_volume", Misc::RadioVolume);
	SaveValue(j, "misc_radio_pause_hotkey", Misc::RadioPauseHotkey);
	/* Write file */
	o << std::setw(4) << j << std::endl;
}

void Settings::LoadSettings(std::string fileName)
{
	ResetConfig();
	std::string orgfile = fileName;
	fileName = AppDataFolder + "configs\\" + fileName;

	std::ifstream i(fileName);
	json j;
	i >> j;
	
	/* Read RageBot Settings */
	LoadValue(j, "rbot_enabled", 	RageBot::Enabled);
	for (size_t i = 0; i < 11; i++)
	{
		LoadNestedValue(j, "rbot_weapon" + std::to_string(i), "Hitchance", 		RageBot::WeaponSettings[i].Hitchance);
		LoadNestedValue(j, "rbot_weapon" + std::to_string(i), "MinDamage", 		RageBot::WeaponSettings[i].MinDamage);
		LoadNestedValue(j, "rbot_weapon" + std::to_string(i), "BAimAfterShots", 		RageBot::WeaponSettings[i].BAimAfterShots);
		LoadNestedValue(j, "rbot_weapon" + std::to_string(i), "ForceBAimAfterShots", 		RageBot::WeaponSettings[i].ForceBAimAfterShots);
		LoadNestedValue(j, "rbot_weapon" + std::to_string(i), "MovingBAim", 		RageBot::WeaponSettings[i].BAimWhileMoving);
	}
	LoadValue(j, "rbot_baim_mode", 	RageBot::BAimMode);
	LoadValue(j, "rbot_air_baim", 	RageBot::AirBAim);
	LoadValue(j, "rbot_baim_hotkey", 	RageBot::BAimHotkey);

	for (size_t i = 0; i < 7; i++)
	{
		LoadNestedValue(j, "rbot_hitbox" + std::to_string(i), "Enabled", 		RageBot::Hitboxes[i].Enabled);
		LoadNestedValue(j, "rbot_hitbox" + std::to_string(i), "Scale", 		RageBot::Hitboxes[i].Scale);
	}

	LoadValue(j, "rbot_aa_enabled", 	RageBot::EnabledAA);
	for (size_t i = 0; i < 3; i++)
	{
		LoadNestedValue(j, "rbot_aa" + std::to_string(i), "Pitch", 		RageBot::AntiAimSettings[i].Pitch);
		LoadNestedValue(j, "rbot_aa" + std::to_string(i), "Yaw", 		RageBot::AntiAimSettings[i].Yaw);
		LoadNestedValue(j, "rbot_aa" + std::to_string(i), "YawAdd", 		RageBot::AntiAimSettings[i].YawAdd);
		LoadNestedValue(j, "rbot_aa" + std::to_string(i), "Range", 		RageBot::AntiAimSettings[i].Range);
		LoadNestedValue(j, "rbot_aa" + std::to_string(i), "FakelagMode", 		RageBot::AntiAimSettings[i].FakelagMode);
		LoadNestedValue(j, "rbot_aa" + std::to_string(i), "FakelagTicks", 		RageBot::AntiAimSettings[i].FakelagTicks);
	}
	LoadValue(j, "rbot_spinbot_speed", 	RageBot::SpinBotSpeed);
	LoadValue(j, "rbot_slidewalk", 	RageBot::SlideWalk);
	LoadValue(j, "rbot_desync", RageBot::Desync);
	LoadValue(j, "rbot_manual_leftkey", 	RageBot::ManualAALeftKey);
	LoadValue(j, "rbot_manual_rightkey", 	RageBot::ManualAARightKey);
	LoadValue(j, "rbot_manual_backkey", 	RageBot::ManualAABackKey);
	LoadValue(j, "rbot_desync_hotkey", RageBot::DesyncFlipHotkey);

	LoadValue(j, "rbot_autostop", 	RageBot::AutoStop);
	LoadValue(j, "rbot_autoscope", 	RageBot::AutoScope);
	LoadValue(j, "rbot_autocrouch", 	RageBot::AutoCrouch);
	LoadValue(j, "rbot_slowwalk", 	RageBot::SlowWalk);
	LoadValue(j, "rbot_slowwalk_hotkey", 	RageBot::SlowWalkHotkey);
	LoadValue(j, "rbot_fakeduck_hotkey", RageBot::FakeDuckHotkey);
	LoadValue(j, "rbot_slowwalk_speed", RageBot::SlowWalkMod);
	LoadValue(j, "rbot_fakeduck", RageBot::FakeDuck);

	LoadValue(j, "rbot_fakelag_prediction", 	RageBot::FakelagPrediction);
	LoadValue(j, "rbot_shooting_mode", 	RageBot::ShootingMode);
	LoadValue(j, "rbot_force_unlag", 	RageBot::ForceUnlag);
	LoadValue(j, "rbot_resolver", 	RageBot::Resolver);

	/* Read LBot Settings */
	LoadValue(j, "lbot_enabled", 	Aimbot::Enabled);
	LoadValue(j, "lbot_key", 	Aimbot::Hotkey);
	for (int i = 0; i < 11; i++)
	{
		/* Base Weapon Settings */
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "FOV", 		Aimbot::WeaponAimSetting[i].FOV);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Smooth", 		Aimbot::WeaponAimSetting[i].Smooth);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Randomize", 		Aimbot::WeaponAimSetting[i].Randomize);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Delay", 		Aimbot::WeaponAimSetting[i].Delay);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Autopistol", Aimbot::WeaponAimSetting[i].Autopistol);
		/* RCS */
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "RCS", 		Aimbot::WeaponAimSetting[i].RCS);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "RCS_X", 		Aimbot::WeaponAimSetting[i].RCS_X);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "RCS_Y", 		Aimbot::WeaponAimSetting[i].RCS_Y);
		/* Hitboxes */
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Head", 		Aimbot::WeaponAimSetting[i].HitboxHead);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Neck", 		Aimbot::WeaponAimSetting[i].HitboxNeck);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Chest", 		Aimbot::WeaponAimSetting[i].HitboxChest);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Pelvis", 		Aimbot::WeaponAimSetting[i].HitboxPelvis);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Stomach", 		Aimbot::WeaponAimSetting[i].HitboxStomach);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Arm", 		Aimbot::WeaponAimSetting[i].HitboxArm);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Leg", 		Aimbot::WeaponAimSetting[i].HitboxLeg);
		LoadNestedValue(j, "lbot_weapon" + std::to_string(i), "Foot", 		Aimbot::WeaponAimSetting[i].HitboxFoot);
	}
	LoadValue(j, "lbot_backtrack", 	Aimbot::Backtrack);
	LoadValue(j, "lbot_backtrack_aim", 	Aimbot::BacktrackAtAim);
	LoadValue(j, "lbot_backtrack_tick", 	Aimbot::BacktrackTick );
	/* Triggerbot */
	LoadValue(j, "lbot_trigger_enabled", TriggerBot::Enabled);
	LoadValue(j, "lbot_trigger_key", TriggerBot::Key);
	/* Visuals */
	// Chams
	LoadValue(j, "vis_chams_local_enabled", 	Visual::LocalChams.Enabled);
	LoadValue(j, "vis_chams_local_mode", 	Visual::LocalChams.Mode);
	LoadColorValue(j, "vis_chams_local_invisible", Visual::LocalChams.Invisible);
	LoadColorValue(j, "vis_chams_local_visible", Visual::LocalChams.Visible);
	LoadValue(j, "vis_ghost_enabled", Visual::GhostEnabled);
	LoadColorValue(j, "vis_ghost_color", Visual::GhostColor);
	LoadValue(j, "vis_chams_enemy_enabled", 	Visual::EnemyChams.Enabled);
	LoadValue(j, "vis_chams_enemy_mode", Visual::EnemyChams.Mode);
	LoadColorValue(j, "vis_chams_enemy_invisible", Visual::EnemyChams.Invisible);
	LoadColorValue(j, "vis_chams_enemy_visible", Visual::EnemyChams.Visible);
	LoadValue(j, "vis_chams_team_enabled", 	Visual::TeamChams.Enabled);
	LoadValue(j, "vis_chams_team_mode", 	Visual::TeamChams.Mode);
	LoadColorValue(j, "vis_chams_team_invisible", Visual::TeamChams.Invisible);
	LoadColorValue(j, "vis_chams_team_visible", Visual::TeamChams.Visible);
	// Player ESP
	LoadValue(j, "vis_esp_local_enabled", 	Visual::LocalESP.Enabled);
	LoadValue(j, "vis_esp_local_box_enabled", 	Visual::LocalESP.BoxEnabled);
	LoadValue(j, "vis_esp_local_box_type", 	Visual::LocalESP.BoxType);
	LoadColorValue(j, "vis_esp_local_box_color", Visual::LocalESP.BoxColor);
	LoadValue(j, "vis_esp_local_name_enabled", 	Visual::LocalESP.NameEnabled);
	LoadColorValue(j, "vis_esp_local_name_color", Visual::LocalESP.NameColor);
	LoadValue(j, "vis_esp_local_health_enabled", 	Visual::LocalESP.HealthEnabled);
	LoadValue(j, "vis_esp_local_armor_enabled", 	Visual::LocalESP.ArmorEnabled);
	LoadColorValue(j, "vis_esp_local_armor_color", Visual::LocalESP.ArmorColor);
	LoadValue(j, "vis_esp_local_weapon_enabled", 	Visual::LocalESP.WeaponEnabled);
	LoadColorValue(j, "vis_esp_local_weapon_color", Visual::LocalESP.WeaponColor);
	// Enemy ESP
	LoadValue(j, "vis_esp_enemy_enabled", 	Visual::EnemyESP.Enabled);
	LoadValue(j, "vis_esp_enemy_box_enabled", 	Visual::EnemyESP.BoxEnabled);
	LoadValue(j, "vis_esp_enemy_box_type", 	Visual::EnemyESP.BoxType);
	LoadColorValue(j, "vis_esp_enemy_box_color", Visual::EnemyESP.BoxColor);
	LoadValue(j, "vis_esp_enemy_name_enabled", 	Visual::EnemyESP.NameEnabled);
	LoadColorValue(j, "vis_esp_enemy_name_color", Visual::EnemyESP.NameColor);
	LoadValue(j, "vis_esp_enemy_health_enabled", 	Visual::EnemyESP.HealthEnabled);
	LoadValue(j, "vis_esp_enemy_armor_enabled", 	Visual::EnemyESP.ArmorEnabled);
	LoadColorValue(j, "vis_esp_enemy_armor_color", Visual::EnemyESP.ArmorColor);
	LoadValue(j, "vis_esp_enemy_weapon_enabled", 	Visual::EnemyESP.WeaponEnabled);
	LoadColorValue(j, "vis_esp_enemy_weapon_color", Visual::EnemyESP.WeaponColor);
	LoadValue(j, "vis_esp_enemy_snapline_enabled", 	Visual::EnemyESP.SnaplineEnabled);
	LoadColorValue(j, "vis_esp_enemy_snapline_color", Visual::EnemyESP.SnaplineColor);
	// Team ESP
	LoadValue(j, "vis_esp_team_enabled", 	Visual::TeamESP.Enabled);
	LoadValue(j, "vis_esp_team_box_enabled", 	Visual::TeamESP.BoxEnabled);
	LoadValue(j, "vis_esp_team_box_type", 	Visual::TeamESP.BoxType);
	LoadColorValue(j, "vis_esp_team_box_color", Visual::TeamESP.BoxColor);
	LoadValue(j, "vis_esp_team_name_enabled", 	Visual::TeamESP.NameEnabled);
	LoadColorValue(j, "vis_esp_team_name_color", Visual::TeamESP.NameColor);
	LoadValue(j, "vis_esp_team_health_enabled", 	Visual::TeamESP.HealthEnabled);
	LoadValue(j, "vis_esp_team_armor_enabled", 	Visual::TeamESP.ArmorEnabled);
	LoadColorValue(j, "vis_esp_team_armor_color", Visual::TeamESP.ArmorColor);
	LoadValue(j, "vis_esp_team_weapon_enabled", 	Visual::TeamESP.WeaponEnabled);
	LoadColorValue(j, "vis_esp_team_weapon_color", Visual::TeamESP.WeaponColor);
	LoadValue(j, "vis_esp_team_snapline_enabled", 	Visual::TeamESP.SnaplineEnabled);
	LoadColorValue(j, "vis_esp_team_snapline_color", Visual::TeamESP.SnaplineColor);
	// Other
	LoadValue(j, "vis_thirdperson", 	Visual::ThirdPersonEnabled);
	LoadValue(j, "vis_thirdperson_hotkey", 	Visual::ThirdPersonHotkey);
	LoadValue(j, "vis_esp_other_enabled", 	Visual::GlobalESP.Enabled);
	LoadValue(j, "vis_esp_other_grenade_enable", 	Visual::GlobalESP.GrenadeEnabled);
	LoadValue(j, "vis_esp_other_bomb_enabled", 	Visual::GlobalESP.BombEnabled);
	LoadColorValue(j, "vis_esp_other_bomb_color", Visual::GlobalESP.BombColor);
	LoadValue(j, "vis_esp_other_weapon_enabled", Visual::GlobalESP.DropedWeaponsEnabled);
	LoadValue(j, "vis_esp_other_sound_enabled", Visual::GlobalESP.SoundESPEnabled);
	LoadValue(j, "vis_esp_other_dz_item", 	Visual::GlobalESP.DZEnabled);
	LoadValue(j, "vis_esp_other_dz_range", 	Visual::GlobalESP.DZRange);
	LoadValue(j, "vis_noscope_overlay", 	Visual::NoScopeOverlay);
	LoadValue(j, "vis_bullet_tracers", 	Visual::BulletTracers);
	LoadValue(j, "vis_noflash", 	Visual::NoFlash);
	LoadValue(j, "vis_disabe_pp", Visual::DisablePP);
	LoadValue(j, "vis_nightmode", Visual::NightMode);
	LoadValue(j, "vis_nadetracer_enabled", Visual::NadeTracerEnabled);
	LoadColorValue(j, "vis_nadetracer_color", Visual::NadeTracerColor);
	LoadValue(j, "vis_spread_cirlce_enabled", 	Visual::SpreadCircleEnabled);
	LoadColorValue(j, "vis_spread_circle_color", Visual::SpreadCircleColor);
	LoadValue(j, "vis_damage_ind_enabled", Visual::DamageIndicator);
	LoadColorValue(j, "vis_damage_ind_color", Visual::DamageIndicatorColor);
	LoadValue(j, "vis_disable_scoop_zoom", 	Visual::DisableScopeZoom);
	LoadValue(j, "vis_fov", 	Visual::FOV);
	LoadValue(j, "vis_viewmodel_fov", 	Visual::ViewModelFOV);
	LoadValue(j, "vis_nosmoke", 	Visual::NoSmoke);
	LoadValue(j, "vis_hitmarker", 	Visual::Hitmarker);
	LoadValue(j, "vis_hitmarker_sound", Visual::HitmarkerSound);
	LoadValue(j, "vis_ragdoll_force", 	Visual::RagdollForce);
	/* Misc */
	LoadValue(j, "misc_bhop", 	Misc::BHop);
	LoadValue(j, "misc_autostrafe", 	Misc::AutoStrafe);
	LoadValue(j, "misc_rank_reveal", 	Misc::RankReveal);
	LoadValue(j, "misc_no_crouch_cooldown", 	Misc::NoCrouchCooldown);
	LoadValue(j, "misc_autoaccept", 	Misc::AutoAccept);
	LoadValue(j, "misc_spectator_list", Misc::SpectatorsEnabled);
	LoadValue(j, "misc_clantag", 	Misc::Clantag);
	LoadValue(j, "misc_buybot_enabled", 	Misc::BuyBot);
	LoadValue(j, "misc_buybot_pistol", 	Misc::BuyBotPistol);
	LoadValue(j, "misc_buybot_weapon", 	Misc::BuyBotWeapon);
	LoadValue(j, "misc_buybot_armor", 	Misc::BuyBotArmor);
	LoadValue(j, "misc_buybot_zeus", 	Misc::BuyBotZeus);
	LoadValue(j, "misc_buybot_defuser", 	Misc::BuyBotDefuser);
	LoadValue(j, "misc_skin_enabled", Misc::SkinchangerEnabled);
	/* Radio */
	LoadValue(j, "misc_radio_enabled", Misc::RadioEnabled);
	LoadValue(j, "misc_radio_selected", Misc::RadioSelected);
	LoadValue(j, "misc_radio_volume", Misc::RadioVolume);
	LoadValue(j, "misc_radio_pause_hotkey", Misc::RadioPauseHotkey);
}

void Settings::SaveSkinsSettings()
{
	std::string fileName = AppDataFolder + "skins";

	std::ofstream o(fileName);
	json j;

	int i = 0;
	for (auto lt : Skins::m_items)
	{
		SaveNestedValue(j, std::to_string(i), "id", lt.first);
		SaveNestedValue(j, std::to_string(i), "item_name", lt.second.name);
		SaveNestedValue(j, std::to_string(i), "enabled", lt.second.enabled);
		SaveNestedValue(j, std::to_string(i), "definition_vector_index", lt.second.definition_vector_index);
		SaveNestedValue(j, std::to_string(i), "definition_index", lt.second.definition_index);
		SaveNestedValue(j, std::to_string(i), "paint_kit_vector_index", lt.second.paint_kit_vector_index);
		SaveNestedValue(j, std::to_string(i), "paint_kit_index", lt.second.paint_kit_index);
		SaveNestedValue(j, std::to_string(i), "definition_override_vector_index", lt.second.definition_override_vector_index);
		SaveNestedValue(j, std::to_string(i), "definition_override_index", lt.second.definition_override_index);
		SaveNestedValue(j, std::to_string(i), "seed", lt.second.seed);
		SaveNestedValue(j, std::to_string(i), "stat_trak", lt.second.stat_trak);
		SaveNestedValue(j, std::to_string(i), "wear", lt.second.wear);
		SaveNestedValue(j, std::to_string(i), "custom_name", lt.second.custom_name);
		i++;
	}
	SaveValue(j, "items", i);

	o << std::setw(4) << j << std::endl;

}

void Settings::LoadSkinsSettings()
{
	std::string fileName = AppDataFolder + "skins";

	std::ifstream i(fileName);

	try
	{
		//auto ifile = std::ifstream(fileName);
		if (i.good())
		{
			json j;
			i >> j;
			int id;
			int items;
			LoadValue(j, "items", items);
			item_setting temp_settings;
			for (int i = 0; i < items; i++)
			{
				LoadNestedValue(j, std::to_string(i), "id", id);
				//LoadNestedValue(j, std::to_string(i), "item_name", temp_settings.name);
				LoadNestedValue(j, std::to_string(i), "enabled", temp_settings.enabled);
				LoadNestedValue(j, std::to_string(i), "definition_vector_index", temp_settings.definition_vector_index);
				LoadNestedValue(j, std::to_string(i), "definition_index", temp_settings.definition_index);
				LoadNestedValue(j, std::to_string(i), "paint_kit_vector_index", temp_settings.paint_kit_vector_index);
				LoadNestedValue(j, std::to_string(i), "paint_kit_index", temp_settings.paint_kit_index);
				LoadNestedValue(j, std::to_string(i), "definition_override_vector_index", temp_settings.definition_override_vector_index);
				LoadNestedValue(j, std::to_string(i), "definition_override_index", temp_settings.definition_override_index);
				LoadNestedValue(j, std::to_string(i), "seed", temp_settings.seed);
				LoadNestedValue(j, std::to_string(i), "stat_trak", temp_settings.stat_trak);
				LoadNestedValue(j, std::to_string(i), "wear", temp_settings.wear);
				Skins::m_items.insert({ id, temp_settings });
				//LoadNestedValue(j, std::to_string(i), "custom_name", temp_settings.custom_name);
			}

			//Skins::m_items = json::parse(ifile).get<std::vector<item_setting>>();
			g_ClientState->ForceFullUpdate();
		}
	}
	catch (const std::exception&)
	{
		// Config file doesn't exists or is malformed, just ignore it
		// This will probably crash if you use a manual mapper that doesnt do proper exception handling
	}
}

void Settings::ResetRagebot()
{
	/* Setting up variables */
	RageBot::Enabled = false;
	RageBot::BAimMode = 0;
	RageBot::AirBAim = false;
	RageBot::BAimHotkey = 0x0;

	RageBot::EnabledAA = false;
	RageBot::SpinBotSpeed = 0.f;
	RageBot::SlideWalk = false;
	RageBot::Desync = false;
	RageBot::ManualAAState = 0;
	RageBot::ManualAALeftKey = 0x0;
	RageBot::ManualAARightKey = 0x0;
	RageBot::ManualAABackKey = 0x0;
	RageBot::DesyncFlipHotkey = 0x0;

	RageBot::AutoScope = false;
	RageBot::AutoStop = false;
	RageBot::AutoCrouch = false;
	RageBot::SlowWalk = false;
	RageBot::SlowWalkHotkey = 0x0;
	RageBot::FakeDuck = false;
	RageBot::FakeDuckHotkey = 0x0;
	RageBot::SlowWalkMod = .0f;

	RageBot::FakelagPrediction = false;
	RageBot::ShootingMode = 0;
	RageBot::ForceUnlag = false;
	RageBot::Resolver = false;
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
}

void Settings::ResetAimbot()
{
	/* Init weapon settings for LBot */
	Aimbot::Enabled = false;
	Aimbot::Hotkey = 0x0;
	Aimbot::Backtrack = false;
	Aimbot::BacktrackAtAim = false;
	Aimbot::BacktrackTick = false;
	for (size_t i = 0; i < 11; i++)
	{
		Aimbot::WeaponAimSetting[i].FOV = 0.f;
		Aimbot::WeaponAimSetting[i].Smooth = 1.f;
		Aimbot::WeaponAimSetting[i].Randomize = 0.f;
		Aimbot::WeaponAimSetting[i].Delay = 0.f;
		Aimbot::WeaponAimSetting[i].Autopistol = false;

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
}

void Settings::ResetTriggerbot()
{
	TriggerBot::Enabled = false;
	TriggerBot::Key = 0x0;
}

void Settings::ResetVisuals()
{
	Visual::GhostEnabled = false;
	Visual::GhostColor = Color::White;

	Visual::ThirdPersonEnabled = false;
	Visual::ThirdPersonHotkey = 0x0;

	Visual::NoScopeOverlay = false;
	Visual::BulletTracers = false;
	Visual::NoFlash = false;
	Visual::NadeTracerEnabled = false;
	Visual::NadeTracerColor = Color::Red;
	Visual::SpreadCircleEnabled = false;
	Visual::SpreadCircleColor = Color::Blue;
	Visual::DamageIndicator = false;
	Visual::DamageIndicatorColor = Color::Red;
	Visual::DisableScopeZoom = false;
	Visual::ViewModelFOV = 80;
	Visual::FOV = 80;
	Visual::DisablePP = false;
	Visual::NightMode = false;
	Visual::NoSmoke = false;
	Visual::Hitmarker = false;
	Visual::HitmarkerSound = false;
	Visual::RagdollForce = 0;

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
	Visual::GlobalESP.Enabled = false;
	Visual::GlobalESP.DZEnabled = false;
	Visual::GlobalESP.DZRange = 100.f;
	Visual::GlobalESP.SoundESPEnabled = false;
}

void Settings::ResetMisc()
{
	Misc::BHop = false;
	Misc::AutoStrafe = false;
	Misc::RankReveal = false;
	Misc::NoCrouchCooldown = false;
	Misc::AutoAccept = false;
	Misc::Clantag = false;
	Misc::SpectatorsEnabled = false;
	Misc::BuyBot = false;
	Misc::BuyBotPistol = 0;
	Misc::BuyBotWeapon = 0;
	Misc::BuyBotArmor = false;
	Misc::BuyBotZeus = false;
	Misc::BuyBotDefuser = false;
	Misc::SkinchangerEnabled = false;

	Misc::RadioEnabled = false;
	Misc::RadioSelected = -1;
	Misc::RadioVolume = 20;
	Misc::RadioPauseHotkey = 0x0;
}

void Settings::SaveColorValue(json & j, std::string name, const Color & value)
{
	try
	{
		j[name]["R"] = value.r();
		j[name]["G"] = value.g();
		j[name]["B"] = value.b();
		j[name]["A"] = value.a();
	}
	catch (json::exception& ex)
	{
		g_Logger.Warning("CONFIG", ex.what());
		//g_Logger.Warning( "CONFIG", "Loading error: " + std::to_string(ex.what()) );
	}
}

void Settings::LoadColorValue(json & j, std::string name, Color & value)
{
	try
	{
		value.SetColor(
			(int)j[name]["R"],
			(int)j[name]["G"],
			(int)j[name]["B"],
			(int)j[name]["A"]
		);
	}
	catch (json::exception& ex)
	{
		g_Logger.Warning("CONFIG", ex.what());
		//g_Logger.Warning( "CONFIG", "Loading error: " + std::to_string(ex.what()) );
	}
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
	int DesyncFlipHotkey = 0x0;

	bool AutoScope = false;
	bool AutoStop = false;
	bool AutoCrouch = false;
	bool SlowWalk = false;
	int SlowWalkHotkey = 0x0;
	int FakeDuckHotkey = 0x0;
	bool FakeDuck = false;
	float SlowWalkMod = .0f; 

	bool FakelagPrediction = false;
	int ShootingMode = 0;
	bool ForceUnlag = false;
	bool Resolver = false;

	int GetWeaponType(C_BaseCombatWeapon * weapon)
	{
		if ( weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_DEAGLE )
			return WeaponType::WEAPON_DEAGLE;
		else if ( weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER )
			return WeaponType::WEAPON_REVOLVER;
		else if ( weapon->IsRifle() || weapon->IsMachinegun() )
			return WeaponType::WEAPON_RIFLE;
		else if ( weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 ) 
			return WeaponType::WEAPON_SSG08;
		else if ( weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP )
			return WeaponType::WEAPON_AWP;
		else if ( weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SCAR20 || weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_G3SG1 )
			return WeaponType::WEAPON_AUTO;
		else if ( weapon->IsPistol() )
			return WeaponType::WEAPON_PISTOL;
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

	int GetWeaponType(C_BaseCombatWeapon* weapon)
	{
		if ( weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_DEAGLE )
			return WeaponType::WEAPON_DEAGLE;
		else if ( weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 )
			return WeaponType::WEAPON_SSG08;
		else if ( weapon->IsSubmachinegun() )
			return WeaponType::WEAPON_SMG;
		else if ( weapon->IsMachinegun() )
			return WeaponType::WEAPON_MACHINEGUN;
		else if ( weapon->IsRifle() )
			return WeaponType::WEAPON_RIFLE;
		else if ( weapon->IsPistol() )
			return WeaponType::WEAPON_PISTOL;
		else if ( weapon->IsShotgun() )
			return WeaponType::WEAPON_SHOTGUN;
		else if ( weapon->IsSniper() )
			return WeaponType::WEAPON_SNIPER;
		else
			return WeaponType::WEAPON_SHOTGUN;
	}
}

namespace Settings::TriggerBot
{
	bool Enabled = false;
	int Key = 0x0;
}

namespace Settings::Skins
{
	std::map<int, item_setting> m_items;
	std::unordered_map<std::string, std::string> m_icon_overrides;
}

namespace Settings::Visual
{
	Chams LocalChams;
	PlayerESP LocalESP;
	bool GhostEnabled = false;
	Color GhostColor = Color::White;
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
	bool NadeTracerEnabled = false;
	Color NadeTracerColor = Color::Red;
	bool SpreadCircleEnabled = false;
	Color SpreadCircleColor = Color::Blue;
	bool DamageIndicator = false;
	Color DamageIndicatorColor = Color::Red;
	bool DisableScopeZoom = false;
	int ViewModelFOV = 80;
	int FOV = 80;
	bool NoSmoke = false;
	bool NightMode = false;
	bool DisablePP = false;
	bool Hitmarker = false;
	bool HitmarkerSound = false;
	int RagdollForce = 0;
}

namespace Settings::Misc
{
	bool BHop = false;
	bool AutoStrafe = false;
	bool RankReveal = false;
	bool NoCrouchCooldown = false;
	bool AutoAccept = false;
	bool Clantag = false;
	bool BuyBot = false;
	bool SpectatorsEnabled = false;
	int BuyBotPistol = 0;
	int BuyBotWeapon = 0;
	bool BuyBotArmor = false;
	bool BuyBotZeus = false;
	bool BuyBotDefuser = false;
	bool SkinchangerEnabled = false;
	bool RadioEnabled = false;
	int RadioSelected = -1;
	int RadioVolume = 20;
	int RadioPauseHotkey = 0x0;
}