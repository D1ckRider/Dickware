
#include "bhop.hpp"
#include "../helpers/math.hpp"
#include "../MovementFix.h"
#include "../Settings.h"

void BunnyHop::OnCreateMove(CUserCmd* cmd)
{
    static bool jumped_last_tick = false;
    static bool should_fake_jump = false;

    if (!jumped_last_tick && should_fake_jump)
    {
        should_fake_jump = false;
        cmd->buttons |= IN_JUMP;
    }
    else if (cmd->buttons & IN_JUMP || should_fake_jump)
    {
        if (should_fake_jump)
        {
            cmd->buttons |= IN_JUMP;
            jumped_last_tick = true;
            should_fake_jump = false;
        }
        else if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
        {
            jumped_last_tick = true;
            should_fake_jump = true;
            cmd->buttons |= IN_JUMP;
        }
        else
        {
            cmd->buttons &= ~IN_JUMP;
            jumped_last_tick = false;
        }
    }
    else
    {
        jumped_last_tick = false;
        should_fake_jump = false;
    }
}

void BunnyHop::AutoStrafe(CUserCmd* cmd, QAngle va)
{
    if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
        return;

    static bool leftRight;
    bool inMove = cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT;

    if (cmd->buttons & IN_FORWARD && g_LocalPlayer->m_vecVelocity().Length() <= 50.0f)
    {
        cmd->forwardmove = 250.0f;
    }

    float yaw_change = 0.0f;
    if (g_LocalPlayer->m_vecVelocity().Length() > 50.f)
    {
        yaw_change = 30.0f * fabsf(30.0f / g_LocalPlayer->m_vecVelocity().Length());
    }

    C_BaseCombatWeapon* ActiveWeapon = g_LocalPlayer->m_hActiveWeapon();
    if (ActiveWeapon && ActiveWeapon->CanFire() && cmd->buttons & IN_ATTACK)
    {
        yaw_change = 0.0f;
    }

    QAngle viewAngles = va;

    bool OnGround = (g_LocalPlayer->m_fFlags() & FL_ONGROUND);
    if (!OnGround && !inMove)
    {
        if (leftRight || cmd->mousedx > 1)
        {
            viewAngles.yaw += yaw_change;
            cmd->sidemove = 350.0f;
        }
        else if (!leftRight || cmd->mousedx < 1)
        {
            viewAngles.yaw -= yaw_change;
            cmd->sidemove = -350.0f;
        }

        leftRight = !leftRight;
    }
    viewAngles.Normalize();
    Math::ClampAngles(viewAngles);
    MovementFix::Get().Correct(viewAngles, cmd, cmd->forwardmove, cmd->sidemove);
}

template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}


void BunnyHop::AutoStrafe(CUserCmd* cmd)
{
	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || !g_LocalPlayer->IsAlive()) return;
	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER) return;


	// If we're not jumping or want to manually move out of the way/jump over an obstacle don't strafe.
	//if (!g_InputSystem->IsButtonDown(ButtonCode_t::KEY_SPACE) ||
	//	g_InputSystem->IsButtonDown(ButtonCode_t::KEY_A) ||
	//	g_InputSystem->IsButtonDown(ButtonCode_t::KEY_D) ||
	//	g_InputSystem->IsButtonDown(ButtonCode_t::KEY_S) ||
	//	g_InputSystem->IsButtonDown(ButtonCode_t::KEY_W))
	//	return;

	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) 
	{
		if (cmd->mousedx > 1 || cmd->mousedx < -1) {
			cmd->sidemove = clamp(cmd->mousedx < 0.f ? -400.f : 400.f, -400, 400);
		}
		else {
			if (g_LocalPlayer->m_vecVelocity().Length2D() == 0 || g_LocalPlayer->m_vecVelocity().Length2D() == NAN || g_LocalPlayer->m_vecVelocity().Length2D() == INFINITE)
			{
				cmd->forwardmove = 400;
				return;
			}
			cmd->forwardmove = clamp(5850.f / g_LocalPlayer->m_vecVelocity().Length2D(), -400, 400);
			if (cmd->forwardmove < -400 || cmd->forwardmove > 400)
				cmd->forwardmove = 0;
			cmd->sidemove = clamp((cmd->command_number % 2) == 0 ? -400.f : 400.f, -400, 400);
			if (cmd->sidemove < -400 || cmd->sidemove > 400)
				cmd->sidemove = 0;
		}
	}
}
