
#include "Lbot.h"
#include "helpers\math.hpp"
#include "Logger.h"
#include "ConsoleHelper.h"
#include "helpers\input.hpp"
#include "RuntimeSaver.h"
#include "Backtrack.h"
#include "Settings.h"

void Lbot::OnCreateMove(CUserCmd* cmd)
{
    C_BasePlayer* local = g_LocalPlayer;
    if (!local || !local->IsAlive())
    {
        return;
    }
    C_BaseCombatWeapon* weapon = local->m_hActiveWeapon().Get();
    if (!weapon || weapon->IsReloading())
    {
        return;
    }
    UpdateWeaponConfig(weapon);

    std::deque<int> hb_enabled;

    if (WeaponHitboxHead)
    {
        hb_enabled.push_back(HITBOX_HEAD);
    }
    if (WeaponHitboxNeck)
    {
        hb_enabled.push_back(HITBOX_NECK);
    }
    if (WeaponHitboxChest)
    {
        hb_enabled.push_back(HITBOX_CHEST);
    }
    if (WeaponHitboxPelvis)
    {
        hb_enabled.push_back(HITBOX_PELVIS);
    }
    if (WeaponHitboxStomach)
    {
        hb_enabled.push_back(HITBOX_STOMACH);
    }

	if(Settings::Aimbot::Backtrack)
    {
        Backtrack::Get().LegitOnCreateMove(hb_enabled);
    }

	bool bAttack = (cmd->buttons & IN_ATTACK);

	if (bAttack)
		if (weapon->IsPistol() && WeaponAutopistol)
			AutoPistol(cmd);

	//StandaloneRCS(cmd);
    if (WeaponRcs)
		ResetRecoil(cmd);
    if (WeaponFov != 0.f)
        DoAimbot(cmd, local, weapon);
    if (WeaponRcs)
		RemoveRecoil(local, cmd);

    //Math::NormalizeAngles(cmd->viewangles);

    g_EngineClient->SetViewAngles(cmd->viewangles);
}


void Lbot::UpdateWeaponConfig(C_BaseCombatWeapon* weapon)
{
	int WeaponID = Settings::Aimbot::GetWeaponType(weapon);

	WeaponFov = Settings::Aimbot::WeaponAimSetting[WeaponID].FOV;
	WeaponSmooth = Settings::Aimbot::WeaponAimSetting[WeaponID].Smooth;
	WeaponRandomness = Settings::Aimbot::WeaponAimSetting[WeaponID].Randomize;
	WeaponDelay = Settings::Aimbot::WeaponAimSetting[WeaponID].Delay;
	WeaponAutopistol = Settings::Aimbot::WeaponAimSetting[WeaponID].Autopistol;

	WeaponRcs = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS;
	WeaponRecoilX = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_X;
	WeaponRecoilY = Settings::Aimbot::WeaponAimSetting[WeaponID].RCS_Y;

	WeaponHitboxHead = Settings::Aimbot::WeaponAimSetting[WeaponID].HitboxHead;
	WeaponHitboxNeck = Settings::Aimbot::WeaponAimSetting[WeaponID].HitboxNeck;
	WeaponHitboxChest = Settings::Aimbot::WeaponAimSetting[WeaponID].HitboxChest;
	WeaponHitboxPelvis = Settings::Aimbot::WeaponAimSetting[WeaponID].HitboxPelvis;
	WeaponHitboxStomach = Settings::Aimbot::WeaponAimSetting[WeaponID].HitboxStomach;
	WeaponHitboxArm = Settings::Aimbot::WeaponAimSetting[WeaponID].HitboxArm;
	WeaponHitboxLeg = Settings::Aimbot::WeaponAimSetting[WeaponID].HitboxLeg;
	WeaponHitboxFoot = Settings::Aimbot::WeaponAimSetting[WeaponID].HitboxFoot;
}

int Lbot::GetBestTarget(C_BasePlayer* local, C_BaseCombatWeapon* weapon, CUserCmd* cmd, Vector& hitpos)
{
    QAngle viewangles = cmd->viewangles;

    float BestFov = WeaponFov;
    Vector BestPos = Vector(0, 0, 0);
    int BestIndex = -1;
    //bool UsingBacktrack = false;
    //LegitTickRecord BestBacktrackTick;
	bool lbot_backtrack = Settings::Aimbot::Backtrack; 
	bool lbot_backtrack_aim = Settings::Aimbot::BacktrackAtAim;
	float lbot_backtrack_ms = Settings::Aimbot::BacktrackTick;

    //float flRange = weapon->GetCSWeaponData()->flRange;

    for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
    {
        auto entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
        if (!entity || !entity->IsPlayer() || entity == local || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy())
        {
            continue;
        }

        //Console.WriteLine("Found valid target")

        //if (local->m_vecOrigin().DistTo(entity->m_vecOrigin()) > flRange) continue;

        entity->PrecaceOptimizedHitboxes();

        for (int hitbox = 0; hitbox < HITBOX_MAX; hitbox++)
        {
            switch (hitbox)
            {
                case HITBOX_HEAD:
                    if (!WeaponHitboxHead)
                    {
                        continue;
                    }
                    break;
                case HITBOX_NECK:
                    if (!WeaponHitboxNeck)
                    {
                        continue;
                    }
                    break;
                case HITBOX_PELVIS:
                    if (!WeaponHitboxPelvis)
                    {
                        continue;
                    }
                    break;
                case HITBOX_STOMACH:
                    if (!WeaponHitboxStomach)
                    {
                        continue;
                    }
                    break;
                case HITBOX_LOWER_CHEST:
                case HITBOX_CHEST:
                case HITBOX_UPPER_CHEST:
                    if (!WeaponHitboxChest)
                    {
                        continue;
                    }
                    break;
                case HITBOX_RIGHT_THIGH:
                case HITBOX_LEFT_THIGH:
                case HITBOX_RIGHT_CALF:
                case HITBOX_LEFT_CALF:
                    if (!WeaponHitboxLeg)
                    {
                        continue;
                    }
                    break;
                case HITBOX_RIGHT_FOOT:
                case HITBOX_LEFT_FOOT:
                    if (!WeaponHitboxFoot)
                    {
                        continue;
                    }
                    break;
                case HITBOX_RIGHT_HAND:
                case HITBOX_LEFT_HAND:
                case HITBOX_RIGHT_UPPER_ARM:
                case HITBOX_RIGHT_FOREARM:
                case HITBOX_LEFT_UPPER_ARM:
                case HITBOX_LEFT_FOREARM:
                    if (!WeaponHitboxArm)
                    {
                        continue;
                    }
                    break;
            }

            Vector pos;
            if (!entity->GetOptimizedHitboxPos(hitbox, pos))
            {
                continue;
            }
            if (!local->CanSeePlayer(local, pos))
            {
                continue;
            }

			if (Settings::Aimbot::SmokeCheck && Utils::LineThroughSmoke(local->GetEyePos(), pos))
				continue;

            float fov = Math::GetFOV(viewangles, Math::CalcAngle(local->GetEyePos(), pos));

            if (fov < BestFov)
            {
                //UsingBacktrack = false;
                BestPos = pos;
                BestFov = fov;
                BestIndex = i;
            }
        }

        if (lbot_backtrack && lbot_backtrack_aim)
        {
            std::deque<LegitTickRecord> BacktrackRecords = Backtrack::Get().GetValidLegitRecords(i, lbot_backtrack_ms);

            for (auto record = BacktrackRecords.begin(); record != BacktrackRecords.end(); record++)
            {
                for (int hitbox = 0; hitbox < HITBOX_MAX; hitbox++)
                {
                    switch (hitbox)
                    {
                        case HITBOX_HEAD:
                            if (!WeaponHitboxHead)
                            {
                                continue;
                            }
                            break;
                        case HITBOX_NECK:
                            if (!WeaponHitboxNeck)
                            {
                                continue;
                            }
                            break;
                        case HITBOX_PELVIS:
                            if (!WeaponHitboxPelvis)
                            {
                                continue;
                            }
                            break;
                        case HITBOX_STOMACH:
                            if (!WeaponHitboxStomach)
                            {
                                continue;
                            }
                            break;
                        case HITBOX_LOWER_CHEST:
                        case HITBOX_CHEST:
                        case HITBOX_UPPER_CHEST:
                            if (!WeaponHitboxChest)
                            {
                                continue;
                            }
                            break;
                        case HITBOX_RIGHT_THIGH:
                        case HITBOX_LEFT_THIGH:
                        case HITBOX_RIGHT_CALF:
                        case HITBOX_LEFT_CALF:
                        case HITBOX_RIGHT_FOOT:
                        case HITBOX_LEFT_FOOT:
                        case HITBOX_RIGHT_HAND:
                        case HITBOX_LEFT_HAND:
                        case HITBOX_RIGHT_UPPER_ARM:
                        case HITBOX_RIGHT_FOREARM:
                        case HITBOX_LEFT_UPPER_ARM:
                        case HITBOX_LEFT_FOREARM:
                            continue;
                            break;
                    }

                    if (!local->CanSeePlayer(local, record->hitboxes[hitbox]))
                    {
                        continue;
                    }
                    float fov = Math::GetFOV(viewangles, Math::CalcAngle(local->GetEyePos(), record->hitboxes[hitbox]));


                    if (fov < BestFov)
                    {
                        BestPos = record->hitboxes[hitbox];
                        BestFov = fov;
                        BestIndex = i;
                    }
                }
            }
        }
    }

    //if (UsingBacktrack)
    //{
    //	cmd->tick_count = TIME_TO_TICKS(BestBacktrackTick.m_flSimulationTime);
    //}

    hitpos = BestPos;
    return BestIndex;
}

void Lbot::ResetRecoil(CUserCmd* cmd)
{
	cmd->viewangles += LastAimpunchRemove;
}

void Lbot::AutoPistol(CUserCmd * cmd)
{
	float NextAttack = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack();
	float Tick = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick; 

	if (NextAttack < Tick)
		return;

	if (g_LocalPlayer->m_hActiveWeapon()->GetItemDefinitionIndex() == WEAPON_REVOLVER)
		cmd->buttons &= ~IN_ATTACK2;
	else
		cmd->buttons &= ~IN_ATTACK;
}

void Lbot::RemoveRecoil(C_BasePlayer* local, CUserCmd* cmd)
{
	QAngle AimPunch = local->m_aimPunchAngle();
	QAngle FinalViewAngle = (AimPunch + LastAimpunch);
	LastAimpunch = AimPunch;
	AimPunch.pitch *= WeaponRecoilY;
	AimPunch.yaw *= WeaponRecoilX;
	//Math::NormalizeAngles(AimPunch);
	cmd->viewangles -= FinalViewAngle;
	LastAimpunchRemove = FinalViewAngle;
}

void Lbot::DoAimbot(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon)
{
    static int LastEntity = -1;
    static float EntityFoundTime = 0.f;
    static bool DidLastShot = false;

    if (!weapon->CanFire())
        return;
    
    if (!weapon->HasBullets())
        return;
	
	if ( !InputSys::Get().IsKeyDown(Settings::Aimbot::Hotkey) )
        return;

    Vector Outpos = Vector(0, 0, 0);
    int Target = GetBestTarget(local, weapon, cmd, Outpos);
    if (Target == -1)
    {
        LastEntity = -1;
        EntityFoundTime = g_GlobalVars->curtime;
        return;
    }

	//auto TargetEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(Target));
	//if (g_LocalPlayer->CanSeePlayer(g_LocalPlayer, TargetEntity->GetRenderOrigin()))
	//	return;

    if (LastEntity != Target)
    {
        EntityFoundTime = g_GlobalVars->curtime;
        LastEntity = Target;
    }

    if (WeaponDelay != 0.f)
    {
        if (g_GlobalVars->curtime - EntityFoundTime > WeaponDelay)
            cmd->buttons |= IN_ATTACK;

        else if(weapon->m_Item().m_iItemDefinitionIndex() != WEAPON_REVOLVER)
            cmd->buttons &= ~IN_ATTACK;
    }

    if (WeaponRandomness != 0.f)
        Outpos += Vector(Math::RandomFloat(-WeaponRandomness, WeaponRandomness), Math::RandomFloat(-WeaponRandomness, WeaponRandomness), Math::RandomFloat(-WeaponRandomness, WeaponRandomness));

    QAngle CalcAng = Math::CalcAngle(local->GetEyePos(), Outpos);
    QAngle ViewAngle = cmd->viewangles;

    Math::NormalizeAngles(CalcAng);
    Math::NormalizeAngles(ViewAngle);

    QAngle Delta = ViewAngle - CalcAng;

    Math::NormalizeAngles(Delta);
    Math::ClampAngles(Delta);


    float RandomFactor = 1.f;
    //if (WeaponRandomness != 0.f) RandomFactor = Math::RandomFloat(0.f, 1.f + WeaponRandomness);

    QAngle FinalAngle = ViewAngle - (Delta / (WeaponSmooth * RandomFactor));

    Math::NormalizeAngles(FinalAngle);
    Math::ClampAngles(FinalAngle);

    if (DidLastShot && weapon->GetItemDefinitionIndex() != WEAPON_REVOLVER && (weapon->IsPistol() || weapon->IsShotgun() || weapon->IsSniper()))
        cmd->buttons &= ~IN_ATTACK;

    DidLastShot = !DidLastShot;
    cmd->viewangles = FinalAngle;
    if (weapon->CanFire())
    {
        auto entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(Target));
        if (!entity || !entity->IsPlayer() || entity == local || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy())
        {
            return;
        }

        cmd->tick_count = TIME_TO_TICKS(entity->m_flSimulationTime() + Backtrack::Get().GetLerpTime());
    }
}



void Lbot::LegitAA(CUserCmd * cmd, bool & bSendPacket)
{
	static bool sw = false;
	bSendPacket = sw;
	sw = !sw;

	auto animstate = g_LocalPlayer->GetBasePlayerAnimState();
	float feet_yaw = animstate->m_flGoalFeetYaw;
	float feet_delta = Math::NormalizeAngle(cmd->viewangles.yaw - feet_yaw);
	float desync_delta = g_LocalPlayer->GetMaxDesyncAngle();
	float delta = std::clamp(Math::NormalizeAngle(desync_delta - feet_delta), -desync_delta, desync_delta);

	static bool bSwitch = false;
	if (!bSendPacket && !(cmd->buttons & IN_ATTACK))
	{
		cmd->viewangles.yaw += bSwitch ? delta : -delta;
		bSwitch != bSwitch;
	}
}
