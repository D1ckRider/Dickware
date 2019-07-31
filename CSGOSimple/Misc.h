
#include "singleton.hpp"
#include "valve_sdk\csgostructs.hpp"

#pragma once
class Misc : public Singleton<Misc>
{
public:
    void OnCreateMove(CUserCmd* cmd);
    void OnFrameStageNotify(ClientFrameStage_t stage);
	void ExecuteGameConfig(const std::string& config);
	void SetLocalPlayerReady();
	void SpectatorList();
	/*void PunchAngleFix_RunCommand(C_BasePlayer* base_player);
	void PunchAngleFix_FSN();*/
private:
	CUserCmd* pCmd;

	QAngle m_aimPunchAngle[128];
    void NoCrouchCooldown(CUserCmd* cmd);
    void NoFlash();
};

