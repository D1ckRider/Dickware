
#include "Backtrack.h"
#include "Rbot.h"
#include "ConfigSystem.h"
#include "helpers/input.hpp"
#include "helpers/math.hpp"
#include "Autowall.h"
#include "Logger.h"
#include "Resolver.h"
#include "helpers/utils.hpp"
#include "RuntimeSaver.h"
#include "Settings.h"
#include "ConsoleHelper.h"

RbotMatrixData matrixData[128];

TickRecord bRecord;

void Rbot::CreateMove(CUserCmd* cmd, bool& bSendPacket)
{
	static bool DidShotLastTick = false;

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();

	if (!weapon)
		return;

	CurrentCmd = cmd;
	UpdateWeaponConfig(weapon);

	// Get debug config info
	if (GetAsyncKeyState(VK_DELETE))
	{
		g_Logger.Info("INFO", "WeaponID is " + std::to_string(Settings::RageBot::GetWeaponType(weapon)));
		g_Logger.Info("INFO", "Min Damage is " + std::to_string(MinDmg));
	}

	if (weapon->IsKnife())
		return;



	if (weapon->IsZeus())
	{
		ZeusBot(cmd, weapon);
		return;
	}

	/* Calling SlowWalk */
	if (weapon->GetItemDefinitionIndex() == WEAPON_SCAR20 || weapon->GetItemDefinitionIndex() == WEAPON_G3SG1)
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
			SlowWalk(cmd, 40);

	if (weapon->GetItemDefinitionIndex() == WEAPON_AWP)
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
			SlowWalk(cmd, 33);

	if (weapon->GetItemDefinitionIndex() == WEAPON_SSG08)
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
			SlowWalk(cmd, 70);

	if (!weapon->IsSniper())  //for every weapon btw
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
			SlowWalk(cmd, 34);

	if (Settings::RageBot::FakeDuck && InputSys::Get().IsKeyDown(Settings::RageBot::FakeDuckHotkey))
		FakeDuck(cmd);


	//Console.WriteLine(g_GlobalVars->curtime - weapon->m_flNextPrimaryAttack());
	//if (g_Saver.curtime - weapon->m_flNextPrimaryAttack() < 0.f) return;

	Vector hitpos = Vector(0, 0, 0);
	bool bBacktrack = false;
	//TickRecord bBacktrackRecord = bRecord;
	UpdateConfigData();
	int BestEntity = FindBestEntity(cmd, weapon, hitpos, bBacktrack);

	//Console.WriteLine("Backtrack");
	//Console.WriteLine(bBacktrack ? "true" : "false");

	if (hitpos == Vector(0, 0, 0))
		return;

	if (BestEntity == -1)
		return;

	C_BasePlayer* entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(BestEntity));

	QAngle newAng = Math::CalcAngle(g_LocalPlayer->GetEyePos(), hitpos);

	//float recoil_scale = g_CVar->FindVar("weapon_recoil_scale")->GetFloat();
	QAngle a = (g_LocalPlayer->m_aimPunchAngle() * 2);
	//Math::NormalizeAngles(a);
	a.roll = 0.f;
	newAng -= a;

	//Vector v;
	//Math::AngleVectors(a, v);

	//float tdmg = Autowall::Get().CanHit(hitpos - v);

	//if (tdmg == -1.f) return;

	if (weapon->IsSniper() && !g_LocalPlayer->m_bIsScoped() && (g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_Config.GetBool("rbot_autoscope"))
	{
		if (!(cmd->buttons & IN_ZOOM))
			cmd->buttons |= IN_ZOOM;

		if (Settings::RageBot::AutoStop)
			AutoStop(cmd);

		if (!(cmd->buttons & IN_DUCK) && Settings::RageBot::AutoCrouch)
			AutoCrouch(cmd);

		return;
	}

	if (!HitChance(newAng, entity, Hitchance))
	{
		if (Settings::RageBot::AutoStop)
			AutoStop(cmd);

		if (!(cmd->buttons & IN_DUCK) && Settings::RageBot::AutoCrouch)
			AutoCrouch(cmd);

		return;
	}

	if (!weapon->CanFire() && g_Saver.curtime <= weapon->m_flNextPrimaryAttack())
		return;

	if (DidShotLastTick)
	{
		DidShotLastTick = false;
		return;
	}

	int rbot_shooting_mode = Settings::RageBot::ShootingMode;

	if (rbot_shooting_mode == 2) g_Saver.RbotDidLastShot = true;

	LastRbotEnemyIndex = BestEntity;

	//newAng.Normalize();
	Math::NormalizeAngles(newAng);
	Math::ClampAngles(newAng);

	//if (newAng.pitch < -89.f || newAng.pitch > 89.f) return;

	//if (newAng.yaw == 0.f && newAng.pitch == 0.f && newAng.roll == 0.f) return;

	int tick = TIME_TO_TICKS(entity->m_flSimulationTime() + Backtrack::Get().GetLerpTime());

	if (!Backtrack::Get().IsTickValid(tick))
		return;

	DidShotLastTick = true;
	cmd->viewangles = newAng;
	if (weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
	{
		static int delay = 0;
		delay++;

		if (delay <= 15)
			cmd->buttons |= IN_ATTACK;
		else
			delay = 0;
	}

	cmd->buttons |= IN_ATTACK;
	
    //DidShotLastTick = true;
    g_Saver.RbotShotInfo.InLbyUpdate = g_Resolver.GResolverData[BestEntity].mode == ResolverModes::LBY_BREAK;
    g_Saver.RbotShotInfo.InLc = g_Resolver.GResolverData[BestEntity].BreakingLC;
    g_Saver.RbotShotInfo.Moving = g_Resolver.GResolverData[BestEntity].Moving;
    g_Saver.AARealAngle = cmd->viewangles;
    //Console.WriteLine(Backtrack::Get().GetLerpTime());
    //cmd->tick_count = TIME_TO_TICKS(entity->m_flSimulationTime() - GetLerpTimeX());

    if ( !bBacktrack )
        cmd->tick_count = tick;

    switch ( rbot_shooting_mode )
    {
        case 0:
            bSendPacket = true;
            break;

        case 2:
            //g_Saver.RbotDidLastShot = true;
            g_Saver.RbotDidLastShot = true;
            break;
    }

    /*
    - add mode choke after shot
    */
}

void Rbot::OnFireEvent ( IGameEvent* event )
{
    if ( !g_LocalPlayer )
        return;

    //Utils::ConsolePrint(std::string(std::string(event->GetName()) + std::string("\n")).c_str());
    static int LastEventShotLastEntIndex = -1;

    if ( !strcmp ( event->GetName(), "weapon_fire" ) )
    {
        if ( g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) ) == g_LocalPlayer->EntIndex() )
        {
            //g_Logger.Add("onfire entity", std::to_string(LastRbotEnemyIndex), Color::Blue);
            if ( LastRbotEnemyIndex != -1 )
            {
                LastEventShotLastEntIndex = LastRbotEnemyIndex;
                //g_Resolver.GResolverData[LastRbotEnemyIndex].Shots++;
                //g_Resolver.GResolverData[LastRbotEnemyIndex].ShotsAtMode[(int)g_Resolver.GResolverData[LastRbotEnemyIndex].mode]++;
                LastRbotEnemyIndex = -1;
            }
        }
    }

    static float lastEventTime = 0.f;

    /*


    needs fix for getting old eye pos (g_saver.lastshoteyepos)


    */
    if ( !strcmp ( event->GetName(), "bullet_impact" ) )
    {
        C_BasePlayer* shooter = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) ) ) );

        if ( !shooter || shooter != g_LocalPlayer )
            return;

        //ShotTracer(g_LocalPlayer->GetEyePos(), p);
        if ( LastEventShotLastEntIndex > 128 )
            LastEventShotLastEntIndex = -1;

        if ( lastEventTime == g_GlobalVars->curtime || LastEventShotLastEntIndex == -1 )
            return;

        lastEventTime = g_GlobalVars->curtime;

        Vector p = Vector ( event->GetFloat ( "x" ), event->GetFloat ( "y" ), event->GetFloat ( "z" ) );
        //g_LocalPlayer

        float Dmg = -1.f;

        if ( g_Saver.LastShotEyePos == Vector ( 0.f, 0.f, 0.f ) )
            Dmg = Autowall::Get().CanHit ( p );
        else
            Dmg = Autowall::Get().CanHit ( g_Saver.LastShotEyePos, p );

        //Console.WriteLine("Damage");
        //Console.WriteLine(Dmg);
        if ( Dmg == -1.f )
        {
            g_Logger.Error ( "MISS", "missed due to spread" );
            //LastMissedDueToSpread = true;
        }
        else
        {
            /*
            if (!g_Saver.RbotShotInfo.InLbyUpdate)
            {
            	g_Resolver.GResolverData[LastEventShotLastEntIndex].Shots++;
            	g_Resolver.GResolverData[LastEventShotLastEntIndex].ShotsAtMode[(int)g_Resolver.GResolverData[LastEventShotLastEntIndex].mode]++;
            	LastEventShotLastEntIndex = -1;
            }
            */
            //LastMissedDueToSpread = false;
        }

        g_Resolver.GResolverData[LastEventShotLastEntIndex].Shots++;
        g_Resolver.GResolverData[LastEventShotLastEntIndex].ShotsAtMode[ ( int ) g_Resolver.GResolverData[LastEventShotLastEntIndex].mode]++;
        LastEventShotLastEntIndex = -1;
    }
}

void Rbot::UpdateWeaponConfig(C_BaseCombatWeapon * weapon)
{
	int WeaponID = Settings::RageBot::GetWeaponType(weapon);

	Hitchance = Settings::RageBot::WeaponSettings[WeaponID].Hitchance;
	MinDmg = Settings::RageBot::WeaponSettings[WeaponID].MinDamage;

	BAimAfter = Settings::RageBot::WeaponSettings[WeaponID].BAimAfterShots;
	ForceBAimAfter = Settings::RageBot::WeaponSettings[WeaponID].ForceBAimAfterShots;
	MovingBAim = Settings::RageBot::WeaponSettings[WeaponID].BAimWhileMoving;
}

bool Rbot::InFakeLag ( C_BasePlayer* player )
{
    bool rBool = true;
    float CurrentSimtime = player->m_flSimulationTime();
    int i = player->EntIndex();

    if ( Simtimes[i] != CurrentSimtime )
        rBool = false;

    Simtimes[i] = CurrentSimtime;
    return rBool;
}

void Rbot::FakeDuck(CUserCmd * cmd, bool &bSendPackets)
{
	if (cmd->buttons & IN_DUCK) 
	{
		static bool counter = false;
		static int counte = 0;
		if (counte == 9) 
		{
			counte = 0;
			counter = !counter;
		}
		counte++;
		if (counter) 
		{
			cmd->buttons |= IN_DUCK;
			bSendPackets = true;
		}
		else 
			cmd->buttons &= ~IN_DUCK;
	}
}

void Rbot::FakeDuck(CUserCmd * cmd)
{
	int fakelag_limit = Settings::RageBot::AntiAimSettings[0].FakelagTicks;
	int choked_goal = fakelag_limit / 2;
	bool should_crouch = g_ClientState->chokedcommands >= choked_goal;

	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
	{
		cmd->buttons |= IN_BULLRUSH;
		if (should_crouch)
			cmd->buttons |= IN_DUCK;
		else
			cmd->buttons &= ~IN_DUCK;
	}
}

void Rbot::AutoStop ( CUserCmd* cmd )
{
    cmd->forwardmove = 0;
    cmd->sidemove = 0;
    cmd->upmove = 0;
}

void Rbot::AutoCrouch ( CUserCmd* cmd )
{
    cmd->buttons |= IN_DUCK;
}

void Rbot::SlowWalk( CUserCmd * cmd, float speed )
{
	if( !Settings::RageBot::SlowWalk || !InputSys::Get().IsKeyDown(Settings::RageBot::SlowWalkHotkey) )
		return;

	if (speed <= 0.f)
		return;

	float min_speed = (float)(Math::FASTSQRT((cmd->forwardmove)*(cmd->forwardmove) + (cmd->sidemove)*(cmd->sidemove) + (cmd->upmove)*(cmd->upmove)));
	if (min_speed <= 0.f)
		return;

	if (cmd->buttons & IN_DUCK)
		speed *= 2.94117647f;

	if (min_speed <= speed)
		return;

	float finalSpeed = (speed / min_speed) * Settings::RageBot::SlowWalkMod;

	cmd->forwardmove *= finalSpeed;
	cmd->sidemove *= finalSpeed;
	cmd->upmove *= finalSpeed;
}

void Rbot::ZeusBot ( CUserCmd* cmd, C_BaseCombatWeapon* weapon )
{
    cmd->buttons &= ~IN_ATTACK;

    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return;

    C_BasePlayer* local = g_LocalPlayer;

    static bool DidShotLast = false;

    float range = weapon->GetCSWeaponData()->flRange;

    Vector BestPos;
    float BestDmg = 0.f;
    bool Found = false;

    if ( DidShotLast )
    {
        DidShotLast = false;
        return;
    }

    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
            continue;

        if ( local->m_vecOrigin().DistTo ( entity->m_vecOrigin() ) >= range )
            continue;

        for ( int hitboxn = 0; hitboxn < 2; hitboxn++ )
        {
            Vector pos = Vector ( 0, 0, 0 );

            switch ( hitboxn )
            {
                case 0: //pelvis
                    pos = entity->GetHitboxPos ( HITBOX_PELVIS );
                    break;

                case 1: //chest
                    pos = entity->GetHitboxPos ( HITBOX_CHEST );
                    break;
            }

            if ( local->GetEyePos().DistTo ( pos ) >= range )
                continue;

            float Dmg = Autowall::Get().CanHit ( pos );

            if ( Dmg >= entity->m_iHealth() && Dmg > BestDmg )
            {
                BestDmg = Dmg;
                Found = true;
                BestPos = pos;
            }
        }
    }

    if ( !Found )
        return;

    QAngle newAng = Math::CalcAngle ( g_LocalPlayer->GetEyePos(), BestPos );

    Math::NormalizeAngles ( newAng );
    Math::ClampAngles ( newAng );

    cmd->viewangles = newAng;
    cmd->buttons |= IN_ATTACK;
    DidShotLast = true;
}

bool Rbot::HitChance ( QAngle angles, C_BasePlayer* ent, float chance )
{
    auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

    if ( !weapon )
        return false;

    Vector forward, right, up;
    Vector src = g_LocalPlayer->GetEyePos();
    Math::AngleVectors ( angles, forward, right, up );

    int cHits = 0;
    int cNeededHits = static_cast<int> ( 150.f * ( chance / 100.f ) );

    weapon->UpdateAccuracyPenalty();
    float weap_spread = weapon->GetSpread();
    float weap_inaccuracy = weapon->GetInaccuracy();

    for ( int i = 0; i < 150; i++ )
    {
        float a = Math::RandomFloat ( 0.f, 1.f );
        float b = Math::RandomFloat ( 0.f, 2.f * PI_F );
        float c = Math::RandomFloat ( 0.f, 1.f );
        float d = Math::RandomFloat ( 0.f, 2.f * PI_F );

        float inaccuracy = a * weap_inaccuracy;
        float spread = c * weap_spread;

        if ( weapon->m_Item().m_iItemDefinitionIndex() == 64 )
        {
            a = 1.f - a * a;
            a = 1.f - c * c;
        }

        Vector spreadView ( ( cos ( b ) * inaccuracy ) + ( cos ( d ) * spread ), ( sin ( b ) * inaccuracy ) + ( sin ( d ) * spread ), 0 ), direction;

        direction.x = forward.x + ( spreadView.x * right.x ) + ( spreadView.y * up.x );
        direction.y = forward.y + ( spreadView.x * right.y ) + ( spreadView.y * up.y );
        direction.z = forward.z + ( spreadView.x * right.z ) + ( spreadView.y * up.z );
        direction.Normalized();

        QAngle viewAnglesSpread;
        Math::VectorAngles ( direction, up, viewAnglesSpread );
        viewAnglesSpread.Normalize();

        Vector viewForward;
        Math::AngleVectors ( viewAnglesSpread, viewForward );
        viewForward.NormalizeInPlace();

        viewForward = src + ( viewForward * weapon->GetCSWeaponData()->flRange );

        trace_t tr;
        Ray_t ray;

        ray.Init ( src, viewForward );
        g_EngineTrace->ClipRayToEntity ( ray, MASK_SHOT | CONTENTS_GRATE, ent, &tr );

        if ( tr.hit_entity == ent )
            ++cHits;

        if ( static_cast<int> ( ( static_cast<float> ( cHits ) / 150.f ) * 100.f ) >= chance )
            return true;

        if ( ( 150 - i + cHits ) < cNeededHits )
            return false;
    }

    return false;
}

BaimMode* Rbot::GetBAimStatus()
{
	return &baim;
}

int Rbot::FindBestEntity ( CUserCmd* cmd, C_BaseCombatWeapon* weapon, Vector& hitpos, bool& bBacktrack )
{
    int BestEntityIndex = -1;
    float WeaponRange = weapon->GetCSWeaponData()->flRange;

    float BestDamage = 0.f;
    Vector BestHitpoint;
    bool BestBacktrack = false;
    TickRecord BestBacktrackRecord;

    bool rbot_lagcompensation = g_Config.GetBool ( "rbot_lagcompensation" );
	bool rbot_force_unlage = Settings::RageBot::ForceUnlag;
	bool rbot_resolver = Settings::RageBot::Resolver; 
	int rbot_baimmode = Settings::RageBot::BAimMode; 

    bool FoundGoodEntity = false;

    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
            continue;

        //Console.WriteLine("Got valid entity");

        if ( !matrixData[i].gotMatrix )
            continue;

        if ( rbot_force_unlage && entity->m_vecVelocity().Length2D() > 0.1f && g_Resolver.GResolverData[i].InFakelag )
            continue;

        //if (FoundGoodEntity) continue;


        float Distance = Math::VectorDistance ( g_LocalPlayer->GetEyePos(), entity->GetEyePos() );

        if ( Distance > WeaponRange )
            continue;

        float CDamage = 0.f;
        Vector CHitpos;
        TickRecord CBacktrackRecord;
        bool CUsingBacktrack = false;

        baim = g_Resolver.GResolverData[i].Resolved ? BaimMode::NONE : BaimMode::BAIM;

		if (g_Saver.ForceBAim)
		{
			baim = BaimMode::FORCE_BAIM;
		}
		else
		{
			if (!rbot_resolver)
				baim = BaimMode::NONE;

			if (g_Resolver.GResolverData[i].Shots > BAimAfter && ForceBAimAfter != 0)
				baim = BaimMode::BAIM;

			if (MovingBAim && g_LocalPlayer->m_vecVelocity().Length() > 0.1f)
				baim = BaimMode::BAIM;

			if (g_Resolver.GResolverData[i].Shots > BAimAfter && ForceBAimAfter != 0)
				baim = BaimMode::FORCE_BAIM;

			if (rbot_baimmode == 0) baim = BaimMode::NONE;
		}
        


        //if(g_Resolver.GResolverData[i].ForceBaim) { baim = BaimMode::FORCE_BAIM; }

        bool WillKillEntity = false;

        if ( !rbot_lagcompensation )
        {
            if ( !GetBestHitboxPoint ( entity, CDamage, CHitpos, baim, WillKillEntity ) )
                continue;
        }
        else
        {
            bool ShouldSkip = true;
            Vector NormalHitpos = Vector ( 0, 0, 0 );
            float NormalDamage = 0.f;
            bool GotNormal = false;

            Vector BacktrackHitpos = Vector ( 0, 0, 0 );
            float BacktrackDamage = 0.f;
            bool GotBacktrack = false;
            TickRecord CBacktrackRecord;


            //if (GetBestHitboxPoint(entity, NormalDamage, NormalHitpos, baim, WillKillEntity))
            //{
            //	GotNormal = true;
            //	ShouldSkip = false;
            //}

            //bool CanKillNormal = !ShouldSkip && CDamage > entity->m_iHealth();

            if ( Backtrack::Get().RageBacktrack ( entity, i, BacktrackDamage, BacktrackHitpos, CBacktrackRecord ) )
            {
                //Console.Write("RageBacktrack: "); Console.WriteLine(BacktrackHitpos);
                GotBacktrack = true;
                ShouldSkip = false;
            }

            if ( !GotBacktrack && !GotNormal )
                continue;

            if ( ShouldSkip )
                continue;

            if ( GotNormal && NormalDamage > BacktrackDamage )
            {
                CDamage = NormalDamage;
                CHitpos = NormalHitpos;
            }
            else
            {
                CDamage = BacktrackDamage;
                CHitpos = BacktrackHitpos;
                CUsingBacktrack = true;
            }

            //CUsingBacktrack = true;
        }

        //g_Logger.Add("FindBestEntity", std::to_string(CDamage));

        if ( CDamage > BestDamage )
        {
            BestDamage = CDamage;
            BestHitpoint = CHitpos;
            BestEntityIndex = entity->EntIndex();


            if ( CDamage > entity->m_iHealth() )
                FoundGoodEntity = true;


            if ( CUsingBacktrack )
            {
                BestBacktrack = true;
                BestBacktrackRecord = CBacktrackRecord;
            }
            else
                BestBacktrack = false;
        }
    }


    if ( BestBacktrack )
    {
        g_Saver.LastBacktrackTick = TIME_TO_TICKS ( BestBacktrackRecord.m_flSimulationTime + Backtrack::Get().GetLerpTime() );
        bBacktrack = true;
        bRecord = BestBacktrackRecord;
        cmd->tick_count = g_Saver.LastBacktrackTick;//Math::TimeToTicks(BestBacktrackRecord.m_flSimulationTime + Backtrack::Get().GetLerpTime());
    }


    hitpos = BestHitpoint;
    return BestEntityIndex;
}


/*
bool Rbot::PrecachePointers(C_BasePlayer * entity)
{

	if (!entity->SetupBones(PrecachedMatrix, 128, 256, 0.f)) {
		return false;
	}

	model_t* pModel = entity->GetModel();
	if (!pModel) {
		return false;
	}

	PrecachedHdr = g_MdlInfo->GetStudiomodel(pModel);
	if (!PrecachedHdr) {
		return false;
	}

	PrecachedSet = PrecachedHdr->GetHitboxSet(0);

	if (!PrecachedSet)
		return false;

	return true;
}
*/

bool Rbot::GetBestHitboxPoint ( C_BasePlayer* entity, float& damage, Vector& hitbox, BaimMode baim, bool& WillKill, matrix3x4_t matrix[MAXSTUDIOBONES], mstudiohitboxset_t* StudioSet, bool NoPointscale )
{
    matrix3x4_t cmatrix[MAXSTUDIOBONES];

    if ( !matrix )
    {
        //PrecachePointers(entity);
        matrix = matrixData[entity->EntIndex()].matrix;
    }

    if ( !StudioSet )
        StudioSet = matrixData[entity->EntIndex()].StudioSet;

    WillKill = false;

    float BestDamage = 0.f;
    Vector BestHitpoint;
    bool FoundHitableEntity = false;

    CanHitStruct CanHitHead;
    CanHitStruct CanBaimKill;

    for ( int hitbox = 0; hitbox < Hitboxes::HITBOX_MAX; hitbox++ )
    {
        //if (CanKillWithBaim) continue;

        if ( ( hitbox == HITBOX_HEAD || hitbox == HITBOX_NECK || hitbox >= HITBOX_RIGHT_THIGH ) && baim == BaimMode::FORCE_BAIM )
            continue;

        float pointscale = 0.f;

        switch ( hitbox )
        {
            case HITBOX_HEAD:
                if ( !HitboxHead )
                    continue;

                pointscale = HitboxHeadScale;
                break;

            case HITBOX_NECK:
                if ( !HitboxNeck )
                    continue;

                pointscale = HitboxNeckScale;
                break;

            case HITBOX_PELVIS:
                if ( !HitboxPelvis )
                    continue;

                pointscale = HitboxPelvisScale;
                break;

            case HITBOX_STOMACH:
                if ( !HitboxStomach )
                    continue;

                pointscale = HitboxStomachScale;
                break;

            case HITBOX_LOWER_CHEST:
            case HITBOX_CHEST:
            case HITBOX_UPPER_CHEST:
                if ( !HitboxChest )
                    continue;

                pointscale = HitboxChestScale;
                break;

            case HITBOX_RIGHT_THIGH:
            case HITBOX_LEFT_THIGH:
            case HITBOX_RIGHT_CALF:
            case HITBOX_LEFT_CALF:
                if ( !HitboxLeg )
                    continue;

                pointscale = HitboxLegScale;
                break;

            case HITBOX_RIGHT_FOOT:
            case HITBOX_LEFT_FOOT:
                if ( !HitboxFoot )
                    continue;

                pointscale = HitboxFootScale;
                break;

            case HITBOX_RIGHT_HAND:
            case HITBOX_LEFT_HAND:
            case HITBOX_RIGHT_UPPER_ARM:
            case HITBOX_RIGHT_FOREARM:
            case HITBOX_LEFT_UPPER_ARM:
            case HITBOX_LEFT_FOREARM:
                if ( !HitboxArm )
                    continue;

                pointscale = HitboxArmScale;
                break;

            default:
                continue;
        }

        //float pointscale = hitbox == HITBOX_HEAD ? rbot_head_scale : rbot_body_scale;

        //if (pointscale != 0.f) {

        if ( NoPointscale )
            pointscale = 0.f;

        std::vector<PointScanStruct> Points = GetPointsForScan ( entity, hitbox, StudioSet, matrix, pointscale );

        //Utils::ConsolePrint("got pints\n");

        float CBestDamage = 0.f;
        Vector CBestPoint;
        Vector CCenter;
        bool CCanHitCenter = false;
        float CCenterDamage = -1.f;

        for ( size_t p = 0; p < Points.size(); p++ )
        {
            /*
            if (g_LocalPlayer->m_hActiveWeapon()->IsSniper() && !g_LocalPlayer->m_bIsScoped() && (g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_Config.GetBool("rbot_autoscope"))
            {
            	if (!(CurrentCmd->buttons & IN_ZOOM)) CurrentCmd->buttons |= IN_ZOOM;
            	if (g_Config.GetBool("rbot_autostop")) AutoStop(CurrentCmd);
            	if (!(CurrentCmd->buttons & IN_DUCK) && g_Config.GetBool("rbot_autocrouch")) AutoCrouch(CurrentCmd);
            	continue;
            }
            if (!HitChance(Math::CalcAngle(g_LocalPlayer->GetEyePos(), Points[p].pos), entity, g_Config.GetFloat("rbot_min_hitchance")))
            {
            	if (g_Config.GetBool("rbot_autostop")) AutoStop(CurrentCmd);
            	if (!(CurrentCmd->buttons & IN_DUCK) && g_Config.GetBool("rbot_autocrouch")) AutoCrouch(CurrentCmd);
            	continue;
            }
            */
            float CDamage = 0.f;
            CDamage = Autowall::Get().CanHit ( Points[p].pos );

            if ( ( hitbox == HITBOX_HEAD || hitbox == HITBOX_NECK ) && baim == BaimMode::BAIM )
            {
                if ( CDamage >= MinDmg && CDamage > CanHitHead.damage )
                {
                    CanHitHead.CanHit = true;
                    CanHitHead.damage = CDamage;
                    CanHitHead.pos = Points[p].pos;
                }

                continue;
            }

            if ( baim == BaimMode::BAIM && ( hitbox >= HITBOX_RIGHT_HAND || hitbox == HITBOX_RIGHT_THIGH || hitbox == HITBOX_LEFT_THIGH || hitbox == HITBOX_RIGHT_CALF || hitbox == HITBOX_LEFT_CALF ) )
                continue;

            if ( Points[p].center && CDamage >= MinDmg )
            {
                CCanHitCenter = true;
                CCenter = Points[p].pos;
                CCenterDamage = CDamage;
            }


            if ( CDamage >= MinDmg && CDamage > CBestDamage )
            {
                CBestDamage = CDamage;
                CBestPoint = Points[p].pos;
            }
        }

        if ( CCanHitCenter && CCenterDamage >= entity->m_iHealth() / 2.f )
        {
            CBestDamage = CCenterDamage;
            CBestPoint = CCenter;
        }

        if ( CBestDamage >= MinDmg && CanBaimKill.damage < CBestDamage && ( hitbox == HITBOX_CHEST || hitbox == HITBOX_LOWER_CHEST || hitbox == HITBOX_PELVIS || hitbox == HITBOX_STOMACH ) )
        {
            CanBaimKill.CanHit = true;
            CanBaimKill.damage = CBestDamage;
            CanBaimKill.pos = CBestPoint;
        }


        if ( CBestDamage >= MinDmg && CBestDamage > BestDamage )
        {
            BestDamage = CBestDamage;
            BestHitpoint = CBestPoint;
            FoundHitableEntity = true;
        }

    }

    if ( !FoundHitableEntity && CanHitHead.CanHit )
    {
        FoundHitableEntity = true;
        BestDamage = CanHitHead.damage;
        BestHitpoint = CanHitHead.pos;
    }

    if ( CanBaimKill.damage >= entity->m_iHealth() )
    {
        FoundHitableEntity = true;
        BestDamage = CanBaimKill.damage;
        BestHitpoint = CanBaimKill.pos;
    }

    if ( BestDamage >= entity->m_iHealth() )
        WillKill = true;

    damage = BestDamage;
    hitbox = BestHitpoint;

    return FoundHitableEntity;
}

void Rbot::PrecacheShit() //need fix
{
    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
            continue;


        model_t* pModel = entity->GetModel();

        if ( !pModel )
        {
            matrixData[i].gotMatrix = false;
            continue;
        }

        matrixData[i].StudioHdr = g_MdlInfo->GetStudiomodel ( pModel );

        if ( !matrixData[i].StudioHdr )
        {
            matrixData[i].gotMatrix = false;
            continue;
        }

        matrixData[i].StudioSet = matrixData[i].StudioHdr->GetHitboxSet ( 0 );

        if ( !matrixData[i].StudioSet )
        {
            matrixData[i].gotMatrix = false;
            continue;
        }

        matrixData[i].gotMatrix = entity->SetupBones ( matrixData[i].matrix, 128, 256, entity->m_flSimulationTime() );

        //if (matrixData[i].gotMatrix) Console.WriteLine("GotMatrix");
    }
}

std::vector<PointScanStruct> Rbot::GetPointsForScan ( C_BasePlayer* entity, int hitbox, mstudiohitboxset_t* hitset, matrix3x4_t matrix[MAXSTUDIOBONES], float pointscale )
{
    std::vector<PointScanStruct> pointsToScan;

    if ( !matrix )
        return pointsToScan;

    if ( !hitset )
        return pointsToScan;

    mstudiobbox_t* bbox = hitset->GetHitbox ( hitbox );

    if ( !bbox )
        return pointsToScan;

    float mod = bbox->m_flRadius != -1.f ? bbox->m_flRadius : 0.f;

    //Utils::ConsolePrint("mod r\n");

    Vector max;
    Vector min;

    Vector in1 = bbox->bbmax + mod;
    Vector in2 = bbox->bbmin - mod;


    Math::VectorTransform ( in1, matrix[bbox->bone], max );
    Math::VectorTransform ( in2, matrix[bbox->bone], min );


    Vector center = ( min + max ) * 0.5f;

    QAngle curAngles = Math::CalcAngle ( center, g_LocalPlayer->GetEyePos() );

    Vector forward;
    Math::AngleVectors ( curAngles, forward );

    Vector right = forward.Cross ( Vector ( 0, 0, 1 ) );
    Vector left = Vector ( -right.x, -right.y, right.z );

    Vector top = Vector ( 0, 0, 1 );
    Vector bot = Vector ( 0, 0, -1 );

    //Utils::ConsolePrint("t\n");

    //float POINT_SCALE = pointscale;//hitbox == HITBOX_HEAD ? g_Options.rbot_head_pointscale : g_Options.rbot_pointscale;

    if ( pointscale == 0.f )
    {
        pointsToScan.emplace_back ( PointScanStruct{ center, true } );
        return pointsToScan;
    }

    if ( hitbox == HITBOX_HEAD )
    {
        for ( auto i = 0; i < 5; ++i )
        {
            pointsToScan.emplace_back ( PointScanStruct{ center } );
        }

        pointsToScan[1].pos += top * ( bbox->m_flRadius * pointscale );
        pointsToScan[2].pos += right * ( bbox->m_flRadius * pointscale );
        pointsToScan[3].pos += left * ( bbox->m_flRadius * pointscale );
        pointsToScan[4].pos = center;
        pointsToScan[4].center = true;

        //pointsToScan[4] = center;
        //pointsToScan[4].center = true;
    }
    else
    {
        for ( auto i = 0; i < 3; ++i )
        {
            pointsToScan.emplace_back ( PointScanStruct{ center } );
        }

        pointsToScan[0].pos += right * ( bbox->m_flRadius * pointscale );
        pointsToScan[1].pos += left * ( bbox->m_flRadius * pointscale );
        pointsToScan[2].pos = center;
        pointsToScan[2].center = true;
        //pointsToScan[2] = center;
        //pointsToScan[2].center = true;
    }

    //}
    //else
    //	pointsToScan.emplace_back(center);

    return pointsToScan;

    //return std::vector<Vector>();
}

void Rbot::UpdateConfigData()
{
	HitboxHead = Settings::RageBot::Hitboxes[Settings::HitboxType::HEAD].Enabled;

	if (HitboxHead)
		HitboxHeadScale = Settings::RageBot::Hitboxes[Settings::HitboxType::HEAD].Scale;

	HitboxNeck = Settings::RageBot::Hitboxes[Settings::HitboxType::NECK].Enabled;

	if (HitboxNeck)
		HitboxHeadScale = Settings::RageBot::Hitboxes[Settings::HitboxType::NECK].Scale;

	HitboxChest = Settings::RageBot::Hitboxes[Settings::HitboxType::CHEST].Enabled;

	if (HitboxChest)
		HitboxHeadScale = Settings::RageBot::Hitboxes[Settings::HitboxType::CHEST].Scale;

	HitboxPelvis = Settings::RageBot::Hitboxes[Settings::HitboxType::PELVIS].Enabled;

	if (HitboxPelvis)
		HitboxHeadScale = Settings::RageBot::Hitboxes[Settings::HitboxType::PELVIS].Scale;

	HitboxStomach = Settings::RageBot::Hitboxes[Settings::HitboxType::STOMACH].Enabled;

	if (HitboxStomach)
		HitboxHeadScale = Settings::RageBot::Hitboxes[Settings::HitboxType::STOMACH].Scale;

	HitboxArm = Settings::RageBot::Hitboxes[Settings::HitboxType::ARM].Enabled;

	if (HitboxArm)
		HitboxHeadScale = Settings::RageBot::Hitboxes[Settings::HitboxType::ARM].Scale; 

	HitboxLeg = Settings::RageBot::Hitboxes[Settings::HitboxType::LEG].Enabled; 

	if (HitboxLeg)
		HitboxHeadScale = Settings::RageBot::Hitboxes[Settings::HitboxType::LEG].Scale;

	HitboxFoot = Settings::RageBot::Hitboxes[Settings::HitboxType::FOOT].Enabled;

	if (HitboxFoot)
		HitboxHeadScale = Settings::RageBot::Hitboxes[Settings::HitboxType::FOOT].Scale;

    //MinDmg = g_Config.GetFloat ( "rbot_mindamage" );
}
