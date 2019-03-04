
#include "KeyLoop.h"
#include "ConfigSystem.h"
#include "helpers/input.hpp"
#include "RuntimeSaver.h"
#include "Rbot.h"
#include "AntiAim.h"
#include "Logger.h"
#include "Settings.h"

void KeyLoop::OnCreateMove()
{
	if(InputSys::Get().WasKeyPressed(Settings::Visual::ThirdPersonHotkey))
		Settings::Visual::ThirdPersonEnabled = !Settings::Visual::ThirdPersonEnabled;

	if ( InputSys::Get().WasKeyPressed(Settings::RageBot::BAimHotkey) )
		g_Saver.ForceBAim = !g_Saver.ForceBAim;

/*	if (InputSys::Get().WasKeyPressed(Settings::RageBot::DesyncFlipHotkey))
	{
		AntiAim::Get().DesyncFlip = !AntiAim::Get().DesyncFlip;
	}*/
		
	if( InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAARightKey) )
    {
		if ( Settings::RageBot::ManualAAState == 1 )
			Settings::RageBot::ManualAAState = 0;
        else
			Settings::RageBot::ManualAAState = 1;
        g_Saver.CurrentShouldSkipAnimations = true;
    }

	if ( InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAALeftKey) )
    {
		if ( Settings::RageBot::ManualAAState == 2 )
			Settings::RageBot::ManualAAState = 0;
        else
			Settings::RageBot::ManualAAState = 2;
        g_Saver.CurrentShouldSkipAnimations = true;
    }

	if( InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAABackKey) )
    {
		if(Settings::RageBot::ManualAAState == 3)
			Settings::RageBot::ManualAAState = 0;
        else
			Settings::RageBot::ManualAAState = 3;

        g_Saver.CurrentShouldSkipAnimations = true;
    }

	if (InputSys::Get().WasKeyPressed(Settings::Misc::RadioPauseHotkey))
		g_Saver.RadioPaused = !g_Saver.RadioPaused;
}