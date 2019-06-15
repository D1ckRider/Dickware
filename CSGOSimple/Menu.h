
#include "singleton.hpp"
#include "menu_helpers.hpp"
#include "Settings.h"

enum class MenuAvailable : int
{
    RAGEBOT,
    LEGITBOT,
    VISUALS,
    MISC,
	SKINCHANGER,
    SETTINGS
};

enum class VisualsMenuAvailable : int
{
    LOCAL,
    ENEMY,
    TEAM,
    MISC,
    GLOBAL
};

enum class RbotMenuAvailable : int
{
	GENERAL,
	WEAPONS
};

enum class RbotWeaponsAvailable : int
{
	PISTOL,
	DEAGLE,
	REVOLVER,
	SCOUT,
	AWP,
	AUTO,
	RIFLE,
	SHOTGUN
};

enum class RbotAntiAimAvailable : int
{
    STANDING,
    MOVING,
    AIR,
    MISC
};

enum class LbotMenuAvailable : int
{
	Aimbot,
	Backtrack,
	Triggerbot
};

enum class LbotWeaponsAvailable : int
{
    PISTOL,
	DEAGLE,
    SMG,
    MG,
    RIFLE,
    SHOTGUN,
	SCOUT,
    SNIPER
};

enum class HotkeyType : int
{
	Hold = 0,
	Tap = 1
};

#pragma once
class Menu : public Singleton<Menu>
{
public:
    Menu();
    void Initialize();
    void Render();

    /* sections */
    void RenderRagebot();
    void RenderLegitbot();
    void RenderVisuals();
    void RenderMisc();
	void RenderSkinchanger();
    void RenderSettings();
	void RenderSpectatorList();


	void RenderRageWeapon(Settings::WeaponType wtype);
	void RenderLegitWeapon(Settings::WeaponType wtype);
	void RenderRageHitboxes(std::string hitbox_name, Settings::HitboxType htype);
private:
    MenuHelper::components Components;
    //MenuFonts* fonts;
    //MenuFonts* Mfonts;
    bool Loaded = false;
};

