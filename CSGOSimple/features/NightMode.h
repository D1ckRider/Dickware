#pragma once
#include "..\valve_sdk\sdk.hpp"
#include "..\singleton.hpp"

class NightMode : public Singleton<NightMode>
{
public:
	void Apply();
	void Revert();
private:
	bool Active;
};

