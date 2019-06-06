
#include "Resolver.h"
#include "ConfigSystem.h"
#include "AntiAim.h"
#include "Autowall.h"
#include "Logger.h"
#include "ConsoleHelper.h"
#include "valve_sdk\csgostructs.hpp"
#include "helpers\math.hpp"
#include "Backtrack.h"
#include "RuntimeSaver.h"

/*
is fakelag --> set first tick angle
*/

void Resolver::OnCreateMove ( QAngle OrgViewang )
{
    //return;
    if (!g_LocalPlayer) return;

    /*for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
    {
    	auto entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
    	if (!entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
    		|| !entity->IsAlive() || !entity->IsEnemy()) continue;

    	float FreestandAng = 0.f;
    	bool CanFreestand = FreestandingSim(entity, FreestandAng);//AntiAim::Get().Freestanding(entity, FreestandAng);
    	//if (CanFreestand) CanFreestand = false;

    	float Edge		   = 0.f;
    	bool CanEdge = false;//AntiAim::Get().GetEdgeDetectAngle(entity, Edge);

    	float Distance     = 0.f;
    	bool CanDistance   = GetWallDistance(entity, Distance);

    	float BackwardsAng = 0.f;
    	bool CanUseBackwards = AtTargetSim(entity, BackwardsAng);

    	SimulatedAAs[i] = { CanFreestand, FreestandAng, CanEdge, Edge, CanDistance, Distance, CanUseBackwards, BackwardsAng };
    }*/
}

#ifdef FakeAnglesEnabled
void Resolver::OnFramestageNotify()
{
    return;
    /*
    Todo
    - check if is breaking lby --> can hit when shooting lby only --> function (MoveStandDelta < 35.f)

    */

    //bool Disable

    bool rbot_lby_prediction = g_Config.GetBool ( "rbot_lby_prediction" );
    //bool rbot_flag_prediction = g_Config.GetBool("rbot_flag_prediction");

    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
        {
            g_Resolver.StandAAData[i].UsedEdge = false;
            SavedResolverData[i].WasDormantBetweenMoving = true;
            SavedResolverData[i].CanUseMoveStandDelta = false;
            continue;
        }

        SavedResolverData[i].FakeYaw = entity->m_angEyeAngles().yaw;

        //g_Logger.Add("RESOLVER", "active");

        //if(SavedResolverData[i].LegitTestLastSimtime != entity->m_flSimulationTime())

        bool InFakeLag = true;

        if ( SavedResolverData[i].LastSimtime != entity->m_flSimulationTime() )
            InFakeLag = false;

        if ( InFakeLag || g_GlobalVars->realtime - SavedResolverData[i].LastWasUsingAA < 5.f )
        {
            SavedResolverData[i].UsingAA = true;

            if ( InFakeLag )
                SavedResolverData[i].LastWasUsingAA = g_GlobalVars->realtime;
        }
        else
            SavedResolverData[i].UsingAA = false;

        SavedResolverData[i].LastPos = entity->m_vecOrigin();

        if ( !SavedResolverData[i].UsingAA )
        {
            g_Resolver.GResolverData[i].ResolvedYaw = entity->m_angEyeAngles().yaw;
            //g_Resolver#
            entity->SetAbsAngles ( QAngle ( 0.f, g_Resolver.GResolverData[i].ResolvedYaw, 0.f ) );
            g_Resolver.GResolverData[i].Resolved = true;
            g_Resolver.GResolverData[i].Fake = !g_Resolver.GResolverData[i].Resolved;
            g_Resolver.GResolverData[i].mode = ResolverModes::NONE;
            SavedResolverData[i].LastSimtime = entity->m_flSimulationTime();
            return;
        }

        if ( SavedResolverData[i].UsingAA && InFakeLag )
            g_Resolver.GResolverData[i].BreakingLC = ( entity->m_vecOrigin() - SavedResolverData[i].LastPos ).LengthSqr() > 4096.f;


        SavedResolverData[i].LastSimtime = entity->m_flSimulationTime();

        //SavedResolverData[i].UsingAA

        //entity->SetAbsOriginal(entity->m_vecOrigin()); //xD

        //entity->SetAbsOriginal(entity->m_vecOrigin()); //fix possition

        g_Resolver.GResolverData[i].InFakelag = InFakeLag;

        ResolverAnimDataStorage AnimationStorage;
        AnimationStorage.UpdateAnimationLayers ( entity );

        bool Moving = entity->m_vecVelocity().Length2D() > 0.1f;
        //bool InPrediction = Moving && g_FlagPrediction.PredictionData[i].InPrediction && rbot_flag_prediction;
        //18 if lower than 18 force fakewalk
        bool Fakewalking = InFakeWalk ( entity, AnimationStorage ) || InFakeWalkOld ( entity, AnimationStorage ); // || (entity->m_vecVelocity().Length2D() <= 21.f && Moving);
        bool InAir = ! ( entity->m_fFlags() & FL_ONGROUND );
        bool Standing = !Moving && !InAir;
        auto SimAAs = SimulatedAAs[i];

        /*

        TOdo remove is lby cheat --> force baim if so


        */

        if ( !g_LocalPlayer->IsAlive() )
            SimAAs.CanUseFreestanding = false;

        bool EntInLbyUpdate = false;
        g_Resolver.GResolverData[i].ForceBaim = false;

        float lby = entity->m_flLowerBodyYawTarget();
        g_Resolver.GResolverData[i].Moving = Moving;
        g_Resolver.GResolverData[i].InAir = InAir;

        if ( Moving && !Fakewalking && !InAir )
        {
            DetectionData[i].BeforeMoving = DetectionData[i].mode;
            DetectionData[i].AllLbys.clear();
            DetectionData[i].WasMoving = true;

            //g_Resolver.GResolverData[i].Moving = true;
            g_Resolver.GResolverData[i].detection = ResolverDetections::MOVING;
            g_Resolver.GResolverData[i].mode = ResolverModes::MOVING;
            entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget();
            SavedResolverData[i].LastMovingLby = lby;
            SavedResolverData[i].WasDormantBetweenMoving = false;
            g_Resolver.GResolverData[i].Resolved = true;
            g_Resolver.StandAAData[i].UsedEdge = false;
            g_Resolver.GResolverData[i].ResolverState = 3;
            g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::moving;

            if ( !SavedResolverData[i].WasLastMoving )
            {
                SavedResolverData[i].MoveStandDelta = entity->m_flLowerBodyYawTarget() - SavedResolverData[i].LastStandingLby;
                SavedResolverData[i].CanUseMoveStandDelta = true;
                SavedResolverData[i].WasLastMoving = true;
            }

            g_Resolver.GResolverData[i].CanuseLbyPrediction = true;
            g_Resolver.GResolverData[i].NextPredictedLbyBreak = entity->m_flSimulationTime() + 0.22f;
        }
        /*
        else if (Moving && !Fakewalking && !InAir && InPrediction)
        {
        	//prediction
        	g_Resolver.GResolverData[i].Moving = false;
        	if (SimAAs.CanUseFreestanding && g_Resolver.GResolverData[i].ShotsAtMode[(int)ResolverModes::PREDICT_FREESTANDING] <= 2)
        	{
        		g_Resolver.GResolverData[i].mode = ResolverModes::PREDICT_FREESTANDING;
        		entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
        	}
        	else
        	{
        		g_Resolver.GResolverData[i].mode = ResolverModes::PREDICT_BRUTFORCE;
        		if (SimAAs.CanUseFreestanding)
        		{
        			switch (g_Resolver.GResolverData[i].ShotsAtMode[(int)ResolverModes::PREDICT_FREESTANDING] % 2)
        			{
        			case 0:
        				entity->m_angEyeAngles().yaw = SavedResolverData[i].LastMovingLby;
        				break;
        			case 1:
        				entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
        				break;
        			}
        		}
        		else
        		{
        			entity->m_angEyeAngles().yaw = SavedResolverData[i].LastMovingLby;
        		}
        	}
        }
        */
        else if ( InAir )
        {
            g_Resolver.GResolverData[i].detection = ResolverDetections::AIR;
            //g_Resolver.GResolverData[i].mode = ResolverModes::AIR;b
            g_Resolver.GResolverData[i].Resolved = !g_Config.GetBool ( "rbot_resolver_air_baim" );
            g_Resolver.GResolverData[i].ResolverState = 0;

            g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::normal_prediction;

            if ( SimAAs.CanUseFreestanding && g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::AIR_FREESTANDING] < 5 )
            {
                switch ( g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::AIR_FREESTANDING] )
                {
                    case 0:
                        g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::good_prediction;

                    case 1:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                        break;

                    case 2:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng + 180.f;
                        break;

                    case 3:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                        break;

                    case 4:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng + 180.f;
                        break;

                    default:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                }

                g_Resolver.GResolverData[i].mode = ResolverModes::AIR_FREESTANDING;
            }
            else
            {
                g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::normal_prediction;
                g_Resolver.GResolverData[i].mode = ResolverModes::AIR_BRUTFORCE;
                //entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 180 + (g_Resolver.GResolverData[i].ShotsAtMode[(int)ResolverModes::AIR_BRUTFORCE] * 40);
                g_Resolver.GResolverData[i].Resolved = false;

                switch ( g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::AIR_FREESTANDING] % 4 )
                {
                    case 0:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget();
                        break;

                    case 1:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 180.f;
                        break;

                    case 2:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 45.f;
                        break;

                    case 3:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() - 45.f;
                        break;

                    default:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget();
                }

                g_Resolver.GResolverData[i].mode = ResolverModes::AIR_FREESTANDING;
            }
        }
        else if ( Fakewalking && Moving )
        {
            //lby update
            g_Resolver.GResolverData[i].Resolved = false;
            g_Resolver.GResolverData[i].detection = ResolverDetections::FAKEWALKING;
            g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::normal_prediction;
            SavedResolverData[i].WasFakeWalking = true;
            g_Resolver.GResolverData[i].CanuseLbyPrediction = false;
            SavedResolverData[i].CanUseMoveStandDelta = false;

            bool InLbyUpdate = false;

            if ( entity->m_flLowerBodyYawTarget() != SavedResolverData[i].LastLby )
            {
                g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::lby_update;
                g_Resolver.GResolverData[i].CanuseLbyPrediction = true;
                InLbyUpdate = true;
            }

            if ( InLbyUpdate )
            {
                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget();
                g_Resolver.GResolverData[i].Resolved = true;
            }
            else if ( SimAAs.CanUseFreestanding )
            {
                switch ( g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::FAKEWALK_FREESTANDING] % 4 )
                {
                    case 0:
                        g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::good_prediction;
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                        break;

                    case 1:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng + 180.f;
                        break;

                    case 2:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                        break;

                    case 3:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng + 180.f;
                        break;

                    default:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                }

                g_Resolver.GResolverData[i].mode = ResolverModes::FAKEWALK_FREESTANDING;
            }
            else
            {
                g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::normal_prediction;
                g_Resolver.GResolverData[i].mode = ResolverModes::FAKEWALK_BRUTFORCE;

                //FAKEWALK_BRUTFORCE
                switch ( g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::FAKEWALK_BRUTFORCE] % 8 )
                {
                    case 0:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 45.f;
                        break;

                    case 1:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 120.f;
                        break;

                    case 2:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() - 45.f;
                        break;

                    case 3:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() - 120.f;
                        break;

                    case 4:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 180.f;
                        break;

                    case 5:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() - 90.f;
                        break;

                    case 6:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 90.f;
                        break;

                    case 7:
                        entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget();
                        break;
                }
            }
        }
        else //if (Standing || Fakewalking/* || SavedResolverData[i].WasFakeWalking*/)
        {
            g_Resolver.GResolverData[i].detection = ResolverDetections::STANDING;
            SavedResolverData[i].WasFakeWalking = false;
            SavedResolverData[i].WasLastMoving = false;

            SavedResolverData[i].LastStandingLby = entity->m_flLowerBodyYawTarget();

            g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::normal_prediction;

            /*
            if (InFakeLag)
            {
            	//only update when not in fakelag --> todo | need to test
            	entity->m_angEyeAngles().yaw = SavedResolverData[i].LastResolvedYaw;
            	g_Resolver.GResolverData[i].Resolved = false;
            	g_Resolver.GResolverData[i].Fake = true;
            	Math::NormalizeAngles(entity->m_angEyeAngles());
            	return;
            }
            */

            bool InLbyUpdate = false;

            if ( !InFakeLag )
            {
                if ( rbot_lby_prediction && g_Resolver.GResolverData[i].CanuseLbyPrediction && g_Resolver.GResolverData[i].NextPredictedLbyBreak - entity->m_flSimulationTime() < g_GlobalVars->interval_per_tick )
                {
                    g_Resolver.GResolverData[i].Resolved = true;
                    InLbyUpdate = true;
                }

                if ( entity->m_flLowerBodyYawTarget() != SavedResolverData[i].LastLby )
                {
                    g_Resolver.GResolverData[i].CanuseLbyPrediction = true;
                    InLbyUpdate = true;
                }
            }



            bool CanShootLbyUpdate = Backtrack::Get().IsTickValid ( TIME_TO_TICKS ( SavedResolverData[i].lastLbyUpdateTime ) );

            bool IsSpinbot = false;//IsUsingSpinbot(entity, i, 15.f, 25.f);
            g_Resolver.GResolverData[i].Resolved = true;

            if ( Standing )
                g_Resolver.GResolverData[i].ResolverState = 1;
            else
                g_Resolver.GResolverData[i].ResolverState = 0;

            if ( InLbyUpdate/* || CanShootLbyUpdate*/ )
            {
                g_Resolver.GResolverData[i].mode = ResolverModes::LBY_BREAK;

                if ( InLbyUpdate )
                {
                    g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::lby_update;
                    UpdateDetectionData ( entity, i );
                    g_Resolver.GResolverData[i].NextPredictedLbyBreak = entity->m_flSimulationTime() + g_GlobalVars->interval_per_tick + 1.1f;
                    SavedResolverData[i].lastLbyUpdateTime = entity->m_flSimulationTime();
                }
                else
                    entity->m_flSimulationTime() = SavedResolverData[i].lastLbyUpdateTime;

                EntInLbyUpdate = true;
                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget();
                SavedResolverData[i].LastLby = entity->m_flLowerBodyYawTarget();
                g_Resolver.GResolverData[i].Resolved = true;
            }
            else if ( SimAAs.GotWallDistance && SimAAs.CanUseFreestanding && SimAAs.WallDistance <= 80.f && !IsSpinbot &&
                      g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::FREESTANDING] < 4 )
            {
                switch ( g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::FREESTANDING] )
                {
                    case 0:
                        g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::good_prediction;

                    case 1:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                        break;

                    case 2:
                    case 3:
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng + 180.f;
                        break;

                    default:
                        g_Resolver.GResolverData[i].Resolved = false;
                        entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                }

                g_Resolver.GResolverData[i].mode = ResolverModes::FREESTANDING;
                DetectionData[i].mode = DetectionModes::Static;
            }
            else if ( SavedResolverData[i].CanUseMoveStandDelta && g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::MOVE_STAND_DELTA] < 1 && !IsSpinbot
                      && fabs ( SavedResolverData[i].MoveStandDelta ) > 40.f && fabs ( SavedResolverData[i].MoveStandDelta ) < 119.f )
            {
                g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::good_prediction;
                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + SavedResolverData[i].MoveStandDelta;
                g_Resolver.GResolverData[i].mode = ResolverModes::MOVE_STAND_DELTA;
                DetectionData[i].mode = DetectionModes::Static;
            }
            else if ( IsSpinbot )
            {
                g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::good_prediction;
                float t = DetectionData[i].LastSimtime - entity->m_flSimulationTime();
                entity->m_angEyeAngles().yaw = lby + ( ( DetectionData[i].SpinSpeed / 1.1f ) * t );
                g_Resolver.GResolverData[i].mode = ResolverModes::SPINBOT;
                DetectionData[i].mode = DetectionModes::Spinning;
            }
            else
            {
                g_Resolver.GResolverData[i].BacktrackPriority = BacktrackPriorities::normal_prediction;
                g_Resolver.GResolverData[i].Resolved = false;
                g_Resolver.GResolverData[i].ResolverState = 0;

                if ( SimAAs.CanUseEdge && !g_Resolver.StandAAData[i].UsedEdge && SimAAs.GotWallDistance && SimAAs.WallDistance < 35.f && g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::EDGE] < 1 )
                {
                    g_Resolver.GResolverData[i].mode = ResolverModes::EDGE;
                    DetectionData[i].mode = DetectionModes::Static;

                    if ( g_Resolver.StandAAData[i].LastEdgeShots != g_Resolver.GResolverData[i].Shots )
                        g_Resolver.StandAAData[i].UsedEdge = true;

                    entity->m_angEyeAngles().yaw = SimAAs.EdgeAngle;
                }
                else
                {
                    if ( SimAAs.CanUseFreestanding )
                    {
                        switch ( g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::FORCE_FREESTANDING] % 6 )
                        {
                            case 0:
                                entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                                break;

                            case 1:
                                entity->m_angEyeAngles().yaw = SimAAs.FreestandAng + 180.f;
                                break;

                            case 2:
                                entity->m_angEyeAngles().yaw = SimAAs.FreestandAng + 90.f;
                                break;

                            case 3:
                                entity->m_angEyeAngles().yaw = SimAAs.FreestandAng - 90.f;
                                break;

                            case 4:
                                entity->m_angEyeAngles().yaw = SimAAs.FreestandAng + 45.f;;
                                break;

                            case 5:
                                entity->m_angEyeAngles().yaw = SimAAs.FreestandAng - 45.f;;
                                break;

                            default:
                                g_Resolver.GResolverData[i].Resolved = false;
                                entity->m_angEyeAngles().yaw = SimAAs.FreestandAng;
                        }

                        g_Resolver.GResolverData[i].mode = ResolverModes::FORCE_FREESTANDING;
                    }
                    else if ( SimAAs.CanUseBackwards && g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::FORCE_BACKWARDS] < 1 )
                    {
                        g_Resolver.GResolverData[i].mode = ResolverModes::FORCE_BACKWARDS;
                        entity->m_angEyeAngles().yaw = SimAAs.BackwardsAng;
                    }
                    else
                    {
                        //BRUTFORCE_ALL_DISABLED
                        switch ( g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) ResolverModes::BRUTFORCE_ALL_DISABLED] % 10 )
                        {
                            case 0:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 45.f;
                                break;

                            case 1:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 120.f;
                                break;

                            case 2:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() - 45.f;
                                break;

                            case 3:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() - 120.f;
                                break;

                            case 4:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() - 90.f;
                                break;

                            case 5:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 90.f;
                                break;

                            case 6:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() - 150.f;
                                break;

                            case 7:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 150.f;
                                break;

                            case 8:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget() + 180.f;
                                break;

                            case 9:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget();
                                break;

                            default:
                                entity->m_angEyeAngles().yaw = entity->m_flLowerBodyYawTarget();
                        }

                        g_Resolver.GResolverData[i].mode = ResolverModes::BRUTFORCE_ALL_DISABLED;
                    }
                }
            }

            /*
            AnimationLayer layer;
            if (Is979(entity, AnimationStorage, layer))
            {
            	if (layer.m_flCycle > 0.01f && layer.m_flCycle != 1.f && layer.m_flCycle < SavedResolverData[i].LastCycle && layer.m_flWeight == 1.f)
            	{
            		SavedResolverData[i].LastCycle = layer.m_flCycle;
            		SavedResolverData[i].Flip = !SavedResolverData[i].Flip;
            	}
            }
            */

            //if (SavedResolverData[i].Flip)
            //{
            //	entity->m_angEyeAngles().yaw += 180.f;
            //}

            Math::NormalizeAngles ( entity->m_angEyeAngles() );

            //entity->SetAbsAngles(entity->m_angEyeAngles());

            SavedResolverData[i].LastResolved = g_Resolver.GResolverData[i].Resolved;
            SavedResolverData[i].LastResolvedYaw = entity->m_angEyeAngles().yaw; //save for lag
        }

        //if (!(Moving && !InAir) && g_Resolver.GResolverData[i].Resolved && fabs(entity->m_flLowerBodyYawTarget() - entity->m_angEyeAngles().yaw) < 35.f
        //	&& !EntInLbyUpdate) g_Resolver.GResolverData[i].Resolved = false;
        /*
        if (InFakeLag && !EntInLbyUpdate && (Moving || InAir))
        {
        	if (Moving && !InAir) { g_Resolver.GResolverData[i].Fake = !g_Resolver.GResolverData[i].Resolved; return; }
        	g_Resolver.GResolverData[i].Fake = true;
        	g_Resolver.GResolverData[i].Resolved = false;
        }
        else
        {
        	g_Resolver.GResolverData[i].Fake = !g_Resolver.GResolverData[i].Resolved;
        }
        */

        //entity->SetAbsAngles(entity->m_angEyeAngles());

        entity->SetAbsAngles ( QAngle ( 0.f, entity->m_angEyeAngles().yaw, 0.f ) );

        g_Resolver.GResolverData[i].Fake = !g_Resolver.GResolverData[i].Resolved;

        g_Resolver.GResolverData[i].ResolvedYaw = entity->m_angEyeAngles().yaw;
        //entity->m_angEyeAngles().Normalize();
    }
}
#else

void Resolver::OnFramestageNotify()
{
    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == g_LocalPlayer || entity->IsDormant()
                || !entity->IsAlive() || !entity->IsEnemy() )
        {
            g_Resolver.StandAAData[i].UsedEdge = false;
            SavedResolverData[i].WasDormantBetweenMoving = true;
            SavedResolverData[i].CanUseMoveStandDelta = false;
            continue;
        }

        bool InFakeLag = true;

        if ( SavedResolverData[i].LastSimtime != entity->m_flSimulationTime() )
            InFakeLag = false;

        SavedResolverData[i].LastPos = entity->m_vecOrigin();

        if ( InFakeLag )
            g_Resolver.GResolverData[i].BreakingLC = ( entity->m_vecOrigin() - SavedResolverData[i].LastPos ).LengthSqr() > 4096.f;

        SavedResolverData[i].LastSimtime = entity->m_flSimulationTime();

        //SavedResolverData[i].UsingAA

        //entity->SetAbsOriginal(entity->m_vecOrigin()); //xD

        //entity->SetAbsOriginal(entity->m_vecOrigin()); //fix possition

        g_Resolver.GResolverData[i].InFakelag = InFakeLag;

        ResolverAnimDataStorage AnimationStorage;
        AnimationStorage.UpdateAnimationLayers ( entity );

        bool Moving = entity->m_vecVelocity().Length2D() > 0.1f;
        bool SlowWalk = Moving && entity->m_vecVelocity().Length2D() < 52.f && fabs ( SavedResolverData[i].LastVel - entity->m_vecVelocity().Length2D() ) < 4.f;
        bool InAir = ! ( entity->m_fFlags() & FL_ONGROUND );
        bool Standing = !Moving && !InAir;

        SavedResolverData[i].LastVel = entity->m_vecVelocity().Length2D();

        if ( !InFakeLag )
            AddCurrentYaw ( entity, i );

        float avgang = 0.f;
        bool b = GetAverageYaw ( i, avgang );

        if ( Moving && !SlowWalk && !InAir )
            g_Resolver.GResolverData[i].Resolved = true;
        else if ( Moving && SlowWalk && !InAir )
        {
            if ( IsStaticYaw ( i ) && b )
            {
                if ( g_Resolver.GResolverData[i].Shots >= 1 )
                {
                    switch ( g_Resolver.GResolverData[i].Shots % 4 )
                    {
                        case 0:
                            entity->m_angEyeAngles().yaw += 58.f;
                            break;

                        case 1:
                            entity->m_angEyeAngles().yaw -= 58.f;
                            break;

                        case 2:
                            entity->m_angEyeAngles().yaw += 29.f;
                            break;

                        case 3:
                            entity->m_angEyeAngles().yaw -= 29.f;
                            break;
                    }
                }
            }

            g_Resolver.GResolverData[i].Resolved = false;

        }
        else if ( InAir )
            g_Resolver.GResolverData[i].Resolved = true;
        else
        {
            g_Resolver.GResolverData[i].Resolved = true;
            float fl_ang = fabs ( fabs ( entity->m_flLowerBodyYawTarget() ) - fabs ( entity->m_angEyeAngles().yaw ) );
            bool fake = ( fl_ang >= 45.f && fl_ang <= 85.f ) || !IsStaticYaw ( i );
            float Yaw = entity->m_angEyeAngles().yaw;

            if ( fake )
            {
                g_Resolver.GResolverData[i].Resolved = false;

                if ( !IsStaticYaw ( i ) && b )
                    Yaw = avgang;
            }

            if ( fake && g_Resolver.GResolverData[i].Shots >= 1 )
            {
                switch ( g_Resolver.GResolverData[i].Shots % 2 )
                {
                    case 0:
                        //entity->m_angEyeAngles().yaw = Yaw + 58.f;
						entity->GetBasePlayerAnimState()->m_flGoalFeetYaw +=  entity->GetMaxDesyncAngle();
                        break;

                    case 1:
                        //entity->m_angEyeAngles().yaw = Yaw - 58.f;
						entity->GetBasePlayerAnimState()->m_flGoalFeetYaw -= entity->GetMaxDesyncAngle();
                        break;

                    /*case 2:
                        entity->m_angEyeAngles().yaw = Yaw + 29;
                        break;

                    case 3:
                        entity->m_angEyeAngles().yaw = Yaw - 29;
                        break;*/
                }
            }
        }

        g_Resolver.GResolverData[i].Fake = !g_Resolver.GResolverData[i].Resolved;
    }
}
#endif // FakeAnglesEnabled


void Resolver::OnFireEvent ( IGameEvent* event )
{
    if ( !g_LocalPlayer )
        return;

    static float LastPlayerHurt = 0.f;

    if ( !strcmp ( event->GetName(), "player_hurt" ) )
    {
        if ( g_GlobalVars->curtime == LastPlayerHurt )
            return;

        LastPlayerHurt = g_GlobalVars->curtime;

        int i = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) );
        //C_BasePlayer* p = g_EntityList->GetClientEntity(i);
        //C_BasePlayer* p = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
        int attacker = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "attacker" ) );

        if ( attacker != g_LocalPlayer->EntIndex() )
            return;

        C_BasePlayer* user = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !user )
            return;

        std::string hitgroup = "";

        switch ( event->GetInt ( "hitgroup" ) )
        {
            case HITBOX_HEAD:
                hitgroup = "HEAD";
                break;

            case HITBOX_NECK:
                hitgroup = "NECK";
                break;

            case HITBOX_PELVIS:
                hitgroup = "PELVIS";
                break;

            case HITBOX_STOMACH:
                hitgroup = "STOMACH";
                break;

            case HITBOX_LOWER_CHEST:
            case HITBOX_CHEST:
            case HITBOX_UPPER_CHEST:
                hitgroup = "CHEST";
                break;

            case HITBOX_RIGHT_THIGH:
            case HITBOX_LEFT_THIGH:
                hitgroup = "THIGH";
                break;

            case HITBOX_RIGHT_CALF:
            case HITBOX_LEFT_CALF:
                hitgroup = "CALF";
                break;

            case HITBOX_RIGHT_FOOT:
            case HITBOX_LEFT_FOOT:
                hitgroup = "FOOT";
                break;

            case HITBOX_RIGHT_HAND:
            case HITBOX_LEFT_HAND:
                hitgroup = "HAND";
                break;

            case HITBOX_RIGHT_UPPER_ARM:
            case HITBOX_RIGHT_FOREARM:
            case HITBOX_LEFT_UPPER_ARM:
            case HITBOX_LEFT_FOREARM:
                hitgroup = "ARM";
                break;
        }

        //std::string InLbyUpdate = g_Saver.RbotShotInfo.InLbyUpdate ? "TRUE" : "FALSE";
        std::string InLC = g_Saver.RbotShotInfo.InLc ? "TRUE" : "FALSE";
        std::string InMove = g_Saver.RbotShotInfo.Moving ? "TRUE" : "FALSE";
        g_Logger.Damage (
            "DAMAGE", "-" + std::to_string ( event->GetInt ( "dmg_health" ) ) + " in " + hitgroup + " to " + std::string ( user->GetPlayerInfo().szName )
            + " [ LC: " + InLC + " MOVING: " + InMove + " ]"
        );

        if ( !g_Saver.RbotShotInfo.InLbyUpdate )
        {
            g_Resolver.GResolverData[i].ShotsAtMode[ ( int ) g_Resolver.GResolverData[i].mode]--;
            g_Resolver.GResolverData[i].Shots--;

            if ( g_Resolver.GResolverData[i].Shots < 0 )
                g_Resolver.GResolverData[i].Shots = 0;
        }
    }

    if ( !strcmp ( event->GetName(), "player_death" ) )
    {
        int i = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) );
        g_Resolver.GResolverData[i].Shots = 0;
        SavedResolverData[i].MoveStandDelta = 0.f;

        for ( int i2 = 0; i2 < ( int ) ResolverModes::MAX; i2++ )
            g_Resolver.GResolverData[i].ShotsAtMode[i2] = 0;
    }

    static std::string lastName = "";
    static std::string lastWeapon = "";

    if ( !strcmp ( event->GetName(), "item_purchase" ) )
    {
        int i = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) );
        C_BasePlayer* user = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !user )
            return;

        if ( lastName == user->GetPlayerInfo().szName && lastWeapon == event->GetString ( "weapon" ) )
            return;

        lastName = user->GetPlayerInfo().szName;
        lastWeapon = event->GetString ( "weapon" );

        g_Logger.Info ( "PURCHASE", lastName + " purchased weapon " + lastWeapon );
    }

    if ( !strcmp ( event->GetName(), "round_end" ) )
    {
        for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
        {
            g_Resolver.GResolverData[i].Shots = 0;
            SavedResolverData[i].MoveStandDelta = 0.f;

            for ( int i2 = 0; i2 < ( int ) ResolverModes::MAX; i2++ )
                g_Resolver.GResolverData[i].ShotsAtMode[i2] = 0;
        }
    }

    if ( !strcmp ( event->GetName(), "player_disconnect" ) )
    {
        int i = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) );

        if ( i == g_EngineClient->GetLocalPlayer() )
            return;

        g_Resolver.GResolverData[i].Shots = 0;
        SavedResolverData[i].MoveStandDelta = 0.f;

        for ( int i2 = 0; i2 < ( int ) ResolverModes::MAX; i2++ )
            g_Resolver.GResolverData[i].ShotsAtMode[i2] = 0;
    }
}

#ifdef FakeAnglesEnabled
bool Resolver::InFakeWalk ( C_BasePlayer* player, ResolverAnimDataStorage store )
{
    /*
    bool
    	bFakewalking = false,
    	stage1 = false,			// stages needed cause we are iterating all layers, eitherwise won't work :)
    	stage2 = false,
    	stage3 = false;

    for (int i = 0; i < store.m_iLayerCount; i++)
    {
    	if (store.animationLayer[i].m_nSequence == 26 && store.animationLayer[i].m_flWeight < 0.47f)
    		stage1 = true;
    	if (store.animationLayer[i].m_nSequence == 7 && store.animationLayer[i].m_flWeight > 0.001f)
    		stage2 = true;
    	if (store.animationLayer[i].m_nSequence == 2 && store.animationLayer[i].m_flWeight == 0)
    		stage3 = true;
    }

    if (stage1 && stage2)
    	if (stage3 || (player->m_fFlags() & FL_DUCKING)) // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2
    		bFakewalking = true;
    	else
    		bFakewalking = false;
    else
    	bFakewalking = false;

    return bFakewalking;
    */
    AnimationLayer anim_layers[15];

    bool check_1 = false, check_2 = false, check_3 = false;

    for ( int i = 0; i < store.m_iLayerCount; i++ )
    {
        anim_layers[i] = store.animationLayer[i];

        if ( anim_layers[i].m_nSequence == 8 && anim_layers[i].m_flWeight == 1 )
            check_1 = true;

        if ( anim_layers[i].m_nSequence == 7 && anim_layers[i].m_flWeight > 0.f && anim_layers[i].m_flWeight < 0.003f )
            check_2 = true;

        if ( anim_layers[i].m_nSequence == 26 && anim_layers[i].m_flWeight > 0.f && anim_layers[i].m_flWeight < 0.3f  && anim_layers[i].m_flPlaybackRate > 0.f && anim_layers[i].m_flPlaybackRate < 0.001f )
            check_3 = true;

    }

    if ( ( check_1 && check_2 && check_3 ) )
        return true;
    else
        return false;

    return false;
}

bool Resolver::InFakeWalkOld ( C_BasePlayer* player, ResolverAnimDataStorage store )
{
    bool
    bFakewalking = false,
    stage1 = false,			// stages needed cause we are iterating all layers, eitherwise won't work :)
    stage2 = false,
    stage3 = false;

    for ( int i = 0; i < store.m_iLayerCount; i++ )
    {
        if ( store.animationLayer[i].m_nSequence == 26 && store.animationLayer[i].m_flWeight < 0.47f )
            stage1 = true;

        if ( store.animationLayer[i].m_nSequence == 7 && store.animationLayer[i].m_flWeight > 0.001f )
            stage2 = true;

        if ( store.animationLayer[i].m_nSequence == 2 && store.animationLayer[i].m_flWeight == 0 )
            stage3 = true;
    }

    if ( stage1 && stage2 )
        if ( stage3 || ( player->m_fFlags() & FL_DUCKING ) ) // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2
            bFakewalking = true;
        else
            bFakewalking = false;
    else
        bFakewalking = false;

    return bFakewalking;
}

bool Resolver::IsValidAngle ( C_BasePlayer* player, float angle )
{
    return fabs ( player->m_flLowerBodyYawTarget() - angle ) > 30.f;
}

bool Resolver::GetWallDistance ( C_BasePlayer* player, float& val )
{
    if ( !player )
        return false;

    Vector position = player->m_vecOrigin() + player->m_vecViewOffset();
    float closest_distance = 250.f;
    float radius = 250.f;
    float step = M_PI * 2.0 / 18;

    for ( float a = 0; a < ( M_PI * 2.0 ); a += step )
    {
        Vector location ( radius * cos ( a ) + position.x, radius * sin ( a ) + position.y, position.z );
        Ray_t ray;
        trace_t tr;
        ray.Init ( position, location );
        CTraceFilter traceFilter;
        traceFilter.pSkip = player;
        g_EngineTrace->TraceRay ( ray, 0x4600400B, &traceFilter, &tr );

        float distance = position.DistTo ( tr.endpos );

        if ( distance < closest_distance )
        {
            closest_distance = distance;
            //angle.yaw = RAD2DEG(a);
        }
    }

    if ( closest_distance >= radius )
        return false;

    val = closest_distance;
    return true;
}

bool Resolver::Is979 ( C_BasePlayer* player, ResolverAnimDataStorage store, AnimationLayer& layer )
{
    int layers = store.m_iLayerCount;

    for ( int i = 0; i < layers; i++ )
    {
        const int activity = player->GetSequenceActivity ( store.animationLayer[i].m_nSequence );

        if ( activity == ACT_CSGO_IDLE_TURN_BALANCEADJUST )
        {
            layer = store.animationLayer[i];
            return true;
        }
    }

    return false;
}

bool Resolver::FreestandingSim ( C_BasePlayer* player, float& ang )
{
    if ( !player || !player->IsAlive() || !g_LocalPlayer )
        return false;

    bool no_active = true;
    static float hold = 0.f;
    Vector besthead;

    auto leyepos = player->m_vecOrigin() + player->m_vecViewOffset();
    auto headpos = player->GetHitboxPos ( 0 ); //GetHitboxPosition(local_player, 0);
    auto origin = player->m_vecOrigin();

    auto checkWallThickness = [&] ( C_BasePlayer * pPlayer, Vector newhead ) -> float
    {

        Vector endpos1, endpos2;

        Vector eyepos = pPlayer->m_vecOrigin() + pPlayer->m_vecViewOffset();
        Ray_t ray;
        ray.Init ( newhead, eyepos );
        CTraceFilterSkipTwoEntities filter ( pPlayer, player );

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

    //int index = GetNearestPlayerToCrosshair();
    //static C_BasePlayer* entity;

    if ( !player->IsAlive() )
        hold = 0.f;


    float radius = Vector ( headpos - origin ).Length2D();

    auto CheckAllAnglesForEntity = [&] ( C_BasePlayer * pPlayer, bool & CanUse ) -> float
    {
        CanUse = false;

        if ( !pPlayer )
            return 0.f;

        float cbestrotation = 0.f;
        float chighestthickness = 0.f;

        float step = ( M_PI * 2 ) / 90;

        for ( float besthead = 0; besthead < ( M_PI * 2 ); besthead += step )
        {
            Vector newhead ( radius * cos ( besthead ) + leyepos.x, radius * sin ( besthead ) + leyepos.y, leyepos.z );
            float totalthickness = 0.f;
            totalthickness += checkWallThickness ( pPlayer, newhead );

            if ( totalthickness > chighestthickness && fabs ( pPlayer->m_flLowerBodyYawTarget() - RAD2DEG ( besthead ) ) > 20.f
                    && fabs ( SavedResolverData[pPlayer->EntIndex()].FakeYaw - RAD2DEG ( besthead ) ) > 20.f )
            {
                chighestthickness = totalthickness;
                cbestrotation = besthead;
                CanUse = true;
            }
        }

        return cbestrotation;
    };


    float r = 0.f;
    int c = 0;

    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
    {
        std::vector<C_BasePlayer*> enemies;

        for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
        {
            auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

            if ( !entity || !g_LocalPlayer || !entity->IsPlayer() || entity == player || entity->IsDormant()
                    || !entity->IsAlive() || !entity->IsEnemy() )
                continue;

            enemies.push_back ( entity );
        }

        if ( enemies.size() != 0 )
            no_active = false;

        for ( auto enemy = enemies.begin(); enemy != enemies.end(); enemy++ )
        {
            bool b = false;
            float t = RAD2DEG ( CheckAllAnglesForEntity ( *enemy, b ) );

            if ( b )
            {
                c++;
                r += t;
            }
        }
    }
    else
    {
        c = 1;
        bool b = false;
        r = RAD2DEG ( CheckAllAnglesForEntity ( g_LocalPlayer, b ) );

        if ( !b )
            return false;

        no_active = false;
    }

    if ( no_active )
        return false;
    else
    {
        ang = r / c;
        return true;
    }

    return false;
}

bool Resolver::GetOkFreestandingOrdered ( C_BasePlayer* player, float& ang )
{
    return false;
}

bool Resolver::BackwardsSim ( C_BasePlayer* player, float& ang )
{
    return false;
}

bool Resolver::AtTargetSim ( C_BasePlayer* player, float& ang )
{
    if ( !g_LocalPlayer || !g_LocalPlayer->IsAlive() )
        return false;

    Vector eye_position = g_LocalPlayer->GetEyePos();

    Vector target_position = player->GetEyePos();
    ang = Math::CalcAngle ( target_position, eye_position ).yaw - 180.f;

    return true;
}

void Resolver::UpdateDetectionData ( C_BasePlayer* player, int i )
{
    DetectionData[i].AllLbys.push_back ( player->m_flLowerBodyYawTarget() );
    DetectionData[i].LastAddSimtime.push_back ( player->m_flSimulationTime() );
    DetectionData[i].LastSimtime = player->m_flSimulationTime();

    if ( DetectionData[i].AllLbys.size() >= 2 )
        DetectionData[i].WasMoving = false;

    if ( ( int ) DetectionData[i].AllLbys.size() > 4 )
        DetectionData[i].AllLbys.erase ( DetectionData[i].AllLbys.begin() );
}

bool Resolver::IsUsingSpinbot ( C_BasePlayer* player, int i, float min_range, float tolerance )
{
    if ( DetectionData[i].WasMoving )
        return DetectionData[i].BeforeMoving == DetectionModes::Spinning;

    if ( DetectionData[i].AllLbys.size() < 2 || IsStaticAngle ( player, i ) )
        return false;

    //bool IsSpin = false;
    float LastLby = 0.f;
    float AverageChange = 0.f;
    float LastChange = 0.f;

    for ( auto lby = DetectionData[i].AllLbys.begin(); lby != DetectionData[i].AllLbys.end(); lby++ )
    {
        if ( LastLby == 0.f )
        {
            LastLby = *lby;
            continue;
        }

        float CurrentChange = LastLby - *lby;
        AverageChange += CurrentChange;

        if ( LastChange == 0.f )
        {
            LastChange = CurrentChange;
            continue;
        }

        if ( fabs ( LastChange - CurrentChange ) > tolerance )
            return false;
    }

    AverageChange /= DetectionData[i].AllLbys.size();

    DetectionData[i].SpinSpeed = AverageChange;

    if ( fabs ( AverageChange ) > min_range )
        return true;

    return false;
}

bool Resolver::IsStaticAngle ( C_BasePlayer* player, int i, float tolerance )
{
    if ( DetectionData[i].WasMoving )
        return DetectionData[i].BeforeMoving == DetectionModes::Static;

    if ( DetectionData[i].AllLbys.size() < 2 )
        return true;

    bool IsStatic = true;
    float LastLby = 0.f;

    for ( auto lby = DetectionData[i].AllLbys.begin(); lby != DetectionData[i].AllLbys.end(); lby++ )
    {
        if ( !IsStatic )
            continue;

        if ( LastLby == 0.f )
            LastLby = *lby;

        if ( fabs ( LastLby - *lby ) > tolerance )
            IsStatic = false;
    }

    return IsStatic;
}

bool Resolver::IsStaticSwitchOrJitter ( C_BasePlayer* player, int i, float tolerance )
{
    //if (DetectionData[i].WasMoving) return DetectionData[i].BeforeMoving == DetectionModes::Random;
    if ( DetectionData[i].AllLbys.size() < 2 || IsStaticAngle ( player, i ) )
        return false;

    /*
    bool IsStatic = true;
    float LastLby = 0.f;
    for (auto lby = DetectionData[i].AllLbys.begin(); lby != DetectionData[i].AllLbys.end(); lby++)
    {
    	if (!IsStatic) continue;
    	if (LastLby == 0.f) LastLby = *lby;

    	if (fabs(LastLby - *lby) > tolerance) IsStatic = false;
    }
    */

    return false;
}

bool Resolver::IsRandomAngle ( C_BasePlayer* player, int i, float tolerance )
{
    if ( DetectionData[i].WasMoving )
        return DetectionData[i].BeforeMoving == DetectionModes::Random;

    return !IsStaticAngle ( player, i ) && !IsUsingSpinbot ( player, i, 35.f, tolerance );
}
#else

void Resolver::AddCurrentYaw ( C_BasePlayer* pl, int i )
{
    LastYaws[i].push_back ( pl->m_angEyeAngles().yaw );

    if ( LastYaws[i].size() > 8 )
        LastYaws[i].erase ( LastYaws[i].begin() );
}
bool Resolver::IsStaticYaw ( int i )
{
    if ( LastYaws[i].size() < 3 )
        return true;

    //float LastYaw = 0.f;
    //float LastYaw2 = 0.f;
    //bool ReturnVal = true;

    float HighestDifference = 0.f;

    for ( size_t p = 0; p < LastYaws[i].size(); p++ )
    {
        for ( size_t p2 = 0; p2 < LastYaws[i].size(); p2++ )
        {
            float c = fabs ( fabs ( LastYaws[i].at ( p ) ) - fabs ( LastYaws[i].at ( p2 ) ) );

            if ( c > HighestDifference )
                HighestDifference = c;
        }
    }

    if ( HighestDifference > 15.f )
        return false;
    else
        return true;
}
bool Resolver::GetAverageYaw ( int i, float& ang )
{
    if ( LastYaws[i].size() < 3 )
        return true;

    float add = 0.f;

    for ( size_t p = 0; p < LastYaws[i].size(); p++ )
        add += LastYaws[i].at ( p );

    ang = add / LastYaws[i].size();
    return true;
}
#endif// FakeAnglesEnabled

Resolver g_Resolver;