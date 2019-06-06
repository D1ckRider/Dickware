
#include "glow.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../ConfigSystem.h"
#include "../Settings.h"


Glow::Glow()
{
}

Glow::~Glow()
{
    // We cannot call shutdown here unfortunately.
    // Reason is not very straightforward but anyways:
    // - This destructor will be called when the dll unloads
    //   but it cannot distinguish between manual unload
    //   (pressing the Unload button or calling FreeLibrary)
    //   or unload due to game exit.
    //   What that means is that this destructor will be called
    //   when the game exits.
    // - When the game is exiting, other dlls might already
    //   have been unloaded before us, so it is not safe to
    //   access intermodular variables or functions.
    //
    //   Trying to call Shutdown here will crash CSGO when it is
    //   exiting (because we try to access g_GlowObjManager).
    //
}

void Glow::Shutdown()
{
    // Remove glow from all entities
    for(auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++)
    {
        auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
        auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

        if(glowObject.IsUnused())
        {
            continue;
        }

        if(!entity || entity->IsDormant())
        {
            continue;
        }

        glowObject.m_flAlpha = 0.0f;
    }
}

void Glow::Run()
{
    if (!g_LocalPlayer)
        return;


    for(auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++)
    {
        auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
        auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

        if(glowObject.IsUnused())
            continue;

        if(!entity || entity->IsDormant())
            continue;

        auto class_id = entity->GetClientClass()->m_ClassID;
        auto color = Color{};

        switch(class_id)
        {
            case ClassId::CCSPlayer:
            {
                auto is_enemy = entity->IsEnemy();
				bool glow_enabled = Settings::Visual::TeamGlow.Enabled || Settings::Visual::EnemyGlow.Enabled;

                /*if(entity->HasC4() && is_enemy && g_Config.GetBool("glow_c4_carrier"))
                {
                    color = g_Config.GetColor("color_glow_c4_carrier");
                    break;
                }*/

                //if(!g_Config.GetBool("glow_players") || !entity->IsAlive())
				if(!glow_enabled || !entity->IsAlive())
                    continue;

                //if(!is_enemy && g_Config.GetBool("glow_enemies_only"))
				if(!is_enemy && !Settings::Visual::TeamGlow.Enabled)
                    continue;

                color = is_enemy ? Settings::Visual::EnemyGlow.Visible : Settings::Visual::TeamGlow.Visible;
				
				glowObject.m_nGlowStyle = is_enemy ? Settings::Visual::EnemyGlow.Type : Settings::Visual::TeamGlow.Type;
                break;
            }
            case ClassId::CChicken:
                /*if(!g_Config.GetBool("glow_chickens"))
                {
                    continue;
                }
                entity->m_bShouldGlow() = true;
                color = g_Config.GetColor("color_glow_chickens");
                break;*/
            case ClassId::CBaseAnimating:
                /*if(!g_Config.GetBool("glow_defuse_kits"))
                {
                    continue;
                }
                color = g_Config.GetColor("color_glow_defuse");
                break;*/
            case ClassId::CPlantedC4:
                /*if(!g_Config.GetBool("glow_planted_c4"))
                {
                    continue;
                }
                color = g_Config.GetColor("color_glow_planted_c4");
                break;*/
            default:
            {
                /*if(entity->IsWeapon())
                {
                    if(!g_Config.GetBool("glow_weapons"))
                    {
                        continue;
                    }
                    color = g_Config.GetColor("color_glow_weapons");
                }*/
            }
        }

        glowObject.m_flRed = color.r() / 255.0f;
        glowObject.m_flGreen = color.g() / 255.0f;
        glowObject.m_flBlue = color.b() / 255.0f;
        glowObject.m_flAlpha = color.a() / 255.0f;
        glowObject.m_bRenderWhenOccluded = true;
        glowObject.m_bRenderWhenUnoccluded = false;
		
    }
}
