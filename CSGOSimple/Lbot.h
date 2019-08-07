
#include "singleton.hpp"
#include "valve_sdk\csgostructs.hpp"
#include <deque>

#pragma once
class Lbot : public Singleton<Lbot>
{
public:
    void OnCreateMove(CUserCmd* cmd);
	bool IsEnabled(CUserCmd* pCmd);
	float GetFovToPlayer(QAngle viewAngle, QAngle aimAngle);

	bool IsRcs();
	float GetSmooth();
	float GetFov();

    void LegitAA(CUserCmd* cmd, bool & bSendPackets);
private:
    /* weapon settings */
	int WeaponID = -1;
	
	void RCS(QAngle& angle, C_BasePlayer* target, bool should_run);
	bool IsLineGoesThroughSmoke(Vector vStartPos, Vector vEndPos);
	void Smooth(QAngle currentAngle, QAngle aimAngle, QAngle& angle);
	bool IsNotSilent(float fov);
	C_BasePlayer* GetClosestPlayer(CUserCmd* cmd, int& bestBone);
	float shot_delay_time;
	bool shot_delay = false;
	bool silent_enabled = false;
	QAngle CurrentPunch = { 0,0,0 };
	QAngle RCSLastPunch = { 0,0,0 };
	bool is_delayed = false;
	int kill_delay_time;
	bool kill_delay = false;
	C_BasePlayer* target = NULL;
	float side = 1.0f;
	float next_lby = 0.0f;

    /* data */
    QAngle LastAimpunchRemove = QAngle(0, 0, 0);
    QAngle LastAimpunch = QAngle(0, 0, 0);
};

