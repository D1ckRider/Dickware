#include "Menu.h"
#include "FontManager.h"
#include "ConfigSystem.h"
#include "ConsoleHelper.h"
#include "options.hpp"
#include "Misc.h"
#include "ui.hpp"
#include "features\Skinchanger.h"
#include "definitions.h"
#include "Media\Player.h"
#include "RuntimeSaver.h"
#include "ClantagChanger.h"


using WeaponType = Settings::WeaponType;
using HitboxType = Settings::HitboxType;
using AntiAimState = Settings::RageBot::AntiAimType;

const std::string Version = "190802.01d";
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

int GetRageWeaponType(C_BaseCombatWeapon* weapon)
{
	if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_DEAGLE)
		return (int)RbotWeaponsAvailable::DEAGLE;
	else if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		return (int)RbotWeaponsAvailable::REVOLVER;
	else if (weapon->IsRifle() || weapon->IsMachinegun())
		return (int)RbotWeaponsAvailable::RIFLE;
	else if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08)
		return (int)RbotWeaponsAvailable::SCOUT;
	else if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP)
		return (int)RbotWeaponsAvailable::AWP;
	else if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SCAR20 || weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_G3SG1)
		return (int)RbotWeaponsAvailable::AUTO;
	else if (weapon->IsPistol())
		return (int)RbotWeaponsAvailable::PISTOL;
	else
		return (int)RbotWeaponsAvailable::SHOTGUN;
}

int GetLegitWeaponType(C_BaseCombatWeapon* weapon)
{
	if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_DEAGLE)
		return (int)LbotWeaponsAvailable::DEAGLE;
	else if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08)
		return (int)LbotWeaponsAvailable::SCOUT;
	else if (weapon->IsSubmachinegun())
		return (int)LbotWeaponsAvailable::SMG;
	else if (weapon->IsMachinegun())
		return (int)LbotWeaponsAvailable::MG;
	else if (weapon->IsRifle())
		return (int)LbotWeaponsAvailable::RIFLE;
	else if (weapon->IsPistol())
		return (int)LbotWeaponsAvailable::PISTOL;
	else if (weapon->IsShotgun())
		return (int)LbotWeaponsAvailable::SHOTGUN;
	else if (weapon->IsSniper())
		return (int)LbotWeaponsAvailable::SNIPER;
	else
		return (int)LbotWeaponsAvailable::SHOTGUN;
}

void Menu::Render()
{
    if ( !Loaded || g_Unload )
        return;

	Components.StartWindow("DickWare", ImVec2(880, 570), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    static char* NavBarItems[] = { "s", "o", "t", "u", "4", "v" };
    static char* NavBarItemsText[] = { "Ragebot", "Legitbot", "Visuals", "Misc", "Skinchanger", "Settings" };
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
#ifdef _DEBUG
	Components.Label("DickWare Beta | Version: " + Version);
#else
	Components.Label("DickWare | Version: " + Version);
#endif
	Components.NextColumn();
	Components.Label("Current Config: " + CurrentConfig);
	Components.NextColumn();
	static bool open = false;
	//ImGui::PushItemWidth(-100.f);
	if (Components.Button("Unload"))
	{
		g_Unload = true;
		//player.Release();
	}

    Components.EndWindow();
}

void Menu::RenderRagebot()
{
    Components.BeginChild ( "#ragebot", ImVec2 ( 0, 500 ) );

	Components.Checkbox("Enable##rbot.enabled", Settings::RageBot::Enabled);

	static char* RageBotItems[] = { "General", "Weapons" };
	static int RageBotSelected = 0;
	
	const char* BaimModes[] = { "never", "auto" };
	const char* ShootingModes[] = { "normal", "in fakelag", "fakelag while shooting" };

	static const char* PitchAAs[] = { "none", "emotion", "down", "up", "zero", "jitter", "down jitter", "up jitter", "zero jitter", "spin", "up spin", "down spin", "random", "switch", "down switch", "up switch", "fake up", "fake down", "custom" };
	static const char* YawAAs[] = { "none", "backwards", "spinbot", "lower body yaw", "random", "freestanding", "custom" };
	static const char* YawAddAAs[] = { "none", "jitter", "switch", "spin", "random" };
#ifdef DEBUG
	static const char* DesyncAA[] = { "none", "static", "balance", "stretch", "jitter" };
#else
	static const char* DesyncAA[] = { "none", "static", "balance" };
#endif // DEBUG

	
	static char* AntiAimMenus[] = { "stand", "move", "air", "misc" };
	static const char* LagCompMode[] = { "Best", "Newest", "All" };

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
		ImGui::SliderFloat("##rbot.slowalk_mod", &Settings::RageBot::SlowWalkMod, 0, 1, "Speed: %.1f");
		Components.Checkbox("Fake Duck", Settings::RageBot::FakeDuck);
		Components.Hotkey("Fake Duck Hotkey", Settings::RageBot::FakeDuckHotkey);
		Components.Checkbox("Auto Scope", Settings::RageBot::AutoScope);
		Components.Checkbox("Auto Stop", Settings::RageBot::AutoStop);
		Components.Checkbox("Auto Crouch", Settings::RageBot::AutoCrouch);

#ifdef _DEBUG
		Components.Checkbox("Aim Step", Settings::RageBot::AimStepEnabled);
		Components.SliderInt("Aim Step Value", Settings::RageBot::AimStepValue, 0, 20);
#endif // _DEBUG

		Components.Checkbox("Fakelag prediction", Settings::RageBot::FakelagPrediction);
		//Components.ComboBox("Shooting Mode", ShootingModes, IM_ARRAYSIZE(ShootingModes), Settings::RageBot::ShootingMode);
		Components.Checkbox("Resolver (WIP)", Settings::RageBot::Resolver);
		Components.Checkbox("Lag compensation (WIP)", Settings::RageBot::LagComp);
		if (Settings::RageBot::LagComp)
			ImGui::Combo("Lag compensation type", &Settings::RageBot::LagCompType, LagCompMode, IM_ARRAYSIZE(LagCompMode));
#ifdef _DEBUG

		Components.Checkbox("Force unlag", Settings::RageBot::ForceUnlag);
#endif // _DEBUG

		Components.NextColumn();

		Components.Checkbox("Enable AA", Settings::RageBot::EnabledAA);
		Components.Spacing();

		Components.Navbar(AntiAimMenus, IM_ARRAYSIZE(AntiAimMenus), AAMenuSelected);
		switch ( (RbotAntiAimAvailable)AAMenuSelected )
		{
		case RbotAntiAimAvailable::STANDING:
			Components.ComboBox("Pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].Pitch);
			Components.ComboBox("Yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].Yaw);
			Components.ComboBox("Yaw Add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].YawAdd);
			ImGui::SliderFloat("##aa.range", &Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].Range, 0.f, 360.f, "Range: %.2f");
			break;
		case RbotAntiAimAvailable::MOVING:
			Components.ComboBox("Pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].Pitch);
			Components.ComboBox("Yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].Yaw);
			Components.ComboBox("Yaw Add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].YawAdd);
			ImGui::SliderFloat("##aa.range", &Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].Range, 0.f, 360.f, "Range: %.2f");
			break;

		case RbotAntiAimAvailable::AIR:
			Components.ComboBox("Pitch AA", PitchAAs, IM_ARRAYSIZE(PitchAAs), Settings::RageBot::AntiAimSettings[AntiAimState::AIR].Pitch);
			Components.ComboBox("Yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), Settings::RageBot::AntiAimSettings[AntiAimState::AIR].Yaw);
			Components.ComboBox("Yaw Add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), Settings::RageBot::AntiAimSettings[AntiAimState::AIR].YawAdd);
			ImGui::SliderFloat("##aa.range", &Settings::RageBot::AntiAimSettings[AntiAimState::AIR].Range, 0.f, 360.f, "Range: %.2f");
			break;

		case RbotAntiAimAvailable::MISC:

			Components.SliderFloat("Spinbot speed", Settings::RageBot::SpinBotSpeed, -20.f, 20.f);
			Components.Checkbox("SlideWalk", Settings::RageBot::SlideWalk);
			Components.ComboBox("Desync AntiAim", DesyncAA, IM_ARRAYSIZE(DesyncAA), Settings::RageBot::DesyncType);
#ifdef _DEBUG
			Components.Hotkey("Desync Hotkey", Settings::RageBot::DesyncFlipHotkey, (int)HotkeyType::Tap);
#endif
			Components.Spacing();
			Components.Label("Manual AA Binds: ");
			Components.Hotkey("Right", Settings::RageBot::ManualAARightKey, (int)HotkeyType::Tap);
			Components.Hotkey("Left", Settings::RageBot::ManualAALeftKey, (int)HotkeyType::Tap);
			Components.Hotkey("Back", Settings::RageBot::ManualAABackKey, (int)HotkeyType::Tap);
			break;
		}
		break;
	case RbotMenuAvailable::WEAPONS:

		Components.Columns(2, false);

		

		Components.NavbarIcons(WeaponConfigSelectionItems, WeaponConfigSelectionItemsText, IM_ARRAYSIZE(WeaponConfigSelectionItems), WeaponSelected, IconsFont);
		Components.BeginChild("#weapons", ImVec2(0.f, 180.f));
		switch ((RbotWeaponsAvailable)WeaponSelected)
		{
		case RbotWeaponsAvailable::PISTOL:
			RenderRageWeapon(WeaponType::WEAPON_PISTOL);
			break;
		case RbotWeaponsAvailable::DEAGLE:
			RenderRageWeapon(WeaponType::WEAPON_DEAGLE);
			break;
		case RbotWeaponsAvailable::REVOLVER:
			RenderRageWeapon(WeaponType::WEAPON_REVOLVER);
			break;
		case RbotWeaponsAvailable::SCOUT:
			RenderRageWeapon(WeaponType::WEAPON_SSG08);
			break;
		case RbotWeaponsAvailable::AWP:
			RenderRageWeapon(WeaponType::WEAPON_AWP);
			break;
		case RbotWeaponsAvailable::AUTO:
			RenderRageWeapon(WeaponType::WEAPON_AUTO);
			break;
		case RbotWeaponsAvailable::RIFLE:
			RenderRageWeapon(WeaponType::WEAPON_RIFLE);
			break;
		case RbotWeaponsAvailable::SHOTGUN:
			RenderRageWeapon(WeaponType::WEAPON_SHOTGUN);
			break;
		}
		Components.EndChild();
		if (ImGui::Button("Current Weapon") && g_Saver.CurrentWeaponRef)
			WeaponSelected = GetRageWeaponType(g_Saver.CurrentWeaponRef);

		Components.NextColumn();
#pragma region Hitbox Scales
		Components.Label("Hitbox Scales");
		Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));

		RenderRageHitboxes("Head", HitboxType::HEAD);
		RenderRageHitboxes("Neck", HitboxType::NECK);
		RenderRageHitboxes("Chest", HitboxType::CHEST);
		RenderRageHitboxes("Pelvis", HitboxType::PELVIS);
		RenderRageHitboxes("Stomach", HitboxType::STOMACH);
		RenderRageHitboxes("Arm", HitboxType::ARM);
		RenderRageHitboxes("Leg", HitboxType::LEG);
		RenderRageHitboxes("Foot", HitboxType::FOOT);

		Components.EndChild();
#pragma endregion
		break;
	}
	
	
    Components.EndChild();
}

void Menu::RenderRageWeapon(Settings::WeaponType wtype)
{
	ImGui::SliderFloat("##rbot.hitchance", &Settings::RageBot::WeaponSettings[wtype].Hitchance, 0, 100, "Hitchance: %.1f");
	ImGui::SliderFloat("##rbot.mindmg", &Settings::RageBot::WeaponSettings[wtype].MinDamage, 0.f, 100.f, "Minimal Damage: %.2f");
	ImGui::SliderInt("##rbot.baim_after_x", &Settings::RageBot::WeaponSettings[wtype].BAimAfterShots, 0, 10, "BAim After %.0f Shots");
	ImGui::SliderInt("##rbot.force_baim_after_x", &Settings::RageBot::WeaponSettings[wtype].ForceBAimAfterShots, 0, 10, "Force BAim After %.0f Shots");
	ImGui::Checkbox("BAim while moving", &Settings::RageBot::WeaponSettings[wtype].BAimWhileMoving);
}

void Menu::RenderRageHitboxes(std::string hitbox_name, Settings::HitboxType htype)
{
	ImGui::Checkbox(hitbox_name.data(), &Settings::RageBot::Hitboxes[htype].Enabled);
	ImGui::SameLine();
	ImGui::SameLine();
	std::string id = "##rbot.scale_" + std::to_string(htype);
	ImGui::SliderFloat(id.data(), &Settings::RageBot::Hitboxes[htype].Scale, 0.f, 1.f, "Scale: %.2f");
}

void Menu::RenderLegitWeapon(WeaponType wtype)
{
	static char* rcs_types[] = { "Standalone", "Aim" };
	static char* aim_types[] = { "Hitbox", "Nearest" };
	static char* smooth_types[] = { "Static", "Dynamic" };
	static char* fov_types[] = { "Static", "Dynamic" };
	static char* hitbox_list[] = { "Head", "Neck", "Pelvis", "Stomach", "Lower chest", "Chest", "Upper chest" };

	ImGui::Combo("Aim Type", &Settings::Aimbot::WeaponAimSetting[wtype].AimType, aim_types, IM_ARRAYSIZE(aim_types));

	if (Settings::Aimbot::WeaponAimSetting[wtype].AimType == 0)
		ImGui::Combo("Hitbox", &Settings::Aimbot::WeaponAimSetting[wtype].Hitbox, hitbox_list, IM_ARRAYSIZE(hitbox_list));

	ImGui::SliderFloat("##aimbot.fov", &Settings::Aimbot::WeaponAimSetting[wtype].FOV, 0, 30, "Fov: %.2f");
	ImGui::Combo("Smooth Type", &Settings::Aimbot::WeaponAimSetting[wtype].SmoothType, smooth_types, IM_ARRAYSIZE(smooth_types));
	ImGui::SliderFloat("##aimbot.smooth", &Settings::Aimbot::WeaponAimSetting[wtype].Smooth, 1, 20, "Smooth: %.2f");
	Components.Checkbox("Silent Aim", Settings::Aimbot::WeaponAimSetting[wtype].Silent);
	if (Settings::Aimbot::WeaponAimSetting[wtype].Silent)
		ImGui::SliderFloat("##aimbot.silent_fov", &Settings::Aimbot::WeaponAimSetting[wtype].SilentFOV, 0, 20, "Silent Fov: %.2f");
	ImGui::SliderInt("##aimbot.kill_delay", &Settings::Aimbot::WeaponAimSetting[wtype].KillDelay, 0, 1000, "Kill Delay: %.0f");

	if (!Settings::Aimbot::WeaponAimSetting[wtype].Silent)
		ImGui::SliderInt("##aimbot.shot_delay", &Settings::Aimbot::WeaponAimSetting[wtype].Delay, 0, 100, "Shot Delay: %.0f");
	
	if(wtype == WeaponType::WEAPON_PISTOL)
		Components.Checkbox("Auto Pistol", Settings::Aimbot::WeaponAimSetting[wtype].Autopistol);

	Components.Spacing();

	Components.NextColumn();
	if (wtype != WeaponType::WEAPON_DEAGLE && wtype != WeaponType::WEAPON_SHOTGUN && wtype != WeaponType::WEAPON_SNIPER && wtype != WeaponType::WEAPON_SSG08)
	{
		Components.Checkbox("RCS", Settings::Aimbot::WeaponAimSetting[wtype].RCS);
		if (Settings::Aimbot::WeaponAimSetting[wtype].RCS)
		{
			ImGui::Combo("RCS Type", &Settings::Aimbot::WeaponAimSetting[wtype].RCSType, rcs_types, IM_ARRAYSIZE(rcs_types));
			ImGui::SliderInt("##aimbot.rcs_x", &Settings::Aimbot::WeaponAimSetting[wtype].RCS_X, 0, 100, "RCS X: %.0f");
			ImGui::SliderInt("##aimbot.rcs_y", &Settings::Aimbot::WeaponAimSetting[wtype].RCS_Y, 0, 100, "RCS Y: %.0f");
			ImGui::SliderInt("##aimbot.rcs_start", &Settings::Aimbot::WeaponAimSetting[wtype].RCS_Bullet, 1, 30, "RCS Start: %.0f");
			Components.Checkbox("RCS FOV", Settings::Aimbot::WeaponAimSetting[wtype].RCS_FOVEnabled);
			if (Settings::Aimbot::WeaponAimSetting[wtype].RCS_FOVEnabled)
				ImGui::SliderFloat("##aimbot.rcs_fov", &Settings::Aimbot::WeaponAimSetting[wtype].RCS_FOV, 0, 20, "RCS Fov: %.2f");
			Components.Checkbox("RCS Smooth", Settings::Aimbot::WeaponAimSetting[wtype].RCS_SmoothEnabled);
			if (Settings::Aimbot::WeaponAimSetting[wtype].RCS_SmoothEnabled)
				ImGui::SliderFloat("##aimbot.rcs_smooth", &Settings::Aimbot::WeaponAimSetting[wtype].RCS_Smooth, 1, 15, "RCS Smooth: %.2f");
		}
	}
#ifdef DEBUG
	ImGui::Checkbox("Auto Fire", &Settings::Aimbot::WeaponAimSetting[wtype].Autofire);
#endif // DEBUG
	ImGui::Checkbox("Auto Wall", &Settings::Aimbot::WeaponAimSetting[wtype].AutowallEnabled);
	if (Settings::Aimbot::WeaponAimSetting[wtype].AutowallEnabled)
		ImGui::SliderInt("##aimbot.min_damage", &Settings::Aimbot::WeaponAimSetting[wtype].MinDamage, 1, 100, "Min Damage: %.0f");

}

void Menu::RenderLegitbot()
{
    Components.BeginChild ( "#lbot", ImVec2 ( 0, 500) );
	
	static char*	LBotMenuItems[] = { "AimBot", "Misc", "Triggerbot" };
	static int		LBotSelected = 0;
    static char*	WeaponConfigSelectionItems[] = { "G", "A", "L", "f", "W", "c", "a", "Z" };
    static char*	WeaponConfigSelectionItemsText[] = { "Pistol", "Deagle", "SMG", "MG", "Rifle", "Shotgun", "Scout", "Sniper" };
    static int		WeaponSelected = 0;
	static char*	priorities[] =  { "Fov", "Health", "Damage", "Distance" };

	Components.Navbar(LBotMenuItems, IM_ARRAYSIZE(LBotMenuItems), LBotSelected);
	switch ((LbotMenuAvailable)LBotSelected)
	{
	case LbotMenuAvailable::Aimbot:
		Components.Checkbox("Enable", Settings::Aimbot::Enabled);
		ImGui::SameLine();
		if (ImGui::Button("Current Weapon") && g_Saver.CurrentWeaponRef)
			WeaponSelected = GetLegitWeaponType(g_Saver.CurrentWeaponRef);
		Components.NavbarIcons(WeaponConfigSelectionItems, WeaponConfigSelectionItemsText, IM_ARRAYSIZE(WeaponConfigSelectionItems), WeaponSelected, IconsFont);
		Components.BeginChild("#lbot.weapons", ImVec2(0, 300));
		Components.Columns(2, false);

		Components.Hotkey("Aimkey", Settings::Aimbot::Hotkey);
		
		switch ((LbotWeaponsAvailable)WeaponSelected)
		{
		case LbotWeaponsAvailable::PISTOL:
			RenderLegitWeapon(WeaponType::WEAPON_PISTOL);
			break;

		case LbotWeaponsAvailable::DEAGLE:
			RenderLegitWeapon(WeaponType::WEAPON_DEAGLE);
			break;
		case LbotWeaponsAvailable::SMG:
			RenderLegitWeapon(WeaponType::WEAPON_SMG);
			break;

		case LbotWeaponsAvailable::MG:
			RenderLegitWeapon(WeaponType::WEAPON_MACHINEGUN);
			break;

		case LbotWeaponsAvailable::RIFLE:
			RenderLegitWeapon(WeaponType::WEAPON_RIFLE);
			break;

		case LbotWeaponsAvailable::SHOTGUN:
			RenderLegitWeapon(WeaponType::WEAPON_SHOTGUN);
			break;
		case LbotWeaponsAvailable::SCOUT:
			RenderLegitWeapon(WeaponType::WEAPON_SSG08);
			break;
		case LbotWeaponsAvailable::SNIPER:
			RenderLegitWeapon(WeaponType::WEAPON_SNIPER);
			break;
		}
		Components.EndChild();
		
		Components.Columns(2, false);
		ImGui::Combo("Priority", &Settings::Aimbot::Priority, priorities, IM_ARRAYSIZE(priorities));
		Components.Checkbox("Draw FOV", Settings::Aimbot::DrawFov);
		Components.Checkbox("Smoke Check", Settings::Aimbot::SmokeCheck);
		Components.Checkbox("Flash Check", Settings::Aimbot::FlashCheck);
		Components.NextColumn();
		Components.Checkbox("Jump Check", Settings::Aimbot::JumpCheck);
		Components.Checkbox("Deathmatch Mode", Settings::Aimbot::DeathmatchMode);
#ifdef DEBUG
		Components.Hotkey("AutoFire hotkey", Settings::Aimbot::AutofireHotkey);
#endif // DEBUG
		break;
	case LbotMenuAvailable::Backtrack:
		Components.Columns(2, false);
		ImGui::Checkbox("Backtrack", &Settings::Aimbot::Backtrack);
		Components.Checkbox("Aim at backtrack", Settings::Aimbot::BacktrackAtAim);
		Components.SliderFloat("Backtrack time", Settings::Aimbot::BacktrackTick, 0.f, .2f);
#ifdef DEBUG
		ImGui::Combo("Desync", &Settings::Aimbot::LegitAA, std::vector<std::string>{ "Off", "Static", "Balance" });
		if (Settings::Aimbot::LegitAA)
			Components.Hotkey("Desync Flip Key", Settings::Aimbot::AAFlipHotkey);
#endif // DEBUG
		break;
	case LbotMenuAvailable::Triggerbot:
		Components.Columns(2, false);
		Components.Label("Triggerbot");
		Components.Checkbox("Enabled", Settings::TriggerBot::Enabled);
		Components.Hotkey("Key", Settings::TriggerBot::Key);
		break;
	}
    Components.EndChild();
}

void Menu::RenderVisuals()
{
    Components.BeginChild ( "#visuals", ImVec2 ( 0, 500) );

	static char* VisualsCategories[] = { "Local", "Enemy", "Team", "Misc", "Globals" };
    static int SelectedMenu = 0;
    Components.Navbar ( VisualsCategories, IM_ARRAYSIZE ( VisualsCategories ), SelectedMenu );

    static const char* ChamsTypes[] = { "normal", "flat", "wireframe", "glass", "metallic",  "xqz", "metallic xqz", "flat xqz" };
	static const char* GlowTypes[] = { "outline outer", "cover", "outline inner" };
    static const char* BoxTypes[] = { "normal", "edge" };

	static const char* RadarTypes[] = { "none", "in-game", "external" };

    switch ( ( VisualsMenuAvailable ) SelectedMenu )
    {
        case VisualsMenuAvailable::LOCAL:
        {
			Components.Columns(3, false);
			Components.Label("Chams");
			Components.BeginChild("##esp.local.chams", ImVec2(0, 250.f));
			Components.ColorCheckbox2("Enable", Settings::Visual::LocalChams.Enabled, Settings::Visual::LocalChams.Visible, Settings::Visual::LocalChams.Invisible);
			Components.ComboBox("Chams Type", ChamsTypes, IM_ARRAYSIZE(ChamsTypes), Settings::Visual::LocalChams.Mode);
			//Components.ColorCheckbox("Ghost Chams", Settings::Visual::GhostEnabled, Settings::Visual::GhostColor);
			Components.EndChild();
			Components.NextColumn();

			Components.Label("ESP");
			Components.BeginChild("##esp.local.esp", ImVec2(0, 400.f));
			Components.Checkbox("Enable ",  Settings::Visual::LocalESP.Enabled);
			Components.ColorCheckbox("Box", Settings::Visual::LocalESP.BoxEnabled, Settings::Visual::LocalESP.BoxColor);
			Components.ComboBox("Box Type", BoxTypes, IM_ARRAYSIZE(BoxTypes), Settings::Visual::LocalESP.BoxType);
			Components.ColorCheckbox("Name", Settings::Visual::LocalESP.NameEnabled, Settings::Visual::LocalESP.NameColor);
			Components.Checkbox("Health",  Settings::Visual::LocalESP.HealthEnabled);
			Components.ColorCheckbox("Armor", Settings::Visual::LocalESP.ArmorEnabled, Settings::Visual::LocalESP.ArmorColor);
			Components.ColorCheckbox("Weapon", Settings::Visual::LocalESP.WeaponEnabled, Settings::Visual::LocalESP.WeaponColor);
			Components.EndChild();
            break;
        }

        case VisualsMenuAvailable::ENEMY:
        {
			Components.Columns(3, false);
			Components.Label("Chams");
			Components.BeginChild("##esp.enemy.chams", ImVec2(0, 250.f));
			Components.ColorCheckbox2("Enable", Settings::Visual::EnemyChams.Enabled, Settings::Visual::EnemyChams.Visible, Settings::Visual::EnemyChams.Invisible);
			Components.ComboBox("Type", ChamsTypes, IM_ARRAYSIZE(ChamsTypes), Settings::Visual::EnemyChams.Mode);
			Components.Label("Glow");
			Components.ColorCheckbox("Enabled ", Settings::Visual::EnemyGlow.Enabled, Settings::Visual::EnemyGlow.Visible);
			Components.ComboBox("Type ", GlowTypes, IM_ARRAYSIZE(GlowTypes), Settings::Visual::EnemyGlow.Type);
			Components.EndChild();
			Components.NextColumn();

			Components.Label("ESP");
			Components.BeginChild("##esp.enemy.esp", ImVec2(0, 400.f));
			Components.Checkbox("Enable ",  Settings::Visual::EnemyESP.Enabled);
			Components.ColorCheckbox("Box", Settings::Visual::EnemyESP.BoxEnabled, Settings::Visual::EnemyESP.BoxColor);
			Components.ComboBox("Box Type", BoxTypes, IM_ARRAYSIZE(BoxTypes), Settings::Visual::EnemyESP.BoxType);
			Components.ColorCheckbox("Name", Settings::Visual::EnemyESP.NameEnabled, Settings::Visual::EnemyESP.NameColor);
			Components.Checkbox("Health",  Settings::Visual::EnemyESP.HealthEnabled);
			Components.ColorCheckbox("Armor", Settings::Visual::EnemyESP.ArmorEnabled, Settings::Visual::EnemyESP.ArmorColor);
			Components.ColorCheckbox("Weapon", Settings::Visual::EnemyESP.WeaponEnabled, Settings::Visual::EnemyESP.WeaponColor);
			Components.ColorCheckbox("Snapline", Settings::Visual::EnemyESP.SnaplineEnabled, Settings::Visual::EnemyESP.SnaplineColor);
			Components.ColorCheckbox("Offscreen ESP", Settings::Visual::OffscreenESPEnabled, Settings::Visual::OffscreenESPColor);
#ifdef _DEBUG
			Components.Checkbox("Debug overlay info", Settings::Visual::DebugInfoEnabled);
#endif // _DEBUG
			Components.EndChild();
			break;
        }

        case VisualsMenuAvailable::TEAM:
        {
			Components.Columns(3, false);
			Components.Label("Chams");
			Components.BeginChild("##esp.team.chams", ImVec2(0, 250.f));
			Components.ColorCheckbox2("Enable", Settings::Visual::TeamChams.Enabled, Settings::Visual::TeamChams.Visible, Settings::Visual::TeamChams.Invisible);
			Components.ComboBox("Type", ChamsTypes, IM_ARRAYSIZE(ChamsTypes), Settings::Visual::TeamChams.Mode);
			Components.Label("Glow");
			Components.ColorCheckbox("Enabled ", Settings::Visual::TeamGlow.Enabled, Settings::Visual::TeamGlow.Visible);
			Components.ComboBox("Type ", GlowTypes, IM_ARRAYSIZE(GlowTypes), Settings::Visual::TeamGlow.Type);
			Components.EndChild();
			Components.NextColumn();

			Components.Label("ESP");
			Components.BeginChild("##esp.team.esp", ImVec2(0, 400.f));
			Components.Checkbox("Enable ", Settings::Visual::TeamESP.Enabled);
			Components.ColorCheckbox("Box", Settings::Visual::TeamESP.BoxEnabled, Settings::Visual::TeamESP.BoxColor);
			Components.ComboBox("Box Type", BoxTypes, IM_ARRAYSIZE(BoxTypes), Settings::Visual::TeamESP.BoxType);
			Components.ColorCheckbox("Name", Settings::Visual::TeamESP.NameEnabled, Settings::Visual::TeamESP.NameColor);
			Components.Checkbox("Health",  Settings::Visual::TeamESP.HealthEnabled);
			Components.ColorCheckbox("Armor", Settings::Visual::TeamESP.ArmorEnabled, Settings::Visual::TeamESP.ArmorColor);
			Components.ColorCheckbox("Weapon", Settings::Visual::TeamESP.WeaponEnabled, Settings::Visual::TeamESP.WeaponColor);
			Components.ColorCheckbox("Snapline", Settings::Visual::TeamESP.SnaplineEnabled, Settings::Visual::TeamESP.SnaplineColor);
			Components.EndChild();
			break;
        }

        case VisualsMenuAvailable::MISC:
        {
            Components.Columns ( 3, false );
            Components.Label ( "Thirdperson " );
			Components.BeginChild("##vis.misc.tp", ImVec2(0, 60.f));
			Components.Checkbox("Enabled ",  Settings::Visual::ThirdPersonEnabled);
			Components.Hotkey("Hotkey: ",  Settings::Visual::ThirdPersonHotkey, (int)HotkeyType::Tap);
			Components.EndChild();
			Components.NextColumn();

            Components.Label ( "ESP" );
			Components.BeginChild("#vis.misc.esp", ImVec2(0, 400.f));
			Components.Checkbox("Enable ",  Settings::Visual::GlobalESP.Enabled);
			Components.Checkbox("Grenade",  Settings::Visual::GlobalESP.GrenadeEnabled);
			Components.ColorCheckbox("Bomb", Settings::Visual::GlobalESP.BombEnabled, Settings::Visual::GlobalESP.BombColor);
			Components.Checkbox("Droped Weapons", Settings::Visual::GlobalESP.DropedWeaponsEnabled);
			Components.Checkbox("DangerZone item ESP",  Settings::Visual::GlobalESP.DZEnabled);
			ImGui::SliderFloat("##misc.esp.dz_range", &Settings::Visual::GlobalESP.DZRange, 0, 1000.f, "DangerZone ESP Range: %3.f");
			Components.ComboBox("Radar type", RadarTypes, IM_ARRAYSIZE(RadarTypes), Settings::Visual::GlobalESP.RadarType);
			ImGui::SliderFloat("##misc.esp.radar_range", &Settings::Visual::GlobalESP.RadarRange, 200, 1500, "Radar Range: %.2f");
			Components.Checkbox("Sound ESP", Settings::Visual::GlobalESP.SoundESPEnabled);
			Components.EndChild();

			Components.NextColumn();

            Components.Label ( "Misc" );
			Components.BeginChild("##vis.misc.misc", ImVec2(0, 400.f));
			Components.Checkbox("No Scope Overlay",  Settings::Visual::NoScopeOverlay);
			Components.Checkbox("Bullet Tracers",  Settings::Visual::BulletTracers);
			Components.Checkbox("No Flash",  Settings::Visual::NoFlash);
			Components.Checkbox("No Smoke",  Settings::Visual::NoSmoke);
			Components.Checkbox("Disable Post-processing", Settings::Visual::DisablePP);
			if(!Settings::Visual::DisablePP)
				ImGui::Checkbox("Night Mode", &Settings::Visual::NightMode);
			Components.ColorCheckbox("Nade Trajectory", Settings::Visual::NadeTracerEnabled, Settings::Visual::NadeTracerColor);
			Components.ColorCheckbox("Spread Circle", Settings::Visual::SpreadCircleEnabled, Settings::Visual::SpreadCircleColor);
			Components.ColorCheckbox("Damage Indicators", Settings::Visual::DamageIndicator, Settings::Visual::DamageIndicatorColor);
			ImGui::Checkbox("Sniper Crosshair", &Settings::Visual::SniperCrosshair);
			Components.Checkbox("Disable Sniper Zoom",  Settings::Visual::DisableScopeZoom);
			ImGui::SliderInt("##esp.misc.viewfov", &Settings::Visual::ViewModelFOV, 60, 150, "Viewmodel FOV: %.0f");
			ImGui::SliderInt("##esp.misc.fov", &Settings::Visual::FOV, 60, 150, "FOV: %.0f");
			Components.Checkbox("Hitmarker",  Settings::Visual::Hitmarker);
			Components.Checkbox("Hitmarker Sound", Settings::Visual::HitmarkerSound);
			
#ifdef _DEBUG
			Components.SliderInt("Ragdoll Force", Settings::Visual::RagdollForce, 0, 10);
#endif // _DEBUG
			Components.EndChild();
            break;
        }

        case VisualsMenuAvailable::GLOBAL:
        {
            Components.Columns ( 3, false );
            static const char* ItemPositions[] = { "top", "right", "bottom", "left" };
			Components.Label("Positioning");;
			Components.BeginChild("##vis.global.pos", ImVec2(0, 60.f));
			Components.ComboBox("Health Position", ItemPositions, IM_ARRAYSIZE(ItemPositions), Settings::Visual::HealthPos);
			Components.ComboBox("Armor Position", ItemPositions, IM_ARRAYSIZE(ItemPositions), Settings::Visual::ArmorPos);
			Components.EndChild();
            break;
        }
    }

	Components.EndChild();
}

void Menu::RenderMisc()
{
    Components.BeginChild ( "#misc", ImVec2 ( 0, 500 ) );

	Components.Columns(2, false);

    //Components.Checkbox("No hands", "misc_no_hands");
	Components.Label("General");
	Components.BeginChild("##misc.general", ImVec2(0, 260.f));

	Components.Checkbox("Watermark", Settings::Misc::WatermarkEnabled);
	Components.Checkbox("Event Log", Settings::Misc::EventLogEnabled);

	Components.Checkbox("BHop",  Settings::Misc::BHop);
	Components.Checkbox("Autostrafe",  Settings::Misc::AutoStrafe);
	/*if(Settings::Misc::AutoStrafe)
		ImGui::SliderFloat("Retrack speed", &Settings::Misc::Retrack, 2.0f, 8.0f, "%.1f");*/

    Components.Spacing();

    Components.Spacing();

    //Components.Checkbox("Anti untrusted", "misc_antiuntrusted");*/

	Components.Checkbox("Rank reveal",  Settings::Misc::RankReveal);
	Components.Checkbox("No Visual Recoil", Settings::Misc::NoVisualRecoil);
	Components.Checkbox("AutoAccept", Settings::Misc::AutoAccept);

	static const char* Clantags[]{ "None", "Static", "Dynamic", "Custom Static", "Custom Slide"};
	static char str0[128] = "";
	Components.ComboBox("Clantag Changer", Clantags, IM_ARRAYSIZE(Clantags), Settings::Misc::ClantagType);
	if (Settings::Misc::ClantagType == Settings::Misc::CustomStatic)
	{
		ImGui::InputText(" ", str0, IM_ARRAYSIZE(str0));
		Components.SameLine();
		if (ImGui::Button("Apply"))
			Utils::SetClantag(str0);
	}
	else if (Settings::Misc::ClantagType == Settings::Misc::CustomSlide)
	{
		ImGui::InputText(" ", str0, IM_ARRAYSIZE(str0));
		Components.SameLine();
		if (ImGui::Button("Apply"))
			ClantagChanger::Get().SetCustomSlide(str0);
	}

	Components.Checkbox("Spectator List", Settings::Misc::SpectatorsEnabled);
	Components.EndChild();
	Components.Label("Buy Bot");
    //buybot
	Components.BeginChild("##misc.buybot", ImVec2(0, 165.f));
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
	Components.EndChild();

	Components.NextColumn();

	Components.Label("Fakelags");
	Components.BeginChild("##misc.fakelags", ImVec2(0, 90.f));
	static char* FakelagMenu[] = { "stand", "move", "air" };

	static const char* FakelagModesMove[] = { "normal", "adaptive", "on peek" };
	static const char* FakelagModesAir[] = { "normal", "adaptive" };
	static int FakelagSelected = 0;

	Components.Navbar(FakelagMenu, IM_ARRAYSIZE(FakelagMenu), FakelagSelected);

	switch (FakelagSelected)
	{
	case 0:
		ImGui::SliderInt("##misc.fakelag_ticks", &Settings::RageBot::AntiAimSettings[AntiAimState::STANDING].FakelagTicks, 0, 14, "Ticks: %.0f");
		break;
	case 1:
		Components.ComboBox("Fakelag mode", FakelagModesMove, IM_ARRAYSIZE(FakelagModesMove), Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].FakelagMode);
		ImGui::SliderInt("##misc.fakelag_ticks", &Settings::RageBot::AntiAimSettings[AntiAimState::MOVING].FakelagTicks, 0, 14, "Ticks: %.0f");
		break;
	case 2:
		Components.ComboBox("Fakelag mode", FakelagModesAir, IM_ARRAYSIZE(FakelagModesAir), Settings::RageBot::AntiAimSettings[AntiAimState::AIR].FakelagMode);
		ImGui::SliderInt("##misc.fakelag_ticks", &Settings::RageBot::AntiAimSettings[AntiAimState::AIR].FakelagTicks, 0, 14, "Ticks: %.0f");
		break;
	}
	Components.EndChild();

	// Radio feature
	Components.Checkbox("Radio", Settings::Misc::RadioEnabled);
	
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
		Components.BeginChild("##misc.radio", ImVec2(0, 60.f));

		if (ImGui::Combo("##Station", &Settings::Misc::RadioSelected, radio_name))
		{
			player.PlayStreamFromURL(stations[Settings::Misc::RadioSelected].data());
		}

		char* name;

		if (!g_Saver.RadioPaused)
			name = "||";
		else
			name = ">";

		//Components.Hotkey("Pause Hotkey", Settings::Misc::RadioPauseHotkey, (int)HotkeyType::Tap);

		if (ImGui::Button(name, ImVec2(132, 20)))
		{
			g_Saver.RadioPaused = !g_Saver.RadioPaused;
			//player.PausePlayer();
		}
		Components.SameLine();
		ImGui::SliderInt("##radio.volume", &Settings::Misc::RadioVolume, 0, 100, "Volume: %d %");
		//Components.SliderInt("Volume", Settings::Misc::RadioVolume, 0, 100);
		Components.EndChild();
	}
	else
	{
		if (radioInit)
			player.PausePlayer();
	}

    Components.EndChild();
}

void Menu::RenderSkinchanger()
 {
	Components.BeginChild("#skinchanger", ImVec2(0, 500));

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
    Components.BeginChild ( "#settings", ImVec2 ( 0, 500) );

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
		/*if (ImGui::BeginPopup("WARNING", ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you want to rewrite config?");
			if (ImGui::Button("Yes"))
			{
				Settings::SaveSettings(Settings::Configs[Selected]);
				CurrentConfig = Settings::Configs[Selected];
			}
			if (ImGui::Button("No"))
				ImGui::EndPopup();
		}*/
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
