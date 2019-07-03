
#include "valve_sdk/csgostructs.hpp"
#include "RuntimeSaver.h"

#pragma once
class EnginePrediction
{
public:

	/*EnginePrediction() 
	{
		auto client = GetModuleHandle(L"client_panorama.dll");
		predictionRandomSeed = *(int**)(Utils::PatternScan(client, "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);
		predictionPlayer = *reinterpret_cast<C_BasePlayer * *>(Utils::PatternScan(client, "89 35 ? ? ? ? F3 0F 10 46") + 2);
	}*/

    void run_prediction(CUserCmd* cmd, C_BasePlayer* plr);
    void end_prediction(C_BasePlayer* player);

	void Setup(player_prediction_data& data);
	void RunPrediction(player_prediction_data& data, CUserCmd* cmd);
	void EndPrediction(player_prediction_data& data);

private:
	float m_flOldCurTime;
	float m_flOldFrametime;

	CMoveData moveData;

	int* predictionRandomSeed;
	C_BasePlayer* predictionPlayer;
};

extern EnginePrediction* prediction;