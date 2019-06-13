
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
    //bool CanShoot(C_BaseCombatWeapon* weapon);
    void UpdateWeaponConfig(C_BaseCombatWeapon* weapon);
    //int GetBestTarget(C_BasePlayer* local, C_BaseCombatWeapon* weapon, CUserCmd* cmd, Vector& hitpos);

    //void ResetRecoil(CUserCmd* cmd);
	//void AutoPistol(CUserCmd* cmd);
    //void RemoveRecoil(C_BasePlayer* local, CUserCmd* cmd);
    //void DoAimbot(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon);




    /* weapon settings */
	int WeaponID = -1;
	
	/*bool WeaponEnabled = false;
	
	bool WeaponSilent;
    float WeaponFov = 0.f;
	int WeaponFOVType = 0;
	float WeaponSilentFOV;
	int WeaponAimType = 0;
	int WeaponHitbox = 0;
	int WeaponSmoothType = 0;
    float WeaponSmooth = 1.f;
    float WeaponRandomness = 0.f;
    int WeaponDelay = 0;
	int WeaponKillDelay = 0;
    bool WeaponAutopistol = false;
	bool WeaponAutowall = false;
	int WeaponMinDamage = 0;
	bool WeaponAutoFire = false;

    bool WeaponRcs = false;
	int WeaponRCSType = 0;
	int WeaponRCSBullet = 0;
	bool WeaponRCSSmoothEnabled = false;
	bool WeaponRCSFOVEnabled = false;
	float WeaponRCSFOV = 0.f;
	float WeaponRCSSmooth = 0.f;
    int WeaponRecoilX = 1.f;
    int WeaponRecoilY = 1.f;

    bool WeaponHitboxHead = false;
    bool WeaponHitboxNeck = false;
    bool WeaponHitboxChest = false;
    bool WeaponHitboxPelvis = false;
    bool WeaponHitboxStomach = false;
    bool WeaponHitboxArm = false;
    bool WeaponHitboxLeg = false;
    bool WeaponHitboxFoot = false;*/
	
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

