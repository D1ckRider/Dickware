
#include "Fakelag.h"
#include "helpers/math.hpp"
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

	auto LegitPeek = [ticks](CUserCmd* cmd, bool* send_packet) {
		static bool m_bIsPeeking = false;
		if (m_bIsPeeking) {
			*send_packet = !(g_ClientState->chokedcommands < ticks);
			if (*send_packet)
				m_bIsPeeking = false;
			return;
		}

		auto speed = g_LocalPlayer->m_vecVelocity().Length();
		if (speed <= 100.0f)
			return;

		auto collidable = g_LocalPlayer->GetCollideable();

		Vector min, max;
		min = collidable->OBBMins();
		max = collidable->OBBMaxs();

		min += g_LocalPlayer->m_vecOrigin();
		max += g_LocalPlayer->m_vecOrigin();

		Vector center = (min + max) * 0.5f;

		for (int i = 1; i <= g_GlobalVars->maxClients; ++i) {
			auto player = C_BasePlayer::GetPlayerByIndex(i);
			if (!player || !player->IsAlive() || player->IsDormant())
				continue;
			if (player == g_LocalPlayer || g_LocalPlayer->m_iTeamNum() == player->m_iTeamNum())
				continue;

			auto weapon = player->m_hActiveWeapon().Get();
			if (!weapon || weapon->m_iClip1() <= 0)
				continue;

			auto weapon_data = weapon->GetCSWeaponData();
			if (!weapon_data || weapon_data->WeaponType <= WEAPONTYPE_KNIFE || weapon_data->WeaponType >= WEAPONTYPE_C4)
				continue;

			auto eye_pos = player->GetEyePos();

			Vector direction;
			Math::AngleVectors(player->m_angEyeAngles(), direction);
			direction.NormalizeInPlace();

			Vector hit_point;
			bool hit = Math::IntersectionBoundingBox(eye_pos, direction, min, max, &hit_point);
			if (hit && eye_pos.DistTo(hit_point) <= weapon_data->flRange) {
				Ray_t ray;
				trace_t tr;
				CTraceFilterSkipEntity filter((C_BasePlayer*)player);
				ray.Init(eye_pos, hit_point);

				g_EngineTrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &tr);
				if (tr.contents & CONTENTS_WINDOW) { // skip windows
																							// at this moment, we dont care about local player
					filter.pSkip = tr.hit_entity;
					ray.Init(tr.endpos, hit_point);
					g_EngineTrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &tr);
				}

				if (tr.fraction == 1.0f || tr.hit_entity == g_LocalPlayer) {
					m_bIsPeeking = true;
					break;
				}
			}
		}
	};

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
			break;
        }
		case 2:
			LegitPeek(cmd, &bSendPacket);
			break;
    }

    if (g_ClientState->chokedcommands == 0)
    {
        g_Saver.LCbroken = (g_LocalPlayer->m_vecOrigin() - g_Saver.FakelagData.pos).LengthSqr() > 4096.f;
        g_Saver.FakelagData.pos = g_LocalPlayer->m_vecOrigin();
        g_Saver.FakelagData.ang = g_Saver.AARealAngle;
    }
}
