
#include "ConfigSystem.h"
#include "valve_sdk/csgostructs.hpp"
#include <atlstr.h>
#include "Logger.h"
#include "ConsoleHelper.h"
#include "includes/INIReader.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <string>
#include "Logger.h"
namespace fs = std::filesystem;

int ConfigSystem::GetInt ( std::string name )
{
    return intOptions[name];
}

float ConfigSystem::GetFloat ( std::string name )
{
    return floatOptions[name];
}

bool ConfigSystem::GetBool ( std::string name )
{
    return boolOptions[name];
}

Color ConfigSystem::GetColor ( std::string name )
{
    return colorOptions[name];
}

void ConfigSystem::Set ( std::string name, int value )
{
    intOptions[name] = value;
}

void ConfigSystem::Set ( std::string name, float value )
{
    floatOptions[name] = value;
}

void ConfigSystem::Set ( std::string name, bool value )
{
    boolOptions[name] = value;
}

void ConfigSystem::Set ( std::string name, Color value )
{
    colorOptions[name] = value;
}

void ConfigSystem::Set ( int i, SkinSaverData value )
{
    skinOptions[i] = value;
}

void ConfigSystem::Save ( std::string file )
{
    std::string orgfile = file;
    file = AppdataFolder + "configs\\" + file;
    std::wstring filename ( file.begin(), file.end() );

    for ( auto option : intOptions )
    {
        std::string v = std::to_string ( option.second );
        std::wstring value ( v.begin(), v.end() );
        std::string n = option.first;
        std::wstring name ( n.begin(), n.end() );
        WritePrivateProfileStringW ( L"int", name.c_str(), value.c_str(), filename.c_str() );
    }

    for ( auto option : floatOptions )
    {
        std::string v = std::to_string ( option.second );
        std::wstring value ( v.begin(), v.end() );
        std::string n = option.first;
        std::wstring name ( n.begin(), n.end() );
        WritePrivateProfileStringW ( L"float", name.c_str(), value.c_str(), filename.c_str() );
    }

    for ( auto option : boolOptions )
    {
        std::string v = option.second ? "true" : "false";
        std::wstring value ( v.begin(), v.end() );
        std::string n = option.first;
        std::wstring name ( n.begin(), n.end() );
        WritePrivateProfileStringW ( L"bool", name.c_str(), value.c_str(), filename.c_str() );
    }

    for ( auto option : colorOptions )
    {
        Color c = option.second;
        std::string v = std::string ( std::to_string ( c.r() ) + ";" + std::to_string ( c.g() ) + ";" + std::to_string ( c.b() ) + ";" + std::to_string ( c.a() ) );
        std::wstring value ( v.begin(), v.end() );
        std::string n = option.first;
        std::wstring name ( n.begin(), n.end() );
        WritePrivateProfileStringW ( L"color", name.c_str(), value.c_str(), filename.c_str() );
    }

    g_Logger.Success ( "CONFIG", "saved " + orgfile );
}

void ConfigSystem::Load ( std::string file )
{
    std::string orgfile = file;
    file = AppdataFolder + "configs\\" + file;
    INIReader reader ( file );

    if ( reader.ParseError() < 0 )
    {
        g_Logger.Error ( "CONFIG", "error parsing file" );
        return;
    }

    for ( auto option : intOptions )
    {
        std::string n = option.first;
        intOptions[n] = reader.GetInteger ( "int", n, 0 );
    }

    for ( auto option : floatOptions )
    {
        std::string n = option.first;
        floatOptions[n] = ( float ) reader.GetReal ( "float", n, 0 );
    }

    for ( auto option : boolOptions )
    {
        std::string n = option.first;
        boolOptions[n] = reader.Get ( "bool", n, "false" ) == "true";
    }

    for ( auto option : colorOptions )
    {
        std::string n = option.first;
        //Console.WriteLine(n);
        std::string val = reader.Get ( "color", n, "255;255;255;255" );
        std::istringstream f ( val );
        std::string s;
        int i = 0;
        int r, g, b, a;

        while ( getline ( f, s, ';' ) )
        {
            switch ( i )
            {
                case 0:
                    r = std::stoi ( s );
                    break;

                case 1:
                    g = std::stoi ( s );
                    break;

                case 2:
                    b = std::stoi ( s );
                    break;

                case 3:
                    a = std::stoi ( s );
                    break;
            }

            i++;
        }

        colorOptions[n] = Color ( r, g, b, a );
    }

    g_Logger.Success ( "CONFIG", "loaded " + orgfile );
}

void ConfigSystem::RefreshConfigList()
{
    Configs.clear();

    for ( auto& p : fs::directory_iterator ( AppdataFolder + "configs\\" ) )
    {
        fs::path path = fs::path ( p );
        Configs.push_back ( path.filename().string() );
    }

    g_Logger.Info ( "CONFIG", "refreshed config list" );
}

void ConfigSystem::CreateConfig ( std::string file )
{
    std::string orgfile = file;

    if ( fs::exists ( AppdataFolder + "configs\\" + file ) )
        return;

    std::ofstream ofs ( AppdataFolder + "configs\\" + file );
    ofs << "";
    ofs.close();

    g_Logger.Success ( "CONFIG", "config " + orgfile + " created" );

    RefreshConfigList();
}

std::string ConfigSystem::LoadGameConfig()
{
	if (!fs::exists(AppdataFolder + "GameConfig.txt"))
		return "";
	std::ifstream t(AppdataFolder + "GameConfig.txt");
	std::string str;

	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return str;
}

void ConfigSystem::LoadSkins()
{
    if ( !fs::exists ( AppdataFolder + "skins.d" ) )
        return;

    INIReader reader ( AppdataFolder + "skins.d" );

    if ( reader.ParseError() < 0 )
    {
        g_Logger.Error ( "CONFIG", "error parsing file" );
        return;
    }

    for ( int i = 0; i < 33; i++ )
    {
        g_Config.skinOptions[i].Enabled = reader.GetBoolean ( "skins", ( "enabled_" + std::to_string ( i ) ), false );
        g_Config.skinOptions[i].Model = reader.GetInteger ( "skins", ( "model_" + std::to_string ( i ) ), 0 );
        g_Config.skinOptions[i].m_nFallbackPaintKit = reader.GetInteger ( "skins", ( "paintkit_" + std::to_string ( i ) ), 0 );
    }
}

void ConfigSystem::SaveSkins()
{
    std::string fn = ( AppdataFolder + "skins.d" );
    std::wstring filename ( fn.begin(), fn.end() );

    for ( int i = 0; i < 33; i++ )
    {
        std::string enabledstr = ( "enabled_" + std::to_string ( i ) );
        std::wstring enabled ( enabledstr.begin(), enabledstr.end() );
        std::string enabledbstr = std::to_string ( g_Config.skinOptions[i].Enabled );
        std::wstring enabledb ( enabledbstr.begin(), enabledbstr.end() );
        WritePrivateProfileStringW ( L"skins", enabled.c_str(), enabledb.c_str(), filename.c_str() );

        std::string modelstr = ( "model_" + std::to_string ( i ) );
        std::wstring model ( modelstr.begin(), modelstr.end() );
        std::string modelvbstr = std::to_string ( g_Config.skinOptions[i].Model );
        std::wstring modelv ( modelvbstr.begin(), modelvbstr.end() );
        WritePrivateProfileStringW ( L"skins", model.c_str(), modelv.c_str(), filename.c_str() );

        std::string paintkitstr = ( "paintkit_" + std::to_string ( i ) );
        std::wstring paintkit ( paintkitstr.begin(), paintkitstr.end() );
        std::string paintkitvbstr = std::to_string ( g_Config.skinOptions[i].m_nFallbackPaintKit );
        std::wstring paintkitv ( paintkitvbstr.begin(), paintkitvbstr.end() );
        WritePrivateProfileStringW ( L"skins", paintkit.c_str(), paintkitv.c_str(), filename.c_str() );
    }
}

void ConfigSystem::Setup()
{
    //C:\Users\TOR Exit Node\AppData\Roaming
    AppdataFolder = std::string ( getenv ( "APPDATA" ) ) + "\\AmeraHvH\\";

    if ( !fs::exists ( AppdataFolder ) )
    {
        fs::create_directory ( AppdataFolder );
        //MessageBox ( NULL, L"An important folder did not exist!", L"", 0 );
        //std::exit ( 1 );
    }

    if ( !fs::exists ( AppdataFolder + "configs\\" ) )
        fs::create_directory ( AppdataFolder + "configs\\" );

    RefreshConfigList();

    //
    // VISUALS
    //
    SetupVar ( "visuals_enable", false );
    //
    // ESP
    //
    //local
    SetupVar ( "esp_local_enabled", true );
    SetupVar ( "esp_local_boxes", false );
    SetupVar ( "color_esp_local_boxes", Color ( 50, 50, 255 ) );
    SetupVar ( "esp_local_boxes_type", 0 );
    SetupVar ( "esp_local_names", false );
    SetupVar ( "color_esp_local_names", Color ( 255, 255, 255 ) );
    SetupVar ( "esp_local_health", false );
    SetupVar ( "esp_local_armour", false );
    SetupVar ( "color_esp_local_armour", Color ( 0, 0, 255 ) );
    SetupVar ( "esp_local_weapons", false );
    SetupVar ( "color_esp_local_weapons", Color ( 255, 255, 255 ) );
    SetupVar ( "glow_local_enabled", false );
    SetupVar ( "color_glow_local_enabled", Color ( 55, 255, 150 ) );
    /*
    SetupVar("color_chams_team", Color(0, 200, 50));
    SetupVar("color_chams_enemy", Color(255, 50, 0));
    */
    /*
    SetupVar("glow_players", false);
    SetupVar("glow_chickens", false);
    SetupVar("glow_c4_carrier", false);
    SetupVar("glow_planted_c4", false);
    SetupVar("glow_defuse_kits", false);
    SetupVar("glow_weapons", false);
    */
    //team
    SetupVar ( "esp_team_enabled", true );
    SetupVar ( "esp_team_boxes", false );
    SetupVar ( "color_esp_team_boxes", Color ( 50, 255, 0 ) );
    SetupVar ( "esp_team_boxes_type", 0 );
    SetupVar ( "esp_team_names", false );
    SetupVar ( "color_esp_team_names", Color ( 255, 255, 255 ) );
    SetupVar ( "esp_team_health", false );
    SetupVar ( "esp_team_armour", false );
    SetupVar ( "color_esp_team_armour", Color ( 0, 0, 255 ) );
    SetupVar ( "esp_team_weapons", false );
    SetupVar ( "color_esp_team_weapons", Color ( 255, 255, 255 ) );
    SetupVar ( "esp_team_snaplines", false );
    SetupVar ( "color_esp_team_snaplines", Color ( 0, 255, 0 ) );
    SetupVar ( "glow_team_enabled", false );
    SetupVar ( "color_glow_team_enabled", Color ( 0, 200, 50 ) );
    /*
    SetupVar("color_chams_enemy", Color(255, 50, 0));
    */
    //enemy
    SetupVar ( "esp_enemy_enabled", true );
    SetupVar ( "esp_enemy_boxes", false );
    SetupVar ( "color_esp_enemy_boxes", Color ( 255, 50, 0 ) );
    SetupVar ( "esp_enemy_boxes_type", 0 );
    SetupVar ( "esp_enemy_names", false );
    SetupVar ( "color_esp_enemy_names", Color ( 255, 255, 255 ) );
    SetupVar ( "esp_enemy_health", false );
    SetupVar ( "esp_enemy_armour", false );
    SetupVar ( "color_esp_enemy_armour", Color ( 0, 0, 255 ) );
    SetupVar ( "esp_enemy_weapons", false );
    SetupVar ( "color_esp_enemy_weapons", Color ( 255, 255, 255 ) );
    SetupVar ( "esp_enemy_snaplines", false );
    SetupVar ( "color_esp_enemy_snaplines", Color ( 255, 0, 0 ) );
    SetupVar ( "esp_enemy_info", false );
    SetupVar ( "color_esp_enemy_info", Color ( 255, 255, 255 ) );
    SetupVar ( "esp_enemy_lby_timer", false );
    SetupVar ( "color_esp_enemy_lby_timer", Color::Blue );
    SetupVar ( "glow_enemy_enabled", false );
    SetupVar ( "color_glow_enemy_enabled", Color ( 255, 50, 0 ) );
    /*
    SetupVar("color_chams_enemy", Color(255, 50, 0));
    */
    //misc esp
    SetupVar ( "esp_misc_enabled", true );
    SetupVar ( "esp_misc_bullettracer", true );
    SetupVar ( "esp_crosshair", false );
    SetupVar ( "esp_dropped_weapons", false );
    SetupVar ( "esp_defuse_kit", false );
    SetupVar ( "esp_planted_c4", false );
    SetupVar ( "esp_misc_grenade", false );
    SetupVar ( "esp_misc_dangerzone_item_esp", false );
    SetupVar ( "esp_misc_dangerzone_item_esp_dist", 0.f );
    //SetupVar("esp_misc_outline", false);
    //extra settings
    //SetupVar("esp_name_pos", 1);
    //SetupVar("esp_weapons_pos", 1);
    SetupVar ( "esp_health_pos", 3 );
    SetupVar ( "esp_armour_pos", 1 );
    SetupVar ( "vis_misc_noscoope", false );
    SetupVar ( "vis_misc_bullettracer", false );
    SetupVar ( "vis_misc_addforce", 0 );
    SetupVar ( "vis_misc_autowall_crosshair", false );
    SetupVar ( "vis_misc_asuswalls_percent", 100 );
    SetupVar ( "vis_misc_nosmoke", false );
    SetupVar ( "vis_misc_hitmarker", false );
    SetupVar ( "vis_misc_hitmarker_sound", 0 );
    //
    // GLOW
    //

    //
    // MISC VISUALS
    //
    //
    SetupVar ( "vis_misc_thirdperson", false );
    SetupVar ( "vis_misc_thirdperson_hotkey", 0x43 );
    SetupVar ( "vis_misc_noflash", false );
    SetupVar ( "vis_misc_draw_circle", false );
    SetupVar ( "vis_misc_draw_circle_clr", Color::Green );
    SetupVar ( "vis_misc_disable_scope_zoom", false );
    // CHAMS
    //
    /*
    SetupVar("chams_player_enabled", false);
    SetupVar("chams_player_enemies_only", false);
    SetupVar("chams_player_wireframe", false);
    SetupVar("chams_player_flat", false);
    SetupVar("chams_player_ignorez", false);
    SetupVar("chams_player_glass", false);
    SetupVar("chams_arms_enabled", false);
    SetupVar("chams_arms_wireframe", false);
    SetupVar("chams_arms_flat", false);
    SetupVar("chams_arms_ignorez", false);
    SetupVar("chams_arms_glass", false);
    ChamsModes mode = IsLocal ? (ChamsModes)g_Config.GetInt("chams_mode_local") :
    		(IsTeam ? (ChamsModes)g_Config.GetInt("chams_mode_team") : (ChamsModes)g_Config.GetInt("chams_mode_enemy"));

    */
    SetupVar ( "chams_local", false );
    SetupVar ( "chams_team", false );
    SetupVar ( "chams_enemy", false );
    SetupVar ( "chams_mode_local", 0 );
    SetupVar ( "chams_mode_team", 0 );
    SetupVar ( "chams_mode_enemy", 0 );
    //color
    SetupVar ( "color_chams_local", Color ( 55, 255, 150 ) );
    SetupVar ( "color_chams_team", Color ( 0, 200, 50 ) );
    SetupVar ( "color_chams_enemy", Color ( 255, 50, 0 ) );
    SetupVar ( "color_chams_local_xqz", Color ( 155, 255, 150 ) );
    SetupVar ( "color_chams_team_xqz", Color ( 0, 0, 255 ) );
    SetupVar ( "color_chams_enemy_xqz", Color ( 0, 0, 0 ) );
    //
    // MISC
    //
    SetupVar ( "misc_bhop", false );
    SetupVar ( "misc_autostrafe", false );
    SetupVar ( "misc_no_hands", false );
    //SetupVar("misc_thirdperson", false);
    SetupVar ( "misc_showranks", false );
    //SetupVar("misc_thirdperson_dist", 50.f);
    SetupVar ( "viewmodel_fov", 68 );
    SetupVar ( "fov", 90 );
    SetupVar ( "mat_ambient_light_r", 0.f );
    SetupVar ( "mat_ambient_light_g", 0.f );
    SetupVar ( "chams_arms_glass", 0.f );
    SetupVar ( "mat_ambient_light_b", 0.f );

    SetupVar ( "misc_buybot", false );
    SetupVar ( "misc_buybot_grenade", false );
    SetupVar ( "misc_buybot_grenade_molotov", false );
    SetupVar ( "misc_buybot_grenade_decoy", false );
    SetupVar ( "misc_buybot_grenade_flash", false );
    SetupVar ( "misc_buybot_grenade_grenade", false );
    SetupVar ( "misc_buybot_grenade_smoke", false );
    SetupVar ( "misc_buybot_pistol", 0 );
    SetupVar ( "misc_buybot_weapon", 0 );
    SetupVar ( "misc_buybot_armor", false );
    SetupVar ( "misc_buybot_zeus", false );
    SetupVar ( "misc_buybot_defuser", false );

    SetupVar ( "misc_debug_overlay", false );

    //SetupVar("misc_fakelag", false);
    //SetupVar("misc_fakelag_mode", 0);
    //SetupVar("misc_fakelag_ticks", 0);
    /*
    Components.ComboBox("Fakelag mode", FakelagModes, IM_ARRAYSIZE(FakelagModes), "misc_fakelag_mode_moving");
    Components.SliderInt("Fakelag ticks", "misc_fakelag_ticks_moving", 0, 14);
    */
    SetupVar ( "misc_fakelag_ticks_standing", 0 );
    SetupVar ( "misc_fakelag_mode_moving", 0 );
    SetupVar ( "misc_fakelag_ticks_moving", 0 );
    SetupVar ( "misc_fakelag_mode_air", 0 );
    SetupVar ( "misc_fakelag_ticks_air", 0 );


    SetupVar ( "misc_no_crouch_cooldown", false );

    //SetupVar("misc_add_force", 10);
    SetupVar ( "misc_clantagchanger", false );

    //SetupVar("misc_antiuntrusted", true);
    //
    // COLORS
    //
    SetupVar ( "color_esp_ally_visible", Color ( 0, 128, 255 ) );
    SetupVar ( "color_esp_enemy_visible", Color ( 255, 0, 0 ) );
    SetupVar ( "color_esp_ally_occluded", Color ( 0, 128, 255 ) );
    SetupVar ( "color_esp_enemy_occluded", Color ( 255, 0, 0 ) );
    SetupVar ( "color_esp_crosshair", Color ( 255, 255, 255 ) );
    SetupVar ( "color_esp_weapons", Color ( 128, 0, 128 ) );
    SetupVar ( "color_esp_defuse", Color ( 0, 128, 255 ) );
    SetupVar ( "color_esp_c4", Color ( 255, 255, 0 ) );

    SetupVar ( "color_glow_ally", Color ( 0, 128, 255 ) );
    SetupVar ( "color_glow_enemy", Color ( 255, 0, 0 ) );
    SetupVar ( "color_glow_chickens", Color ( 0, 128, 0 ) );
    SetupVar ( "color_glow_c4_carrier", Color ( 255, 255, 0 ) );
    SetupVar ( "color_glow_planted_c4", Color ( 128, 0, 128 ) );
    SetupVar ( "color_glow_defuse", Color ( 255, 255, 255 ) );
    SetupVar ( "color_glow_weapons", Color ( 255, 128, 0 ) );

    SetupVar ( "color_chams_player_ally_visible", Color ( 0, 128, 255 ) );
    SetupVar ( "color_chams_player_ally_occluded", Color ( 0, 255, 128 ) );
    SetupVar ( "color_chams_player_enemy_visible", Color ( 255, 0, 0 ) );
    SetupVar ( "color_chams_player_enemy_occluded", Color ( 255, 128, 0 ) );
    SetupVar ( "color_chams_arms_visible", Color ( 0, 128, 255 ) );
    SetupVar ( "color_chams_arms_occluded", Color ( 0, 128, 255 ) );
    SetupVar ( "color_watermark", Color ( 0, 128, 255 ) );

    //
    // RBOT
    //
    SetupVar ( "rbot", false );
    SetupVar ( "rbot_mindamage", 0.f );
    SetupVar ( "rbot_min_hitchance", 0.f );
    SetupVar ( "rbot_baim_after_shots", 2 );
    SetupVar ( "rbot_force_baim_after_shots", 0 );
    SetupVar ( "rbot_baim_while_moving", false );
    SetupVar ( "rbot_force_unlage", false );
    SetupVar ( "rbot_lby_prediction", false );
    SetupVar ( "rbot_flag_prediction", false );
    //SetupVar("rbot_brutforce_after_shots", 0);
    SetupVar ( "rbot_autoscope", true );
    SetupVar ( "rbot_baimmode", 0 );
    // resolver
    SetupVar ( "rbot_resolver", false );
    SetupVar ( "rbot_resolver_air_baim", false );
    SetupVar ( "rbot_shooting_mode", 0 );
    SetupVar ( "rbot_lagcompensation", false );
    // antiaims
    SetupVar ( "rbot_aa_desync", false );
    SetupVar ( "rbot_aa", false );
    SetupVar ( "rbot_slidewalk", false );
    SetupVar ( "rbot_manual_aa_state", 0 );
    SetupVar ( "rbot_manual_key_right", 0x0 );
    SetupVar ( "rbot_manual_key_left", 0x0 );
    SetupVar ( "rbot_manual_key_back", 0x0 );
    SetupVar ( "rbot_aa_lby_breaker", false ); //
    SetupVar ( "rbot_aa_fake_lby_breaker", false );
    SetupVar ( "rbot_aa_lby_breaker_yaw", 0.f ); //
    SetupVar ( "rbot_aa_lby_breaker_freestanding", false );
    SetupVar ( "rbot_autostop", false );
    SetupVar ( "rbot_autocrouch", false );
    /*
    	- Fake break
    	- pre break
    */
    SetupVar ( "rbot_aa_spinbot_speed", 5.f );
    SetupVar ( "rbot_aa_stand_pitch", 0 );
    SetupVar ( "rbot_aa_move_pitch", 0 );
    SetupVar ( "rbot_aa_air_pitch", 0 );
    SetupVar ( "rbot_aa_stand_pitch_custom", 0.f );
    SetupVar ( "rbot_aa_move_pitch_custom", 0.f );
    SetupVar ( "rbot_aa_air_pitch_custom", 0.f );
    SetupVar ( "rbot_aa_stand_real_yaw", 0 );
    SetupVar ( "rbot_aa_move_real_yaw", 0 );
    SetupVar ( "rbot_aa_air_real_yaw", 0 );
    SetupVar ( "rbot_aa_stand_fake_yaw", 0 );
    SetupVar ( "rbot_aa_move_fake_yaw", 0 );
    SetupVar ( "rbot_aa_air_fake_yaw", 0 );
    SetupVar ( "rbot_aa_stand_real_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_move_real_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_air_real_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_stand_fake_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_move_fake_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_air_fake_yaw_custom", 0.f );
    SetupVar ( "rbot_aa_stand_real_add_yaw", 0 );
    SetupVar ( "rbot_aa_move_real_add_yaw", 0 );
    SetupVar ( "rbot_aa_air_real_add_yaw", 0 );
    SetupVar ( "rbot_aa_stand_fake_add_yaw", 0 );
    SetupVar ( "rbot_aa_move_fake_add_yaw", 0 );
    SetupVar ( "rbot_aa_air_fake_add_yaw", 0 );
    SetupVar ( "rbot_aa_stand_real_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_move_real_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_air_real_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_stand_fake_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_move_fake_add_yaw_add_range", 0.f );
    SetupVar ( "rbot_aa_air_fake_add_yaw_add_range", 0.f );

    //SetupVar("rbot_head_scale", 65.f);
    //SetupVar("rbot_body_scale", 72.f);

    SetupVar ( "rbot_hitbox_head_scale", 0.f );
    SetupVar ( "rbot_hitbox_neck_scale", 0.f );
    SetupVar ( "rbot_hitbox_chest_scale", 0.f );
    SetupVar ( "rbot_hitbox_pelvis_scale", 0.f );
    SetupVar ( "rbot_hitbox_stomach_scale", 0.f );
    SetupVar ( "rbot_hitbox_arm_scale", 0.f );
    SetupVar ( "rbot_hitbox_leg_scale", 0.f );
    SetupVar ( "rbot_hitbox_foot_scale", 0.f );

    SetupVar ( "rbot_aa_fakewalk_key", 0x0 );
    // hitboxes
    SetupVar ( "rbot_hitbox_head", false );
    SetupVar ( "rbot_hitbox_neck", false );
    SetupVar ( "rbot_hitbox_chest", false );
    SetupVar ( "rbot_hitbox_pelvis", false );
    SetupVar ( "rbot_hitbox_stomach", false );
    SetupVar ( "rbot_hitbox_arm", false );
    SetupVar ( "rbot_hitbox_leg", false );
    SetupVar ( "rbot_hitbox_foot", false );

    //Lbot
    SetupVar ( "lbot", false );
    SetupVar ( "lbot_legitaa", false );
    SetupVar ( "lbot_legitaa_mode", 0 );
    SetupVar ( "lbot_legitaa_yaw", 0.f );
    SetupVar ( "lbot_aimkey", 0x0 );
    SetupVar ( "lbot_backtrack", false );
    SetupVar ( "lbot_backtrack_aim", false );
    SetupVar ( "lbot_backtrack_ms", 0.2f );

    SetupVar ( "lbot_pistol_hitbox_head", false );
    SetupVar ( "lbot_pistol_hitbox_neck", false );
    SetupVar ( "lbot_pistol_hitbox_chest", false );
    SetupVar ( "lbot_pistol_hitbox_pelvis", false );
    SetupVar ( "lbot_pistol_hitbox_stomach", false );
    SetupVar ( "lbot_pistol_hitbox_arm", false );
    SetupVar ( "lbot_pistol_hitbox_leg", false );
    SetupVar ( "lbot_pistol_hitbox_foot", false );
    SetupVar ( "lbot_pistol_fov", 0.f );
    SetupVar ( "lbot_pistol_smooth", 1.f );
    SetupVar ( "lbot_pistol_randomize", 0.f );
    SetupVar ( "lbot_pistol_rcs", false );
    SetupVar ( "lbot_pistol_rcs_x", 0.f );
    SetupVar ( "lbot_pistol_rcs_y", 0.f );
    SetupVar ( "lbot_pistol_delay", 0.f );
    SetupVar ( "lbot_pistol_flickbot", false );

    SetupVar ( "lbot_smg_hitbox_head", false );
    SetupVar ( "lbot_smg_hitbox_neck", false );
    SetupVar ( "lbot_smg_hitbox_chest", false );
    SetupVar ( "lbot_smg_hitbox_pelvis", false );
    SetupVar ( "lbot_smg_hitbox_stomach", false );
    SetupVar ( "lbot_smg_hitbox_arm", false );
    SetupVar ( "lbot_smg_hitbox_leg", false );
    SetupVar ( "lbot_smg_hitbox_foot", false );
    SetupVar ( "lbot_smg_fov", 0.f );
    SetupVar ( "lbot_smg_smooth", 1.f );
    SetupVar ( "lbot_smg_randomize", 0.f );
    SetupVar ( "lbot_smg_rcs", false );
    SetupVar ( "lbot_smg_rcs_x", 0.f );
    SetupVar ( "lbot_smg_rcs_y", 0.f );
    SetupVar ( "lbot_smg_delay", 0.f );
    SetupVar ( "lbot_smg_flickbot", false );

    SetupVar ( "lbot_mg_hitbox_head", false );
    SetupVar ( "lbot_mg_hitbox_neck", false );
    SetupVar ( "lbot_mg_hitbox_chest", false );
    SetupVar ( "lbot_mg_hitbox_pelvis", false );
    SetupVar ( "lbot_mg_hitbox_stomach", false );
    SetupVar ( "lbot_mg_hitbox_arm", false );
    SetupVar ( "lbot_mg_hitbox_leg", false );
    SetupVar ( "lbot_mg_hitbox_foot", false );
    SetupVar ( "lbot_mg_fov", 0.f );
    SetupVar ( "lbot_mg_smooth", 1.f );
    SetupVar ( "lbot_mg_randomize", 0.f );
    SetupVar ( "lbot_mg_rcs", false );
    SetupVar ( "lbot_mg_rcs_x", 0.f );
    SetupVar ( "lbot_mg_rcs_y", 0.f );
    SetupVar ( "lbot_mg_delay", 0.f );
    SetupVar ( "lbot_mg_flickbot", false );

    SetupVar ( "lbot_rifle_hitbox_head", false );
    SetupVar ( "lbot_rifle_hitbox_neck", false );
    SetupVar ( "lbot_rifle_hitbox_chest", false );
    SetupVar ( "lbot_rifle_hitbox_pelvis", false );
    SetupVar ( "lbot_rifle_hitbox_stomach", false );
    SetupVar ( "lbot_rifle_hitbox_arm", false );
    SetupVar ( "lbot_rifle_hitbox_leg", false );
    SetupVar ( "lbot_rifle_hitbox_foot", false );
    SetupVar ( "lbot_rifle_fov", 0.f );
    SetupVar ( "lbot_rifle_smooth", 1.f );
    SetupVar ( "lbot_rifle_randomize", 0.f );
    SetupVar ( "lbot_rifle_rcs", false );
    SetupVar ( "lbot_rifle_rcs_x", 0.f );
    SetupVar ( "lbot_rifle_rcs_y", 0.f );
    SetupVar ( "lbot_rifle_delay", 0.f );
    SetupVar ( "lbot_rifle_flickbot", false );

    SetupVar ( "lbot_shotgun_hitbox_head", false );
    SetupVar ( "lbot_shotgun_hitbox_neck", false );
    SetupVar ( "lbot_shotgun_hitbox_chest", false );
    SetupVar ( "lbot_shotgun_hitbox_pelvis", false );
    SetupVar ( "lbot_shotgun_hitbox_stomach", false );
    SetupVar ( "lbot_shotgun_hitbox_arm", false );
    SetupVar ( "lbot_shotgun_hitbox_leg", false );
    SetupVar ( "lbot_shotgun_hitbox_foot", false );
    SetupVar ( "lbot_shotgun_fov", 0.f );
    SetupVar ( "lbot_shotgun_smooth", 1.f );
    SetupVar ( "lbot_shotgun_randomize", 0.f );
    SetupVar ( "lbot_shotgun_rcs", false );
    SetupVar ( "lbot_shotgun_rcs_x", 0.f );
    SetupVar ( "lbot_shotgun_rcs_y", 0.f );
    SetupVar ( "lbot_shotgun_delay", 0.f );
    SetupVar ( "lbot_shotgun_flickbot", false );

    SetupVar ( "lbot_sniper_hitbox_head", false );
    SetupVar ( "lbot_sniper_hitbox_neck", false );
    SetupVar ( "lbot_sniper_hitbox_chest", false );
    SetupVar ( "lbot_sniper_hitbox_pelvis", false );
    SetupVar ( "lbot_sniper_hitbox_stomach", false );
    SetupVar ( "lbot_sniper_hitbox_arm", false );
    SetupVar ( "lbot_sniper_hitbox_leg", false );
    SetupVar ( "lbot_sniper_hitbox_foot", false );
    SetupVar ( "lbot_sniper_fov", 0.f );
    SetupVar ( "lbot_sniper_smooth", 1.f );
    SetupVar ( "lbot_sniper_randomize", 0.f );
    SetupVar ( "lbot_sniper_rcs", false );
    SetupVar ( "lbot_sniper_rcs_x", 0.f );
    SetupVar ( "lbot_sniper_rcs_y", 0.f );
    SetupVar ( "lbot_sniper_delay", 0.f );
    SetupVar ( "lbot_sniper_flickbot", false );
}

void ConfigSystem::SetupVar ( std::string name, int value )
{
    intOptions[name] = value;
}

void ConfigSystem::SetupVar ( std::string name, float value )
{
    floatOptions[name] = value;
}

void ConfigSystem::SetupVar ( std::string name, bool value )
{
    boolOptions[name] = value;
}

void ConfigSystem::SetupVar ( std::string name, Color value )
{
    colorOptions[name] = value;
}
ConfigSystem g_Config;