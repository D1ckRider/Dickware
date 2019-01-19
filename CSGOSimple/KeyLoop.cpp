
#include "KeyLoop.h"
#include "ConfigSystem.h"
#include "helpers/input.hpp"
#include "RuntimeSaver.h"
#include "Rbot.h"
#include "Logger.h"

void KeyLoop::OnCreateMove()
{
    if(InputSys::Get().WasKeyPressed(g_Config.GetInt("vis_misc_thirdperson_hotkey")))
    {
        g_Config.Set("vis_misc_thirdperson", !g_Config.GetBool("vis_misc_thirdperson"));
    }

	// FIXME: 3:
	if (InputSys::Get().WasKeyPressed(g_Config.GetInt("rbot_forceaim_hotkey")))
	{
		Rbot::Get().ForceBAim = !Rbot::Get().ForceBAim;
		g_Logger.Info("INFO", "ForceBAim is " + std::to_string(Rbot::Get().ForceBAim));
	}
		

    if (InputSys::Get().WasKeyPressed(g_Config.GetInt("rbot_manual_key_right")))
    {
        if(g_Config.GetInt("rbot_manual_aa_state") == 1)
        {
            g_Config.Set("rbot_manual_aa_state", 0);
        }
        else
        {
            g_Config.Set("rbot_manual_aa_state", 1);
        }
        g_Saver.CurrentShouldSkipAnimations = true;
    }
    if (InputSys::Get().WasKeyPressed(g_Config.GetInt("rbot_manual_key_left")))
    {
        if (g_Config.GetInt("rbot_manual_aa_state") == 2)
        {
            g_Config.Set("rbot_manual_aa_state", 0);
        }
        else
        {
            g_Config.Set("rbot_manual_aa_state", 2);
        }
        g_Saver.CurrentShouldSkipAnimations = true;
    }
    if (InputSys::Get().WasKeyPressed(g_Config.GetInt("rbot_manual_key_back")))
    {
        if (g_Config.GetInt("rbot_manual_aa_state") == 3)
        {
            g_Config.Set("rbot_manual_aa_state", 0);
        }
        else
        {
            g_Config.Set("rbot_manual_aa_state", 3);
        }
        g_Saver.CurrentShouldSkipAnimations = true;
    }
}