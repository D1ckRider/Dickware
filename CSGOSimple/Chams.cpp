
#include "Chams.h"
#include "Settings.h"
#include "ConfigSystem.h"

void Chams::OnSceneEnd()
{
    if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected() || !g_LocalPlayer)
    {
        return;
    }

    /*ChamsModes LocalChamsMode = (ChamsModes)g_Config.GetInt("chams_mode_local");
    ChamsModes TeamChamsMode = (ChamsModes)g_Config.GetInt("chams_mode_team");
    ChamsModes EnemyChamsMode = (ChamsModes)g_Config.GetInt("chams_mode_enemy");

    bool LocalChams = g_Config.GetBool("chams_local");
    bool TeamChams = g_Config.GetBool("chams_team");
    bool EnemyChams = g_Config.GetBool("chams_enemy");

    Color LocalColor = g_Config.GetColor("color_chams_local");
    Color TeamColor = g_Config.GetColor("color_chams_team");
    Color EnemyColor = g_Config.GetColor("color_chams_enemy");

    Color LocalColorXqz = g_Config.GetColor("color_chams_local_xqz");
    Color TeamColorXqz = g_Config.GetColor("color_chams_team_xqz");
    Color EnemyColorXqz = g_Config.GetColor("color_chams_enemy_xqz");*/
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

    g_MdlRender->ForcedMaterialOverride(nullptr);
}