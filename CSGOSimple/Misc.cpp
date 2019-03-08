
#include "Misc.h"
#include "ConfigSystem.h"
#include "Settings.h"
#include "render.hpp"
#include "imgui\imgui.h"

void Misc::OnCreateMove(CUserCmd* cmd)
{
	pCmd = cmd;
    if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
        return;

    NoCrouchCooldown(cmd);
}

void Misc::OnFrameStageNotify(ClientFrameStage_t stage)
{
    switch (stage)
    {
        case FRAME_UNDEFINED:
            break;
        case FRAME_START:
            break;
        case FRAME_NET_UPDATE_START:
            break;
        case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
            break;
        case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
            break;
        case FRAME_NET_UPDATE_END:
            break;
        case FRAME_RENDER_START:
            NoFlash();
            break;
        case FRAME_RENDER_END:
            break;
    }
}

void Misc::ExecuteGameConfig(const std::string & config)
{
	g_EngineClient->ExecuteClientCmd(config.c_str());
}

void Misc::SetLocalPlayerReady()
{
	static auto SetLocalPlayerReadyFn = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));
	if (SetLocalPlayerReadyFn && Settings::Misc::AutoAccept)
		SetLocalPlayerReadyFn("");
}

void Misc::SpectatorList()
{
	if (!g_EngineClient->IsInGame() || !g_LocalPlayer)
		return;

	std::string spectators;

	for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* entity = C_BasePlayer::GetPlayerByIndex(i);

		if (!entity)
			continue;

		if (entity->IsAlive())
			continue;

		if (entity->IsDormant())
			continue;

		if (!entity->m_hObserverTarget())
			continue;

		C_BasePlayer* target = entity->m_hObserverTarget();

		if (!target->IsPlayer())
			continue;

		player_info_t entityinfo = entity->GetPlayerInfo();
		player_info_t targetinfo = target->GetPlayerInfo();

		spectators += std::string(entityinfo.szName) + " > " + targetinfo.szName + "\n";
	}

	auto size = g_pDefaultFont->CalcTextSizeA(16.f, FLT_MAX, NULL, spectators.c_str()); // 16 на размер шрифта меняете и Menu::Get().globalFont на ваш шрифт

	ImGui::SetNextWindowSize(ImVec2(300, size.y + 40.f));
	if (ImGui::Begin("Spectator List", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text(spectators.c_str());
		ImGui::End();
	}
}

void Misc::NoCrouchCooldown(CUserCmd* cmd)
{
	if(!Settings::Misc::NoCrouchCooldown)
        return;

    cmd->buttons |= IN_BULLRUSH;
}

void Misc::NoFlash()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || !Settings::Visual::NoFlash)
        return;

    //g_LocalPlayer->m_flFlashMaxAlpha() = 0.f;
    g_LocalPlayer->m_flFlashDuration() = 0.f;
}
