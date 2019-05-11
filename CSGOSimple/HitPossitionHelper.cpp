
#include "HitPossitionHelper.h"
#include "RuntimeSaver.h"
#include "Logger.h"

void HitPossitionHelper::OnFireEvent(IGameEvent* event)
{
    if (!event || !g_LocalPlayer)
    {
        return;
    }
    if (!strcmp(event->GetName(), "player_hurt"))
    {
        PlayerHurt(event);
    }
    if (!strcmp(event->GetName(), "bullet_impact"))
    {
        BulletImpact(event);
    }
}

void HitPossitionHelper::PlayerHurt(IGameEvent* event)
{
    //2nd

    C_BasePlayer* attacker = GetPlayer(event->GetInt("attacker"));
    C_BasePlayer* hurt = GetPlayer(event->GetInt("userid"));

    if (!attacker || !hurt || attacker != g_LocalPlayer)
        return;

    Vector EPosition = hurt->m_vecOrigin();
    ImpactInfoStruct BestImpact;
    //std::vector<ImpactInfoStruct> BestImpacts;
    float BestImpactDistance = -1.f;
    float Time = g_GlobalVars->realtime;
    //int Tick = g_GlobalVars->tickcount;
    //int LastTick = -1;
    for (auto info = g_Saver.hitpositions.begin(); info != g_Saver.hitpositions.end();)
    {
        if (Time > info->Time + (g_GlobalVars->interval_per_tick * 2))
        {
            info = g_Saver.hitpositions.erase(info);
            continue;
        }
        //g_Logger.Add("PlayerHurt", "player hurt info");
        //LastTick = info->Tick;
        //if () continue;

        Vector position = info->Pos;
        float Distance = EPosition.DistTo(position);

        BulletTracerInfoStruct binfo;
        binfo.DidHit = false;
        binfo.HitPos = info->Pos;
        binfo.ShotPos = info->ShotPos;
        binfo.Time = info->Time;
        //

        if (Distance < BestImpactDistance || BestImpactDistance == -1)
        {
            BestImpactDistance = Distance;
            BestImpact = *info;
            binfo.DidHit = true;
        }
        g_Saver.BulletTracers.push_back(binfo);

        info++;
    }

    if (BestImpactDistance == -1)
    {
        return;
    }
}

void HitPossitionHelper::BulletImpact(IGameEvent* event)
{
    //1st
    C_BasePlayer* shooter = GetPlayer(event->GetInt("userid"));

    if (!shooter || shooter != g_LocalPlayer)
    {
        return;
    }

    ImpactInfoStruct info;
    info.Pos = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
    info.ShotPos = g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset();
    info.Time = g_GlobalVars->realtime;
    info.Tick = g_GlobalVars->tickcount;
    g_Saver.hitpositions.push_back(info);
}

C_BasePlayer* HitPossitionHelper::GetPlayer(int i)
{
    return static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(i)));
}
