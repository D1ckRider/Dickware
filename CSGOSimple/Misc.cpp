
#include "Misc.h"
#include "ConfigSystem.h"
#include "Settings.h"
#include "render.hpp"
#include "Rbot.h"
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
	int specs = 0;
	std::string spect = "";

	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected()) 
	{
		
		if (g_LocalPlayer)
		{
			for (int i = 0; i < g_EngineClient->GetMaxClients(); i++) {
				C_BasePlayer* pBaseEntity = C_BasePlayer::GetPlayerByIndex(i);
				if (!pBaseEntity)										     continue;
				if (pBaseEntity->m_iHealth() > 0)							 continue;
				if (pBaseEntity == g_LocalPlayer)							 continue;
				if (pBaseEntity->IsDormant())								 continue;
				if (pBaseEntity->m_hObserverTarget() != g_LocalPlayer)		 continue;
				player_info_t pInfo;
				g_EngineClient->GetPlayerInfo(pBaseEntity->EntIndex(), &pInfo);
				if (pInfo.ishltv) continue;

				spect += pInfo.szName;
				spect += "\n";
				specs++;
			}
		}
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
	if (ImGui::Begin("Spectator List", nullptr, ImVec2(0, 0), 0.4F, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) 
	{
		if (specs > 0) spect += "\n";

		ImVec2 size = ImGui::CalcTextSize(spect.c_str());
		ImGui::SetWindowSize(ImVec2(200, 25 + size.y));
		ImGui::Text(spect.c_str());
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void Misc::NoCrouchCooldown(CUserCmd* cmd)
{
#if 0
	if (!Settings::Misc::NoCrouchCooldown)
		return;

	cmd->buttons |= IN_BULLRUSH;
#endif // 0
}

void Misc::NoFlash()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || !Settings::Visual::NoFlash)
        return;

	g_LocalPlayer->m_flFlashDuration() = 0.f;
}

//
//
//void Misc::PunchAngleFix_RunCommand(C_BasePlayer* base_player)
//{
//	if (g_LocalPlayer && g_LocalPlayer->IsAlive() && g_LocalPlayer == &base_player)
//		m_aimPunchAngle[Rbot::Get().GetTickbase() % 128] = g_LocalPlayer->m_aimPunchAngle();
//}
//
//void Misc::PunchAngleFix_FSN()
//{
//	if (g_LocalPlayer && g_LocalPlayer->IsAlive())
//	{
//		QAngle new_punch_angle = m_aimPunchAngle[Rbot::Get().GetTickbase() % 128];
//
//		if (!new_punch_angle.IsValid())
//			return;
//
//		g_LocalPlayer->m_aimPunchAngle() = new_punch_angle;
//	}
//}
