
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
private:
	CUserCmd* pCmd;

    void NoCrouchCooldown(CUserCmd* cmd);
    void NoFlash();
};

