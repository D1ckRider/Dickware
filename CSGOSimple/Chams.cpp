
#include "Chams.h"
#include "AntiAim.h"
#include "Settings.h"

void Chams::OnSceneEnd()
{
    if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer)
    {
        return;
    }

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
        {
            continue;
        }

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
        {
            continue;
        }
        if ((IsTeam && !IsLocal) && !TeamChams)
        {
            continue;
        }
        if (!IsTeam && !EnemyChams)
        {
            continue;
        }

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
		g_LocalPlayer->SetAngle2(QAngle(0, AntiAim::Get().DesyncAngles.yaw, 0));
		bool LbyColor = false; // u can make LBY INDICATOR. When LbyColor is true. Color will be Green , if false it will be White
		float NormalColor[3] = { Settings::Visual::GhostColor.r() / 255.f,
									Settings::Visual::GhostColor.g() / 255.f,
									Settings::Visual::GhostColor.b() / 255.f, };
		float lbyUpdateColor[3] = { 0, 1, 0 };
		MaterialManager::Get().OverrideMaterial(false, true, false, false, false);
		g_RenderView->SetColorModulation(LbyColor ? lbyUpdateColor : NormalColor);
		g_RenderView->SetBlend(.3f);
		//g_MdlRender->ForcedMaterialOverride(mat);
		g_LocalPlayer->DrawModel(1, 255);
		g_MdlRender->ForcedMaterialOverride(nullptr);
		g_RenderView->SetBlend(1.f);
		g_LocalPlayer->SetAngle2(OrigAng);
	}
    g_MdlRender->ForcedMaterialOverride(nullptr);
}