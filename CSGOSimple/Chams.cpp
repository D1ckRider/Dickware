
#include "Chams.h"
#include "RuntimeSaver.h"
#include "AntiAim.h"
#include "Settings.h"
#include "hooks.hpp"
#include "features/LagCompensation.h"

void Chams::OnSceneEnd()
{
    if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer)
        return;

	ChamsModes LocalChamsMode = (ChamsModes)Settings::Visual::LocalChams.Mode;
	ChamsModes TeamChamsMode = (ChamsModes)Settings::Visual::TeamChams.Mode;
	ChamsModes EnemyChamsMode = (ChamsModes)Settings::Visual::EnemyChams.Mode;

	bool LocalChams = Settings::Visual::LocalChams.Enabled;
	bool TeamChams = Settings::Visual::TeamChams.Enabled;
	bool EnemyChams = Settings::Visual::EnemyChams.Enabled;

	Color LocalColor = Settings::Visual::LocalChams.Visible;
	Color TeamColor = Settings::Visual::TeamChams.Visible;
	Color EnemyColor = Settings::Visual::EnemyChams.Visible;

	Color LocalColorXqz = Settings::Visual::LocalChams.Invisible;
	Color TeamColorXqz = Settings::Visual::TeamChams.Invisible;
	Color EnemyColorXqz = Settings::Visual::EnemyChams.Invisible;

    for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
    {
        auto entity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
        if (!entity || !entity->IsPlayer() || entity->IsDormant() || !entity->IsAlive())
            continue;

        bool IsLocal = entity == g_LocalPlayer;
        bool IsTeam = !entity->IsEnemy();

        bool normal = false;
        bool flat = false;
        bool wireframe = false;
        bool glass = false;
        bool metallic = false;
        bool xqz = false;
        bool metallic_xqz = false;
        bool flat_xqz = false;

        ChamsModes mode = IsLocal ? LocalChamsMode : (IsTeam ? TeamChamsMode : EnemyChamsMode);

        if (IsLocal && !LocalChams)
            continue;
        if ((IsTeam && !IsLocal) && !TeamChams)
            continue;
        if (!IsTeam && !EnemyChams)
            continue;

        Color clr = IsLocal ? LocalColor : (IsTeam ? TeamColor : EnemyColor);
        Color clr2 = IsLocal ? LocalColorXqz : (IsTeam ? TeamColorXqz : EnemyColorXqz);


        switch (mode)
        {
            case ChamsModes::NORMAL:
                normal = true;
                break;
            case ChamsModes::FLAT:
                flat = true;
                break;
            case ChamsModes::WIREFRAME:
                wireframe = true;
                break;
            case ChamsModes::GLASS:
                glass = true;
                break;
            case ChamsModes::METALLIC:
                metallic = true;
                break;
            case ChamsModes::XQZ:
                xqz = true;
                break;
            case ChamsModes::METALLIC_XQZ:
                metallic = true;
                metallic_xqz = true;
                break;
            case ChamsModes::FLAT_XQZ:
                flat = true;
                flat_xqz = true;
                break;
        }

        MaterialManager::Get().OverrideMaterial(xqz || metallic_xqz || flat_xqz, flat, wireframe, glass, metallic);
        g_RenderView->SetColorModulation(clr2.r() / 255.f, clr2.g() / 255.f, clr2.b() / 255.f);
        entity->GetClientRenderable()->DrawModel(0x1, 255);
        if (xqz || metallic_xqz || flat_xqz)
        {
            MaterialManager::Get().OverrideMaterial(false, flat, wireframe, glass, metallic);
            g_RenderView->SetColorModulation(clr.r() / 255.f, clr.g() / 255.f, clr.b() / 255.f);
            entity->GetClientRenderable()->DrawModel(0x1, 255);
        }
        g_MdlRender->ForcedMaterialOverride(nullptr);
    }
	if (g_LocalPlayer && Settings::RageBot::Enabled && Settings::Visual::GhostEnabled)
	{
		QAngle OrigAng;
		OrigAng = g_LocalPlayer->m_angEyeAngles();
		g_LocalPlayer->SetAngle2(QAngle(0, g_Saver.AADesyncAngle.yaw, 0));
		bool LbyColor = false; // u can make LBY INDICATOR. When LbyColor is true. Color will be Green , if false it will be White
		float NormalColor[3] = { Settings::Visual::GhostColor.r() / 255.f,
									Settings::Visual::GhostColor.g() / 255.f,
									Settings::Visual::GhostColor.b() / 255.f, };
		float lbyUpdateColor[3] = { 0, 1, 0 };
		MaterialManager::Get().OverrideMaterial(false, true, false, false, false);
		g_RenderView->SetColorModulation(LbyColor ? lbyUpdateColor : NormalColor);
		g_RenderView->SetBlend(.5f);
		g_LocalPlayer->DrawModel(1, 255);
		g_MdlRender->ForcedMaterialOverride(nullptr);
		g_RenderView->SetBlend(1.f);
		g_LocalPlayer->SetAngle2(OrigAng);
	}
    g_MdlRender->ForcedMaterialOverride(nullptr);
}

void Chams::OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld,
								float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int iFlags)
{
	static auto fnDME = Hooks::mdlrender_hook.get_original<Hooks::DrawModelExecute>(index::DrawModelExecute);

	if (!pInfo->m_pClientEntity || !g_LocalPlayer)
		return;

	const auto mdl = pInfo->m_pClientEntity->GetModel();

	//bool is_sleeve = strstr( mdl->szName, "sleeve" ) != nullptr;
	//bool is_weapon = strstr(mdl->szName, "weapons/v_")  != nullptr;

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

			/*if (enemy_bt_enabled && LagCompensation::Get().m_LagRecord[ent->EntIndex()].size() > 0) 
{
				auto& data = LagCompensation::Get().m_LagRecord[ent->EntIndex()];//g_Backtrack.data.at(ent->EntIndex());
				if (data.size() > 0) 
				{
					/*if (g_Options.chams_backtrack == 2) 
					{
						for (auto& record : data) {
							OverrideMaterial(
								false,
								g_Options.chams_backtrack_flat,
								false,
								false,
								Color(g_Options.color_chams_backtrack));
							fnDME(g_StudioRender, pResults, pInfo, record.boneMatrix, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}
					}
					else if (g_Options.chams_backtrack == 1) 
					{
						auto& back = data.back();
						MaterialManager::Get().OverrideMaterial(true, true, false, false, false, Color::White);
						fnDME(g_StudioRender, pResults, pInfo, back.matrix, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
					//}
				}
			}*/


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
	/*else if (strstr(mdl->szName, "arms") != nullptr) {
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);

		if (!material)
			return;

		if (g_Options.other_no_hands) 
		{
			material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			g_MdlRender->ForcedMaterialOverride(material);
		}
	}*/
}