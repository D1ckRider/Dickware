#include "Menu.h"
#include "FontManager.h"
#include "ConfigSystem.h"
#include "ConsoleHelper.h"
#include "options.hpp"
#include "Misc.h"
#include "Settings.h"
#include "ui.hpp"
#include "features\Skinchanger.h"
#include "definitions.h"
#include "Media\Player.h"
#include "RuntimeSaver.h"


using WeaponType = Settings::WeaponType;
using HitboxType = Settings::HitboxType;
using AntiAimState = Settings::RageBot::AntiAimType;

const std::string Version = "190302.01";
std::string CurrentConfig = "None";

ImFont* IconsFont;
Menu::Menu()
{
    Components = MenuHelper::components();
}

void Menu::Initialize()
{
    ImGuiIO& io = ImGui::GetIO();

    FontManager fontmgr;

    IconsFont = fontmgr.GetIconFont ( io, 24.f );
    Loaded = true;
}

void Menu::Render()
{
    if ( !Loaded || g_Unload )
        return;

	Components.StartWindow("DickWare", ImVec2(880, 515), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    static char* NavBarItems[] = { "s", "o", "t", "u", "4", "v" };
    static char* NavBarItemsText[] = { "ragebot", "legitbot", "visuals", "misc", "skinchanger", "settings" };
    static int NavBarSelected = 0;
    Components.NavbarIcons ( NavBarItems, NavBarItemsText, IM_ARRAYSIZE ( NavBarItems ), NavBarSelected, IconsFont );

    switch ( ( MenuAvailable ) NavBarSelected )
    {
        case MenuAvailable::RAGEBOT:
            RenderRagebot();
            break;

        case MenuAvailable::LEGITBOT:
            RenderLegitbot();
            break;

        case MenuAvailable::VISUALS:
            RenderVisuals();
            break;

        case MenuAvailable::MISC:
            RenderMisc();
            break;
		case MenuAvailable::SKINCHANGER:
			RenderSkinchanger();
			break;
        case MenuAvailable::SETTINGS:
            RenderSettings();
            break;
    }

	Components.Columns(3, false);
	Components.Label("DickWare Beta | Version: " + Version);
	Components.NextColumn();
	Components.Label("Current Config: " + CurrentConfig);
	Components.NextColumn();
	//Components.Label("Time: ");

    Components.EndWindow();
}

void Menu::RenderRagebot()
{
    Components.BeginChild ( "#ragebot", ImVec2 ( 0, 450 ) );

	//Components.Label("Ragebot");
	Components.Checkbox(" Ragebot", Settings::RageBot::Enabled);

	static char* RageBotItems[] = { "General", "Weapons" };
	static int RageBotSelected = 0;
	
	const char* BaimModes[] = { "never", "auto" };
	const char* ShootingModes[] = { "normal", "in fakelag", "fakelag while shooting" };


	static const char* YawAAs[] = { "none", "backwards", "spinbot", "lower body yaw", "random", "freestanding", "custom" };
	static const char* YawAddAAs[] = { "none", "jitter", "switch", "spin", "random" };
	static const char* PitchAAs[] = { "none", "emotion", "down", "up", "zero", "jitter", "down jitter", "up jitter", "zero jitter", "spin", "up spin", "down spin", "random", "switch", "down switch", "up switch", "fake up", "fake down", "custom" };
	static char* AntiAimMenus[] = { "stand", "move", "air", "misc" };
	static const char* FakelagModes[] = { "normal", "adaptive" };
	static int AAMenuSelected = 0;

	static char* WeaponConfigSelectionItems[] = { "G", "A", "J", "a", "Z", "Y", "W", "c" };
	static char* WeaponConfigSelectionItemsText[] = { "Pistol", "Deagle", "R8", "Scout", "AWP", "Auto", "Rifle", "Shotgun" };
	static int WeaponSelected = 0;


	Components.Navbar(RageBotItems, IM_ARRAYSIZE(RageBotItems), RageBotSelected);

	switch ((RbotMenuAvailable)RageBotSelected)
	{
	case RbotMenuAvailable::GENERAL:
		Components.Columns(2, false);
		Components.Spacing();
		Components.Label("BAim:");
		Components.ComboBox("BAim Mode", BaimModes, IM_ARRAYSIZE(BaimModes), Settings::RageBot::BAimMode);
		Components.Checkbox("Air BAim", Settings::RageBot::AirBAim);
		Components.Hotkey("Force BAim hotkey", Settings::RageBot::BAimHotkey, (int)HotkeyType::Tap);

		Components.Spacing();
		Components.Label("Movement:");
		Components.Checkbox("Slow Walk", Settings::RageBot::SlowWalk);
		Components.Hotkey("Slow Walk Hotkey", Settings::RageBot::SlowWalkHotkey);
		Components.SliderFloat("Slow Walk Speed", Settings::RageBot::SlowWalkMod, 0.f, 1.f);
		Components.Checkbox("Fake Duck", Settings::RageBot::FakeDuck);
		Components.Hotkey("FakeDuck Hotkey", Settings::RageBot::FakeDuckHotkey);
		Components.Checkbox("Auto Scope", Settings::RageBot::AutoScope);
		Components.Checkbox("Auto Stop", Settings::RageBot::AutoStop);
		Components.Checkbox("Auto Crouch", Settings::RageBot::AutoCrouch);

		//Components.Checkbox("Lby prediction", "rbot_lby_prediction");
#ifdef _DEBUG
	//Components.Checkbox("Fakelag prediction", "rbot_flag_prediction");
		Components.Checkbox("Fakelag prediction", Settings::RageBot::FakelagPrediction);
#endif // _DEBUG

		Components.ComboBox("Shooting Mode", ShootingModes, IM_ARRAYSIZE(ShootingModes), Settings::RageBot::ShootingMode);
#ifdef _DEBUG
		//Components.Checkbox("Lagcompensation", "rbot_lagcompensation");
		//Components.Checkbox("Lag compensation",  Settings::RageBot::);
#endif // _DEBUG
		Components.Checkbox("Force unlag", Settings::RageBot::ForceUnlag);

		Components.Checkbox("Resolver", Settings::RageBot::Resolver);
		Components.NextColumn();

		Components.Checkbox("Enable AA", Settings::RageBot::EnabledAA);
		Components.Spacing();

		Components.Navbar(AntiAimMenus, IM_ARRAYSIZE(AntiAimMenus), AAMenuSelected);
		switch ( (RbotAntiAimAvailable)AAMenuSelected )
		{
		case RbotAntiAimAvailable::STANDING:
			//if(g_Config.GetInt())
			//Components.SliderFloat("real add angel", "", -180.f, 180.f);
			//Components.ComboBox("fake yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_stand_fake_yaw");

			Components.ComboBox("Pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].Pitch);
			Components.ComboBox("Yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].Yaw);

			//Components.ComboBox("fake add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_stand_fake_add_yaw_add");
			//Components.SliderFloat("range ", "rbot_aa_stand_fake_add_yaw_add_range", 0.f, 360.f);
			Components.ComboBox("Yaw Add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].YawAdd);
			Components.SliderFloat("Range", Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].Range, 0.f, 360.f);

			Components.Spacing();
			Components.SliderInt("Fakelag Ticks", Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].FakelagTicks, 0, 14);
			break;
		case RbotAntiAimAvailable::MOVING:
			//Components.ComboBox("fake yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_move_fake_yaw");
			Components.ComboBox("Pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].Pitch);
			Components.ComboBox("Yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].Yaw);

			//Components.ComboBox("fake add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_move_fake_add_yaw_add");
			//Components.SliderFloat("range ", "rbot_aa_move_fake_add_yaw_add_range", 0.f, 360.f);

			Components.ComboBox("Yaw Add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].YawAdd);
			Components.SliderFloat("Range", Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].Range, 0.f, 360.f);

			Components.Spacing();
			Components.ComboBox("Fakelag mode", FakelagModes, IM_ARRAYSIZE(FakelagModes), Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].FakelagMode);
			Components.SliderInt("Fakelag Ticks", Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].FakelagTicks, 0, 14);
			break;

		case RbotAntiAimAvailable::AIR:
			//Components.ComboBox("fake yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_air_fake_yaw");
			Components.ComboBox("Pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), Settings::RageBot::AntiAimSettings[AntiAimState::AIR].Pitch);
			Components.ComboBox("Yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), Settings::RageBot::AntiAimSettings[AntiAimState::AIR].Yaw);

			Components.ComboBox("Yaw Add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), Settings::RageBot::AntiAimSettings[AntiAimState::AIR].YawAdd);
			Components.SliderFloat("Range", Settings::RageBot::AntiAimSettings[AntiAimState::AIR].Range, 0.f, 360.f);

			Components.Spacing();
			Components.ComboBox("Fakelag mode", FakelagModes, IM_ARRAYSIZE(FakelagModes), Settings::RageBot::AntiAimSettings[AntiAimState::AIR].FakelagMode);
			Components.SliderInt("Fakelag Ticks", Settings::RageBot::AntiAimSettings[AntiAimState::AIR].FakelagTicks, 0, 14);
			//Components.ComboBox("fake add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_air_fake_add_yaw_add");
			//Components.SliderFloat("range ", "rbot_aa_air_fake_add_yaw_add_range", 0.f, 360.f);
			break;

		case RbotAntiAimAvailable::MISC:

			Components.SliderFloat("Spinbot speed", Settings::RageBot::SpinBotSpeed, -20.f, 20.f);
			Components.Checkbox("SlideWalk", Settings::RageBot::SlideWalk);
			Components.Checkbox("Desync", Settings::RageBot::Desync);
			Components.Hotkey("Desync Hotkey", Settings::RageBot::DesyncFlipHotkey, (int)HotkeyType::Tap);

			//Components.Checkbox("Lby breaker", "rbot_aa_lby_breaker");
			//Components.Checkbox("Fake lby break", "rbot_aa_fake_lby_breaker");
			//Components.Checkbox("Lby breaker auto", "rbot_aa_lby_breaker_freestanding");
			/*
			if (!g_Config.GetBool("rbot_aa_lby_breaker_freestanding"))
			{
				Components.SliderFloat("Lby breaker add angle", "rbot_aa_lby_breaker_yaw", -180.f, 180.f);
			}
			else
			{
				Components.SliderFloat("Lby breaker backup angle", "rbot_aa_lby_breaker_yaw", -180.f, 180.f);
			}
			*/
			Components.Spacing();
			Components.Hotkey("Manual AA Right", Settings::RageBot::ManualAARightKey, (int)HotkeyType::Tap);
			Components.Hotkey("Manual AA Left", Settings::RageBot::ManualAALeftKey, (int)HotkeyType::Tap);
			Components.Hotkey("Manual AA Back", Settings::RageBot::ManualAABackKey, (int)HotkeyType::Tap);
			

			break;
		}
		break;
	case RbotMenuAvailable::WEAPONS:

		Components.Columns(2, false);

		Components.NavbarIcons(WeaponConfigSelectionItems, WeaponConfigSelectionItemsText, IM_ARRAYSIZE(WeaponConfigSelectionItems), WeaponSelected, IconsFont);

		switch ((RbotWeaponsAvailable)WeaponSelected)
		{
		case RbotWeaponsAvailable::PISTOL:
			Components.SliderFloat("Hitchance", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_PISTOL].Hitchance, 0.f, 100.f);
			Components.SliderFloat("Min Damage", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_PISTOL].MinDamage, 0.f, 100.f);
			Components.SliderInt("BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_PISTOL].BAimAfterShots, 0, 10);
			Components.SliderInt("Force BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_PISTOL].ForceBAimAfterShots, 0, 10);
			Components.Checkbox("BAim while moving", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_PISTOL].BAimWhileMoving);
			break;
		case RbotWeaponsAvailable::DEAGLE:
			Components.SliderFloat("Hitchance", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_DEAGLE].Hitchance, 0.f, 100.f);
			Components.SliderFloat("Min Damage", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_DEAGLE].MinDamage, 0.f, 100.f);
			Components.SliderInt("BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_DEAGLE].BAimAfterShots, 0, 10);
			Components.SliderInt("Force BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_DEAGLE].ForceBAimAfterShots, 0, 10);
			Components.Checkbox("BAim while moving", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_DEAGLE].BAimWhileMoving);
			break;
		case RbotWeaponsAvailable::REVOLVER:
			Components.SliderFloat("Hitchance", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_REVOLVER].Hitchance, 0.f, 100.f);
			Components.SliderFloat("Min Damage", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_REVOLVER].MinDamage, 0.f, 100.f);
			Components.SliderInt("BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_REVOLVER].BAimAfterShots, 0, 10);
			Components.SliderInt("Force BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_REVOLVER].ForceBAimAfterShots, 0, 10);
			Components.Checkbox("BAim while moving", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_REVOLVER].BAimWhileMoving);
			break;
		case RbotWeaponsAvailable::SCOUT:
			Components.SliderFloat("Hitchance", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SSG08].Hitchance, 0.f, 100.f);
			Components.SliderFloat("Min Damage", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SSG08].MinDamage, 0.f, 100.f);
			Components.SliderInt("BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SSG08].BAimAfterShots, 0, 10);
			Components.SliderInt("Force BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SSG08].ForceBAimAfterShots, 0, 10);
			Components.Checkbox("BAim while moving", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SSG08].BAimWhileMoving);
			break;
		case RbotWeaponsAvailable::AWP:
			Components.SliderFloat("Hitchance", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AWP].Hitchance, 0.f, 100.f);
			Components.SliderFloat("Min Damage", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AWP].MinDamage, 0.f, 100.f);
			Components.SliderInt("BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AWP].BAimAfterShots, 0, 10);
			Components.SliderInt("Force BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AWP].ForceBAimAfterShots, 0, 10);
			Components.Checkbox("BAim while moving", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AWP].BAimWhileMoving);
			break;
		case RbotWeaponsAvailable::AUTO:
			Components.SliderFloat("Hitchance", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AUTO].Hitchance, 0.f, 100.f);
			Components.SliderFloat("Min Damage", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AUTO].MinDamage, 0.f, 100.f);
			Components.SliderInt("BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AUTO].BAimAfterShots, 0, 10);
			Components.SliderInt("Force BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AUTO].ForceBAimAfterShots, 0, 10);
			Components.Checkbox("BAim while moving", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_AUTO].BAimWhileMoving);

			break;
		case RbotWeaponsAvailable::RIFLE:
			Components.SliderFloat("Hitchance", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_RIFLE].Hitchance, 0.f, 100.f);
			Components.SliderFloat("Min Damage", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_RIFLE].MinDamage, 0.f, 100.f);
			Components.SliderInt("BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_RIFLE].BAimAfterShots, 0, 10);
			Components.SliderInt("Force BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_RIFLE].ForceBAimAfterShots, 0, 10);
			Components.Checkbox("BAim while moving", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_RIFLE].BAimWhileMoving);

			break;
		case RbotWeaponsAvailable::SHOTGUN:
			Components.SliderFloat("Hitchance", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SHOTGUN].Hitchance, 0.f, 100.f);
			Components.SliderFloat("Min Damage", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SHOTGUN].MinDamage, 0.f, 100.f);
			Components.SliderInt("BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SHOTGUN].BAimAfterShots, 0, 10);
			Components.SliderInt("Force BAim after x shots", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SHOTGUN].ForceBAimAfterShots, 0, 10);
			Components.Checkbox("BAim while moving", Settings::RageBot::WeaponSettings[WeaponType::WEAPON_SHOTGUN].BAimWhileMoving);

			break;
		}
		Components.NextColumn();
#pragma region Hitbox Scales
		Components.Label("Hitbox Scales");
		Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
		Components.Checkbox("Head   ", Settings::RageBot::Hitboxes[HitboxType::HEAD].Enabled);
		Components.SameLine();
		Components.Label(" Scale:");
		Components.SameLine();
		Components.SliderFloat(" ", Settings::RageBot::Hitboxes[HitboxType::HEAD].Scale, 0.f, 1.f);

		Components.Checkbox("Neck   ", Settings::RageBot::Hitboxes[HitboxType::NECK].Enabled);
		Components.SameLine();
		Components.Label(" Scale:");
		Components.SameLine();
		Components.SliderFloat("  ", Settings::RageBot::Hitboxes[HitboxType::NECK].Scale, 0.f, 1.f);

		Components.Checkbox("Chest  ", Settings::RageBot::Hitboxes[HitboxType::CHEST].Enabled);
		Components.SameLine();
		Components.Label(" Scale:");
		Components.SameLine();
		Components.SliderFloat("   ", Settings::RageBot::Hitboxes[HitboxType::CHEST].Scale, 0.f, 1.f);

		Components.Checkbox("Pelvis ", Settings::RageBot::Hitboxes[HitboxType::PELVIS].Enabled);
		Components.SameLine();
		Components.Label(" Scale:");
		Components.SameLine();
		Components.SliderFloat("    ", Settings::RageBot::Hitboxes[HitboxType::PELVIS].Scale, 0.f, 1.f);

		Components.Checkbox("Stomach", Settings::RageBot::Hitboxes[HitboxType::STOMACH].Enabled);
		Components.SameLine();
		Components.Label(" Scale:");
		Components.SameLine();
		Components.SliderFloat("     ", Settings::RageBot::Hitboxes[HitboxType::STOMACH].Scale, 0.f, 1.f);

		Components.Checkbox("Arm    ", Settings::RageBot::Hitboxes[HitboxType::ARM].Enabled);
		Components.SameLine();
		Components.Label(" Scale:");
		Components.SameLine();
		Components.SliderFloat("         ", Settings::RageBot::Hitboxes[HitboxType::ARM].Scale, 0.f, 1.f);

		Components.Checkbox("Leg    ", Settings::RageBot::Hitboxes[HitboxType::LEG].Enabled);
		Components.SameLine();
		Components.Label(" Scale:");
		Components.SameLine();
		Components.SliderFloat("       ", Settings::RageBot::Hitboxes[HitboxType::LEG].Scale, 0.f, 1.f);

		Components.Checkbox("Foot   ", Settings::RageBot::Hitboxes[HitboxType::FOOT].Enabled);
 		Components.SameLine();
		Components.Label(" Scale:");
		Components.SameLine();
		Components.SliderFloat("        ", Settings::RageBot::Hitboxes[HitboxType::FOOT].Scale, 0.f, 1.f);
		Components.EndChild();
#pragma endregion
		break;
	}

	//Components.SliderInt("Brut after x shots", "rbot_brutforce_after_shots", 0, 10);

	//Components.SliderFloat("Pointscale head", "rbot_head_scale", 0.f, 1.f);
	//Components.SliderFloat("Pointscale body", "rbot_body_scale", 0.f, 1.f);
    //Components.NextColumn();
	
    Components.EndChild();
}

void Menu::RenderLegitbot()
{
    Components.BeginChild ( "#lbot", ImVec2 ( 0, 450) );

    //Components.Label ( "AimBot" );
	

    //Components.Spacing();
    //Components.Columns ( 2, false );
	
	static char*	LBotMenuItems[] = { "AimBot", "Backtrack", "Triggerbot" };
	static int		LBotSelected = 0;
    static char*	WeaponConfigSelectionItems[] = { "G", "A", "L", "f", "W", "c", "a", "Z" };
    static char*	WeaponConfigSelectionItemsText[] = { "pistol", "deagle", "smg", "mg", "rifle", "shotgun", "scout", "sniper" };
    static int		WeaponSelected = 0;

	Components.Navbar(LBotMenuItems, IM_ARRAYSIZE(LBotMenuItems), LBotSelected);
	switch ((LbotMenuAvailable)LBotSelected)
	{
	case LbotMenuAvailable::Aimbot:
		Components.Checkbox("Enable", Settings::Aimbot::Enabled);
		Components.NavbarIcons(WeaponConfigSelectionItems, WeaponConfigSelectionItemsText, IM_ARRAYSIZE(WeaponConfigSelectionItems), WeaponSelected, IconsFont);
		Components.Columns(2, false);

		Components.Hotkey("Aimkey", Settings::Aimbot::Hotkey);
		Components.Checkbox("Smoke Check", Settings::Aimbot::SmokeCheck);

		switch ((LbotWeaponsAvailable)WeaponSelected)
		{
		case LbotWeaponsAvailable::PISTOL:

			Components.SliderFloat("FOV", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].FOV, 0.f, 15.f);
			Components.SliderFloat("Smooth", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].Smooth, 1.f, 30.f);
			Components.SliderFloat("Randomize", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].Randomize, 0.f, 10.f);
			Components.SliderFloat("Delay", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].Delay, 0.f, 1.f);
			Components.Checkbox("Auto Pistol", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].Autopistol);

			Components.Spacing();

			Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].RCS);
			Components.SliderFloat("Amount X", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].RCS_X, 0.f, 1.f);
			Components.SliderFloat("Amount Y", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].RCS_Y, 0.f, 1.f);

			Components.NextColumn();
			
			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));

			Components.Checkbox("Head", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].HitboxHead);
			Components.Checkbox("Neck", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].HitboxNeck);
			Components.Checkbox("Chest", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].HitboxChest);
			Components.Checkbox("Pelvis", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].HitboxPelvis);
			Components.Checkbox("Stomach", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].HitboxStomach);
			Components.Checkbox("Arm", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].HitboxArm);
			Components.Checkbox("Leg", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].HitboxLeg);
			Components.Checkbox("Foot", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_PISTOL].HitboxFoot);

			Components.EndChild();
			break;

		case LbotWeaponsAvailable::DEAGLE:
			Components.SliderFloat("FOV", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].FOV, 0.f, 15.f);
			Components.SliderFloat("Smooth", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].Smooth, 1.f, 30.f);
			Components.SliderFloat("Randomize", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].Randomize, 0.f, 10.f);
			Components.SliderFloat("Delay", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].Delay, 0.f, 1.f);
			Components.Checkbox("Auto Pistol", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].Autopistol);

			Components.Spacing();

			Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].RCS);
			Components.SliderFloat("Amount X", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].RCS_X, 0.f, 1.f);
			Components.SliderFloat("Amount Y", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].RCS_Y, 0.f, 1.f);


			Components.NextColumn();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));

			Components.Checkbox("Head", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].HitboxHead);
			Components.Checkbox("Neck", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].HitboxNeck);
			Components.Checkbox("Chest", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].HitboxChest);
			Components.Checkbox("Pelvis", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].HitboxPelvis);
			Components.Checkbox("Stomach", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].HitboxStomach);
			Components.Checkbox("Arm", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].HitboxArm);
			Components.Checkbox("Leg", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].HitboxLeg);
			Components.Checkbox("Foot", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_DEAGLE].HitboxFoot);

			Components.EndChild();
			break;
		case LbotWeaponsAvailable::SMG:
			Components.SliderFloat("FOV", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].FOV, 0.f, 15.f);
			Components.SliderFloat("Smooth", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].Smooth, 1.f, 30.f);
			Components.SliderFloat("Randomize", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].Randomize, 0.f, 10.f);
			Components.SliderFloat("Delay", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].Delay, 0.f, 1.f);

			Components.Spacing();

			Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].RCS);
			Components.SliderFloat("Amount X", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].RCS_X, 0.f, 1.f);
			Components.SliderFloat("Amount Y", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].RCS_Y, 0.f, 1.f);

			Components.NextColumn();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));

			Components.Checkbox("Head", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].HitboxHead);
			Components.Checkbox("Neck", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].HitboxNeck);
			Components.Checkbox("Chest", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].HitboxChest);
			Components.Checkbox("Pelvis", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].HitboxPelvis);
			Components.Checkbox("Stomach", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].HitboxStomach);
			Components.Checkbox("Arm", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].HitboxArm);
			Components.Checkbox("Leg", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].HitboxLeg);
			Components.Checkbox("Foot", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SMG].HitboxFoot);

			Components.EndChild();
			break;

		case LbotWeaponsAvailable::MG:
			Components.SliderFloat("FOV", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].FOV, 0.f, 15.f);
			Components.SliderFloat("Smooth", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].Smooth, 1.f, 30.f);
			Components.SliderFloat("Randomize", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].Randomize, 0.f, 10.f);
			Components.SliderFloat("Delay", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].Delay, 0.f, 1.f);

			Components.Spacing();

			Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].RCS);
			Components.SliderFloat("Amount X", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].RCS_X, 0.f, 1.f);
			Components.SliderFloat("Amount Y", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].RCS_Y, 0.f, 1.f);


			Components.NextColumn();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));

			Components.Checkbox("Head", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].HitboxHead);
			Components.Checkbox("Neck", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].HitboxNeck);
			Components.Checkbox("Chest", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].HitboxChest);
			Components.Checkbox("Pelvis", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].HitboxPelvis);
			Components.Checkbox("Stomach", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].HitboxStomach);
			Components.Checkbox("Arm", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].HitboxArm);
			Components.Checkbox("Leg", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].HitboxLeg);
			Components.Checkbox("Foot", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_MACHINEGUN].HitboxFoot);

			Components.EndChild();
			break;

		case LbotWeaponsAvailable::RIFLE:
			Components.SliderFloat("FOV", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].FOV, 0.f, 15.f);
			Components.SliderFloat("Smooth", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].Smooth, 1.f, 30.f);
			Components.SliderFloat("Randomize", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].Randomize, 0.f, 10.f);
			Components.SliderFloat("Delay", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].Delay, 0.f, 1.f);

			Components.Spacing();

			Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].RCS);
			Components.SliderFloat("Amount X", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].RCS_X, 0.f, 1.f);
			Components.SliderFloat("Amount Y", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].RCS_Y, 0.f, 1.f);


			Components.NextColumn();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));

			Components.Checkbox("Head", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].HitboxHead);
			Components.Checkbox("Neck", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].HitboxNeck);
			Components.Checkbox("Chest", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].HitboxChest);
			Components.Checkbox("Pelvis", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].HitboxPelvis);
			Components.Checkbox("Stomach", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].HitboxStomach);
			Components.Checkbox("Arm", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].HitboxArm);
			Components.Checkbox("Leg", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].HitboxLeg);
			Components.Checkbox("Foot", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_RIFLE].HitboxFoot);

			Components.EndChild();
			break;

		case LbotWeaponsAvailable::SHOTGUN:
			Components.SliderFloat("FOV", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].FOV, 0.f, 15.f);
			Components.SliderFloat("Smooth", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].Smooth, 1.f, 30.f);
			Components.SliderFloat("Randomize", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].Randomize, 0.f, 10.f);
			Components.SliderFloat("Delay", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].Delay, 0.f, 1.f);

			Components.Spacing();

			Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].RCS);
			Components.SliderFloat("Amount X", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].RCS_X, 0.f, 1.f);
			Components.SliderFloat("Amount Y", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].RCS_Y, 0.f, 1.f);


			Components.NextColumn();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));

			Components.Checkbox("Head", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].HitboxHead);
			Components.Checkbox("Neck", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].HitboxNeck);
			Components.Checkbox("Chest", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].HitboxChest);
			Components.Checkbox("Pelvis", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].HitboxPelvis);
			Components.Checkbox("Stomach", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].HitboxStomach);
			Components.Checkbox("Arm", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].HitboxArm);
			Components.Checkbox("Leg", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].HitboxLeg);
			Components.Checkbox("Foot", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SHOTGUN].HitboxFoot);
			Components.EndChild();
			break;
		case LbotWeaponsAvailable::SCOUT:
			Components.SliderFloat("FOV", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].FOV, 0.f, 15.f);
			Components.SliderFloat("Smooth", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].Smooth, 1.f, 30.f);
			Components.SliderFloat("Randomize", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].Randomize, 0.f, 10.f);
			Components.SliderFloat("Delay", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].Delay, 0.f, 1.f);

			Components.Spacing();

			Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].RCS);
			Components.SliderFloat("Amount X", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].RCS_X, 0.f, 1.f);
			Components.SliderFloat("Amount Y", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].RCS_Y, 0.f, 1.f);


			Components.NextColumn();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
			{
				Components.Checkbox("Head", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].HitboxHead);
				Components.Checkbox("Neck", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].HitboxNeck);
				Components.Checkbox("Chest", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].HitboxChest);
				Components.Checkbox("Pelvis", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].HitboxPelvis);
				Components.Checkbox("Stomach", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].HitboxStomach);
				Components.Checkbox("Arm", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].HitboxArm);
				Components.Checkbox("Leg", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].HitboxLeg);
				Components.Checkbox("Foot", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SSG08].HitboxFoot);
			}
			Components.EndChild();
			break;
		case LbotWeaponsAvailable::SNIPER:
			Components.SliderFloat("FOV", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].FOV, 0.f, 15.f);
			Components.SliderFloat("Smooth", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].Smooth, 1.f, 30.f);
			Components.SliderFloat("Randomize", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].Randomize, 0.f, 10.f);
			Components.SliderFloat("Delay", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].Delay, 0.f, 1.f);

			Components.Spacing();

			Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].RCS);
			Components.SliderFloat("Amount X", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].RCS_X, 0.f, 1.f);
			Components.SliderFloat("Amount Y", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].RCS_Y, 0.f, 1.f);

			Components.NextColumn();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
			{
				Components.Checkbox("Head", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].HitboxHead);
				Components.Checkbox("Neck", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].HitboxNeck);
				Components.Checkbox("Chest", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].HitboxChest);
				Components.Checkbox("Pelvis", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].HitboxPelvis);
				Components.Checkbox("Stomach", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].HitboxStomach);
				Components.Checkbox("Arm", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].HitboxArm);
				Components.Checkbox("Leg", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].HitboxLeg);
				Components.Checkbox("Foot", Settings::Aimbot::WeaponAimSetting[WeaponType::WEAPON_SNIPER].HitboxFoot);

			}
			Components.EndChild();
			break;
		}
		break;
	case LbotMenuAvailable::Backtrack:
		Components.Columns(2, false);
		Components.Checkbox("Backtrack", Settings::Aimbot::Backtrack);
		Components.Checkbox("Aim at backtrack", Settings::Aimbot::BacktrackAtAim);
		Components.SliderFloat("Backtrack time", Settings::Aimbot::BacktrackTick, 0.f, .2f);
		break;
	case LbotMenuAvailable::Triggerbot:
		Components.Columns(2, false);
		Components.Label("Triggerbot");
		Components.Checkbox("Enabled", Settings::TriggerBot::Enabled);
		Components.Hotkey("Key", Settings::TriggerBot::Key);
		break;
	}

	//Components.Spacing();
	
    //legit aa

    Components.EndChild();
}

void Menu::RenderVisuals()
{
    Components.BeginChild ( "#visuals", ImVec2 ( 0, 450) );

    Components.Label ( "Visuals" );
    static char* VisualsCategories[] = { "local", "enemy", "team", "misc", "globals" };
    static int SelectedMenu = 0;
    Components.Navbar ( VisualsCategories, IM_ARRAYSIZE ( VisualsCategories ), SelectedMenu );

    static const char* ChamsTypes[] = { "normal", "flat", "wireframe", "glass", "metallic",  "xqz", "metallic xqz", "flat xqz" };
    static const char* BoxTypes[] = { "normal", "edge" };

    switch ( ( VisualsMenuAvailable ) SelectedMenu )
    {
        case VisualsMenuAvailable::LOCAL:
        {
			Components.Columns(3, false);
			Components.Label("Chams");
			Components.ColorCheckbox2("Enable", Settings::Visual::LocalChams.Enabled, Settings::Visual::LocalChams.Visible, Settings::Visual::LocalChams.Invisible);
			Components.ComboBox("Chams Type", ChamsTypes, IM_ARRAYSIZE(ChamsTypes), Settings::Visual::LocalChams.Mode);
			Components.ColorCheckbox("Ghost Chams", Settings::Visual::GhostEnabled, Settings::Visual::GhostColor);
			Components.NextColumn();

			Components.Label("ESP");
			Components.Spacing();
			Components.Checkbox("Enable ",  Settings::Visual::LocalESP.Enabled);
			Components.ColorCheckbox("Box", Settings::Visual::LocalESP.BoxEnabled, Settings::Visual::LocalESP.BoxColor);
			Components.ComboBox("Box Type", BoxTypes, IM_ARRAYSIZE(BoxTypes), Settings::Visual::LocalESP.BoxType);
			Components.ColorCheckbox("Name", Settings::Visual::LocalESP.NameEnabled, Settings::Visual::LocalESP.NameColor);
			Components.Checkbox("Health",  Settings::Visual::LocalESP.HealthEnabled);
			Components.ColorCheckbox("Armor", Settings::Visual::LocalESP.ArmorEnabled, Settings::Visual::LocalESP.ArmorColor);
			Components.ColorCheckbox("Weapon", Settings::Visual::LocalESP.WeaponEnabled, Settings::Visual::LocalESP.WeaponColor);
			
            break;
        }

        case VisualsMenuAvailable::ENEMY:
        {
			Components.Columns(3, false);
            //Components.ColorCheckbox ( "Resolver info", "esp_enemy_info", "color_esp_enemy_info" );
            //Components.ColorCheckbox ( "Lby timer", "esp_enemy_lby_timer", "color_esp_enemy_lby_timer" );
			Components.Label("Chams");
			Components.ColorCheckbox2("Enable", Settings::Visual::EnemyChams.Enabled, Settings::Visual::EnemyChams.Visible, Settings::Visual::EnemyChams.Invisible);
			Components.ComboBox("Chams Type", ChamsTypes, IM_ARRAYSIZE(ChamsTypes), Settings::Visual::EnemyChams.Mode);
			Components.NextColumn();

			Components.Label("ESP");
			Components.Spacing();
			Components.Checkbox("Enable ",  Settings::Visual::EnemyESP.Enabled);
			Components.ColorCheckbox("Box", Settings::Visual::EnemyESP.BoxEnabled, Settings::Visual::EnemyESP.BoxColor);
			Components.ComboBox("Box Type", BoxTypes, IM_ARRAYSIZE(BoxTypes), Settings::Visual::EnemyESP.BoxType);
			Components.ColorCheckbox("Name", Settings::Visual::EnemyESP.NameEnabled, Settings::Visual::EnemyESP.NameColor);
			Components.Checkbox("Health",  Settings::Visual::EnemyESP.HealthEnabled);
			Components.ColorCheckbox("Armor", Settings::Visual::EnemyESP.ArmorEnabled, Settings::Visual::EnemyESP.ArmorColor);
			Components.ColorCheckbox("Weapon", Settings::Visual::EnemyESP.WeaponEnabled, Settings::Visual::EnemyESP.WeaponColor);
			Components.ColorCheckbox("Snapline", Settings::Visual::EnemyESP.SnaplineEnabled, Settings::Visual::EnemyESP.SnaplineColor);
			break;
        }

        case VisualsMenuAvailable::TEAM:
        {
			Components.Columns(3, false);
			Components.Label("Chams");
			Components.ColorCheckbox2("Enable", Settings::Visual::TeamChams.Enabled, Settings::Visual::TeamChams.Visible, Settings::Visual::TeamChams.Invisible);
			Components.ComboBox("Chams Type", ChamsTypes, IM_ARRAYSIZE(ChamsTypes), Settings::Visual::TeamChams.Mode);
			Components.NextColumn();

			Components.Label("ESP");
			Components.Spacing();
			Components.Checkbox("Enable ", Settings::Visual::TeamESP.Enabled);
			Components.ColorCheckbox("Box", Settings::Visual::TeamESP.BoxEnabled, Settings::Visual::TeamESP.BoxColor);
			Components.ComboBox("Box Type", BoxTypes, IM_ARRAYSIZE(BoxTypes), Settings::Visual::TeamESP.BoxType);
			Components.ColorCheckbox("Name", Settings::Visual::TeamESP.NameEnabled, Settings::Visual::TeamESP.NameColor);
			Components.Checkbox("Health",  Settings::Visual::TeamESP.HealthEnabled);
			Components.ColorCheckbox("Armor", Settings::Visual::TeamESP.ArmorEnabled, Settings::Visual::TeamESP.ArmorColor);
			Components.ColorCheckbox("Weapon", Settings::Visual::TeamESP.WeaponEnabled, Settings::Visual::TeamESP.WeaponColor);
			Components.ColorCheckbox("Snapline", Settings::Visual::TeamESP.SnaplineEnabled, Settings::Visual::TeamESP.SnaplineColor);
			
			break;
        }

        case VisualsMenuAvailable::MISC:
        {
            Components.Columns ( 3, false );
            Components.Label ( "Thirdperson: " );
			Components.Spacing();
			Components.Checkbox("Enabled: ",  Settings::Visual::ThirdPersonEnabled);
			Components.Hotkey("Hotkey: ",  Settings::Visual::ThirdPersonHotkey, (int)HotkeyType::Tap);

			Components.NextColumn();

            Components.Label ( "Esp:" );
			Components.Spacing();
			Components.Checkbox("Enable ",  Settings::Visual::GlobalESP.Enabled);
			Components.Checkbox("Grenade",  Settings::Visual::GlobalESP.GrenadeEnabled);
			Components.ColorCheckbox("Bomb", Settings::Visual::GlobalESP.BombEnabled, Settings::Visual::GlobalESP.BombColor);
			Components.Checkbox("Droped Weapons", Settings::Visual::GlobalESP.DropedWeaponsEnabled);
			Components.Checkbox("DangerZone item ESP",  Settings::Visual::GlobalESP.DZEnabled);
			Components.SliderFloat("DangerZone ESP Range",  Settings::Visual::GlobalESP.DZRange, 0.f, 1000.f);
			Components.Checkbox("Sound ESP", Settings::Visual::GlobalESP.SoundESPEnabled);

			Components.NextColumn();

            Components.Label ( "Misc:" );
			Components.Spacing();
			Components.Checkbox("No Scope Overlay",  Settings::Visual::NoScopeOverlay);
			Components.Checkbox("Bullet Tracers",  Settings::Visual::BulletTracers);
			Components.Checkbox("No Flash",  Settings::Visual::NoFlash);
			Components.Checkbox("No Smoke",  Settings::Visual::NoSmoke);
			Components.Checkbox("Disable Post-processing", Settings::Visual::DisablePP);
			Components.Checkbox("Night Mode", Settings::Visual::NightMode);
			Components.ColorCheckbox("Nade Trajectory", Settings::Visual::NadeTracerEnabled, Settings::Visual::NadeTracerColor);
			Components.ColorCheckbox("Spread Circle", Settings::Visual::SpreadCircleEnabled, Settings::Visual::SpreadCircleColor);
			Components.ColorCheckbox("Damage Indicators", Settings::Visual::DamageIndicator, Settings::Visual::DamageIndicatorColor);
			Components.Checkbox("Disable Sniper Zoom",  Settings::Visual::DisableScopeZoom);
			Components.SliderInt("Viewmodel FOV",  Settings::Visual::ViewModelFOV, 1, 150);
			Components.SliderInt("FOV",  Settings::Visual::FOV, 1, 150);
			Components.Checkbox("Hitmarker",  Settings::Visual::Hitmarker);
			Components.Checkbox("Hitmarker Sound", Settings::Visual::HitmarkerSound);
            /*static const char* hitmarkersounds[] = { "amera", "skeet" };
            Components.ComboBox ( "Hitmarker sound", hitmarkersounds, IM_ARRAYSIZE ( hitmarkersounds ), "vis_misc_hitmarker_sound" );*/
            //Components.SliderInt("Asuswalls", "vis_misc_asuswalls_percent", 0, 100);
            //Components.Checkbox("Autowall crosshair", "vis_misc_autowall_crosshair");
            #ifdef _DEBUG
            Components.SliderInt ( "Ragdoll force", "misc_add_force", 0, 10 );
            #endif // _DEBUG

            break;
        }

        case VisualsMenuAvailable::GLOBAL:
        {
            Components.Columns ( 3, false );
            static const char* ItemPositions[] = { "top", "right", "bottom", "left" };
            //Components.ComboBox("Name pos", ItemPositions, IM_ARRAYSIZE(ItemPositions), "esp_name_pos");
            //Components.ComboBox("Weapon name pos", ItemPositions, IM_ARRAYSIZE(ItemPositions), "esp_weapons_pos");
            Components.ComboBox ( "Health pos", ItemPositions, IM_ARRAYSIZE ( ItemPositions ), "esp_health_pos" );
            Components.ComboBox ( "Armour pos", ItemPositions, IM_ARRAYSIZE ( ItemPositions ), "esp_armour_pos" );
            //Components.Checkbox("Esp outline", "esp_misc_outline");
            break;
        }
    }

	Components.EndChild();
}

void Menu::RenderMisc()
{
	
    Components.BeginChild ( "#misc", ImVec2 ( 0, 450 ) );

    Components.Label ( "Misc" );

	Components.Columns(2, false);

    //Components.Checkbox("No hands", "misc_no_hands");

	Components.Checkbox("BHop",  Settings::Misc::BHop);
	Components.Checkbox("Autostrafe",  Settings::Misc::AutoStrafe);

    Components.Spacing();

    Components.Spacing();

    //Components.Checkbox("Anti untrusted", "misc_antiuntrusted");*/

	Components.Checkbox("Rank reveal",  Settings::Misc::RankReveal);
	Components.Checkbox("No crouch cooldown",  Settings::Misc::NoCrouchCooldown);
	Components.Checkbox("AutoAccept", Settings::Misc::AutoAccept);

	Components.Checkbox("Clantag changer",  Settings::Misc::Clantag);
	Components.Checkbox("Spectator List", Settings::Misc::SpectatorsEnabled);

    Components.Spacing();

    //buybot
    static const char* Pistols[] = { "none", "glock|usp|p2000", "duals", "tec9|fiveseven", "deagle|r8" };
    static const char* Weapons[] = { "none", "sg|aug", "ssg", "auto", "mac10", "p90", "bizon", "ak", "awp" };
    static std::string Grenades[] = { "molotov", "decoy", "flash", "grenade", "smoke" };
    static std::string GrenadesSettings[] = { "misc_buybot_grenade_molotov", "misc_buybot_grenade_decoy", "misc_buybot_grenade_flash", "misc_buybot_grenade_grenade", "misc_buybot_grenade_smoke" };
    Components.Checkbox ( "Enable buybot", Settings::Misc::BuyBot );
    Components.ComboBox ( "Pistols", Pistols, IM_ARRAYSIZE ( Pistols ), Settings::Misc::BuyBotPistol );
    Components.ComboBox ( "Weapon", Weapons, IM_ARRAYSIZE ( Weapons ), Settings::Misc::BuyBotWeapon);
    //Components.ComboCheckBox ( "Grenades", Grenades, GrenadesSettings, IM_ARRAYSIZE ( Grenades ) );
    Components.Checkbox ( "Buy armor", Settings::Misc::BuyBotArmor);
    Components.Checkbox ( "Buy zeus", Settings::Misc::BuyBotZeus);
    Components.Checkbox ( "Buy defuser", Settings::Misc::BuyBotDefuser);

	if (Components.Button("unload"))
	{
		g_Unload = true;
		//player.Release();
	}

	Components.NextColumn();

	Components.Checkbox("Radio Enabled", Settings::Misc::RadioEnabled);
	// Radio feature
	static RadioPlayer player;
	static bool radioInit = false;
	
	if (Settings::Misc::RadioEnabled)
	{
		if (!radioInit)
		{
			player.PlayStreamFromURL(stations[0].data());
			radioInit = true;
			g_Saver.RadioPaused = false;
		}

		player.SetVolume(Settings::Misc::RadioVolume);

		if (!g_Saver.RadioPaused)
		{
			player.ResumePlayer();
		}
		else
		{
			player.PausePlayer();
		}

		if (ImGui::Combo("##Station", &Settings::Misc::RadioSelected, radio_name))
		{
			player.PlayStreamFromURL(stations[Settings::Misc::RadioSelected].data());
		}

		char* name;

		if (!g_Saver.RadioPaused)
			name = "||";
		else
			name = ">";

		Components.Hotkey("Pause Hotkey", Settings::Misc::RadioPauseHotkey, (int)HotkeyType::Tap);

		if (ImGui::Button(name, ImVec2(132, 32)))
		{
			g_Saver.RadioPaused = !g_Saver.RadioPaused;
			//player.PausePlayer();
		}

		Components.SliderInt("Volume", Settings::Misc::RadioVolume, 0, 100);
	}
	else
	{
		if (radioInit)
			player.PausePlayer();
	}
	
    #ifdef _DEBUG
    //Components.Checkbox ( "misc_debug_overlay", "misc_debug_overlay" );
    #endif // _DEBUG

    Components.EndChild();
}

void Menu::RenderSkinchanger()
 {
	Components.BeginChild("#skinchanger", ImVec2(0, 450));

	Components.Label("Skinchanger");

	if (k_skins.size() == 0)
		initialize_kits();


	auto& entries = Settings::Skins::m_items;
	static auto definition_vector_index = 0;
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::PushItemWidth(-1);
		ImGui::ListBoxHeader("##config");
		{
			for (size_t w = 0; w < k_weapon_names.size(); w++) {
				if (ImGui::Selectable(k_weapon_names[w].name, definition_vector_index == w)) 
				{
					definition_vector_index = w;
				}
			}
		}
		ImGui::ListBoxFooter();
		if (ImGui::Button("Update"))
			g_ClientState->ForceFullUpdate();

		if (Components.Button("Save Config"))
			Settings::SaveSkinsSettings();
		Components.SameLine();
		if (Components.Button("Load Config"))
			Settings::LoadSkinsSettings();


		ImGui::PopItemWidth();
	}
	ImGui::NextColumn();
	{
		auto& selected_entry = entries[k_weapon_names[definition_vector_index].definition_index];
		selected_entry.definition_index = k_weapon_names[definition_vector_index].definition_index;
		selected_entry.definition_vector_index = definition_vector_index;
		ImGui::Checkbox("Enabled", &selected_entry.enabled);
		ImGui::InputInt("Seed", &selected_entry.seed);
		ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
		ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);
		if (selected_entry.definition_index != GLOVE_T_SIDE)
		{
			ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_skins[idx].name.c_str();
				return true;
			}, nullptr, k_skins.size(), 10);
			selected_entry.paint_kit_index = k_skins[selected_entry.paint_kit_vector_index].id;
		}
		else
		{
			ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_gloves[idx].name.c_str();
				return true;
			}, nullptr, k_gloves.size(), 10);
			selected_entry.paint_kit_index = k_gloves[selected_entry.paint_kit_vector_index].id;
		}
		if (selected_entry.definition_index == WEAPON_KNIFE)
		{
			ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_knife_names.at(idx).name;
				return true;
			}, nullptr, k_knife_names.size(), 5);

			selected_entry.definition_override_index = k_knife_names.at(selected_entry.definition_override_vector_index).definition_index;
		}
		else if (selected_entry.definition_index == GLOVE_T_SIDE)
		{
			ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
			{
				*out_text = k_glove_names.at(idx).name;
				return true;
			}, nullptr, k_glove_names.size(), 5);
			selected_entry.definition_override_index = k_glove_names.at(selected_entry.definition_override_vector_index).definition_index;
		}
		else
		{
			static auto unused_value = 0;
			selected_entry.definition_override_vector_index = 0;
			ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
		}
		ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
	}
	ImGui::Columns(1, nullptr, false);

	
	Components.EndChild();
}

void Menu::RenderSettings()
{
    Components.BeginChild ( "#settings", ImVec2 ( 0, 450) );

    Components.Label ( "Settings" );
    Components.Spacing();

    Components.Columns ( 3, false );

    Components.BeginChild ( "#allConfigs", ImVec2 ( 0.f, 250.f ) );

    //for() Configs
    static int Selected = -1;
    int i = 0;

	for (auto config = Settings::Configs.begin(); config != Settings::Configs.end(); config++, i++)
	{
		if (ImGui::Selectable(config->data(), i == Selected))
			Selected = i;
	}

    Components.EndChild();

    Components.NextColumn();

    static char str0[128] = "";
    Components.Label ( "Name " );
    Components.SameLine();
	ImGui::InputText ( "  ", str0, IM_ARRAYSIZE ( str0 ) );

	if ( Components.Button("Create") && str0 != "" )
		Settings::CreateConfig(str0);

	if (Components.Button("Save") && Selected != -1)
	{
		Settings::SaveSettings(Settings::Configs[Selected]);
		CurrentConfig = Settings::Configs[Selected];
	}

	Components.SameLine();

	if (Components.Button("Load") && Selected != -1)
	{
		Settings::LoadSettings(Settings::Configs[Selected]);
		CurrentConfig = Settings::Configs[Selected];
	}

	if ( Components.Button("Refresh") )
		Settings::RefreshConfigList();

	if ( Components.Button("Reset") )
		Settings::ResetConfig();

	Components.SameLine();

	if (Components.Button("Load Game CFG"))
		g_EngineClient->ExecuteClientCmd(Settings::LoadGameCfg().data());

    Components.EndChild();
}

void Menu::RenderSpectatorList()
{
	if (Settings::Misc::SpectatorsEnabled)
		Misc::Get().SpectatorList();
}
