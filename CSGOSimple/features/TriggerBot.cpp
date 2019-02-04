#include "TriggerBot.h"
#include "..\Settings.h"
#include "..\helpers\input.hpp"

void TriggerBot::OnCreateMove(CUserCmd * cmd)
{
	if (g_LocalPlayer && g_LocalPlayer->IsAlive())
	{
		C_BaseCombatWeapon* pWeapon = g_LocalPlayer->m_hActiveWeapon();
		if (pWeapon)
		{
			Vector src, dst, forward;
			trace_t tr;
			Ray_t ray;
			CTraceFilter filter;

			QAngle viewangle = cmd->viewangles;

			viewangle += g_LocalPlayer->m_aimPunchAngle() * 2.f;

			Math::AngleVectors(viewangle, forward);

			forward *= pWeapon->GetCSWeaponData()->flRange;
			filter.pSkip = g_LocalPlayer;
			src = g_LocalPlayer->GetEyePos();
			dst = src + forward;
			ray.Init(src, dst);

			g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);

			if (!tr.hit_entity)
				return;

			int hitgroup = tr.hitgroup;
			bool didHit = false;
			/*if ((head && tr.hitgroup == 1)
				|| (chest && tr.hitgroup == 2)
				|| (stomach && tr.hitgroup == 3)
				|| (arms && (tr.hitgroup == 4 || tr.hitgroup == 5))
				|| (legs && (tr.hitgroup == 6 || tr.hitgroup == 7)))*/
			if(tr.hitgroup > 0 && hitgroup <= 7)
			{
				didHit = true;
			}


			if (didHit && (tr.hit_entity->GetBaseEntity()->m_iTeamNum() != g_LocalPlayer->m_iTeamNum()))
			{
				//if (key > 0 && GetAsyncKeyState(key) & 0x8000 && enable)
				if ( Settings::TriggerBot::Enabled && InputSys::Get().IsKeyDown(Settings::TriggerBot::Key) )
				{
					cmd->buttons |= IN_ATTACK;
				}
			}

		}
	}
}
