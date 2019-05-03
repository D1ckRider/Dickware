#pragma once
#include "../singleton.hpp"
#include "../valve_sdk/csgostructs.hpp"

class Radar : public Singleton<Radar>
{
public:
	void Render();
private:
	void RadarRange(float* x, float* y, float range);
	void CalcRadarPoint(Vector vOrigin, int& screenx, int& screeny);

	void OnRenderPlayer();
	void OnRenderSounds();
};