
#include "KeyLoop.h"
#include "ConfigSystem.h"
#include "helpers/input.hpp"
#include "RuntimeSaver.h"
#include "Rbot.h"
#include "Logger.h"
#include "Settings.h"

void KeyLoop::OnCreateMove()
{
    //if ( InputSys::Get().WasKeyPressed(g_Config.GetInt("vis_misc_thirdperson_hotkey")) )
	if(InputSys::Get().WasKeyPressed(Settings::Visual::ThirdPersonHotkey))
    {
        //g_Config.Set("vis_misc_thirdperson", !g_Config.GetBool("vis_misc_thirdperson"));
		Settings::Visual::ThirdPersonEnabled = !Settings::Visual::ThirdPersonEnabled;
    }

	// FIXME: 3:
	//if (InputSys::Get().WasKeyPressed(g_Config.GetInt("rbot_forceaim_hotkey")))
	if ( InputSys::Get().WasKeyPressed(Settings::RageBot::BAimHotkey) )
	{
		Rbot::Get().ForceBAim = !Rbot::Get().ForceBAim;
		g_Logger.Info("INFO", "ForceBAim is " + std::to_string(Rbot::Get().ForceBAim));
	}
		

    //if (InputSys::Get().WasKeyPressed(g_Config.GetInt("rbot_manual_key_right")))
	if( InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAARightKey) )
    {
        //if(g_Config.GetInt("rbot_manual_aa_state") == 1)
		if ( Settings::RageBot::ManualAAState == 1 )
        {
            //g_Config.Set("rbot_manual_aa_state", 0);
			Settings::RageBot::ManualAAState = 0;
        }
        else
        {
            //g_Config.Set("rbot_manual_aa_state", 1);
			Settings::RageBot::ManualAAState = 1;
        }
        g_Saver.CurrentShouldSkipAnimations = true;
    }
    //if (InputSys::Get().WasKeyPressed(g_Config.GetInt("rbot_manual_key_left")))
	if ( InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAALeftKey) )
    {
        //if (g_Config.GetInt("rbot_manual_aa_state") == 2)
		if ( Settings::RageBot::ManualAAState == 2 )
        {
            //g_Config.Set("rbot_manual_aa_state", 0);
			Settings::RageBot::ManualAAState = 0;
        }
        else
        {
           // g_Config.Set("rbot_manual_aa_state", 2);
			Settings::RageBot::ManualAAState = 2;
        }
        g_Saver.CurrentShouldSkipAnimations = true;
    }
    //if (InputSys::Get().WasKeyPressed(g_Config.GetInt("rbot_manual_key_back")))
	if( InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAABackKey) )
    {
        //if (g_Config.GetInt("rbot_manual_aa_state") == 3)
		if(Settings::RageBot::ManualAAState == 3)
        {
            //g_Config.Set("rbot_manual_aa_state", 0);
			Settings::RageBot::ManualAAState = 0;
        }
        else
        {
			Settings::RageBot::ManualAAState = 3;
            //g_Config.Set("rbot_manual_aa_state", 3);
        }
        g_Saver.CurrentShouldSkipAnimations = true;
    }
}