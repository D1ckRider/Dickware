
#include "Backtrack.h"
#include "helpers\math.hpp"
#include "Rbot.h"
#include "ConsoleHelper.h"
#include "Autowall.h"
#include "Settings.h"

void Backtrack::OnCreateMove()
{
    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
        {
            continue;
        }

        AddTick ( entity, i );
    }
}

float Backtrack::GetLerpTime()
{
    int ud_rate = g_CVar->FindVar ( "cl_updaterate" )->GetInt();
    ConVar* min_ud_rate = g_CVar->FindVar ( "sv_minupdaterate" );
    ConVar* max_ud_rate = g_CVar->FindVar ( "sv_maxupdaterate" );

    //Console.WriteLine(min_ud_rate->GetFlags)

    if ( min_ud_rate && max_ud_rate )
    {
        ud_rate = max_ud_rate->GetInt();
    }

    float ratio = g_CVar->FindVar ( "cl_interp_ratio" )->GetFloat();

    if ( ratio == 0 )
    {
        ratio = 1.0f;
    }

    float lerp = g_CVar->FindVar ( "cl_interp" )->GetFloat();
    ConVar* c_min_ratio = g_CVar->FindVar ( "sv_client_min_interp_ratio" );
    ConVar* c_max_ratio = g_CVar->FindVar ( "sv_client_max_interp_ratio" );

    if ( c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1 )
    {
        ratio = std::clamp ( ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat() );
    }

    return std::max<float> ( lerp, ( ratio / ud_rate ) );
}

bool Backtrack::IsTickValid ( int tick )
{
    INetChannelInfo* nci = g_EngineClient->GetNetChannelInfo();
    static auto sv_maxunlag = g_CVar->FindVar ( "sv_maxunlag" );

    //Console.WriteLine(sv_maxunlag->GetFloat());

    if ( !nci || !sv_maxunlag )
    {
        return false;
    }

    float correct = std::clamp ( nci->GetLatency ( FLOW_OUTGOING ) + GetLerpTime(), 0.f, sv_maxunlag->GetFloat() );

    //Console.Write("correct: "); Console.WriteLine(correct);

    float deltaTime = correct - ( g_GlobalVars->curtime - TICKS_TO_TIME ( tick ) );

    return fabsf ( deltaTime ) < 0.2f;
}

/*
Have to change:
	- get studiohdr
	- studiomdl
	- studio hitboxset
	    --> here for better damage calculation
	//
*/

bool Backtrack::RageBacktrack ( C_BasePlayer* player, int i, float& damage, Vector& hitpos, TickRecord& _record )
{
    std::deque<TickRecord> bestRecords = GetBestTicks ( i );

    if ( bestRecords.size() == 0 )
        return false;

    OriginalRecord orgData = OriginalRecord ( player );

    bool foundGoodRecord = false;
    float bestDamage = 0.f;
    Vector bestHitpos = Vector ( 0, 0, 0 );
    TickRecord bestRecord;
	float MinDamage = Settings::RageBot::WeaponSettings[0].MinDamage; //g_Config.GetFloat ( "rbot_mindamage" );
    //bool foundKillable = false;

    if ( bestRecords.size() > 4 )
    {
        bool skip = false;

        for ( auto record = bestRecords.begin(); record != bestRecords.end(); )
        {
            if ( skip )
            {
                record = bestRecords.erase ( record );
            }

            else
            {
                record++;
            }

            skip = !skip;
        }
    }

    for ( auto record = bestRecords.begin(); record != bestRecords.end(); record++ )
    {
        //if (foundKillable) continue;
        float currentDamage = 0.f;
        Vector currentHitpos = Vector ( 0, 0, 0 );
        bool b_bool = false;

        if ( GetBestPointForRecord ( player, *record, MinDamage, currentHitpos, currentDamage, b_bool ) )
        {
            if ( currentDamage > bestDamage )
            {
                //Console.WriteLine("b1g");
                foundGoodRecord = true;

                bestDamage = currentDamage;
                bestHitpos = currentHitpos;
                bestRecord = *record;
            }
        }
    }

    //Console.WriteLine("Loob finished");

    if ( !foundGoodRecord )
    {
        return false;
    }

    //Console.WriteLine(bestHitpos);

    damage = bestDamage;
    hitpos = bestHitpos;
    _record = bestRecord;

    player->InvalidateBoneCache();
    player->SetAbsOriginal ( orgData.m_vecOrigin );
    player->SetAbsAngles ( QAngle ( 0, orgData.m_angEyeAngles.yaw, 0 ) );
    player->m_fFlags() = orgData.m_fFlags;
    player->m_flPoseParameter() = orgData.m_flPoseParameter;
    player->UpdateClientSideAnimation();

    return true;
}

void Backtrack::LegitOnCreateMove ( std::deque<int> hb_enabled )
{
    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
        {
            continue;
        }

        LegitTickRecord record = LegitTickRecord ( entity, g_Resolver.GResolverData[i], hb_enabled );
        LegitTickRecords[i].push_back ( record );

        if ( LegitTickRecords[i].size() > 128 )
        {
            LegitTickRecords[i].erase ( LegitTickRecords[i].begin() );
        }
    }
}

void Backtrack::FinishLegitBacktrack ( CUserCmd* cmd )
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;


    if ( ! ( cmd->buttons & IN_ATTACK ) && ! ( cmd->buttons & IN_ATTACK2 ) )
        return;

    int BestEntity = -1;
    LegitTickRecord BestRecord;
    float BestFov = FLT_MAX;

	float lbot_backtrack_ms = Settings::Aimbot::BacktrackTick;//g_Config.GetFloat ( "lbot_backtrack_ms" );

    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
        {
            continue;
        }

        std::deque<LegitTickRecord> lcr = GetValidLegitRecords ( i, lbot_backtrack_ms );

        for ( auto record = lcr.begin(); record != lcr.end(); record++ )
        {
            if ( !g_LocalPlayer->CanSeePlayer ( g_LocalPlayer, record->hitboxes[HITBOX_HEAD] ) )
            {
                continue;
            }

            float fov = Math::GetFOV ( cmd->viewangles, Math::CalcAngle ( g_LocalPlayer->GetEyePos(), record->hitboxes[HITBOX_HEAD] ) );

            if ( fov < BestFov )
            {
                BestRecord = *record;
                BestFov = fov;
                BestEntity = i;
            }

            /*
            for (int hitbox = 0; hitbox < HITBOX_MAX; hitbox++)
            {
            	if (!g_LocalPlayer->CanSeePlayer(g_LocalPlayer, record->hitboxes[hitbox])) continue;
            	float fov = Math::GetFOV(cmd->viewangles, Math::CalcAngle(g_LocalPlayer->GetEyePos(), record->hitboxes[hitbox]));


            	if (fov < BestFov)
            	{
            		BestRecord = *record;
            		BestFov = fov;
            		BestEntity = i;
            	}
            }
            */
        }
    }

    if ( BestEntity != -1 )
    {
        cmd->tick_count = TIME_TO_TICKS ( BestRecord.m_flSimulationTime + Backtrack::Get().GetLerpTime() );
    }
}

std::deque<LegitTickRecord> Backtrack::GetValidLegitRecords ( int i, float lbot_backtrack_ms )
{
    std::deque<LegitTickRecord> returnVal;

    //float lbot_backtrack_ms = g_Config.GetFloat("lbot_backtrack_ms");

    for ( auto record = LegitTickRecords[i].begin(); record != LegitTickRecords[i].end(); record++ )
    {
        if ( !record->Moving )
        {
            continue;
        }

        if ( record->CanUse && IsTickValid ( TIME_TO_TICKS ( record->m_flSimulationTime + GetLerpTime() ) ) && ( fabsf ( g_GlobalVars->curtime - record->m_flSimulationTime ) + Backtrack::Get().GetLerpTime() ) < lbot_backtrack_ms )
        {
            returnVal.push_back ( *record );
        }
    }

    /*
    if (returnVal.size() > 6)
    {
        bool skip = false;
        for (auto record = returnVal.begin(); record != returnVal.end(); )
        {
            if (skip)
            {
                record = returnVal.erase(record);
            }
            else
            {
                record++;
            }
            skip = !skip;
        }
    }
    */

    return returnVal;
}

bool Backtrack::GetHitboxPos ( int hb, mstudiohitboxset_t* hitboxset, matrix3x4_t matrix[MAXSTUDIOBONES], Vector& pos )
{
    mstudiobbox_t* studioBox = hitboxset->GetHitbox ( hb );

    if ( !studioBox || !matrix )
    {
        return false;
    }

    Vector min, max;

    Math::VectorTransform ( studioBox->bbmin, matrix[studioBox->bone], min );
    Math::VectorTransform ( studioBox->bbmax, matrix[studioBox->bone], max );

    pos = ( min + max ) * 0.5f;

    return true;
}

std::deque<TickRecord> Backtrack::GetValidTicks ( int i )
{
    std::deque<TickRecord> returnVal;
    //if(TickRecords[i].size() == 0) { Console.WriteLine("!TickRecords == 0"); return returnVal; }

    //Console.WriteLine((int)TickRecords[i].size());

    for ( auto record = TickRecords[i].begin(); record != TickRecords[i].end(); record++ )
    {
        if ( record->CanUse && IsTickValid ( TIME_TO_TICKS ( record->m_flSimulationTime + GetLerpTime() ) ) )
        {
            returnVal.push_back ( *record );
        }
    }

    //Console.WriteLine((int)returnVal.size());

    return returnVal;
}

std::deque<TickRecord> Backtrack::GetBestTicks ( int i )
{
    std::deque<TickRecord> returnVal;
    std::deque<TickRecord> ValidRecords = GetValidTicks ( i );

    if ( ValidRecords.size() == 0 )
    {
        return returnVal;
    }

    for ( auto record = ValidRecords.begin(); record != ValidRecords.end(); record++ )
    {
        if ( record->resolverData.mode == ResolverModes::LBY_BREAK || ( !record->resolverData.InFakelag && record->resolverData.Moving && !record->resolverData.InAir ) )
        {
            returnVal.push_back ( *record );
        }
    }

    if ( returnVal.size() < 3 )
    {
        for ( auto record = ValidRecords.begin(); record != ValidRecords.end(); record++ )
        {
            if ( !record->resolverData.InFakelag && record->resolverData.InAir && record->resolverData.Moving )
            {
                returnVal.push_back ( *record );
            }
        }
    }

    return returnVal;
}

bool Backtrack::GetBestPointForRecord ( C_BasePlayer* player, TickRecord record, float minDmg, Vector& hitpos, float& damage, bool& willKill )
{
    player->InvalidateBoneCache();

    player->m_fFlags() = record.m_fFlags;
    player->m_flCycle() = record.m_flCycle;
    player->m_nSequence() = record.m_nSequence;
    player->SetAbsOriginal ( record.m_vecOrigin );
    player->m_angEyeAngles() = record.m_angEyeAngles;
    //player->SetAbsAngles(QAngle(0, record.m_angEyeAngles.yaw, 0));
    player->m_flLowerBodyYawTarget() = record.m_flLowerBodyYawTarget;
    player->m_flSimulationTime() = record.m_flSimulationTime;
    //player->m_flPoseParameter() = record.m_flPoseParameter;
    player->UpdateClientSideAnimation();

    //if (!record.StudioHdr) return false;



    if ( !record.matrixBuilt )
    {
        if ( !player->SetupBones ( record.matrix, 128, 256, record.m_flSimulationTime ) )
        {
            return false;
        }

        record.matrixBuilt = true;
    }


    const model_t* model = player->GetModel();

    if ( !model )
    {
        return false;
    }

    studiohdr_t* studioHdr = g_MdlInfo->GetStudiomodel ( model );

    if ( !studioHdr )
    {
        return false;
    }

    mstudiohitboxset_t* studioSet = studioHdr->GetHitboxSet ( 0 );

    if ( !studioSet )
    {
        return false;
    }


    bool GotHead = false, GotPelvis = false;
    Vector head, body;

    if ( !GetHitboxPos ( HITBOX_HEAD, studioSet, record.matrix, head ) )
    {
        return false;
    }

    //minDmg; //g_Config.GetFloat("rbot_mindamage");
    Vector BestPos = Vector ( 0, 0, 0 );
    bool CanDo = false;

    if ( GotHead )
    {
        float Damage = Autowall::Get().CanHit ( head );

        //Console.Write("Head damage: "); Console.WriteLine(Damage);
        if ( Damage > minDmg )
        {
            minDmg = Damage;
            BestPos = head;
            CanDo = true;
        }
    }

    hitpos = BestPos;
    damage = minDmg;

    willKill = minDmg > player->m_iHealth();

    return CanDo;
}

void Backtrack::AddTick ( C_BasePlayer* player, int i )
{
    TickRecord record = TickRecord ( player, g_Resolver.GResolverData[i] );
    TickRecords[i].push_back ( record );

    if ( TickRecords[i].size() > 128 )
    {
        TickRecords[i].erase ( TickRecords[i].begin() );
    }
}
