
#include "valve_sdk/csgostructs.hpp"
#include <Windows.h>

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
	Vector Origin = Vector(0,0,0);
	float Time = 0.f;
	int EntityIndex = -1;
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
	Vector Position;
	float TimeToExpire;
};

struct SmokeInfoStruct
{
	Vector Position;
	float TimeToExpire;
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

/*
 * player_old_flags
 */
struct player_old_flags
{
	int flags;
	float curtime;
};

/*
 * player_prediction_data
 */
struct player_prediction_data
{
	void reset()
	{
		prediction_stage = 0, tickbase = 0;
		random_seed = 0;
		in_prediction = false;
		curtime = 0.f, frametime = 0.f;
		prediction_random_seed = nullptr;
	}

	int prediction_stage = 0, tickbase = 0;
	unsigned random_seed = 0;
	bool in_prediction = false;
	float curtime = 0.f, frametime = 0.f;
	unsigned* prediction_random_seed = nullptr;
	player_old_flags non_predicted;
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

    int LastBacktrackTick = -1;

    bool LCbroken = false;

    HitmarkerInfoStruct HitmarkerInfo;
	HMODULE csgo;

	std::vector<DamageIndicator> DamageIndicators;
	std::vector<StepInfoStruct> StepInfo;
	std::vector<MovotovInfoStruct> MolotovInfo;
	std::vector<SmokeInfoStruct> SmokeInfo;

	bool RadioPaused;

    RbotShotInfoStruct RbotShotInfo;

	player_prediction_data PredictionData;

    Vector LastShotEyePos = Vector(0, 0, 0);

	CCSGOPlayerAnimState AnimState;

	float DesyncYaw = 0.f;
	float RealYaw = 0.f;

    bool RbotDidLastShot = false;
};

extern RuntimeSaver g_Saver;