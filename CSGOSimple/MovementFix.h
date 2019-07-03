
#include "singleton.hpp"
#include "valve_sdk/csgostructs.hpp"

#pragma once
class MovementFix : public Singleton<MovementFix>
{
public:
    void Correct(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	void FixMovement(CUserCmd* cmd, QAngle& wish_angle);
};

