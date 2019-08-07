
#include "Chams.h"
#include "RuntimeSaver.h"
#include "AntiAim.h"
#include "Settings.h"
#include "hooks.hpp"
#include "features/LagCompensation.h"

void Chams::OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld,
								float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int iFlags)
{
	static auto fnDME = Hooks::mdlrender_hook.get_original<Hooks::DrawModelExecute>(index::DrawModelExecute);

	if (!pInfo->m_pClientEntity || !g_LocalPlayer)
		return;

	const auto mdl = pInfo->m_pClientEntity->GetModel();

	bool chamsEnabled = Settings::Visual::LocalChams.Enabled || Settings::Visual::EnemyChams.Enabled || Settings::Visual::TeamChams.Enabled;
	if (chamsEnabled && strstr(mdl->szName, "models/player") != nullptr)
	{
		// 
		// Draw player Chams.
		// 
		auto ent = (C_BasePlayer*)(pInfo->m_pClientEntity->GetIClientUnknown()->GetBaseEntity());

		// Local Player chams
		if (ent == g_LocalPlayer && Settings::Visual::LocalChams.Enabled)
		{
			const auto clr_front =  Color(Settings::Visual::LocalChams.Visible);
			const auto clr_back =  Color(Settings::Visual::LocalChams.Invisible);
			const auto flat = Settings::Visual::LocalChams.Mode == 1 || Settings::Visual::LocalChams.Mode == 7;
			const auto metallic = Settings::Visual::LocalChams.Mode == 4 || Settings::Visual::LocalChams.Mode == 6;
			const auto wireframe = Settings::Visual::LocalChams.Mode == 2;
			const auto glass = Settings::Visual::LocalChams.Mode == 3;
			bool ignoreZ = Settings::Visual::LocalChams.Mode >= 5;



			if (ignoreZ)
			{
				MaterialManager::Get().OverrideMaterial(true, flat, wireframe, false, metallic, clr_back);
				fnDME(g_StudioRender, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

				MaterialManager::Get().OverrideMaterial(false, flat, wireframe, false, metallic, clr_front);
			}
			else
			{
				MaterialManager::Get().OverrideMaterial(false, flat, wireframe, glass, metallic, clr_front);
			}

		}

		// Other Chams
		if (ent && ent->IsPlayer() && ent->IsAlive()) 
		{
			const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
			const auto enemy_bt_enabled = Settings::Visual::EnemyChams.Enabled && Settings::RageBot::LagComp;
			if (!enemy && !Settings::Visual::TeamChams.Enabled)
				return;

			const auto clr_front = enemy ? Color(Settings::Visual::EnemyChams.Visible) : Color(Settings::Visual::TeamChams.Visible);
			const auto clr_back = enemy ? Color(Settings::Visual::EnemyChams.Invisible) : Color(Settings::Visual::TeamChams.Invisible);
			const auto flat = enemy ? (Settings::Visual::EnemyChams.Mode == 1 || Settings::Visual::EnemyChams.Mode == 7)
									: (Settings::Visual::TeamChams.Mode == 1 || Settings::Visual::TeamChams.Mode == 7);
			const auto metallic = enemy ? (Settings::Visual::EnemyChams.Mode == 4 || Settings::Visual::EnemyChams.Mode == 6)
										: (Settings::Visual::TeamChams.Mode == 4 || Settings::Visual::TeamChams.Mode == 6);
			const auto wireframe = enemy ? (Settings::Visual::EnemyChams.Mode == 2)
										 : (Settings::Visual::TeamChams.Mode == 2);
			const auto glass = enemy ? (Settings::Visual::EnemyChams.Mode == 3)
									 : (Settings::Visual::TeamChams.Mode == 3);
			bool ignoreZ = Settings::Visual::EnemyChams.Mode >= 5 || Settings::Visual::TeamChams.Mode >= 5;

			if (ignoreZ) 
			{
				MaterialManager::Get().OverrideMaterial(true, flat, wireframe, false, metallic, clr_back);
				fnDME(g_StudioRender, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
				
				MaterialManager::Get().OverrideMaterial(false, flat, wireframe, false , metallic, clr_front);
			}
			else 
			{
				MaterialManager::Get().OverrideMaterial(false, flat, wireframe, glass, metallic, clr_front);
			}
		}
	}
}