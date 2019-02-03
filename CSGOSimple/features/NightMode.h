#pragma once
#include "..\valve_sdk\sdk.hpp"
#include "..\singleton.hpp"

class NightMode : public Singleton<NightMode>
{
public:
	void Apply(bool ForceUpdate);
	void Revert();
private:
	struct 
	{
		float AutoExposureMax;
		float AutoExposureMin;
		char* sv_skybox;
	} SavedValues;

	bool Active;
};

