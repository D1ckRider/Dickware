#pragma once
#include "..\valve_sdk\sdk.hpp"
#include "..\singleton.hpp"
#include "..\valve_sdk\csgostructs.hpp"
#include "..\helpers\math.hpp"

class TriggerBot : public Singleton<TriggerBot>
{
public:
	void OnCreateMove(CUserCmd* cmd);
};