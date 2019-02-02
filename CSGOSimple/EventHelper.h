
#include "singleton.hpp"
#include "valve_sdk\csgostructs.hpp"
#include "options.hpp"
#include "Resolver.h"
#include "Logger.h"
#include "HitPossitionHelper.h"
#include "ConfigSystem.h"
#include "RuntimeSaver.h"
#include "Settings.h"
#include "Rbot.h"
#include "Features\visuals.hpp"

#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

#pragma once
class EventHelper : public Singleton<EventHelper>
{
    class listener : public IGameEventListener2
    {
    public:
        void Start();
        void Stop();
        void FireGameEvent(IGameEvent* event) override;
        int GetEventDebugID(void) override;
    };
public:
    void init()
    {
        Listener.Start();
    }
    void unInit()
    {
        Listener.Stop();
    }
    void on_fire_event(IGameEvent* event)
    {
        //resolver::Get().OnEvent(event);
        //NewResolver::Get().OnEvent(event);
        //#include "BackTrack.h"
        //BackTrack::Get().OnEvent(event);
        Resolver::Get().OnFireEvent(event);
        HitPossitionHelper::Get().OnFireEvent(event);
        Rbot::Get().OnFireEvent(event);

		static ConVar* sv_showimpact = g_CVar->FindVar("sv_showimpacts");
        
		if (!strcmp(event->GetName(), "player_hurt"))
        {
			if (!Settings::Visual::Hitmarker)
				return;

			int hurt = event->GetInt("userid");
            int attacker = event->GetInt("attacker");
			//C_BaseEntity* hurt = (C_BaseEntity*)g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));
			//C_BaseEntity* attacker = (C_BaseEntity*)g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));

            if (g_EngineClient->GetPlayerForUserID(attacker) == g_EngineClient->GetLocalPlayer())
            {
				if (g_EngineClient->GetPlayerForUserID(hurt) != g_EngineClient->GetLocalPlayer())
				{
					DamageIndicator DmgIndicator;
					DmgIndicator.iDamage = event->GetInt("dmg_health");
					//DmgIndicator.PlayerID = hurt; //hurt;
					DmgIndicator.Player = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid")));
					DmgIndicator.flEraseTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick + 3.f;
						//g_EngineClient->GetLocalPlayer()->TickBase() * Globals->interval_per_tick + 3.f;
					DmgIndicator.bInitialized = false;

					g_Saver.DamageIndicators.push_back(DmgIndicator);
				}

                g_Saver.HitmarkerInfo = HitmarkerInfoStruct{ g_GlobalVars->realtime, 0.f };
                /*switch (g_Config.GetInt("vis_misc_hitmarker_sound"))
                {
                    case 0:
                        PlaySoundA((g_Config.AppdataFolder + "hitsound1.wav").data(), NULL, SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
                        break;
                    case 1:
                        PlaySoundA((g_Config.AppdataFolder + "hitsound2.wav").data(), NULL, SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
                        break;
                }*/

				if(Settings::Visual::HitmarkerSound)
					g_VGuiSurface->PlaySound_("buttons\\arena_switch_press_02.wav");
            }
        }

        if (!strcmp(event->GetName(), "bullet_impact"))
        {
            C_BasePlayer* shooter = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))));

            if (!shooter || shooter != g_LocalPlayer)
                return;
            Vector p = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

			if (Settings::Visual::BulletTracers)
				sv_showimpact->SetValue(1);
			else
				sv_showimpact->SetValue(0);

            ShotTracer(g_Saver.LastShotEyePos, p);
        }

		/*if (!strcmp(event->GetName(), "player_footstep"))
		{
			g_Logger.Info("VISUAL", "Player footstep triggered");
			C_BasePlayer* player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))));
			if (!player || player == g_LocalPlayer)
				return;

			//g_Saver.StepInfo = StepInfoStruct{ event->GetInt("userid"), g_GlobalVars->realtime };
			//Visuals::Get().RenderSoundESP(player);
		}*/
    }
    void ShotTracer(Vector shot_pos, Vector hit_pos);
private:
    listener Listener;
};

