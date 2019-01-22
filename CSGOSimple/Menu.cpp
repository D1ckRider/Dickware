
#include "Menu.h"
#include "FontManager.h"
#include "ConfigSystem.h"
#include "ConsoleHelper.h"
#include "options.hpp"
#include "Misc.h"
#include "Settings.h"

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

    Components.StartWindow ( "DickWare", ImVec2 ( 830, 500 ), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize );
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

    Components.EndWindow();
}

void Menu::RenderRagebot()
{
    Components.BeginChild ( "#ragebot", ImVec2 ( 0, 0 ) );
    Components.Columns ( 2, false );

    Components.Label ( "Ragebot" );
    Components.Checkbox ( "Enable", "rbot" );


	static char* WeaponConfigSelectionItems[] = { "G", "A", "J", "a", "Z", "Y", "W", "c" };
	static char* WeaponConfigSelectionItemsText[] = { "Pistol", "Deagle", "R8", "Scout", "AWP", "Auto", "Rifle", "Shotgun" };
	static int WeaponSelected = 0;
	Components.NavbarIcons(WeaponConfigSelectionItems, WeaponConfigSelectionItemsText, IM_ARRAYSIZE(WeaponConfigSelectionItems), WeaponSelected, IconsFont);


	switch ((RbotWeaponsAvailable)WeaponSelected)
	{
		case RbotWeaponsAvailable::PISTOL:
			Components.SliderFloat("Hitchance", "rbot_pistol_min_hitchance", 0.f, 100.f);
			Components.SliderFloat("Min damage", "rbot_pistol_mindamage", 0.f, 100.f);
			Components.SliderInt("Baim after x shots", "rbot_pistol_baim_after_shots", 0, 10);
			Components.SliderInt("Force baim after x shots", "rbot_pistol_force_baim_after_shots", 0, 10);
			Components.Checkbox("Baim while moving", "rbot_pistol_baim_while_moving");
			
			break;
		case RbotWeaponsAvailable::DEAGLE:
			Components.SliderFloat("Hitchance", "rbot_deagle_min_hitchance", 0.f, 100.f);
			Components.SliderFloat("Min damage", "rbot_deagle_mindamage", 0.f, 100.f);
			Components.SliderInt("Baim after x shots", "rbot_deagle_baim_after_shots", 0, 10);
			Components.SliderInt("Force baim after x shots", "rbot_deagle_force_baim_after_shots", 0, 10);
			Components.Checkbox("Baim while moving", "rbot_deagle_baim_while_moving");
			break;
		case RbotWeaponsAvailable::REVOLVER:
			Components.SliderFloat("Hitchance", "rbot_revolver_min_hitchance", 0.f, 100.f);
			Components.SliderFloat("Min damage", "rbot_revolver_mindamage", 0.f, 100.f);
			Components.SliderInt("Baim after x shots", "rbot_revolver_baim_after_shots", 0, 10);
			Components.SliderInt("Force baim after x shots", "rbot_revolver_force_baim_after_shots", 0, 10);
			Components.Checkbox("Baim while moving", "rbot_revolver_baim_while_moving");
			break;
		case RbotWeaponsAvailable::SCOUT:
			Components.SliderFloat("Hitchance", "rbot_scout_min_hitchance", 0.f, 100.f);
			Components.SliderFloat("Min damage", "rbot_scout_mindamage", 0.f, 100.f);
			Components.SliderInt("Baim after x shots", "rbot_scout_baim_after_shots", 0, 10);
			Components.SliderInt("Force baim after x shots", "rbot_scout_force_baim_after_shots", 0, 10);
			Components.Checkbox("Baim while moving", "rbot_scout_baim_while_moving");
			break;
		case RbotWeaponsAvailable::AWP:
			Components.SliderFloat("Hitchance", "rbot_awp_min_hitchance", 0.f, 100.f);
			Components.SliderFloat("Min damage", "rbot_awp_mindamage", 0.f, 100.f);
			Components.SliderInt("Baim after x shots", "rbot_awp_baim_after_shots", 0, 10);
			Components.SliderInt("Force baim after x shots", "rbot_awp_force_baim_after_shots", 0, 10);
			Components.Checkbox("Baim while moving", "rbot_awp_baim_while_moving");
			break;
		case RbotWeaponsAvailable::AUTO:
			Components.SliderFloat("Hitchance", "rbot_auto_min_hitchance", 0.f, 100.f);
			Components.SliderFloat("Min damage", "rbot_auto_mindamage", 0.f, 100.f);
			Components.SliderInt("Baim after x shots", "rbot_auto_baim_after_shots", 0, 10);
			Components.SliderInt("Force baim after x shots", "rbot_auto_force_baim_after_shots", 0, 10);
			Components.Checkbox("Baim while moving", "rbot_auto_baim_while_moving");
			break;
		case RbotWeaponsAvailable::RIFLE:
			Components.SliderFloat("Hitchance", "rbot_rifle_min_hitchance", 0.f, 100.f);
			Components.SliderFloat("Min damage", "rbot_rifle_mindamage", 0.f, 100.f);
			Components.SliderInt("Baim after x shots", "rbot_rifle_baim_after_shots", 0, 10);
			Components.SliderInt("Force baim after x shots", "rbot_rifle_force_baim_after_shots", 0, 10);
			Components.Checkbox("Baim while moving", "rbot_rifle_baim_while_moving");
			break;
		case RbotWeaponsAvailable::SHOTGUN:
			Components.SliderFloat("Hitchance", "rbot_shotgun_min_hitchance", 0.f, 100.f);
			Components.SliderFloat("Min damage", "rbot_shotgun_mindamage", 0.f, 100.f);
			Components.SliderInt("Baim after x shots", "rbot_shotgun_baim_after_shots", 0, 10);
			Components.SliderInt("Force baim after x shots", "rbot_shotgun_force_baim_after_shots", 0, 10);
			Components.Checkbox("Baim while moving", "rbot_shotgun_baim_while_moving");
			break;
	}

	const char* BaimModes[] = { "never", "auto" };
	Components.ComboBox("Baim mode", BaimModes, IM_ARRAYSIZE(BaimModes), "rbot_baimmode");
	Components.Checkbox("Air baim", "rbot_resolver_air_baim");
	Components.Hotkey("Force BAim hotkey", "rbot_forceaim_hotkey");

	//Components.SliderInt("Brut after x shots", "rbot_brutforce_after_shots", 0, 10);

	//Components.SliderFloat("Pointscale head", "rbot_head_scale", 0.f, 1.f);
	//Components.SliderFloat("Pointscale body", "rbot_body_scale", 0.f, 1.f);

    Components.BeginChild ( "#hitboxes", ImVec2 ( 0.f, 204.f ) );
    Components.Checkbox ( "Head", "rbot_hitbox_head" );
    Components.SameLine();
    Components.Label ( " Scale:" );
    Components.SameLine();
    Components.SliderFloat ( " ", "rbot_hitbox_head_scale", 0.f, 1.f );
    Components.Checkbox ( "Neck", "rbot_hitbox_neck" );
    Components.SameLine();
    Components.Label ( " Scale:" );
    Components.SameLine();
    Components.SliderFloat ( "  ", "rbot_hitbox_neck_scale", 0.f, 1.f );
    Components.Checkbox ( "Chest", "rbot_hitbox_chest" );
    Components.SameLine();
    Components.Label ( " Scale:" );
    Components.SameLine();
    Components.SliderFloat ( "   ", "rbot_hitbox_chest_scale", 0.f, 1.f );
    Components.Checkbox ( "Pelvis", "rbot_hitbox_pelvis" );
    Components.SameLine();
    Components.Label ( " Scale:" );
    Components.SameLine();
    Components.SliderFloat ( "    ", "rbot_hitbox_pelvis_scale", 0.f, 1.f );
    Components.Checkbox ( "Stomach", "rbot_hitbox_stomach" );
    Components.SameLine();
    Components.Label ( " Scale:" );
    Components.SameLine();
    Components.SliderFloat ( "     ", "rbot_hitbox_stomach_scale", 0.f, 1.f );
    Components.Checkbox ( "Arm", "rbot_hitbox_arm" );
    Components.SameLine();
    Components.Label ( " Scale:" );
    Components.SameLine();
    Components.SliderFloat ( "      ", "rbot_hitbox_arm_scale", 0.f, 1.f );
    Components.Checkbox ( "Leg", "rbot_hitbox_leg" );
    Components.SameLine();
    Components.Label ( " Scale:" );
    Components.SameLine();
    Components.SliderFloat ( "       ", "rbot_hitbox_leg_scale", 0.f, 1.f );
    Components.Checkbox ( "Foot", "rbot_hitbox_foot" );
    Components.SameLine();
    Components.Label ( " Scale:" );
    Components.SameLine();
    Components.SliderFloat ( "        ", "rbot_hitbox_foot_scale", 0.f, 1.f );
    Components.EndChild();

    Components.NextColumn();

    static const char* YawAAs[]		= { "none", "backwards", "spinbot", "lower body yaw", "random", "freestanding", "custom" };
    static const char* YawAddAAs[]  = { "none", "jitter", "switch", "spin", "random" };
    static const char* PitchAAs[]	= { "none", "emotion", "down", "up", "zero", "jitter", "down jitter", "up jitter", "zero jitter", "spin", "up spin", "down spin", "random", "switch", "down switch", "up switch", "fake up", "fake down", "custom" };
    static char* AntiAimMenus[]		= { "stand", "move", "air", "misc" };
    static int AAMenuSelected = 0;
    Components.Navbar ( AntiAimMenus, IM_ARRAYSIZE ( AntiAimMenus ), AAMenuSelected );
    Components.Checkbox ( "Enable aa", "rbot_aa" );
    static const char* FakelagModes[] = { "normal", "adaptive" };

    switch ( ( RbotMenuAvailable ) AAMenuSelected )
    {
        case RbotMenuAvailable::STANDING:
            Components.ComboBox ( "pitch AA", PitchAAs, IM_ARRAYSIZE ( PitchAAs ), "rbot_aa_stand_pitch" );
            Components.ComboBox ( "real yaw AA", YawAAs, IM_ARRAYSIZE ( YawAAs ), "rbot_aa_stand_real_yaw" );
            //if(g_Config.GetInt())
            //Components.SliderFloat("real add angel", "", -180.f, 180.f);
            //Components.ComboBox("fake yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_stand_fake_yaw");

            Components.ComboBox ( "real add", YawAddAAs, IM_ARRAYSIZE ( YawAddAAs ), "rbot_aa_stand_real_add_yaw_add" );
            Components.SliderFloat ( "range", "rbot_aa_stand_real_add_yaw_add_range", 0.f, 360.f );
            //Components.ComboBox("fake add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_stand_fake_add_yaw_add");
            //Components.SliderFloat("range ", "rbot_aa_stand_fake_add_yaw_add_range", 0.f, 360.f);

            Components.SliderInt ( "Fakelag ticks", "misc_fakelag_ticks_standing", 0, 14 );
            break;

        case RbotMenuAvailable::MOVING:
            Components.ComboBox ( "pitch AA", PitchAAs, IM_ARRAYSIZE ( PitchAAs ), "rbot_aa_move_pitch" );
            Components.ComboBox ( "real yaw AA", YawAAs, IM_ARRAYSIZE ( YawAAs ), "rbot_aa_move_real_yaw" );
            //Components.ComboBox("fake yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_move_fake_yaw");

            Components.ComboBox ( "real add", YawAddAAs, IM_ARRAYSIZE ( YawAddAAs ), "rbot_aa_move_real_add_yaw_add" );
            Components.SliderFloat ( "range", "rbot_aa_move_real_add_yaw_add_range", 0.f, 360.f );
            //Components.ComboBox("fake add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_move_fake_add_yaw_add");
            //Components.SliderFloat("range ", "rbot_aa_move_fake_add_yaw_add_range", 0.f, 360.f);

            Components.ComboBox ( "Fakelag mode", FakelagModes, IM_ARRAYSIZE ( FakelagModes ), "misc_fakelag_mode_moving" );
            Components.SliderInt ( "Fakelag ticks", "misc_fakelag_ticks_moving", 0, 14 );
            break;

        case RbotMenuAvailable::AIR:
            Components.ComboBox ( "pitch AA", PitchAAs, IM_ARRAYSIZE ( PitchAAs ), "rbot_aa_air_pitch" );
            Components.ComboBox ( "real yaw AA", YawAAs, IM_ARRAYSIZE ( YawAAs ), "rbot_aa_air_real_yaw" );
            //Components.ComboBox("fake yaw AA", YawAAs, IM_ARRAYSIZE(YawAAs), "rbot_aa_air_fake_yaw");

            Components.ComboBox ( "real add", YawAddAAs, IM_ARRAYSIZE ( YawAddAAs ), "rbot_aa_air_real_add_yaw_add" );
            Components.SliderFloat ( "range", "rbot_aa_air_real_add_yaw_add_range", 0.f, 360.f );
            //Components.ComboBox("fake add", YawAddAAs, IM_ARRAYSIZE(YawAddAAs), "rbot_aa_air_fake_add_yaw_add");
            //Components.SliderFloat("range ", "rbot_aa_air_fake_add_yaw_add_range", 0.f, 360.f);

            Components.ComboBox ( "Fakelag mode", FakelagModes, IM_ARRAYSIZE ( FakelagModes ), "misc_fakelag_mode_air" );
            Components.SliderInt ( "Fakelag ticks", "misc_fakelag_ticks_air", 0, 14 );
            break;

        case RbotMenuAvailable::MISC:

            Components.SliderFloat ( "Spinbot speed", "rbot_aa_spinbot_speed", -20.f, 20.f );
            Components.Checkbox ( "Slidewalk", "rbot_slidewalk" );
            Components.Checkbox ( "desync", "rbot_aa_desync" );
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
            Components.Hotkey ( "Manual AA right", "rbot_manual_key_right" );
            Components.Hotkey ( "Manual AA left", "rbot_manual_key_left" );
            Components.Hotkey ( "Manual AA back", "rbot_manual_key_back" );
            //Components.Hotkey("Fakewalk key", "rbot_aa_fakewalk_key");
            break;
    }

	Components.Checkbox("Auto scope", "rbot_autoscope");
	Components.Checkbox("Auto stop", "rbot_autostop");
	Components.Checkbox("Auto crouch", "rbot_autocrouch");
	Components.Checkbox("Slow Walk", "rbot_slowwalk");
	Components.Hotkey("Slow Walk Hotkey", "rbot_slowwalk_hotkey");
	//Components.Checkbox("Lby prediction", "rbot_lby_prediction");
#ifdef _DEBUG
	Components.Checkbox("Fakelag prediction", "rbot_flag_prediction");
#endif // _DEBUG

	const char* ShootingModes[] = { "normal", "in fakelag", "fakelag while shooting" };
	Components.ComboBox("Shooting mode", ShootingModes, IM_ARRAYSIZE(ShootingModes), "rbot_shooting_mode");

#ifdef _DEBUG
	Components.Checkbox("Lagcompensation", "rbot_lagcompensation");
#endif // _DEBUG
	Components.Checkbox("Force unlag", "rbot_force_unlage");

	Components.Checkbox("Resolver", "rbot_resolver");
    Components.EndChild();
}

void Menu::RenderLegitbot()
{
    Components.BeginChild ( "#lbot", ImVec2 ( 0, 0 ) );

    Components.Label ( "Legitbot" );
    Components.Checkbox ( "Enable", "lbot" );

    Components.Spacing();
    Components.Columns ( 2, false );


    static char* WeaponConfigSelectionItems[] = { "G", "A", "L", "f", "W", "c", "a", "Z" };
    static char* WeaponConfigSelectionItemsText[] = { "pistol", "deagle", "smg", "mg", "rifle", "shotgun", "scout", "sniper" };
    static int WeaponSelected = 0;
    Components.NavbarIcons ( WeaponConfigSelectionItems, WeaponConfigSelectionItemsText, IM_ARRAYSIZE ( WeaponConfigSelectionItems ), WeaponSelected, IconsFont );

    Components.Hotkey ( "Aimkey", "lbot_aimkey" );

    switch ( ( LbotWeaponsAvailable ) WeaponSelected )
    {
        case LbotWeaponsAvailable::PISTOL:
            //Components.SliderFloat ( "Fov", "lbot_pistol_fov", 0.f, 15.f );
			ImGui::SliderFloat("FOV", &Settings::Aimbot::WeaponAimSetting[0].FOV, 0.f, 15.f);
            //Components.SliderFloat ( "Smooth", "lbot_pistol_smooth", 1.f, 30.f );
			ImGui::SliderFloat("Smooth", &Settings::Aimbot::WeaponAimSetting[0].Smooth, 1.f, 30.f);
            Components.SliderFloat ( "Randomize", "lbot_pistol_randomize", 0.f, 10.f );
            Components.SliderFloat ( "Delay", "lbot_pistol_delay", 0.f, 1.f );

            Components.Spacing();

            Components.Checkbox ( "Rcs", "lbot_pistol_rcs" );
            Components.SliderFloat ( "Amount x", "lbot_pistol_rcs_x", 0.f, 1.f );
            Components.SliderFloat ( "Amount y", "lbot_pistol_rcs_y", 0.f, 1.f );

            Components.Spacing();

            Components.Label ( "Hitboxes:" );
            Components.BeginChild ( "#hitboxes", ImVec2 ( 0.f, 204.f ) );
            Components.Checkbox ( "head", "lbot_pistol_hitbox_head" );
            Components.Checkbox ( "neck", "lbot_pistol_hitbox_neck" );
            Components.Checkbox ( "chest", "lbot_pistol_hitbox_chest" );
            Components.Checkbox ( "pelvis", "lbot_pistol_hitbox_pelvis" );
            Components.Checkbox ( "stomach", "lbot_pistol_hitbox_stomach" );
            Components.Checkbox ( "arm", "lbot_pistol_hitbox_arm" );
            Components.Checkbox ( "leg", "lbot_pistol_hitbox_leg" );
            Components.Checkbox ( "foot", "lbot_pistol_hitbox_foot" );
            Components.EndChild();
            break;

		case LbotWeaponsAvailable::DEAGLE:
			//Components.SliderFloat("Fov", "lbot_deagle_fov", 0.f, 15.f);
			ImGui::SliderFloat("FOV", &Settings::Aimbot::WeaponAimSetting[1].FOV, 0.f, 15.f);
			ImGui::SliderFloat("Smooth", &Settings::Aimbot::WeaponAimSetting[1].Smooth, 1.f, 30.f);
			//Components.SliderFloat("Smooth", "lbot_deagle_smooth", 1.f, 30.f);
			Components.SliderFloat("Randomize", "lbot_deagle_randomize", 0.f, 10.f);
			Components.SliderFloat("Delay", "lbot_deagle_delay", 0.f, 1.f);

			Components.Spacing();

			Components.Checkbox("Rcs", "lbot_deagle_rcs");
			Components.SliderFloat("Amount x", "lbot_deagle_rcs_x", 0.f, 1.f);
			Components.SliderFloat("Amount y", "lbot_deagle_rcs_y", 0.f, 1.f);

			Components.Spacing();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
			Components.Checkbox("head", "lbot_deagle_hitbox_head");
			Components.Checkbox("neck", "lbot_deagle_hitbox_neck");
			Components.Checkbox("chest", "lbot_deagle_hitbox_chest");
			Components.Checkbox("pelvis", "lbot_deagle_hitbox_pelvis");
			Components.Checkbox("stomach", "lbot_deagle_hitbox_stomach");
			Components.Checkbox("arm", "lbot_deagle_hitbox_arm");
			Components.Checkbox("leg", "lbot_deagle_hitbox_leg");
			Components.Checkbox("foot", "lbot_deagle_hitbox_foot");
			Components.EndChild();
			break;
        case LbotWeaponsAvailable::SMG:
            Components.SliderFloat ( "Fov", "lbot_smg_fov", 0.f, 15.f );
            Components.SliderFloat ( "Smooth", "lbot_smg_smooth", 1.f, 30.f );
            Components.SliderFloat ( "Randomize", "lbot_smg_randomize", 0.f, 10.f );
            Components.SliderFloat ( "Delay", "lbot_smg_delay", 0.f, 1.f );

            Components.Spacing();

            Components.Checkbox ( "Rcs", "lbot_smg_rcs" );
            Components.SliderFloat ( "Amount x", "lbot_smg_rcs_x", 0.f, 1.f );
            Components.SliderFloat ( "Amount y", "lbot_smg_rcs_y", 0.f, 1.f );

            Components.Spacing();

            Components.Label ( "Hitboxes:" );
            Components.BeginChild ( "#hitboxes", ImVec2 ( 0.f, 204.f ) );
            Components.Checkbox ( "head", "lbot_smg_hitbox_head" );
            Components.Checkbox ( "neck", "lbot_smg_hitbox_neck" );
            Components.Checkbox ( "chest", "lbot_smg_hitbox_chest" );
            Components.Checkbox ( "pelvis", "lbot_smg_hitbox_pelvis" );
            Components.Checkbox ( "stomach", "lbot_smg_hitbox_stomach" );
            Components.Checkbox ( "arm", "lbot_smg_hitbox_arm" );
            Components.Checkbox ( "leg", "lbot_smg_hitbox_leg" );
            Components.Checkbox ( "foot", "lbot_smg_hitbox_foot" );
            Components.EndChild();
            break;

        case LbotWeaponsAvailable::MG:
            Components.SliderFloat ( "Fov", "lbot_mg_fov", 0.f, 15.f );
            Components.SliderFloat ( "Smooth", "lbot_mg_smooth", 1.f, 30.f );
            Components.SliderFloat ( "Randomize", "lbot_mg_randomize", 0.f, 10.f );
            Components.SliderFloat ( "Delay", "lbot_mg_delay", 0.f, 1.f );

            Components.Spacing();

            Components.Checkbox ( "Rcs", "lbot_mg_rcs" );
            Components.SliderFloat ( "Amount x", "lbot_mg_rcs_x", 0.f, 1.f );
            Components.SliderFloat ( "Amount y", "lbot_mg_rcs_y", 0.f, 1.f );

            Components.Spacing();

            Components.Label ( "Hitboxes:" );
            Components.BeginChild ( "#hitboxes", ImVec2 ( 0.f, 204.f ) );
            Components.Checkbox ( "head", "lbot_mg_hitbox_head" );
            Components.Checkbox ( "neck", "lbot_mg_hitbox_neck" );
            Components.Checkbox ( "chest", "lbot_mg_hitbox_chest" );
            Components.Checkbox ( "pelvis", "lbot_mg_hitbox_pelvis" );
            Components.Checkbox ( "stomach", "lbot_mg_hitbox_stomach" );
            Components.Checkbox ( "arm", "lbot_mg_hitbox_arm" );
            Components.Checkbox ( "leg", "lbot_mg_hitbox_leg" );
            Components.Checkbox ( "foot", "lbot_mg_hitbox_foot" );
            Components.EndChild();
            break;

        case LbotWeaponsAvailable::RIFLE:
            Components.SliderFloat ( "Fov", "lbot_rifle_fov", 0.f, 15.f );
            Components.SliderFloat ( "Smooth", "lbot_rifle_smooth", 1.f, 30.f );
            Components.SliderFloat ( "Randomize", "lbot_rifle_randomize", 0.f, 10.f );
            Components.SliderFloat ( "Delay", "lbot_rifle_delay", 0.f, 1.f );

            Components.Spacing();

            Components.Checkbox ( "Rcs", "lbot_rifle_rcs" );
            Components.SliderFloat ( "Amount x", "lbot_rifle_rcs_x", 0.f, 1.f );
            Components.SliderFloat ( "Amount y", "lbot_rifle_rcs_y", 0.f, 1.f );

            Components.Spacing();

            Components.Label ( "Hitboxes:" );
            Components.BeginChild ( "#hitboxes", ImVec2 ( 0.f, 204.f ) );
            Components.Checkbox ( "head", "lbot_rifle_hitbox_head" );
            Components.Checkbox ( "neck", "lbot_rifle_hitbox_neck" );
            Components.Checkbox ( "chest", "lbot_rifle_hitbox_chest" );
            Components.Checkbox ( "pelvis", "lbot_rifle_hitbox_pelvis" );
            Components.Checkbox ( "stomach", "lbot_rifle_hitbox_stomach" );
            Components.Checkbox ( "arm", "lbot_rifle_hitbox_arm" );
            Components.Checkbox ( "leg", "lbot_rifle_hitbox_leg" );
            Components.Checkbox ( "foot", "lbot_rifle_hitbox_foot" );
            Components.EndChild();
            break;

        case LbotWeaponsAvailable::SHOTGUN:
            Components.SliderFloat ( "Fov", "lbot_shotgun_fov", 0.f, 15.f );
            Components.SliderFloat ( "Smooth", "lbot_shotgun_smooth", 1.f, 30.f );
            Components.SliderFloat ( "Randomize", "lbot_shotgun_randomize", 0.f, 10.f );
            Components.SliderFloat ( "Delay", "lbot_shotgun_delay", 0.f, 1.f );

            Components.Spacing();

            Components.Checkbox ( "Rcs", "lbot_shotgun_rcs" );
            Components.SliderFloat ( "Amount x", "lbot_shotgun_rcs_x", 0.f, 1.f );
            Components.SliderFloat ( "Amount y", "lbot_shotgun_rcs_y", 0.f, 1.f );

            Components.Spacing();

            Components.Label ( "Hitboxes:" );
            Components.BeginChild ( "#hitboxes", ImVec2 ( 0.f, 204.f ) );
            Components.Checkbox ( "head", "lbot_shotgun_hitbox_head" );
            Components.Checkbox ( "neck", "lbot_shotgun_hitbox_neck" );
            Components.Checkbox ( "chest", "lbot_shotgun_hitbox_chest" );
            Components.Checkbox ( "pelvis", "lbot_shotgun_hitbox_pelvis" );
            Components.Checkbox ( "stomach", "lbot_shotgun_hitbox_stomach" );
            Components.Checkbox ( "arm", "lbot_shotgun_hitbox_arm" );
            Components.Checkbox ( "leg", "lbot_shotgun_hitbox_leg" );
            Components.Checkbox ( "foot", "lbot_shotgun_hitbox_foot" );
            Components.EndChild();
            break;
		case LbotWeaponsAvailable::SCOUT:
			Components.SliderFloat("Fov", "lbot_scout_fov", 0.f, 15.f);
			Components.SliderFloat("Smooth", "lbot_scout_smooth", 1.f, 30.f);
			Components.SliderFloat("Randomize", "lbot_scout_randomize", 0.f, 10.f);
			Components.SliderFloat("Delay", "lbot_scout_delay", 0.f, 1.f);
			Components.Checkbox("Flickbot", "lbot_scout_flickbot");

			Components.Spacing();

			Components.Checkbox("Rcs", "lbot_scout_rcs");
			Components.SliderFloat("Amount x", "lbot_scout_rcs_x", 0.f, 1.f);
			Components.SliderFloat("Amount y", "lbot_scout_rcs_y", 0.f, 1.f);

			Components.Spacing();

			Components.Label("Hitboxes:");
			Components.BeginChild("#hitboxes", ImVec2(0.f, 204.f));
			Components.Checkbox("head", "lbot_scout_hitbox_head");
			Components.Checkbox("neck", "lbot_scout_hitbox_neck");
			Components.Checkbox("chest", "lbot_scout_hitbox_chest");
			Components.Checkbox("pelvis", "lbot_scout_hitbox_pelvis");
			Components.Checkbox("stomach", "lbot_scout_hitbox_stomach");
			Components.Checkbox("arm", "lbot_scout_hitbox_arm");
			Components.Checkbox("leg", "lbot_scout_hitbox_leg");
			Components.Checkbox("foot", "lbot_scout_hitbox_foot");
			Components.EndChild();
			break;
        case LbotWeaponsAvailable::SNIPER:
            Components.SliderFloat ( "Fov", "lbot_sniper_fov", 0.f, 15.f );
            Components.SliderFloat ( "Smooth", "lbot_sniper_smooth", 1.f, 30.f );
            Components.SliderFloat ( "Randomize", "lbot_sniper_randomize", 0.f, 10.f );
            Components.SliderFloat ( "Delay", "lbot_sniper_delay", 0.f, 1.f );
            Components.Checkbox ( "Flickbot", "lbot_sniper_flickbot" );

            Components.Spacing();

            Components.Checkbox ( "Rcs", "lbot_sniper_rcs" );
            Components.SliderFloat ( "Amount x", "lbot_sniper_rcs_x", 0.f, 1.f );
            Components.SliderFloat ( "Amount y", "lbot_sniper_rcs_y", 0.f, 1.f );

            Components.Spacing();

            Components.Label ( "Hitboxes:" );
            Components.BeginChild ( "#hitboxes", ImVec2 ( 0.f, 204.f ) );
            Components.Checkbox ( "head", "lbot_sniper_hitbox_head" );
            Components.Checkbox ( "neck", "lbot_sniper_hitbox_neck" );
            Components.Checkbox ( "chest", "lbot_sniper_hitbox_chest" );
            Components.Checkbox ( "pelvis", "lbot_sniper_hitbox_pelvis" );
            Components.Checkbox ( "stomach", "lbot_sniper_hitbox_stomach" );
            Components.Checkbox ( "arm", "lbot_sniper_hitbox_arm" );
            Components.Checkbox ( "leg", "lbot_sniper_hitbox_leg" );
            Components.Checkbox ( "foot", "lbot_sniper_hitbox_foot" );
            Components.EndChild();
            break;
    }

    Components.NextColumn();

    Components.Checkbox ( "Backtrack", "lbot_backtrack" );
    Components.Checkbox ( "Aim at backtrack", "lbot_backtrack_aim" );
    Components.SliderFloat ( "Backtrack time", "lbot_backtrack_ms", 0.f, .2f );
    //legit aa
    //triggerbot

    Components.EndChild();
}

void Menu::RenderVisuals()
{
    Components.BeginChild ( "#visuals", ImVec2 ( 0, 0 ) );

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

            Components.Columns ( 3, false );
            Components.Label ( "Chams" );
            Components.ColorCheckbox2 ( "Enable  ", "chams_local", "color_chams_local", "color_chams_local_xqz" );
            Components.ComboBox ( "Chams type", ChamsTypes, IM_ARRAYSIZE ( ChamsTypes ), "chams_mode_local" );
            Components.NextColumn();

            Components.Label ( "Esp" );
            Components.Checkbox ( "Enable   ", "esp_local_enabled" );
            Components.ColorCheckbox ( "Box", "esp_local_boxes", "color_esp_local_boxes" );
            Components.ComboBox ( "Box type", BoxTypes, IM_ARRAYSIZE ( BoxTypes ), "esp_local_boxes_type" );
            Components.ColorCheckbox ( "Name", "esp_local_names", "color_esp_local_names" );
            Components.Checkbox ( "Health", "esp_local_health" );
            Components.ColorCheckbox ( "Armor", "esp_local_armour", "color_esp_local_armour" );
            Components.ColorCheckbox ( "Weapon", "esp_local_weapons", "color_esp_local_weapons" );
            //Components.Checkbox("Snapline", "esp_team_snaplines");

            break;
        }

        case VisualsMenuAvailable::ENEMY:
        {
            Components.Columns ( 3, false );
            Components.Label ( "Chams" );
            Components.ColorCheckbox2 ( "Enable  ", "chams_enemy", "color_chams_enemy", "color_chams_enemy_xqz" );
            Components.ComboBox ( "Chams type", ChamsTypes, IM_ARRAYSIZE ( ChamsTypes ), "chams_mode_enemy" );
            Components.NextColumn();

            Components.Label ( "Esp" );
            Components.Checkbox ( "Enable   ", "esp_enemy_enabled" );
            Components.ColorCheckbox ( "Box", "esp_enemy_boxes", "color_esp_enemy_boxes" );
            Components.ComboBox ( "Box type", BoxTypes, IM_ARRAYSIZE ( BoxTypes ), "esp_enemy_boxes_type" );
            Components.ColorCheckbox ( "Name", "esp_enemy_names", "color_esp_enemy_names" );
            Components.Checkbox ( "Health", "esp_enemy_health" );
            Components.ColorCheckbox ( "Armor", "esp_enemy_armour", "color_esp_enemy_armour" );
            Components.ColorCheckbox ( "Weapon", "esp_enemy_weapons", "color_esp_enemy_weapons" );
            Components.ColorCheckbox ( "Snapline", "esp_enemy_snaplines", "color_esp_enemy_snaplines" );
            Components.ColorCheckbox ( "Resolver info", "esp_enemy_info", "color_esp_enemy_info" );
            Components.ColorCheckbox ( "Lby timer", "esp_enemy_lby_timer", "color_esp_enemy_lby_timer" );
            break;
        }

        case VisualsMenuAvailable::TEAM:
        {
            Components.Columns ( 3, false );
            Components.Label ( "Chams" );
            Components.ColorCheckbox2 ( "Enable  ", "chams_team", "color_chams_team", "color_chams_team_xqz" );
            Components.ComboBox ( "Chams type", ChamsTypes, IM_ARRAYSIZE ( ChamsTypes ), "chams_mode_team" );
            Components.NextColumn();

            Components.Label ( "Esp" );
            Components.Checkbox ( "Enable   ", "esp_team_enabled" );
            Components.ColorCheckbox ( "Box", "esp_team_boxes", "color_esp_team_boxes" );
            Components.ComboBox ( "Box type", BoxTypes, IM_ARRAYSIZE ( BoxTypes ), "esp_team_boxes_type" );
            Components.ColorCheckbox ( "Name", "esp_team_names", "color_esp_team_names" );
            Components.Checkbox ( "Health", "esp_team_health" );
            Components.ColorCheckbox ( "Armor", "esp_team_armour", "color_esp_team_armour" );
            Components.ColorCheckbox ( "Weapon", "esp_team_weapons", "color_esp_team_weapons" );
            Components.ColorCheckbox ( "Snapline", "esp_team_snaplines", "color_esp_team_snaplines" );
            break;
        }

        case VisualsMenuAvailable::MISC:
        {
            Components.Columns ( 3, false );
            Components.Label ( "Thirdperson: " );
            Components.Spacing();
            Components.Checkbox ( "Thirdperson", "vis_misc_thirdperson" );
            Components.Hotkey ( "Thirdperson hotkey", "vis_misc_thirdperson_hotkey" );

            Components.NextColumn();
            Components.Label ( "Esp:" );
            Components.Spacing();
            Components.Checkbox ( "Enable   ", "esp_misc_enabled" );
            Components.Checkbox ( "Grenade", "esp_misc_grenade" );
            Components.ColorCheckbox ( "Bomb", "esp_planted_c4", "color_esp_c4" );
            Components.Checkbox ( "Dangerzone item esp", "esp_misc_dangerzone_item_esp" );
            Components.SliderFloat ( "Dangerzone esp range", "esp_misc_dangerzone_item_esp_dist", 0.f, 1000.f );

            Components.NextColumn();

            Components.Label ( "Misc:" );
            Components.Spacing();
            Components.Checkbox ( "No scope overlay", "vis_misc_noscope" );
            Components.Checkbox ( "Bullet tracers", "vis_misc_bullettracer" );
            Components.Checkbox ( "No flash", "vis_misc_noflash" );
            Components.ColorCheckbox ( "Spread circle", "vis_misc_draw_circle", "vis_misc_draw_circle_clr" );
            Components.Checkbox ( "Disable sniper zoom", "vis_misc_disable_scope_zoom" );
            Components.SliderInt ( "Viewmodel fov", "viewmodel_fov", 1, 150 );
            Components.SliderInt ( "Fov", "fov", 1, 150 );
            Components.Checkbox ( "No smoke", "vis_misc_nosmoke" );
            Components.Checkbox ( "Hitmarker", "vis_misc_hitmarker" );
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
    Components.BeginChild ( "#misc", ImVec2 ( 0, 0 ) );

    Components.Label ( "Misc" );
    /*
    SetupVar("misc_bhop", false);
    SetupVar("misc_no_hands", false);
    SetupVar("misc_thirdperson", false);
    SetupVar("misc_showranks", false);
    SetupVar("misc_thirdperson_dist", 50.f);
    SetupVar("viewmodel_fov", 68);
    */
    Components.Checkbox ( "Bhop", "misc_bhop" );
    Components.Checkbox ( "Autostrafer", "misc_autostrafe" );
    //Components.Checkbox("No hands", "misc_no_hands");

    Components.Spacing();

    Components.Spacing();

    Components.Checkbox ( "Rank reveal", "misc_showranks" );
    Components.Checkbox ( "No crouch cooldown", "misc_no_crouch_cooldown" );


    Components.Checkbox ( "Clantag changer", "misc_clantagchanger" );

    //Components.Checkbox("Anti untrusted", "misc_antiuntrusted");

    Components.Spacing();

	if (Components.Button("Load Game CFG"))
		g_EngineClient->ExecuteClientCmd(g_Config.LoadGameConfig().c_str());

    //buybot
    static const char* Pistols[] = { "none", "glock|usp|p2000", "duals", "tec9|fiveseven", "deagle|r8" };
    static const char* Weapons[] = { "none", "sg|aug", "ssg", "auto", "mac10", "p90", "bizon", "ak", "awp" };
    static std::string Grenades[] = { "molotov", "decoy", "flash", "grenade", "smoke" };
    static std::string GrenadesSettings[] = { "misc_buybot_grenade_molotov", "misc_buybot_grenade_decoy", "misc_buybot_grenade_flash", "misc_buybot_grenade_grenade", "misc_buybot_grenade_smoke" };
    Components.Checkbox ( "Enable buybot", "misc_buybot" );
    Components.ComboBox ( "Pistols", Pistols, IM_ARRAYSIZE ( Pistols ), "misc_buybot_pistol" );
    Components.ComboBox ( "Weapon", Weapons, IM_ARRAYSIZE ( Weapons ), "misc_buybot_weapon" );
    Components.ComboCheckBox ( "Grenades", Grenades, GrenadesSettings, IM_ARRAYSIZE ( Grenades ) );
    Components.Checkbox ( "Buy armor", "misc_buybot_armor" );
    Components.Checkbox ( "Buy zeus", "misc_buybot_zeus" );
    Components.Checkbox ( "Buy defuser", "misc_buybot_defuser" );

    if ( Components.Button ( "unload" ) )
        g_Unload = true;

    #ifdef _DEBUG
    Components.Checkbox ( "misc_debug_overlay", "misc_debug_overlay" );
    #endif // _DEBUG


    Components.EndChild();
}

void Menu::RenderSkinchanger()
{
	Components.BeginChild("#skinchanger", ImVec2(0, 0));

	Components.Label("Skinchanger");
	Components.Label("Not today");

	Components.EndChild();
}

void Menu::RenderSettings()
{
    Components.BeginChild ( "#settings", ImVec2 ( 0, 0 ) );

    Components.Label ( "Settings" );
    Components.Spacing();

    Components.Columns ( 3, false );

    Components.BeginChild ( "#allConfigs", ImVec2 ( 0.f, 250.f ) );

    //for() Configs
    static int Selected = -1;
    int i = 0;

    /*for ( auto config = g_Config.Configs.begin(); config != g_Config.Configs.end(); config++, i++ )
    {
        if ( ImGui::Selectable ( config->data(), i == Selected ) )
            Selected = i;
    }*/
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

	if (Components.Button("Create") && str0 != "")
		//g_Config.CreateConfig ( str0 );
		Settings::CreateConfig(str0);

	if (Components.Button("Save"))
		//g_Config.Save ( g_Config.Configs[Selected] );
		Settings::SaveSettings(Settings::Configs[Selected]);

    Components.SameLine();

	if (Components.Button("Load"))
		//g_Config.Load ( g_Config.Configs[Selected] );
		Settings::LoadSettings(Settings::Configs[Selected]);


	if (Components.Button("Refresh"))
		//g_Config.RefreshConfigList();
		Settings::RefreshConfigList();

    Components.SameLine();

    if ( Components.Button ( "Reset" ) )
        g_Config.ResetConfig();

    Components.EndChild();
}