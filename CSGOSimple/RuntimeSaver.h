
#include "valve_sdk/csgostructs.hpp"

#pragma once

struct FakeLagData
{
    Vector pos = Vector(0, 0, 0);
    QAngle ang = QAngle(0, 0, 0);
};

struct ImpactInfoStruct
{
    Vector ShotPos = Vector(0, 0, 0);
    Vector Pos = Vector(0, 0, 0);
    float Time = 0.f;
    int Tick = 0;
};

struct StepInfoStruct
{
	Vector Origin = Vector(0, 0, 0);
	float SoundTime = 0.f;
};

struct DamageIndicator
{
	int iDamage;
	bool bInitialized;
	float flEraseTime;
	float flLastUpdate;
	C_BasePlayer* Player;
	Vector Position;
};

struct MovotovInfoStruct
{
	Vector position;
	float timeToExpire;
};

struct BulletTracerInfoStruct
{
    bool DidHit = false;
    float Time = 0.f;
    Vector ShotPos = Vector(0, 0, 0);
    Vector HitPos = Vector(0, 0, 0);
};

struct HitmarkerInfoStruct
{
    float HitTime = 0.f;
    float Damage = 0.f;
};

struct RbotShotInfoStruct
{
    bool InLc = false;
    bool InLbyUpdate = false;
    bool Moving = false;
};

class RuntimeSaver
{
public:
    QAngle AARealAngle = QAngle(0, 0, 0);
    QAngle AADesyncAngle = QAngle(0, 0, 0);
    bool FakelagCurrentlyEnabled = false;
    FakeLagData FakelagData;
    float curtime = 0.f;
	bool ForceBAim = false;


    //Lby update
    bool InLbyUpdate = false;
    float NextLbyUpdate = 0.f;

    //AnimationFixData
    bool HasChokedLast = false;
    QAngle LastRealAnglesBChoke = QAngle(0, 0, 0);

    //HitposHelper
    std::vector<ImpactInfoStruct> hitpositions;

    //BulletTracers
    std::vector<BulletTracerInfoStruct> BulletTracers;

    //Rbot
    bool RbotAADidShot = false;

    bool CurrentInLbyBreak = false;
    bool CurrentShouldSkipAnimations = false;

    //Fakewalk
    bool InFakewalk = false;

    int LastBacktrackTick = -1;

    bool LCbroken = false;

    HitmarkerInfoStruct HitmarkerInfo;

	std::vector<DamageIndicator> DamageIndicators;
	std::vector<StepInfoStruct> StepInfo;
	std::vector<MovotovInfoStruct> MolotovInfo;

    RbotShotInfoStruct RbotShotInfo;

    Vector LastShotEyePos = Vector(0, 0, 0);

    bool RbotDidLastShot = false;
};

extern RuntimeSaver g_Saver;