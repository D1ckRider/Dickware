#include "AntiAim.h"
#include "helpers/math.hpp"
#include "ConfigSystem.h"
#include "RuntimeSaver.h"
#include <chrono>
#include "Autowall.h"
#include "Settings.h"
#include "ConsoleHelper.h"
#include "helpers\input.hpp"
#include "Logger.h"
#include <algorithm>

using AAState = Settings::RageBot::AntiAimType;
using DesyncState = Settings::RageBot::DesyncAAType;

float side = 1.f;

void AntiAim::OnCreateMove ( CUserCmd* cmd, bool& bSendPacket )
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
        return;

    int movetype = g_LocalPlayer->m_nMoveType();

    if (
        movetype == MOVETYPE_FLY
        || movetype == MOVETYPE_NOCLIP
        || cmd->buttons & IN_USE
        || cmd->buttons & IN_GRENADE1
        || cmd->buttons & IN_GRENADE2
    )
    {
        if ( bSendPacket )
            g_Saver.FakelagData.ang = cmd->viewangles;

        return;
    }

    C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

    if ( !weapon )
        return;

    if ( weapon->m_flNextPrimaryAttack() - g_GlobalVars->curtime < g_GlobalVars->interval_per_tick && ( cmd->buttons & IN_ATTACK) )// || cmd->buttons & IN_ATTACK2 ) )
        return;

    if ( movetype == MOVETYPE_LADDER )
    {
        static bool last = false;
        bSendPacket = last;
        last = !last;

        if ( bSendPacket )
            g_Saver.FakelagData.ang = cmd->viewangles;

        return;

    }

    if ( weapon->IsGrenade() && weapon->m_fThrowTime() > 0.1f )
	{
        bSendPacket = false;
        return;
    }

	if (InputSys::Get().WasKeyPressed(Settings::RageBot::DesyncFlipHotkey)) 
		side = -side;

    DoAntiAim ( cmd, bSendPacket );
}

int AntiAim::GetFPS()
{
    static int fps = 0;
    static int count = 0;
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    static auto last = high_resolution_clock::now();
    count++;

    if ( duration_cast<milliseconds> ( now - last ).count() > 1000 )
    {
        fps = count;
        count = 0;
        last = now;
    }

    return fps;
}

int AntiAim::GetTickBase(CUserCmd* cmd)
{
	static int g_tick = 0;
	static CUserCmd* g_pLastCmd = nullptr;

	if (!cmd)
		return g_tick;

	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = g_LocalPlayer->m_nTickBase();
	}
	else {
		// Required because prediction only runs on frames, not ticks
		// So if your framerate goes below tickrate, m_nTickBase won't update every tick
		++g_tick;
	}

	g_pLastCmd = cmd;
	return g_tick;
}

void AntiAim::SlideWalk ( CUserCmd* cmd )
{
	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER)
		return;

    if ( cmd->forwardmove > 0 )
    {
        cmd->buttons |= IN_BACK;
        cmd->buttons &= ~IN_FORWARD;
    }

    if ( cmd->forwardmove < 0 )
    {
        cmd->buttons |= IN_FORWARD;
        cmd->buttons &= ~IN_BACK;
    }

    if ( cmd->sidemove < 0 )
    {
        cmd->buttons |= IN_MOVERIGHT;
        cmd->buttons &= ~IN_MOVELEFT;
    }

    if ( cmd->sidemove > 0 )
    {
        cmd->buttons |= IN_MOVELEFT;
        cmd->buttons &= ~IN_MOVERIGHT;
    }
}

void NormalizeNum ( Vector& vIn, Vector& vOut )
{
    float flLen = vIn.Length();

    if ( flLen == 0 )
    {
        vOut.Init ( 0, 0, 1 );
        return;
    }

    flLen = 1 / flLen;
    vOut.Init ( vIn.x * flLen, vIn.y * flLen, vIn.z * flLen );
}

float AntiAim::fov_player ( Vector ViewOffSet, QAngle View, C_BasePlayer* entity )
{
    // Anything past 180 degrees is just going to wrap around
    CONST FLOAT MaxDegrees = 180.0f;

    // Get local angles
    QAngle Angles = View;

    // Get local view / eye position
    Vector Origin = ViewOffSet;

    // Create and intiialize vectors for calculations below
    Vector Delta ( 0, 0, 0 );
    //Vector Origin(0, 0, 0);
    Vector Forward ( 0, 0, 0 );

    // Convert angles to normalized directional forward vector
    Math::AngleVectors ( Angles, Forward );

    Vector AimPos = entity->GetHitboxPos ( HITBOX_HEAD ); //pvs fix disabled

    //VectorSubtract(AimPos, Origin, Delta);
    Origin.VectorSubtract ( AimPos, Origin, Delta );
    //Delta = AimPos - Origin;

    // Normalize our delta vector
    NormalizeNum ( Delta, Delta );

    // Get dot product between delta position and directional forward vectors
    FLOAT DotProduct = Forward.Dot ( Delta );

    // Time to calculate the field of view
    return ( acos ( DotProduct ) * ( MaxDegrees / M_PI ) );
}

int AntiAim::GetNearestPlayerToCrosshair()
{
    float BestFov = FLT_MAX;
    int BestEnt = -1;
    QAngle MyAng;
    g_EngineClient->GetViewAngles ( MyAng );

    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
        {
            continue;
        }

        float CFov = fov_player ( g_LocalPlayer->m_vecOrigin(), MyAng, entity ); //Math::GetFOV(MyAng, Math::CalcAngle(g_LocalPlayer->GetEyePos(), entity->GetEyePos()));

        if ( CFov < BestFov )
        {
            BestFov = CFov;
            BestEnt = i;
        }
    }

    return BestEnt;
}

bool AntiAim::Freestanding ( C_BasePlayer* player, float& ang )
{
    if ( !g_LocalPlayer || !player || !player->IsAlive() || !g_LocalPlayer->IsAlive() )
        return false;

    C_BasePlayer* local = g_LocalPlayer;

    bool no_active = true;
    float bestrotation = 0.f;
    float highestthickness = 0.f;
    static float hold = 0.f;
    Vector besthead;

    auto leyepos = local->m_vecOrigin() + local->m_vecViewOffset();
    auto headpos = local->GetHitboxPos ( 0 ); //GetHitboxPosition(local_player, 0);
    auto origin = local->m_vecOrigin();

    auto checkWallThickness = [&] ( C_BasePlayer * pPlayer, Vector newhead ) -> float
    {
        Vector endpos1, endpos2;

        Vector eyepos = pPlayer->m_vecOrigin() + pPlayer->m_vecViewOffset();
        Ray_t ray;
        ray.Init ( newhead, eyepos );
        CTraceFilterSkipTwoEntities filter ( pPlayer, local );

        trace_t trace1, trace2;
        g_EngineTrace->TraceRay ( ray, MASK_SHOT_BRUSHONLY | MASK_OPAQUE_AND_NPCS, &filter, &trace1 );

        if ( trace1.DidHit() )
            endpos1 = trace1.endpos;
        else
            return 0.f;

        ray.Init ( eyepos, newhead );
        g_EngineTrace->TraceRay ( ray, MASK_SHOT_BRUSHONLY | MASK_OPAQUE_AND_NPCS, &filter, &trace2 );

        if ( trace2.DidHit() )
            endpos2 = trace2.endpos;

        float add = newhead.DistTo ( eyepos ) - leyepos.DistTo ( eyepos ) + 3.f;
        return endpos1.DistTo ( endpos2 ) + add / 3;
    };

    int index = GetNearestPlayerToCrosshair();
    static C_BasePlayer* entity;

    if ( !local->IsAlive() )
        hold = 0.f;

    if ( index != -1 )
        entity = ( C_BasePlayer* ) g_EntityList->GetClientEntity ( index ); // maybe?

    if ( !entity || entity == nullptr )
        return false;

	if (entity)
	{
		float distance = g_LocalPlayer->m_vecOrigin().Dot(entity->m_vecOrigin());
		m_iAutoDirection = 2 * (distance <= 0.0f) - 1;
		m_bAutomaticDir = true;
	}
	else
		m_bAutomaticDir = false;

    float radius = Vector ( headpos - origin ).Length2D();

    if ( index == -1 )
        no_active = true;
    else
    {
        float step = ( M_PI * 2 ) / 90;

        for ( float besthead = 0; besthead < ( M_PI * 2 ); besthead += step )
        {
            Vector newhead ( radius * cos ( besthead ) + leyepos.x, radius * sin ( besthead ) + leyepos.y, leyepos.z );
            float totalthickness = 0.f;
            no_active = false;
            totalthickness += checkWallThickness ( entity, newhead );

            if ( totalthickness > highestthickness )
            {
                highestthickness = totalthickness;

                bestrotation = besthead;
            }
        }
    }

    if ( no_active )
        return false;
    else
    {
        ang = RAD2DEG ( bestrotation );
        return true;
    }

    return false;
}

bool AntiAim::FreestandingLbyBreak ( float& ang )
{
    return false;

    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return false;

    C_BasePlayer* local = g_LocalPlayer;

    bool no_active = true;
    float bestrotation = g_Saver.AARealAngle.yaw + g_Config.GetFloat ( "rbot_aa_lby_breaker_yaw" );
    float highestthickness = 0.f;
    static float hold = 0.f;
    Vector besthead;

    auto leyepos = local->m_vecOrigin() + local->m_vecViewOffset();
    auto headpos = local->GetHitboxPos ( 0 ); //GetHitboxPosition(local_player, 0);
    auto origin = local->m_vecOrigin();

    auto checkWallThickness = [&] ( C_BasePlayer * pPlayer, Vector newhead ) -> float
    {
        Vector endpos1, endpos2;

        Vector eyepos = pPlayer->m_vecOrigin() + pPlayer->m_vecViewOffset();
        Ray_t ray;
        ray.Init ( newhead, eyepos );
        CTraceFilterSkipTwoEntities filter ( pPlayer, local );

        trace_t trace1, trace2;
        g_EngineTrace->TraceRay ( ray, MASK_SHOT_BRUSHONLY | MASK_OPAQUE_AND_NPCS, &filter, &trace1 );

        if ( trace1.DidHit() )
            endpos1 = trace1.endpos;
		else
            return 0.f;

        ray.Init ( eyepos, newhead );
        g_EngineTrace->TraceRay ( ray, MASK_SHOT_BRUSHONLY | MASK_OPAQUE_AND_NPCS, &filter, &trace2 );

        if ( trace2.DidHit() )
            endpos2 = trace2.endpos;

        float add = newhead.DistTo ( eyepos ) - leyepos.DistTo ( eyepos ) + 3.f;
        return endpos1.DistTo ( endpos2 ) + add / 3;

    };

    int index = GetNearestPlayerToCrosshair();
    static C_BasePlayer* entity;

    if ( !local->IsAlive() )
        hold = 0.f;

    if ( index != -1 )
        entity = ( C_BasePlayer* ) g_EntityList->GetClientEntity ( index ); // maybe?

    if ( !entity || entity == nullptr )
        return false;

    float radius = Vector ( headpos - origin ).Length2D();

    if ( index == -1 )
    {
        no_active = true;
    }
    else
    {
        float step = ( M_PI * 2 ) / 90;

        for ( float besthead = 0; besthead < ( M_PI * 2 ); besthead += step )
        {
            Vector newhead ( radius * cos ( besthead ) + leyepos.x, radius * sin ( besthead ) + leyepos.y, leyepos.z );
            float totalthickness = 0.f;
            no_active = false;
            totalthickness += checkWallThickness ( entity, newhead );
            float rot = RAD2DEG ( bestrotation );

            if ( totalthickness > highestthickness && fabs ( rot - g_Saver.AARealAngle.yaw ) > 45.f )
            {
                highestthickness = totalthickness;

                bestrotation = rot;
            }
        }
    }

    if ( no_active )
    {
        return false;
    }
    else
    {
        ang = bestrotation;
        return true;
    }

    return false;
}

void AntiAim::LbyBreakerPrediction ( CUserCmd* cmd, bool& bSendPacket )
{
	if (!Settings::RageBot::DesyncType || !Settings::RageBot::Enabled || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
        return;

	//static float SpawnTime = 0.0f;

	if (!g_LocalPlayer || !g_LocalPlayer->m_iHealth())
		return;

	allocate = (m_serverAnimState == nullptr);
	change = (!allocate) && (&g_LocalPlayer->GetRefEHandle() != m_ulEntHandle);
	reset = (!allocate && !change) && (g_LocalPlayer->m_flSpawnTime() != m_flSpawnTime);

	if (change)
		m_serverAnimState = nullptr;
		//g_pMemAlloc->Free(m_serverAnimState);

	if (reset) 
	{
		g_LocalPlayer->ResetAnimationState(m_serverAnimState);
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();
	}

	if (allocate || change) 
	{
		CCSGOPlayerAnimState* state = reinterpret_cast<CCSGOPlayerAnimState*>(g_pMemAlloc->Alloc(sizeof(CCSGOPlayerAnimState)));

		if (state != nullptr)
			g_LocalPlayer->CreateAnimationState(state);

		m_ulEntHandle = const_cast<CBaseHandle*>(&g_LocalPlayer->GetRefEHandle());
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();

		m_serverAnimState = state;
	}

	float_t curtime = TICKS_TO_TIME(g_LocalPlayer->m_nTickBase() + 11.f);

	if (!g_ClientState->chokedcommands && m_serverAnimState)
	{
		g_LocalPlayer->UpdateAnimationState(m_serverAnimState, cmd->viewangles);

		float_t delta = std::abs(Math::NormalizeAngle(cmd->viewangles.yaw - g_LocalPlayer->m_flLowerBodyYawTarget()));

		if (m_serverAnimState->speed_2d > 0.1f && (g_LocalPlayer->m_fFlags() & FL_ONGROUND))
			g_Saver.NextLbyUpdate = curtime + g_LocalPlayer->m_flSimulationTime() + 1.1f;

		else if (curtime >= g_Saver.NextLbyUpdate) 
		{
			if (delta > (DirectX::XM_PI / 180 - g_Saver.NextLbyUpdate) * 0.35f)
				g_Saver.NextLbyUpdate = curtime + g_LocalPlayer->m_flSimulationTime() + 0.22f;
		}
	}

	/*if (g_LocalPlayer->m_flSpawnTime() != SpawnTime) 
	{
		g_Saver.AnimState.pBaseEntity = g_LocalPlayer;
		g_LocalPlayer->ResetAnimationState(&g_Saver.AnimState);
		SpawnTime = g_LocalPlayer->m_flSpawnTime();
	}

	auto anim_state = g_LocalPlayer->GetPlayerAnimState();
	if( anim_state ) 
	{
		CCSGOPlayerAnimState anim_state_backup = *anim_state;
		*anim_state = g_Saver.AnimState;
		*g_LocalPlayer->GetVAngles() = cmd->viewangles;
		g_LocalPlayer->UpdateClientSideAnimation();

		if( anim_state->speed_2d > 0.1f || std::fabsf( anim_state->flUpVelocity ) )
			g_Saver.NextLbyUpdate = g_GlobalVars->curtime + 0.47f + DirectX::XM_2PI;
		else if( g_GlobalVars->curtime > g_Saver.NextLbyUpdate)
			if( std::fabsf( Math::AngleDiff( anim_state->m_flGoalFeetYaw, anim_state->m_flEyeYaw ) ) > 65.0f )
				g_Saver.NextLbyUpdate = g_GlobalVars->curtime + 2.1f;

		g_Saver.AnimState = *anim_state;
		*anim_state = anim_state_backup;
	}*/
}

void AntiAim::ResetLbyPrediction()
{
    if ( !g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() )
    {
        m_ulEntHandle = nullptr;
        m_serverAnimState = nullptr;

        allocate = false;
        change = false;
        reset = false;
    }
}

void AntiAim::DoAntiAim ( CUserCmd* cmd, bool& bSendPacket )
{
	Yaw(cmd, false);
	YawAdd(cmd, false);
	Pitch(cmd);

	if ( Settings::RageBot::EnabledAA )
    {
        bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1f || ( cmd->sidemove != 0.f || cmd->forwardmove != 0.f );
        bool InAir = ! ( g_LocalPlayer->m_fFlags() & FL_ONGROUND );
        bool Standing = !Moving && !InAir;

        int FakeLagTicks = 0;

        if ( Standing )
			FakeLagTicks = Settings::RageBot::AntiAimSettings[AAState::STANDING].FakelagTicks;
        else if ( InAir )
			FakeLagTicks = Settings::RageBot::AntiAimSettings[AAState::AIR].FakelagTicks;
        else
			FakeLagTicks = Settings::RageBot::AntiAimSettings[AAState::MOVING].FakelagTicks;

        if ( FakeLagTicks == 0 )
        {
            static bool sw = false;
            bSendPacket = sw;
            sw = !sw;
        }

        static QAngle LastRealAngle = QAngle ( 0, 0, 0 );

		/*if (InputSys::Get().WasKeyPressed(Settings::RageBot::DesyncFlipHotkey))
			side = -side;			*/

		static bool broke_lby = false;
		if(Settings::RageBot::DesyncType > DesyncState::NONE)
		{
			float desyncAngle = g_LocalPlayer->GetMaxDesyncAngle();
			//DesyncAnimation(cmd, bSendPacket, Settings::RageBot::DesyncType);
			if (Settings::RageBot::DesyncType == DesyncState::STATIC)
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
				{
					cmd->sidemove += minimal_move;
				}
			}
			else 
			{
				if (g_Saver.NextLbyUpdate >= g_GlobalVars->curtime) 
				{
					if (!broke_lby && bSendPacket && g_ClientState->chokedcommands > 0)
						return;

					broke_lby = false;
					bSendPacket = false;
					cmd->viewangles.yaw += 120.0f * side;
				}
				else 
				{
					broke_lby = true;
					bSendPacket = false;
					cmd->viewangles.yaw -= (desyncAngle + 30.f) * side;
				}
			}
		}

		Math::FixAngles(cmd->viewangles);

		if (bSendPacket)
		{
			LastRealAngle = cmd->viewangles;
			g_Saver.RealYaw = m_serverAnimState->m_flGoalFeetYaw;
			g_Saver.DesyncYaw = m_serverAnimState->m_flEyeYaw;
		}

        g_Saver.FakelagData.ang = LastRealAngle;
	}
	else
	{
		g_Saver.AARealAngle = cmd->viewangles;
	}

}

void AntiAim::Pitch ( CUserCmd* cmd )
{
    bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
    bool InAir = ! ( g_LocalPlayer->m_fFlags() & FL_ONGROUND );
    bool Standing = !Moving && !InAir;
    PitchAntiAims mode = PitchAntiAims::NONE;

    float CustomPitch = 0.f;

    if ( Standing )
    {
		mode = (PitchAntiAims)Settings::RageBot::AntiAimSettings[AAState::STANDING].Pitch; //( PitchAntiAims ) g_Config.GetInt ( "rbot_aa_stand_pitch" );
        //CustomPitch = g_Config.GetFloat ( "rbot_aa_stand_pitch_custom" );
    }
    else if ( Moving && !InAir )
    {
		mode = (PitchAntiAims)Settings::RageBot::AntiAimSettings[AAState::MOVING].Pitch;
        //CustomPitch = g_Config.GetFloat ( "rbot_aa_move_pitch_custom" );
    }
    else
    {
		mode = (PitchAntiAims)Settings::RageBot::AntiAimSettings[AAState::AIR].Pitch;
        //CustomPitch = g_Config.GetFloat ( "rbot_aa_air_pitch_custom" );
    }

    switch ( mode )
    {
        case PitchAntiAims::EMOTION:
            cmd->viewangles.pitch = 82.f;
            break;

        case PitchAntiAims::DOWN:
            cmd->viewangles.pitch = 90.f;
            break;

        case PitchAntiAims::UP:
            cmd->viewangles.pitch = -90.f;
            break;

        case PitchAntiAims::ZERO:
            cmd->viewangles.pitch = 0.f;
            break;

        case PitchAntiAims::JITTER:
        {
            static int mode = 0;

            switch ( mode )
            {
                case 0:
                    cmd->viewangles.pitch = 90.f;
                    mode++;
                    break;

                case 1:
                    cmd->viewangles.pitch = 0.f;
                    mode++;
                    break;

                case 2:
                    cmd->viewangles.pitch = -90.f;
                    mode++;
                    break;

                case 3:
                    cmd->viewangles.pitch = 0.f;
                    mode = 0;
                    break;
            }

            break;
        }

        case PitchAntiAims::DOWN_JITTER:
        {
            static int mode = 0;

            switch ( mode )
            {
                case 0:
                    cmd->viewangles.pitch = 90.f;
                    mode++;
                    break;

                case 1:
                    cmd->viewangles.pitch = 45.f;
                    mode++;
                    break;

                case 2:
                    cmd->viewangles.pitch = 0.f;
                    mode++;
                    break;

                case 3:
                    cmd->viewangles.pitch = 45.f;
                    mode = 0;
                    break;
            }

            break;
        }

        case PitchAntiAims::UP_JITTER:
        {
            static int mode = 0;

            switch ( mode )
            {
                case 0:
                    cmd->viewangles.pitch = -90.f;
                    mode++;
                    break;

                case 1:
                    cmd->viewangles.pitch = -45.f;
                    mode++;
                    break;

                case 2:
                    cmd->viewangles.pitch = 0.f;
                    mode++;
                    break;

                case 3:
                    cmd->viewangles.pitch = -45.f;
                    mode = 0;
                    break;
            }

            break;
        }

        case PitchAntiAims::ZERO_JITTER:
        {
            static int mode = 0;

            switch ( mode )
            {
                case 0:
                    cmd->viewangles.pitch = 45.f;
                    mode++;
                    break;

                case 1:
                    cmd->viewangles.pitch = 0.f;
                    mode++;
                    break;

                case 2:
                    cmd->viewangles.pitch = -45.f;
                    mode++;
                    break;

                case 3:
                    cmd->viewangles.pitch = 0.f;
                    mode = 0;
                    break;
            }

            break;
        }
        break;

        case PitchAntiAims::SPIN:
        {
            //float pitch = fmodf ( g_GlobalVars->tickcount * g_Config.GetFloat ( "rbot_aa_spinbot_speed" ), 180.f );
			float pitch = fmodf(g_GlobalVars->tickcount * Settings::RageBot::SpinBotSpeed, 180.f);
			Math::NormalizePitch ( pitch );
            cmd->viewangles.pitch = pitch;
            break;
        }

        case PitchAntiAims::SPIN_UP:
        {
            //float pitch = -fmodf ( g_GlobalVars->tickcount * g_Config.GetFloat ( "rbot_aa_spinbot_speed" ), 90.f );
			float pitch = -fmodf(g_GlobalVars->tickcount * Settings::RageBot::SpinBotSpeed, 90.f);
            Math::NormalizePitch ( pitch );
            cmd->viewangles.pitch = pitch;
            break;
        }

        case PitchAntiAims::SPIN_DOWN:
        {
            //float pitch = fmodf ( g_GlobalVars->tickcount * g_Config.GetFloat ( "rbot_aa_spinbot_speed" ), 90.f );
			float pitch = fmodf(g_GlobalVars->tickcount * Settings::RageBot::SpinBotSpeed, 90.f);
            Math::NormalizePitch ( pitch );
            cmd->viewangles.pitch = pitch;
            break;
        }

        case PitchAntiAims::RANDOM:
            cmd->viewangles.pitch = Math::RandomFloat ( -90.f, 90.f );
            break;

        case PitchAntiAims::SWITCH:
        {
            static bool sbool = false;

            if ( sbool )
            {
                cmd->viewangles.pitch = 90.f;
            }
            else
            {
                cmd->viewangles.pitch = -90.f;
            }

            sbool = !sbool;
            break;
        }

        case PitchAntiAims::DOWN_SWITCH:
        {
            static bool sbool = false;

            if ( sbool )
            {
                cmd->viewangles.pitch = 90.f;
            }
            else
            {
                cmd->viewangles.pitch = 0.f;
            }

            sbool = !sbool;
            break;
        }

        case PitchAntiAims::UP_SWITCH:
        {
            static bool sbool = false;

            if ( sbool )
            {
                cmd->viewangles.pitch = -90.f;
            }
            else
            {
                cmd->viewangles.pitch = 0.f;
            }

            sbool = !sbool;
            break;
        }
        break;

        case PitchAntiAims::FAKE_UP:
            cmd->viewangles.pitch = 90.3f;
            break;

        case PitchAntiAims::FAKE_DOWN:
            cmd->viewangles.pitch = -90.3f;
            break;

        case PitchAntiAims::CUSTOM:
            cmd->viewangles.pitch = CustomPitch;
            break;
    }
}

void AntiAim::Yaw ( CUserCmd* cmd, bool fake )
{
    bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
    bool InAir = ! ( g_LocalPlayer->m_fFlags() & FL_ONGROUND );
    bool Standing = !Moving && !InAir;
    YawAntiAims mode = YawAntiAims::NONE;

    float CustomYaw = 0.f;

    //if ( !fake )
    //{
		if ( Settings::RageBot::ManualAAState )
        {
			switch ( Settings::RageBot::ManualAAState )
            {
                case 1: //left
                    cmd->viewangles.yaw -= 90.f;
                    break;

                case 2: //right
                    cmd->viewangles.yaw += 90.f;
                    break;

                case 3:
                    cmd->viewangles.yaw += 180.f;
                    break;
            }

            return;
        }

        if ( Standing )
        {
            //mode = ( YawAntiAims ) g_Config.GetInt ( "rbot_aa_stand_real_yaw" );
			mode = (YawAntiAims)Settings::RageBot::AntiAimSettings[AAState::STANDING].Yaw;
			CustomYaw = Settings::RageBot::AntiAimSettings[AAState::STANDING].YawCustom;
            //CustomYaw = g_Config.GetFloat ( "rbot_aa_stand_real_yaw_custom" );
        }
        else if ( Moving && !InAir )
        {
            //mode = ( YawAntiAims ) g_Config.GetInt ( "rbot_aa_move_real_yaw" );
			mode = (YawAntiAims)Settings::RageBot::AntiAimSettings[AAState::MOVING].Yaw;
			CustomYaw = Settings::RageBot::AntiAimSettings[AAState::MOVING].YawCustom;
            //CustomYaw = g_Config.GetFloat ( "rbot_aa_move_real_yaw_custom" );
        }
        else
        {
            //mode = ( YawAntiAims ) g_Config.GetInt ( "rbot_aa_air_real_yaw" );
			mode = (YawAntiAims)Settings::RageBot::AntiAimSettings[AAState::AIR].Yaw;
			CustomYaw = Settings::RageBot::AntiAimSettings[AAState::AIR].YawCustom;
            //CustomYaw = g_Config.GetFloat ( "rbot_aa_air_real_yaw_custom" );
        }
    /*}
    else
    {
        if ( Standing )
        {
            mode = ( YawAntiAims ) g_Config.GetInt ( "rbot_aa_stand_fake_yaw" );
            CustomYaw = g_Config.GetFloat ( "rbot_aa_stand_fake_yaw_custom" );
        }
        else if ( Moving && !InAir )
        {
            mode = ( YawAntiAims ) g_Config.GetInt ( "rbot_aa_move_fake_yaw" );
            CustomYaw = g_Config.GetFloat ( "rbot_aa_move_fake_yaw_custom" );
        }
        else
        {
            mode = ( YawAntiAims ) g_Config.GetInt ( "rbot_aa_air_fake_yaw" );
            CustomYaw = g_Config.GetFloat ( "rbot_aa_air_fake_yaw_custom" );
        }
    }*/

    switch ( mode )
    {
        case YawAntiAims::BACKWARDS:
            cmd->viewangles.yaw -= 180.f;
            break;

        case YawAntiAims::SPINBOT:
            //cmd->viewangles.yaw = fmodf ( g_GlobalVars->tickcount * g_Config.GetFloat ( "rbot_aa_spinbot_speed" ), 360.f );
			cmd->viewangles.yaw = fmodf(g_GlobalVars->tickcount * Settings::RageBot::SpinBotSpeed, 360.f);
            break;

        case YawAntiAims::LOWER_BODY:
            cmd->viewangles.yaw = g_LocalPlayer->m_flLowerBodyYawTarget();
            break;

        case YawAntiAims::RANDOM:
            cmd->viewangles.yaw = Math::RandomFloat ( -180.f, 180.f );
            break;

        case YawAntiAims::FREESTANDING:
        {
            float ang = 0.f;
            bool canuse = Freestanding ( g_LocalPlayer, ang );

            if ( !canuse )
            {
                cmd->viewangles.yaw -= 180.f;
            }
            else
            {
                cmd->viewangles.yaw = ang;
            }

            break;
        }

        case YawAntiAims::CUSTOM:
            cmd->viewangles.yaw = CustomYaw;
            break;
    }
}

void AntiAim::YawAdd ( CUserCmd* cmd, bool fake )
{
    bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
    bool InAir = ! ( g_LocalPlayer->m_fFlags() & FL_ONGROUND );
    bool Standing = !Moving && !InAir;
    YawAddAntiAims mode = YawAddAntiAims::NONE;

    float YawAddRange = 0.f;

    if ( Standing )
    {
		mode = (YawAddAntiAims)Settings::RageBot::AntiAimSettings[AAState::STANDING].YawAdd;
		YawAddRange = Settings::RageBot::AntiAimSettings[AAState::STANDING].Range;
    }
    else if ( Moving && !InAir )
    {
		mode = (YawAddAntiAims)Settings::RageBot::AntiAimSettings[AAState::MOVING].YawAdd;
		YawAddRange = Settings::RageBot::AntiAimSettings[AAState::MOVING].Range;
    }
    else
    {
		mode = (YawAddAntiAims)Settings::RageBot::AntiAimSettings[AAState::AIR].YawAdd;
		YawAddRange = Settings::RageBot::AntiAimSettings[AAState::AIR].Range;
    }

    switch ( mode )
    {
        case YawAddAntiAims::JITTER:
        {
            static int mode = 0;

            switch ( mode )
            {
                case 0:
                    cmd->viewangles.yaw += YawAddRange;
                    mode++;
                    break;

                case 1:
                    mode++;
                    break;

                case 2:
                    cmd->viewangles.yaw -= YawAddRange;
                    mode = 0;
                    break;
            }

            break;
        }

        case YawAddAntiAims::SWITCH:
        {
            static bool sbool = false;

            if ( sbool )
            {
                cmd->viewangles.yaw += YawAddRange;
            }
            else
            {
                cmd->viewangles.yaw -= YawAddRange;
            }

            sbool = !sbool;
            break;
        }

        case YawAddAntiAims::SPIN:
        {
			cmd->viewangles.yaw += fmodf(g_GlobalVars->tickcount * Settings::RageBot::SpinBotSpeed, YawAddRange);
			break;
        }

        case YawAddAntiAims::RANDOM:
            cmd->viewangles.yaw += Math::RandomFloat ( -YawAddRange, YawAddRange );
            break;
    }
}

void AntiAim::DesyncAnimation(CUserCmd* cmd, bool& bSendPacket, int type)
{
	if (!type)
		return;

	cmd->viewangles.yaw = Math::NormalizeAngle(cmd->viewangles.yaw);

	if (type == DesyncState::JITTER) 
	{
		int jitter_side = 1;
		/*if (rbot_antiaim_autodir) 
		{
			jitter_side = m_iAutoDirection;
			if (!m_bAutomaticDir)
				cmd->viewangles.yaw += 90.0f * static_cast<float>(m_iAutoDirection);
		}
		else {*/
			cmd->viewangles.yaw += 90.0f;
		//}

		float desync = g_LocalPlayer->GetMaxDesyncAngle();
		float inverse_desync = 190.0f - desync;
		float jitter = 180.0f - inverse_desync * 0.5f;

		if (jitter_side == 1)
			cmd->viewangles.yaw += jitter;
		else if (jitter_side == -1)
			cmd->viewangles.yaw -= jitter;

#if _DEBUG
		float jitterRotate = 90.0f * jitter_side + cmd->viewangles.yaw;
		if (DesyncRotate(jitterRotate, jitter_side, cmd, bSendPacket)) 
		{
			m_iRotate = 0;
			return;
		}
#endif

		int rotate = 0;
		if (g_GlobalVars->curtime < g_Saver.NextLbyUpdate) 
			rotate = m_iRotate;
		else
			m_iRotate = 0;

		rotate--;
		if (rotate) {
			if (rotate == 1) 
			{
				if (jitter_side == 1)
					cmd->viewangles.yaw += inverse_desync;
				else
					cmd->viewangles.yaw += desync - 190.0f;
			}
		}
		else 
		{
			if (jitter_side == 1)
				cmd->viewangles.yaw += desync - 190.0f;
			else
				cmd->viewangles.yaw += inverse_desync;
			bSendPacket = false;
		}

		if (++m_iRotate >= 3)
			m_iRotate = 0;
	}
	else
	{
#if _DEBUG
		float stretch = cmd->viewangles.yaw;
		if (type == DesyncState::STRETCH)
			stretch -= 90.0f;
		else
			stretch += 90.0f;
		stretch = Math::NormalizeAngle(stretch);
		if (DesyncRotate(stretch, type == DesyncState::STRETCH, cmd, bSendPacket))
			return;
#endif



		float desync = g_LocalPlayer->GetMaxDesyncAngle();
		float balance = 1.0f;
		if (type == DesyncState::BALANCE)
			balance = -1.0f;

		/*if (bSendPacket && g_ClientState->chokedcommands >= 2) 
		{
			cmd->viewangles.yaw = Math::NormalizeAngle(cmd->viewangles.yaw);
			return;
		}

		if (type == DesyncState::STATIC)
			cmd->viewangles.yaw -= 100.0f;
		else
			cmd->viewangles.yaw += (balance * 120.0f);

		else if (type != DesyncState::STATIC)
			// lby breaker
			cmd->viewangles.yaw -= (desync + 30.0f) * balance;*/

		if (g_Saver.NextLbyUpdate >= g_GlobalVars->curtime) 
		{
			if (bSendPacket && g_ClientState->chokedcommands >= 2) 
			{
				cmd->viewangles.yaw = Math::NormalizeAngle(cmd->viewangles.yaw);
				return;
			}

			if (type == DesyncState::STATIC)
				cmd->viewangles.yaw -= 100.0f;
			else
				cmd->viewangles.yaw += (balance * 120.0f);
		}
		else if (type != DesyncState::STATIC) 
		{
			// lby breaker
			//cmd->viewangles.yaw -= (desync + 30.0f) * balance;
			cmd->viewangles.yaw -= (desync + 30.f);
		}

		bSendPacket = false;
	}
	cmd->viewangles.yaw = Math::NormalizeAngle(cmd->viewangles.yaw);
}

bool AntiAim::DesyncRotate(float rotation, int direction, CUserCmd* cmd, bool& bSendPacket)
{
	CCSGOPlayerAnimState* animState = g_LocalPlayer->GetPlayerAnimState();
	float feetYaw = DEG2RAD(animState->m_flGoalFeetYaw);
	m_flCurrentFeetYaw = animState->m_flGoalFeetYaw;

	float feetSin, feetCos;
	DirectX::XMScalarSinCos(&feetSin, &feetCos, feetYaw);

	float feetSin1, feetCos1;
	DirectX::XMScalarSinCos(&feetSin1, &feetCos1, m_flCurrentFeetYaw);

	float feetSin2, feetCos2;
	DirectX::XMScalarSinCos(&feetSin2, &feetCos2, m_flPreviousFeetYaw);

	m_flPreviousFeetYaw = m_flCurrentFeetYaw;
	m_flCurrentFeetYaw = animState->m_flGoalFeetYaw;

	float totalRotation = atan2(feetSin1 + feetSin + feetSin2, feetCos1 + feetCos + feetCos2);
	totalRotation = Math::NormalizeAngle(RAD2DEG(totalRotation) - rotation); //Math::AngleNormalize(RAD2DEG(totalRotation) - rotation);
	if (direction == 1)
	{
		if (totalRotation >= 0.0f) 
		{
			m_iRotateIteration = 1;
			return false;
		}
	}
	else if (totalRotation <= 0.0f) 
	{
		m_iRotateIteration = 1;
		return false;
	}

	float rotate = static_cast<float>(30 * (m_iRotateIteration % 12));
	if (direction == 1)
		cmd->viewangles.yaw -= rotate;
	else
		cmd->viewangles.yaw += rotate;

	cmd->viewangles.yaw = Math::NormalizeAngle(cmd->viewangles.yaw); //Math::AngleNormalize(cmd->viewangles.yaw);
	//bSendPacket = true;
	++m_iRotateIteration;
	return true;
}

bool AntiAim::GetEdgeDetectAngle ( C_BasePlayer* entity, float& yaw )
{
    Vector position = entity->m_vecOrigin() + entity->m_vecViewOffset();

    float closest_distance = 100.0f;

    float radius = 40.f + 0.1f;
    float step = M_PI * 2.0 / 60;

    for ( float a = 0; a < ( M_PI * 2.0 ); a += step )
    {
        Vector location ( radius * cos ( a ) + position.x, radius * sin ( a ) + position.y, position.z );

        Ray_t ray;
        trace_t tr;
        ray.Init ( position, location );
        CTraceFilter traceFilter;
        traceFilter.pSkip = entity;
        g_EngineTrace->TraceRay ( ray, 0x4600400B, &traceFilter, &tr );

        float distance = position.DistTo ( tr.endpos );

        if ( distance < closest_distance )
        {
            closest_distance = distance;
            yaw = RAD2DEG ( a );
        }
    }

    return closest_distance < 40.f;
}