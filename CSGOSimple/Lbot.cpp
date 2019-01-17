
#include "Lbot.h"
#include "helpers\math.hpp"
#include "Logger.h"
#include "ConfigSystem.h"
#include "ConsoleHelper.h"
#include "helpers\input.hpp"
#include "RuntimeSaver.h"
#include "Backtrack.h"

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

	if (GetAsyncKeyState(VK_DELETE))
	{
		g_Logger.Info("WEAPON", "FOV is " + std::to_string(WeaponFov));
	}

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

    if (g_Config.GetBool("lbot_backtrack"))
    {
        Backtrack::Get().LegitOnCreateMove(hb_enabled);
    }

    // Console.WriteLine("!local"); return; }

    //Console.WriteLine("lul");

    if (WeaponRcs)
    {
        ResetRecoil(cmd);
    }
    if (WeaponFov != 0.f)
    {
        DoAimbot(cmd, local, weapon);
    }
    if (WeaponRcs)
    {
        RemoveRecoil(local, cmd);
    }

    //Math::NormalizeAngles(cmd->viewangles);

    g_EngineClient->SetViewAngles(cmd->viewangles);
}


void Lbot::UpdateWeaponConfig(C_BaseCombatWeapon* weapon)
{
    /*
    Todo flickbot etc.
    */

	if (weapon->IsDeagle())
	{
		WeaponFov = g_Config.GetFloat("lbot_deagle_fov");
		WeaponSmooth = g_Config.GetFloat("lbot_deagle_smooth");
		WeaponRandomness = g_Config.GetFloat("lbot_deagle_randomize");
		WeaponDelay = g_Config.GetFloat("lbot_deagle_delay");
		WeaponFlickbot = g_Config.GetBool("lbot_deagle_flickbot");

		WeaponRcs = g_Config.GetBool("lbot_deagle_rcs");
		WeaponRecoilX = g_Config.GetFloat("lbot_deagle_rcs_x");
		WeaponRecoilY = g_Config.GetFloat("lbot_deagle_rcs_y");

		WeaponHitboxHead = g_Config.GetBool("lbot_deagle_hitbox_head");
		WeaponHitboxNeck = g_Config.GetBool("lbot_deagle_hitbox_neck");
		WeaponHitboxChest = g_Config.GetBool("lbot_deagle_hitbox_chest");
		WeaponHitboxPelvis = g_Config.GetBool("lbot_deagle_hitbox_pelvis");
		WeaponHitboxStomach = g_Config.GetBool("lbot_deagle_hitbox_stomach");
		WeaponHitboxArm = g_Config.GetBool("lbot_deagle_hitbox_arm");
		WeaponHitboxLeg = g_Config.GetBool("lbot_deagle_hitbox_leg");
		WeaponHitboxFoot = g_Config.GetBool("lbot_deagle_hitbox_foot");
	}
	else if (weapon->IsScout())
	{
		WeaponFov = g_Config.GetFloat("lbot_scout_fov");
		WeaponSmooth = g_Config.GetFloat("lbot_scout_smooth");
		WeaponRandomness = g_Config.GetFloat("lbot_scout_randomize");
		WeaponDelay = g_Config.GetFloat("lbot_scout_delay");
		WeaponFlickbot = g_Config.GetBool("lbot_scout_flickbot");

		WeaponRcs = g_Config.GetBool("lbot_scout_rcs");
		WeaponRecoilX = g_Config.GetFloat("lbot_scoute_rcs_x");
		WeaponRecoilY = g_Config.GetFloat("lbot_scout_rcs_y");

		WeaponHitboxHead = g_Config.GetBool("lbot_scout_hitbox_head");
		WeaponHitboxNeck = g_Config.GetBool("lbot_scout_hitbox_neck");
		WeaponHitboxChest = g_Config.GetBool("lbot_scout_hitbox_chest");
		WeaponHitboxPelvis = g_Config.GetBool("lbot_scout_hitbox_pelvis");
		WeaponHitboxStomach = g_Config.GetBool("lbot_scout_hitbox_stomach");
		WeaponHitboxArm = g_Config.GetBool("lbot_scout_hitbox_arm");
		WeaponHitboxLeg = g_Config.GetBool("lbot_scout_hitbox_leg");
		WeaponHitboxFoot = g_Config.GetBool("lbot_scout_hitbox_foot");
	}
    if (weapon->IsPistol())
    {
        WeaponFov = g_Config.GetFloat("lbot_pistol_fov");
        WeaponSmooth = g_Config.GetFloat("lbot_pistol_smooth");
        WeaponRandomness = g_Config.GetFloat("lbot_pistol_randomize");
        WeaponDelay = g_Config.GetFloat("lbot_pistol_delay");
        WeaponFlickbot = g_Config.GetBool("lbot_pistol_flickbot");

        WeaponRcs = g_Config.GetBool("lbot_pistol_rcs");
        WeaponRecoilX = g_Config.GetFloat("lbot_pistol_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("lbot_pistol_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("lbot_pistol_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("lbot_pistol_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("lbot_pistol_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("lbot_pistol_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("lbot_pistol_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("lbot_pistol_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("lbot_pistol_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("lbot_pistol_hitbox_foot");
    }
    else if (weapon->IsSniper())
    {
        WeaponFov = g_Config.GetFloat("lbot_sniper_fov");
        WeaponSmooth = g_Config.GetFloat("lbot_sniper_smooth");
        WeaponRandomness = g_Config.GetFloat("lbot_sniper_randomize");
        WeaponDelay = g_Config.GetFloat("lbot_sniper_delay");
        WeaponFlickbot = g_Config.GetBool("lbot_sniper_flickbot");

        WeaponRcs = g_Config.GetBool("lbot_sniper_rcs");
        WeaponRecoilX = g_Config.GetFloat("lbot_sniper_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("lbot_sniper_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("lbot_sniper_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("lbot_sniper_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("lbot_sniper_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("lbot_sniper_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("lbot_sniper_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("lbot_sniper_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("lbot_sniper_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("lbot_sniper_hitbox_foot");
    }
    else if (weapon->IsSubmachinegun())
    {
        WeaponFov = g_Config.GetFloat("lbot_smg_fov");
        WeaponSmooth = g_Config.GetFloat("lbot_smg_smooth");
        WeaponRandomness = g_Config.GetFloat("lbot_smg_randomize");
        WeaponDelay = g_Config.GetFloat("lbot_smg_delay");
        WeaponFlickbot = g_Config.GetBool("lbot_smg_flickbot");

        WeaponRcs = g_Config.GetBool("lbot_smg_rcs");
        WeaponRecoilX = g_Config.GetFloat("lbot_smg_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("lbot_smg_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("lbot_smg_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("lbot_smg_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("lbot_smg_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("lbot_smg_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("lbot_smg_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("lbot_smg_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("lbot_smg_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("lbot_smg_hitbox_foot");
    }
    else if (weapon->IsMachinegun())
    {
        WeaponFov = g_Config.GetFloat("lbot_mg_fov");
        WeaponSmooth = g_Config.GetFloat("lbot_mg_smooth");
        WeaponRandomness = g_Config.GetFloat("lbot_mg_randomize");
        WeaponDelay = g_Config.GetFloat("lbot_mg_delay");
        WeaponFlickbot = g_Config.GetBool("lbot_mg_flickbot");

        WeaponRcs = g_Config.GetBool("lbot_mg_rcs");
        WeaponRecoilX = g_Config.GetFloat("lbot_mg_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("lbot_mg_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("lbot_mg_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("lbot_mg_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("lbot_mg_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("lbot_mg_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("lbot_mg_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("lbot_mg_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("lbot_mg_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("lbot_mg_hitbox_foot");
    }
    else if (weapon->IsShotgun())
    {
        WeaponFov = g_Config.GetFloat("lbot_shotgun_fov");
        WeaponSmooth = g_Config.GetFloat("lbot_shotgun_smooth");
        WeaponRandomness = g_Config.GetFloat("lbot_shotgun_randomize");
        WeaponDelay = g_Config.GetFloat("lbot_shotgun_delay");
        WeaponFlickbot = g_Config.GetBool("lbot_shotgun_flickbot");

        WeaponRcs = g_Config.GetBool("lbot_shotgun_rcs");
        WeaponRecoilX = g_Config.GetFloat("lbot_shotgun_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("lbot_shotgun_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("lbot_shotgun_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("lbot_shotgun_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("lbot_shotgun_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("lbot_shotgun_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("lbot_shotgun_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("lbot_shotgun_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("lbot_shotgun_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("lbot_shotgun_hitbox_foot");
    }
    else
    {
        WeaponFov = g_Config.GetFloat("lbot_rifle_fov");
        WeaponSmooth = g_Config.GetFloat("lbot_rifle_smooth");
        WeaponRandomness = g_Config.GetFloat("lbot_rifle_randomize");
        WeaponDelay = g_Config.GetFloat("lbot_rifle_delay");
        WeaponFlickbot = g_Config.GetBool("lbot_rifle_flickbot");

        WeaponRcs = g_Config.GetBool("lbot_rifle_rcs");
        WeaponRecoilX = g_Config.GetFloat("lbot_rifle_rcs_x");
        WeaponRecoilY = g_Config.GetFloat("lbot_rifle_rcs_y");

        WeaponHitboxHead = g_Config.GetBool("lbot_rifle_hitbox_head");
        WeaponHitboxNeck = g_Config.GetBool("lbot_rifle_hitbox_neck");
        WeaponHitboxChest = g_Config.GetBool("lbot_rifle_hitbox_chest");
        WeaponHitboxPelvis = g_Config.GetBool("lbot_rifle_hitbox_pelvis");
        WeaponHitboxStomach = g_Config.GetBool("lbot_rifle_hitbox_stomach");
        WeaponHitboxArm = g_Config.GetBool("lbot_rifle_hitbox_arm");
        WeaponHitboxLeg = g_Config.GetBool("lbot_rifle_hitbox_leg");
        WeaponHitboxFoot = g_Config.GetBool("lbot_rifle_hitbox_foot");
    }
}

int Lbot::GetBestTarget(C_BasePlayer* local, C_BaseCombatWeapon* weapon, CUserCmd* cmd, Vector& hitpos)
{
    QAngle viewangles = cmd->viewangles;

    float BestFov = WeaponFov;
    Vector BestPos = Vector(0, 0, 0);
    int BestIndex = -1;
    //bool UsingBacktrack = false;
    //LegitTickRecord BestBacktrackTick;
    bool lbot_backtrack = g_Config.GetBool("lbot_backtrack");
    bool lbot_backtrack_aim = g_Config.GetBool("lbot_backtrack_aim");
    float lbot_backtrack_ms = g_Config.GetFloat("lbot_backtrack_ms");

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

void Lbot::RemoveRecoil(C_BasePlayer* local, CUserCmd* cmd)
{
    //LastAimpunchRemove
    QAngle odata = local->m_aimPunchAngle();
    QAngle data = (odata + LastAimpunch);
    LastAimpunch = odata;
    data.pitch *= WeaponRecoilY;
    data.yaw *= WeaponRecoilX;
    cmd->viewangles -= data;
    LastAimpunchRemove = data;
}

void Lbot::DoAimbot(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon)
{
    static int LastEntity = -1;
    static float EntityFoundTime = 0.f;
    static bool DidLastShot = false;

    if (WeaponFlickbot && !weapon->CanFire())
    {
        return;
    }

    if (!weapon->HasBullets())
    {
        return;
    }

    if (!InputSys::Get().IsKeyDown(g_Config.GetInt("lbot_aimkey")))
    {
        return;
    }

    Vector Outpos = Vector(0, 0, 0);
    int Target = GetBestTarget(local, weapon, cmd, Outpos);
    if (Target == -1)
    {
        LastEntity = -1;
        EntityFoundTime = g_GlobalVars->curtime;
        return;
    }

    if (LastEntity != Target)
    {
        EntityFoundTime = g_GlobalVars->curtime;
        LastEntity = Target;
    }

    if (WeaponDelay != 0.f)
    {
        //Console.WriteLine(g_GlobalVars->curtime - EntityFoundTime);
        if (g_GlobalVars->curtime - EntityFoundTime > WeaponDelay)
        {
            cmd->buttons |= IN_ATTACK;
        }
        else if(weapon->m_Item().m_iItemDefinitionIndex() != WEAPON_REVOLVER)
        {
            cmd->buttons &= ~IN_ATTACK;
        }
    }

    if (WeaponRandomness != 0.f)
    {
        Outpos += Vector(Math::RandomFloat(-WeaponRandomness, WeaponRandomness), Math::RandomFloat(-WeaponRandomness, WeaponRandomness), Math::RandomFloat(-WeaponRandomness, WeaponRandomness));
    }

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

    if (DidLastShot && weapon->m_Item().m_iItemDefinitionIndex() != WEAPON_REVOLVER && (weapon->IsPistol() || weapon->IsShotgun() || weapon->IsSniper()))
    {
        cmd->buttons &= ~IN_ATTACK;
    }
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
/*
void Lbot::LegitAA(CUserCmd * cmd, bool & bSendPackets)
{
	return;
	if (!g_Config.GetBool("lbot_legitaa")) return;

	static int ChokedPackets = -1;
	//static float LastRealPitch = 0.f;
	if (!g_Saver.InFakewalk && (!g_Config.GetBool("misc_fakelag") || !g_Saver.FakelagCurrentlyEnabled || g_Saver.RbotAADidShot))
	{
		if (g_Saver.RbotAADidShot)
		{
			g_Saver.RbotAADidShot = false;
		}

		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPackets = true;
		}
		else
		{
			bSendPackets = false;
		}
	}

	if (bSendPackets)
	{
		g_Saver.AAFakeAngle = cmd->viewangles;
	}
	else
	{
		cmd->viewangles.yaw += g_Config.GetFloat("lbot_legitaa_yaw");
		ChokedPackets = -1;
		g_Saver.AARealAngle = cmd->viewangles;
	}
}
*/