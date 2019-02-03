
#include "singleton.hpp"
#include "menu_helpers.hpp"

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

enum class RbotMenuAvailable : int
{
    STANDING,
    MOVING,
    AIR,
    MISC
};

enum class LbotSubsectionsAvailable : int
{
	Aimbot,
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
private:
    MenuHelper::components Components;
    //MenuFonts* fonts;
    //MenuFonts* Mfonts;
    bool Loaded = false;
};

