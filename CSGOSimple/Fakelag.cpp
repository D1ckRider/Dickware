
#include "Fakelag.h"
#include "ConfigSystem.h"
#include "RuntimeSaver.h"
#include "ConsoleHelper.h"
#include "Settings.h"

void Fakelag::OnCreateMove(CUserCmd* cmd, bool& bSendPacket)
{
    if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
        return;

    int choked_commands = g_ClientState->chokedcommands + 1;
    static bool WasLastInFakelag = false;

    bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1f || (cmd->sidemove != 0.f || cmd->forwardmove != 0.f);
    bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
    bool Standing = !Moving && !InAir;
    int ticks = 0;
    int mode = 0;
    if (Standing)
		ticks = Settings::RageBot::AntiAimSettings[Settings::RageBot::AntiAimType::STANDING].FakelagTicks;
    else if (InAir)
    {
		ticks = Settings::RageBot::AntiAimSettings[Settings::RageBot::AntiAimType::AIR].FakelagTicks;
		mode = Settings::RageBot::AntiAimSettings[Settings::RageBot::AntiAimType::AIR].FakelagMode;
    }
    else
    {
		ticks = Settings::RageBot::AntiAimSettings[Settings::RageBot::AntiAimType::MOVING].FakelagTicks;
		mode = Settings::RageBot::AntiAimSettings[Settings::RageBot::AntiAimType::MOVING].FakelagMode;
    }

    if (ticks == 0)
    {
        g_Saver.FakelagCurrentlyEnabled = false;
        g_Saver.FakelagData.ang = g_Saver.AARealAngle;
        return;
    }

    g_Saver.FakelagCurrentlyEnabled = true;

    switch (mode)
    {
        case 0:
        {
            if (choked_commands <= ticks)
            {
                WasLastInFakelag = true;
                bSendPacket = false;
            }
            else
                WasLastInFakelag = false;
            break;
        }
        case 1:
        {
            int PacketsToChoke = 0;
            if (g_LocalPlayer->m_vecVelocity().Length() > 0.f)
            {
                PacketsToChoke = (int)(64.f / g_GlobalVars->interval_per_tick / g_LocalPlayer->m_vecVelocity().Length()) + 1;
                if (PacketsToChoke >= 16)
                    PacketsToChoke = 15;

                if (PacketsToChoke >= ticks)
                    PacketsToChoke = ticks;
            }

            if (choked_commands <= PacketsToChoke)
            {
                WasLastInFakelag = true;
                bSendPacket = false;
            }
            else
                WasLastInFakelag = false;
        }
    }

    if (g_ClientState->chokedcommands == 0)
    {
        g_Saver.LCbroken = (g_LocalPlayer->m_vecOrigin() - g_Saver.FakelagData.pos).LengthSqr() > 4096.f;
        g_Saver.FakelagData.pos = g_LocalPlayer->m_vecOrigin();
        g_Saver.FakelagData.ang = g_Saver.AARealAngle;
    }
}
