
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

using AAState = Settings::RageBot::AntiAimType;

void AntiAim::OnCreateMove ( CUserCmd* cmd, bool& bSendPacket )
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive())
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
        {
            g_Saver.FakelagData.ang = cmd->viewangles;
        }

        return;
    }

    C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

    if ( !weapon )
    {
        return;
    }

    if ( weapon->m_flNextPrimaryAttack() - g_GlobalVars->curtime < g_GlobalVars->interval_per_tick && ( cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2 ) )
    {
        return;
    }

    if ( movetype == MOVETYPE_LADDER )
    {
        static bool last = false;
        bSendPacket = last;
        last = !last;

        if ( bSendPacket )
        {
            g_Saver.FakelagData.ang = cmd->viewangles;
        }

        return;
    }

    if ( weapon->IsGrenade() && weapon->m_fThrowTime() > 0.1f )
    {
        bSendPacket = false;
        return;
    }

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

void AntiAim::SlideWalk ( CUserCmd* cmd )
{
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
    {
        return false;
    }

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
        {
            endpos1 = trace1.endpos;
        }
        else
        {
            return 0.f;
        }

        ray.Init ( eyepos, newhead );
        g_EngineTrace->TraceRay ( ray, MASK_SHOT_BRUSHONLY | MASK_OPAQUE_AND_NPCS, &filter, &trace2 );

        if ( trace2.DidHit() )
        {
            endpos2 = trace2.endpos;
        }

        float add = newhead.DistTo ( eyepos ) - leyepos.DistTo ( eyepos ) + 3.f;
        return endpos1.DistTo ( endpos2 ) + add / 3;

    };

    int index = GetNearestPlayerToCrosshair();
    static C_BasePlayer* entity;

    if ( !local->IsAlive() )
    {
        hold = 0.f;
    }

    if ( index != -1 )
    {
        entity = ( C_BasePlayer* ) g_EntityList->GetClientEntity ( index ); // maybe?
    }

    if ( !entity || entity == nullptr )
    {
        return false;
    }

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

            if ( totalthickness > highestthickness )
            {
                highestthickness = totalthickness;

                bestrotation = besthead;
            }
        }
    }

    if ( no_active )
    {
        return false;
    }
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
    {
        return false;
    }

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
        {
            endpos1 = trace1.endpos;
        }
        else
        {
            return 0.f;
        }

        ray.Init ( eyepos, newhead );
        g_EngineTrace->TraceRay ( ray, MASK_SHOT_BRUSHONLY | MASK_OPAQUE_AND_NPCS, &filter, &trace2 );

        if ( trace2.DidHit() )
        {
            endpos2 = trace2.endpos;
        }

        float add = newhead.DistTo ( eyepos ) - leyepos.DistTo ( eyepos ) + 3.f;
        return endpos1.DistTo ( endpos2 ) + add / 3;

    };

    int index = GetNearestPlayerToCrosshair();
    static C_BasePlayer* entity;

    if ( !local->IsAlive() )
    {
        hold = 0.f;
    }

    if ( index != -1 )
    {
        entity = ( C_BasePlayer* ) g_EntityList->GetClientEntity ( index ); // maybe?
    }

    if ( !entity || entity == nullptr )
    {
        return false;
    }

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

float AntiAim::GetMaxDesyncYaw()
{
    /*
    //g_LocalPlayer->GetBasePlayerAnimState();
    auto animstate = uintptr_t(this->GetBasePlayerAnimState());

    //float rate = 180;
    float duckammount = *(float *)(animstate + 0xA4);
    float speedfraction = std::max(0, std::min(*reinterpret_cast<float*>(animstate + 0xF8), 1));

    float speedfactor = std::max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));

    float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
    float unk2 = unk1 + 1.f;
    float unk3;

    if (duckammount > 0) {

    	unk2 += +((duckammount * speedfactor) * (0.5f - unk2));

    }

    unk3 = *(float *)(animstate + 0x334) * unk2;

    return 0.0f;
    */
    return 0.f;
}

void AntiAim::LbyBreakerPrediction ( CUserCmd* cmd, bool& bSendPacket )
{
    return;

    //if ( !g_Config.GetBool ( "rbot_aa_desync" ) || !g_Config.GetBool ( "rbot" ) || !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
	if (!Settings::RageBot::Desync || !Settings::RageBot::Enabled || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
        return;
    }

    /*
    new
    */
    static bool m_bBreakLowerBody = false;
    static bool fakeBreak = false;
    static float_t m_flSpawnTime = 0.f;
    static float_t m_flNextBodyUpdate = 0.f;
    static bool brokeThisTick = false;


    allocate = ( m_serverAnimState == nullptr );
    change = ( !allocate ) && ( &g_LocalPlayer->GetRefEHandle() != m_ulEntHandle );
    reset = ( !allocate && !change ) && ( g_LocalPlayer->m_flSpawnTime() != m_flSpawnTime );


    // player changed, free old animation state.
    if ( change )
    {
        m_serverAnimState = nullptr;
        //g_pMemAlloc->Free(m_serverAnimState);
    }

    // need to reset? (on respawn)

    // need to allocate or create new due to player change.
    if ( allocate || change )
    {

        // only works with games heap alloc.
        CCSGOPlayerAnimState* state = ( CCSGOPlayerAnimState* ) g_pMemAlloc->Alloc ( sizeof ( CCSGOPlayerAnimState ) );

        if ( state != nullptr )
        {
            g_LocalPlayer->CreateAnimationState ( state );
        }

        // used to detect if we need to recreate / reset.
        m_ulEntHandle = const_cast<CBaseHandle*> ( &g_LocalPlayer->GetRefEHandle() );
        m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();

        // note anim state for future use.
        m_serverAnimState = state;
    }

    // back up some data to not mess with game..

    //float cur_time = cur_time = TICKS_TO_TIME(AimRage::Get().GetTickbase());
    if ( !g_ClientState->chokedcommands )
    {
        C_BasePlayer::UpdateAnimationState ( m_serverAnimState, cmd->viewangles );

        // calculate vars
        //float delta = std::fabs(cmd->viewangles.yaw - g_LocalPlayer->m_flLowerBodyYawTarget());

        // walking, delay next update by .22s.
        if ( m_serverAnimState->GetVelocity() > 0.1f && !g_Saver.InFakewalk )
        {
            //Console.WriteLine("moving");
            m_flNextBodyUpdate = g_Saver.curtime + 0.22f;
            g_Saver.NextLbyUpdate = m_flNextBodyUpdate;
            //brokeThisTick = true;
        }
        else if ( g_Saver.curtime >= m_flNextBodyUpdate )
        {
            float delta = std::abs ( cmd->viewangles.yaw - g_LocalPlayer->m_flLowerBodyYawTarget() );

            //Console.WriteLine("moving standing");
            if ( delta > 35.f )
            {
                //brokeThisTick = true;
                m_flNextBodyUpdate = g_Saver.curtime + 1.1f;// + g_GlobalVars->interval_per_tick;
                g_Saver.NextLbyUpdate = m_flNextBodyUpdate;
                Console.WriteLine ( g_Saver.curtime );
                Console.WriteLine ( m_flNextBodyUpdate );
            }
        }
    }

    // if was jumping and then onground and bsendpacket true, we're gonna update.
    m_bBreakLowerBody = ( g_LocalPlayer->m_fFlags() & FL_ONGROUND ) && ( ( m_flNextBodyUpdate - g_Saver.curtime ) <= g_GlobalVars->interval_per_tick );
    float lol = ( ( m_flNextBodyUpdate - ( g_GlobalVars->interval_per_tick * 10 ) ) - g_Saver.curtime );
    fakeBreak = ( g_LocalPlayer->m_fFlags() & FL_ONGROUND ) && lol <= g_GlobalVars->interval_per_tick && lol >= 0.f;
    //if (m_bBreakLowerBody) Console.WriteLine("lby break animation detection");

    static bool WasLastfakeBreak = false;

    if ( WasLastfakeBreak || fakeBreak && g_Config.GetBool ( "rbot_aa_fake_lby_breaker" ) )
    {
        if ( WasLastfakeBreak )
        {
            WasLastfakeBreak = false;
        }
        else
        {
            WasLastfakeBreak = true;
        }

        cmd->viewangles.yaw += 150.f;
        brokeThisTick = true;
    }

    static bool BrokeLast = false;

    //if (!bSendPacket)
    {
        if ( m_bBreakLowerBody && g_LocalPlayer->m_vecVelocity().Length2D() < 0.1f && !BrokeLast )
        {
            cmd->viewangles.yaw += 58.f;
            brokeThisTick = true;
        }


        if ( brokeThisTick && !BrokeLast )
        {
            BrokeLast = true;
            m_bBreakLowerBody = false;
            g_Saver.CurrentInLbyBreak = true;
            //g_Saver.AARealAngle = cmd->viewangles;
            //g_Saver.FakelagData.ang = cmd->viewangles;
            brokeThisTick = false;
            bSendPacket = false;

            if ( g_LocalPlayer->m_hActiveWeapon().Get() && g_LocalPlayer->m_hActiveWeapon().Get()->IsGrenade() )
            {
                return;
            }

            cmd->buttons &= ~IN_ATTACK;
        }
        else
        {
            BrokeLast = false;
        }
    }
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
    /*
    bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
    bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
    bool Standing = !Moving && !InAir;

    if (Standing) { mode = (YawAntiAims)g_Config.GetInt("rbot_aa_stand_fake_yaw"); CustomYaw = g_Config.GetFloat("rbot_aa_stand_fake_yaw_custom"); }
    	else if (Moving && !InAir) { mode = (YawAntiAims)g_Config.GetInt("rbot_aa_move_fake_yaw"); CustomYaw = g_Config.GetFloat("rbot_aa_move_fake_yaw_custom"); }
    	else { mode = (YawAntiAims)g_Config.GetInt("rbot_aa_air_fake_yaw"); CustomYaw = g_Config.GetFloat("rbot_aa_air_fake_yaw_custom"); }
    */
    /*
    bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
    bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
    bool Standing = !Moving && !InAir;

    bool UsingFake = false;
    if (Standing) UsingFake = (YawAntiAims)g_Config.GetInt("rbot_aa_stand_fake_yaw") != YawAntiAims::NONE;
    else if (Moving && !InAir) UsingFake = (YawAntiAims)g_Config.GetInt("rbot_aa_move_fake_yaw") != YawAntiAims::NONE;
    else UsingFake = (YawAntiAims)g_Config.GetInt("rbot_aa_air_fake_yaw") != YawAntiAims::NONE;
    */

    /*
    static int ChokedPackets = -1;
    static float LastRealPitch = 0.f;
    if (!g_Saver.InFakewalk && UsingFake && (!g_Config.GetBool("misc_fakelag") || !g_Saver.FakelagCurrentlyEnabled || g_Saver.RbotAADidShot))
    {
    	if (g_Saver.RbotAADidShot)
    	{
    		g_Saver.RbotAADidShot = false;
    	}

    	ChokedPackets++;
    	if (ChokedPackets < 1)
    	{
    		bSendPacket = true;
    	}
    	else
    	{
    		bSendPacket = false;
    	}
    }
    */

    Yaw ( cmd, false );
    YawAdd ( cmd, false );
    Pitch ( cmd );

    //if ( g_Config.GetBool ( "rbot_aa_desync" ) )
	if ( Settings::RageBot::Desync )
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
        //if (!g_Saver.FakelagCurrentlyEnabled) bSendPacket = cmd->tick_count % 2;

        if ( !bSendPacket && ! ( cmd->buttons & IN_ATTACK ) )
        {
            static bool bFlip = false;
            cmd->viewangles += bFlip ? 58.f : -58.f;
        }

        if ( bSendPacket )
        {
            LastRealAngle = cmd->viewangles;
        }

        g_Saver.FakelagData.ang = LastRealAngle;
    }

    g_Saver.AARealAngle = cmd->viewangles;

    /*

    if (!UsingFake)
    {
    	Yaw(cmd, false);
    	YawAdd(cmd, false);
    	ChokedPackets = -1;
    	Pitch(cmd);
    	LastRealPitch = cmd->viewangles.pitch;
    	g_Saver.AARealAngle = cmd->viewangles;
    	return;
    }
    */
    /*
    if (bSendPacket)
    {
    	Yaw(cmd, true);
    	YawAdd(cmd, true);
    	cmd->viewangles.pitch = LastRealPitch;
    	g_Saver.AAFakeAngle = cmd->viewangles;
    }
    else
    {
    	Yaw(cmd, false);
    	YawAdd(cmd, false);
    	ChokedPackets = -1;
    	Pitch(cmd);
    	LastRealPitch = cmd->viewangles.pitch;
    	g_Saver.AARealAngle = cmd->viewangles;
    }
    */
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

    if ( !fake )
    {
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
    }
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
    }

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

    //float CustomYawAdd = 0.f;
    float YawAddRange = 0.f;

    if ( !fake )
    {
        if ( Standing )
        {
            //mode = ( YawAddAntiAims ) g_Config.GetInt ( "rbot_aa_stand_real_add_yaw_add" );
			mode = (YawAddAntiAims)Settings::RageBot::AntiAimSettings[AAState::STANDING].YawAdd;
			YawAddRange = Settings::RageBot::AntiAimSettings[AAState::STANDING].Range;
            //YawAddRange = g_Config.GetFloat ( "rbot_aa_stand_real_add_yaw_add_range" );
        }
        else if ( Moving && !InAir )
        {
            //mode = ( YawAddAntiAims ) g_Config.GetInt ( "rbot_aa_move_real_add_yaw_add" );
			mode = (YawAddAntiAims)Settings::RageBot::AntiAimSettings[AAState::MOVING].YawAdd;
			YawAddRange = Settings::RageBot::AntiAimSettings[AAState::MOVING].Range;
            //YawAddRange = g_Config.GetFloat ( "rbot_aa_move_real_add_yaw_add_range" );
        }
        else
        {
            //mode = ( YawAddAntiAims ) g_Config.GetInt ( "rbot_aa_air_real_add_yaw_add" );
			mode = (YawAddAntiAims)Settings::RageBot::AntiAimSettings[AAState::AIR].YawAdd;
			YawAddRange = Settings::RageBot::AntiAimSettings[AAState::AIR].Range;
			//YawAddRange = 15.f;
            //YawAddRange = g_Config.GetFloat ( "rbot_aa_air_real_add_yaw_add_range" );
        }
    }
    else
    {
        if ( Standing )
        {
            mode = ( YawAddAntiAims ) g_Config.GetInt ( "rbot_aa_stand_fake_add_yaw_add" );
            YawAddRange = g_Config.GetFloat ( "rbot_aa_stand_fake_add_yaw_add_range" );
        }
        else if ( Moving && !InAir )
        {
            mode = ( YawAddAntiAims ) g_Config.GetInt ( "rbot_aa_move_fake_add_yaw_add" );
            YawAddRange = g_Config.GetFloat ( "rbot_aa_move_fake_add_yaw_add_range" );
        }
        else
        {
            mode = ( YawAddAntiAims ) g_Config.GetInt ( "rbot_aa_air_fake_add_yaw_add" );
            YawAddRange = g_Config.GetFloat ( "rbot_aa_air_fake_add_yaw_add_range" );
        }
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
            //cmd->viewangles.yaw += fmodf ( g_GlobalVars->tickcount * g_Config.GetFloat ( "rbot_aa_spinbot_speed" ), YawAddRange );
			cmd->viewangles.yaw += fmodf(g_GlobalVars->tickcount * Settings::RageBot::SpinBotSpeed, YawAddRange);
			break;
        }

        case YawAddAntiAims::RANDOM:
            cmd->viewangles.yaw += Math::RandomFloat ( -YawAddRange, YawAddRange );
            break;
    }
}

bool AntiAim::GetEdgeDetectAngle ( C_BasePlayer* entity, float& yaw )
{
    //C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

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

        float distance = position.DistTo ( tr.endpos ); // Distance(position, tr.endpos);

        if ( distance < closest_distance )
        {
            closest_distance = distance;
            yaw = RAD2DEG ( a );
        }
    }

    return closest_distance < 40.f;
}


void AntiAim::Fakewalk ( CUserCmd* cmd, bool& bSendPackets )
{
    g_Saver.InFakewalk = false;

    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
    {
        return;
    }

    bool InAir = ! ( g_LocalPlayer->m_fFlags() & FL_ONGROUND );

    if ( InAir || ( cmd->sidemove == 0.f && cmd->forwardmove == 0.f ) )
    {
        return;
    }

    if ( !InputSys::Get().IsKeyDown ( g_Config.GetInt ( "rbot_aa_fakewalk_key" ) ) )
    {
        return;
    }

    static int choked = 0;
    choked = choked > 15 ? 0 : choked + 1;
    bSendPackets = choked > 14;
    cmd->forwardmove = std::clamp ( cmd->forwardmove, -30.f, 30.f );
    cmd->sidemove = std::clamp ( cmd->sidemove, -30.f, 30.f );

    //if (bSendPackets)
    //FakeLag::lastUnchokedPos = Globals::pLocal->GetAbsOriginal2();

    Vector velocity = g_LocalPlayer->m_vecVelocity();

    if ( choked > 5 )
    {
        if ( choked < 8 && velocity.Length2D() != 0.f )
        {
            Vector direction = velocity.Direction();
            float speed = velocity.Length();
            direction.y = cmd->viewangles.yaw - direction.y;

            Vector forward;
            Math::AngleVector ( direction, forward );

            Vector negated_direcition = forward * -speed;

            cmd->forwardmove = negated_direcition.x;
            cmd->sidemove = negated_direcition.y;
        }
        else
        {
            cmd->forwardmove = 0;
            cmd->sidemove = 0;
        }
    }

    g_Saver.InFakewalk = true;
}