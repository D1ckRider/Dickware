
#include "singleton.hpp"
#include "valve_sdk\csgostructs.hpp"
#include <deque>

#pragma once
class Lbot : public Singleton<Lbot>
{
public:
    void OnCreateMove(CUserCmd* cmd);
    //void LegitAA(CUserCmd* cmd, bool & bSendPackets);
private:
    //bool CanShoot(C_BaseCombatWeapon* weapon);
    void UpdateWeaponConfig(C_BaseCombatWeapon* weapon);
    int GetBestTarget(C_BasePlayer* local, C_BaseCombatWeapon* weapon, CUserCmd* cmd, Vector& hitpos);

    void ResetRecoil(CUserCmd* cmd);
	void AutoPistol(CUserCmd* cmd);
    void RemoveRecoil(C_BasePlayer* local, CUserCmd* cmd);
    void DoAimbot(CUserCmd* cmd, C_BasePlayer* local, C_BaseCombatWeapon* weapon);

    /* weapon settings */
    bool WeaponEnabled = false;

    float WeaponFov = 0.f;
    float WeaponSmooth = 1.f;
    float WeaponRandomness = 0.f;
    float WeaponDelay = 0.f;
    bool WeaponAutopistol = false;

    bool WeaponRcs = false;
    float WeaponRecoilX = 1.f;
    float WeaponRecoilY = 1.f;

    bool WeaponHitboxHead = false;
    bool WeaponHitboxNeck = false;
    bool WeaponHitboxChest = false;
    bool WeaponHitboxPelvis = false;
    bool WeaponHitboxStomach = false;
    bool WeaponHitboxArm = false;
    bool WeaponHitboxLeg = false;
    bool WeaponHitboxFoot = false;

    /* data */
    QAngle LastAimpunchRemove = QAngle(0, 0, 0);
    QAngle LastAimpunch = QAngle(0, 0, 0);
};

