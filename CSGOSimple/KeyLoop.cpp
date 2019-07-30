#include "KeyLoop.h"
#include "ConfigSystem.h"
#include "helpers/input.hpp"
#include "RuntimeSaver.h"
#include "Rbot.h"
#include "AntiAim.h"
#include "Logger.h"
#include "helpers/ThreadTools.h"
#include "Settings.h"
#include "features/EventLogger.h"
#include <thread>
#include "helpers/utils.hpp"

unsigned TestThread(void* pParam)
{
	auto id = GetCurrentThreadId();
	EventLogger::Get().AddEvent("DEBUG", "Calling from thread " + std::to_string(id), Color::Black);
	
	//EventLogger::Get().AddEvent("DEBUG", std::to_string(strValueOfBinDir.size()), Color::Black);
	return 0;
}


void KeyLoop::OnCreateMove()
{
	if(InputSys::Get().WasKeyPressed(Settings::Visual::ThirdPersonHotkey))
		Settings::Visual::ThirdPersonEnabled = !Settings::Visual::ThirdPersonEnabled;

	if ( Settings::RageBot::Enabled && InputSys::Get().WasKeyPressed(Settings::RageBot::BAimHotkey) )
		g_Saver.ForceBAim = !g_Saver.ForceBAim;

/*	if (InputSys::Get().WasKeyPressed(Settings::RageBot::DesyncFlipHotkey))
	{
		AntiAim::Get().DesyncFlip = !AntiAim::Get().DesyncFlip;
	}*/
		
	if( (Settings::RageBot::AntiAimSettings[0].Yaw == 5 || Settings::RageBot::AntiAimSettings[1].Yaw == 5) && InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAARightKey) )
    {
		if ( Settings::RageBot::ManualAAState == 1 )
			Settings::RageBot::ManualAAState = 0;
        else
			Settings::RageBot::ManualAAState = 1;
        g_Saver.CurrentShouldSkipAnimations = true;
    }

	if (InputSys::Get().WasKeyPressed(VK_F3))
	{
		ThreadHandle_t h;
		h = CreateSimpleThread_h(TestThread, NULL, false, &h);
	}

	if ((Settings::RageBot::AntiAimSettings[0].Yaw == 5 || Settings::RageBot::AntiAimSettings[1].Yaw == 5) && InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAALeftKey) )
    {
		if ( Settings::RageBot::ManualAAState == 2 )
			Settings::RageBot::ManualAAState = 0;
        else
			Settings::RageBot::ManualAAState = 2;
        g_Saver.CurrentShouldSkipAnimations = true;
    }

	if((Settings::RageBot::AntiAimSettings[0].Yaw == 5 || Settings::RageBot::AntiAimSettings[1].Yaw == 5) && InputSys::Get().WasKeyPressed(Settings::RageBot::ManualAABackKey) )
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