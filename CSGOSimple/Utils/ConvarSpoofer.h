#pragma once

#include <map>
#include "..\valve_sdk\interfaces\ICvar.hpp"
#include "..\valve_sdk\misc\Convar.hpp"

class C_ConvarSpoofer 
{
public:
	void Init();
	void Release();

	void Add(const char *name);
	void Spoof();
	SpoofedConvar* Get(const char *name);
	bool Ready();
};
