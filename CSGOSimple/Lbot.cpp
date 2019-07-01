
#include "Lbot.h"
#include "helpers\math.hpp"
#include "Logger.h"
#include "ConsoleHelper.h"
#include "helpers\input.hpp"
#include "RuntimeSaver.h"
#include "Backtrack.h"
#include "Settings.h"
#include "Autowall.h"
#include "MovementFix.h"

float GetRealDistanceFOV(float distance, QAngle angle, QAngle viewangles)
{
	Vector aimingAt;
	Math::AngleVectors(viewangles, aimingAt);
	aimingAt *= distance;
	Vector aimAt;
	Math::AngleVectors(angle, aimAt);
	aimAt *= distance;
	return aimingAt.DistTo(aimAt) / 5;
}

void Lbot::OnCreateMove(CUserCmd* cmd)
{
	if (!IsEnabled(cmd)) 
	{
		if (g_LocalPlayer && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive() && Settings::Aimbot::Enabled && Settings::Aimbot::WeaponAimSetting[WeaponID].RCSType == 0)
		{
			auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
			if (pWeapon && (pWeapon->IsSniper() || pWeapon->IsPistol() || pWeapon->IsRifle())) 
			{
				RCS(cmd->viewangles, target, false);
				Math::FixAngles(cmd->viewangles);
				g_EngineClient->SetViewAngles(cmd->viewangles);
			}
		}
		else 
		{
			RCSLastPunch = { 0, 0, 0 };
		}

		is_delayed = false;
		shot_delay = false;
		kill_delay = false;
		silent_enabled = Settings::Aimbot::WeaponAimSetting[WeaponID].Silent && Settings::Aimbot::WeaponAimSetting[WeaponID].SilentFOV > 0;
		target = NULL;
		return;
	}

	RandomSeed(cmd->command_number);

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	auto weapon_data = weapon->GetCSWeaponData();
	if (!weapon_data)
		return;
	WeaponID = Settings::Aimbot::GetWeaponType(weapon);
	UpdateWeaponConfig(weapon);

	bool should_do_rcs = false;
	QAngle angles = cmd->viewangles;
	QAngle current = angles;
	float fov = 0.180f;
	if (!(Settings::Aimbot::FlashCheck && g_LocalPlayer->IsFlashed())) 
	{
		int bestBone = -1;
		if (GetClosestPlayer(cmd, bestBone)) 
		{
			Math::VectorAngles(target->GetHitboxPos(bestBone) - g_LocalPlayer->GetEyePos(), angles);
			Math::FixAngles(angles);
			if (Settings::Aimbot::WeaponAimSetting[WeaponID].FOVType == 1)
				fov = GetRealDistanceFOV(g_LocalPlayer->GetEyePos().DistTo(target->GetHitboxPos(bestBone)), angles, cmd->viewangles);
			else
				fov = GetFovToPlayer(cmd->viewangles, angles);

			should_do_rcs = true;

			if (!Settings::Aimbot::WeaponAimSetting[WeaponID].Silent && !is_delayed && !shot_delay && Settings::Aimbot::WeaponAimSetting[WeaponID].Delay > 0)
			{
				is_delayed = true;
				shot_delay = true;
				shot_delay_time = GetTickCount64() + Settings::Aimbot::WeaponAimSetting[WeaponID].Delay;
			}

			if (shot_delay && shot_delay_time <= GetTickCount64())
				shot_delay = false;

			if (shot_delay)
				cmd->buttons &= IN_ATTACK;

			if (Settings::Aimbot::WeaponAimSetting[WeaponID].Autofire)
			{
				if (!Settings::Aimbot::AutofireHotkey || InputSys::Get().IsKeyDown(Settings::Aimbot::AutofireHotkey))
				{
					if (!weapon_data->bFullAuto) 
					{
						if (cmd->command_number % 2 == 0)
							cmd->buttons &= IN_ATTACK;
						else 
							cmd->buttons |= ~IN_ATTACK;
					}
					else 
					{
						cmd->buttons |= ~IN_ATTACK;
					}
				}
			}

			/*if (settings.autostop) {
				cmd->forwardmove = cmd->sidemove = 0;
			}*/
		}
	}

	if (IsNotSilent(fov) && (should_do_rcs || Settings::Aimbot::WeaponAimSetting[WeaponID].RCSType == 0))
		RCS(angles, target, should_do_rcs);

	if (target && IsNotSilent(fov))
		Smooth(current, angles, angles);

	Math::FixAngles(angles);
	cmd->viewangles = angles;
	if (IsNotSilent(fov))
		g_EngineClient->SetViewAngles(angles);

	silent_enabled = false;
	if (g_LocalPlayer->m_hActiveWeapon()->IsPistol() && Settings::Aimbot::WeaponAimSetting[WeaponID].Autopistol)
	{
		float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
		float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;
		if (next_shot > 0)
			cmd->buttons &= ~IN_ATTACK;
	}

	Math::FixAngles(cmd->viewangles);
	cmd->viewangles.yaw = std::remainderf(cmd->viewangles.yaw, 360.0f);
}

bool Lbot::IsEnabled(CUserCmd* pCmd)
{
	if (!g_EngineClient->IsConnected() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return false;

	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	if (!pWeapon || pWeapon->IsKnife() || pWeapon->IsGrenade() || pWeapon->IsZeus()) 
		return false;

	auto weaponData = pWeapon->GetCSWeaponData();
	auto weapontype = weaponData->WeaponType;
	//settings = g_Options.legitbot_items[pWeapon->m_Item().m_iItemDefinitionIndex()];
	if (!Settings::Aimbot::Enabled)
		return false;

	//if ((pWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_AWP || pWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_SSG08) && settings.only_in_zoom && !g_LocalPlayer->m_bIsScoped()) 
	//	return false;

	if (!pWeapon->HasBullets() || pWeapon->IsReloading()) 
		return false;

	return InputSys::Get().IsKeyDown(Settings::Aimbot::Hotkey);
}

float Lbot::GetFovToPlayer(QAngle viewAngle, QAngle aimAngle)
{
	QAngle delta = aimAngle - viewAngle;
	Math::FixAngles(delta);
	return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
}

bool Lbot::IsRcs()
{
	return g_LocalPlayer->m_iShotsFired() >= Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_Bullet + 1;
}

float Lbot::GetSmooth()
{
	float smooth = IsRcs() && Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_SmoothEnabled ? Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_Smooth 
																							 : Settings::Aimbot::WeaponAimSetting[WeaponID].Smooth;
	return smooth;
}

float Lbot::GetFov()
{
	if (IsRcs() && Settings::Aimbot::WeaponAimSetting[WeaponID].RCS && Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_FOVEnabled) return Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_FOV;
	if (!Settings::Aimbot::WeaponAimSetting[WeaponID].Silent) return Settings::Aimbot::WeaponAimSetting[WeaponID].FOV;
	return Settings::Aimbot::WeaponAimSetting[WeaponID].SilentFOV > Settings::Aimbot::WeaponAimSetting[WeaponID].FOV ? Settings::Aimbot::WeaponAimSetting[WeaponID].SilentFOV
																													 : Settings::Aimbot::WeaponAimSetting[WeaponID].FOV;
}

void Lbot::UpdateWeaponConfig(C_BaseCombatWeapon* weapon)
{
	//int WeaponID = Settings::Aimbot::GetWeaponType(weapon);

	/*WeaponFov = Settings::Aimbot::WeaponAimSetting[WeaponID].FOV;
	WeaponSmoothType = Settings::Aimbot::WeaponAimSetting[WeaponID].SmoothType;
	WeaponFOVType = Settings::Aimbot::WeaponAimSetting[WeaponID].FOVType;
	WeaponSmooth = Settings::Aimbot::WeaponAimSetting[WeaponID].Smooth;
	WeaponSilent = Settings::Aimbot::WeaponAimSetting[WeaponID].Silent;
	WeaponSilentFOV = Settings::Aimbot::WeaponAimSetting[WeaponID].SilentFOV;
	WeaponRandomness = Settings::Aimbot::WeaponAimSetting[WeaponID].Randomize;
	WeaponDelay = Settings::Aimbot::WeaponAimSetting[WeaponID].Delay;
	WeaponKillDelay = Settings::Aimbot::WeaponAimSetting[WeaponID].KillDelay;
	WeaponAutopistol = Settings::Aimbot::WeaponAimSetting[WeaponID].Autopistol;
	WeaponAutowall = Settings::Aimbot::WeaponAimSetting[WeaponID].AutowallEnabled;
	WeaponHitbox = Settings::Aimbot::WeaponAimSetting[WeaponID].Hitbox;
	WeaponAimType = Settings::Aimbot::WeaponAimSetting[WeaponID].AimType;
	WeaponMinDamage = Settings::Aimbot::WeaponAimSetting[WeaponID].MinDamage;
	WeaponAutoFire = Settings::Aimbot::WeaponAimSetting[WeaponID].Autofire;

	WeaponRcs = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS;
	WeaponRCSBullet = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_Bullet;
	WeaponRCSType = Settings::Aimbot::WeaponAimSetting[WeaponID].RCSType;
	WeaponRecoilX = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_X;
	WeaponRecoilY = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_Y;
	WeaponRCSFOVEnabled = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_FOVEnabled;
	WeaponRCSFOV = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_FOV;
	WeaponRCSSmoothEnabled = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_SmoothEnabled;
	WeaponRCSSmooth = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_Smooth;*/
}

void Lbot::RCS(QAngle& angle, C_BasePlayer* target, bool should_run)
{
	if (!Settings::Aimbot::WeaponAimSetting[WeaponID].RCS)
	{
		RCSLastPunch.Init();
		return;
	}

	if (Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_X == 0 
		&& Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_Y == 0)
	{
		RCSLastPunch.Init();
		return;
	}

	QAngle punch = g_LocalPlayer->m_aimPunchAngle() * 2.0f;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (weapon && weapon->m_flNextPrimaryAttack() > g_GlobalVars->curtime) 
	{
		auto delta_angles = punch - RCSLastPunch;
		auto delta = weapon->m_flNextPrimaryAttack() - g_GlobalVars->curtime;
		if (delta >= g_GlobalVars->interval_per_tick)
			punch = RCSLastPunch + delta_angles / static_cast<float>(TIME_TO_TICKS(delta));
	}

	CurrentPunch = punch;
	if (Settings::Aimbot::WeaponAimSetting[WeaponID].RCSType == 0 && !should_run)
		punch -= { RCSLastPunch.pitch, RCSLastPunch.yaw, 0.f };

	RCSLastPunch = CurrentPunch;
	if (!IsRcs())
		return;

	angle.pitch -= punch.pitch * (Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_X / 100.0f);
	angle.yaw -= punch.yaw * (Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_Y / 100.0f);

	Math::FixAngles(angle);
}

bool Lbot::IsLineGoesThroughSmoke(Vector vStartPos, Vector vEndPos)
{
	return Utils::LineThroughSmoke(vStartPos, vEndPos);
}

void Lbot::Smooth(QAngle currentAngle, QAngle aimAngle, QAngle& angle)
{
	auto smooth_value = GetSmooth();
	if (smooth_value <= 1)
		return;

	QAngle delta = aimAngle - currentAngle;
	Math::FixAngles(delta);

	if (Settings::Aimbot::WeaponAimSetting[WeaponID].SmoothType == 1)
	{
		float deltaLength = fmaxf(sqrtf((delta.pitch * delta.pitch) + (delta.yaw * delta.yaw)), 0.01f);
		delta *= (1.0f / deltaLength);

		RandomSeed(g_GlobalVars->tickcount);
		float randomize = RandomFloat(-0.1f, 0.1f);
		smooth_value = fminf((g_GlobalVars->interval_per_tick * 64.0f) / (randomize + smooth_value * 0.15f), deltaLength);
	}
	else 
	{
		smooth_value = (g_GlobalVars->interval_per_tick * 64.0f) / smooth_value;
	}

	delta *= smooth_value;
	angle = currentAngle + delta;
	Math::FixAngles(angle);
}

bool Lbot::IsNotSilent(float fov)
{
	return IsRcs() || !Settings::Aimbot::WeaponAimSetting[WeaponID].Silent || 
						(Settings::Aimbot::WeaponAimSetting[WeaponID].Silent && fov > Settings::Aimbot::WeaponAimSetting[WeaponID].SilentFOV);
}

C_BasePlayer* Lbot::GetClosestPlayer(CUserCmd* cmd, int& bestBone)
{
	QAngle ang;
	Vector eVecTarget;
	Vector pVecTarget = g_LocalPlayer->GetEyePos();
	if (target && !kill_delay && Settings::Aimbot::WeaponAimSetting[WeaponID].KillDelay > 0 && target->IsNotTarget())
	{
		target = NULL;
		shot_delay = false;
		kill_delay = true;
		kill_delay_time = (int)GetTickCount64() + Settings::Aimbot::WeaponAimSetting[WeaponID].KillDelay;
	}
	if (kill_delay) 
	{
		if (kill_delay_time <= (int)GetTickCount64()) kill_delay = false;
		else return NULL;
	}

	C_BasePlayer* player;
	target = NULL;
	int bestHealth = 100.f;
	float bestFov = 9999.f;
	float bestDamage = 0.f;
	float bestBoneFov = 9999.f;
	float bestDistance = 9999.f;
	int health;
	float fov;
	float damage;
	float distance;
	int fromBone = Settings::Aimbot::WeaponAimSetting[WeaponID].AimType == 1 ? 0 : Settings::Aimbot::WeaponAimSetting[WeaponID].Hitbox;
	int toBone = Settings::Aimbot::WeaponAimSetting[WeaponID].AimType == 1 ? 7 : Settings::Aimbot::WeaponAimSetting[WeaponID].Hitbox;
	for (int i = 1; i < g_EngineClient->GetMaxClients(); ++i) 
	{
		damage = 0.f;
		player = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (player->IsNotTarget())
			continue;
		if (!Settings::Aimbot::DeathmatchMode && player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;
		for (int bone = fromBone; bone <= toBone; bone++) 
		{
			eVecTarget = player->GetHitboxPos(bone);
			Math::VectorAngles(eVecTarget - pVecTarget, ang);
			Math::FixAngles(ang);
			distance = pVecTarget.DistTo(eVecTarget);
			if (Settings::Aimbot::WeaponAimSetting[WeaponID].FOVType == 1)
				fov = GetRealDistanceFOV(distance, ang, cmd->viewangles + RCSLastPunch);
			else
				fov = GetFovToPlayer(cmd->viewangles + RCSLastPunch, ang);

			if (fov > GetFov())
				continue;

			if (!g_LocalPlayer->CanSeePlayer(player, eVecTarget)) 
			{
				if (!Settings::Aimbot::WeaponAimSetting[WeaponID].AutowallEnabled)
					continue;

				damage = Autowall::Get().CanHit(eVecTarget);
				if (damage < Settings::Aimbot::WeaponAimSetting[WeaponID].MinDamage)
					continue;

			}
			if ((Settings::Aimbot::Priority == 1 || Settings::Aimbot::Priority == 2) && damage == 0.f)
				damage = Autowall::Get().CanHit(eVecTarget);

			health = player->m_iHealth() - damage;
			if (Settings::Aimbot::SmokeCheck && IsLineGoesThroughSmoke(pVecTarget, eVecTarget))
				continue;

			bool OnGround = (g_LocalPlayer->m_fFlags() & FL_ONGROUND);
			if (Settings::Aimbot::JumpCheck && !OnGround)
				continue;

			if (Settings::Aimbot::WeaponAimSetting[WeaponID].AimType == 1 && bestBoneFov < fov)
				continue;


			bestBoneFov = fov;
			if (
				(Settings::Aimbot::Priority == 0 && bestFov > fov) ||
				(Settings::Aimbot::Priority == 1 && bestHealth > health) ||
				(Settings::Aimbot::Priority == 2 && bestDamage < damage) ||
				(Settings::Aimbot::Priority == 3 && distance < bestDistance)
				) 
			{
				bestBone = bone;
				target = player;
				bestFov = fov;
				bestHealth = health;
				bestDamage = damage;
				bestDistance = distance;
			}
		}
	}
	return target;
}

void Lbot::LegitAA(CUserCmd * cmd, bool & bSendPacket)
{
	QAngle OldAngles = cmd->viewangles;
	if (cmd->buttons & (IN_ATTACK | IN_ATTACK2 | IN_USE) ||
		g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP
		|| !g_LocalPlayer->IsAlive())
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	auto weapon_index = weapon->m_Item().m_iItemDefinitionIndex();
	if ((weapon_index == WEAPON_GLOCK || weapon_index == WEAPON_FAMAS) && weapon->m_flNextPrimaryAttack() >= g_GlobalVars->curtime)
		return;

	auto weapon_data = weapon->GetCSWeaponData();

	if (weapon_data->WeaponType == WEAPONTYPE_GRENADE) 
	{
		if (!weapon->m_bPinPulled()) 
		{
			float throwTime = weapon->m_fThrowTime();
			if (throwTime > 0.f)
				return;
		}

		if ((cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK2)) 
		{
			if (weapon->m_fThrowTime() > 0.f)
				return;
		}
	}

	static bool broke_lby = false;

	if (InputSys::Get().WasKeyPressed(Settings::Aimbot::AAFlipHotkey))
		Settings::Aimbot::AaSide = -Settings::Aimbot::AaSide;

	if (Settings::Aimbot::LegitAA == 1)
	{
		float minimal_move = 2.0f;
		if (g_LocalPlayer->m_fFlags() & FL_DUCKING)
			minimal_move *= 3.f;

		if (cmd->buttons & IN_WALK)
			minimal_move *= 3.f;

		bool should_move = g_LocalPlayer->m_vecVelocity().Length2D() <= 0.0f
			|| std::fabsf(g_LocalPlayer->m_vecVelocity().z) <= 100.0f;

		if ((cmd->command_number % 2) == 1) 
		{
			cmd->viewangles.yaw += 120.0f * side;
			if (should_move)
				cmd->sidemove -= minimal_move;
			bSendPacket = false;
		}
		else if (should_move) 
			cmd->sidemove += minimal_move;
	}
	else 
	{
		if (next_lby >= g_GlobalVars->curtime) 
		{
			if (!broke_lby && bSendPacket && g_ClientState->chokedcommands > 0)
				return;

			broke_lby = false;
			bSendPacket = false;
			cmd->viewangles.yaw += 120.0f * Settings::Aimbot::AaSide;
		}
		else 
		{
			broke_lby = true;
			bSendPacket = false;
			cmd->viewangles.yaw += 120.0f * -Settings::Aimbot::AaSide;
		}
	}
	Math::FixAngles(cmd->viewangles);
	Math::MovementFix(cmd, OldAngles, cmd->viewangles);


	auto anim_state = g_LocalPlayer->GetPlayerAnimState();
	if (anim_state) 
	{
		CCSGOPlayerAnimState anim_state_backup = *anim_state;
		*anim_state = g_Saver.AnimState;
		*g_LocalPlayer->GetVAngles() = cmd->viewangles;
		g_LocalPlayer->UpdateClientSideAnimation();

		if (anim_state->speed_2d > 0.1f || std::fabsf(anim_state->flUpVelocity)) {
			next_lby = g_GlobalVars->curtime + 0.22f;
		}
		else if (g_GlobalVars->curtime > next_lby) {
			if (std::fabsf(Math::AngleDiff(anim_state->m_flGoalFeetYaw, anim_state->m_flEyeYaw)) > 35.0f) {
				next_lby = g_GlobalVars->curtime + 1.1f;
			}
		}

		g_Saver.AnimState = *anim_state;
		*anim_state = anim_state_backup;
	}

	/*if (bSendPacket) 
	{
		real_angle = g_AnimState.m_flGoalFeetYaw;
		view_angle = g_AnimState.m_flEyeYaw;
	}*/
}
