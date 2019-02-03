#pragma once
#include "..\valve_sdk\sdk.hpp"
#include "..\valve_sdk\csgostructs.hpp"
#include "..\options.hpp"

class Skinchanger : public Singleton<Skinchanger>
{
public:
	void OnFrameStageNotify(ClientFrameStage_t stage);
};

