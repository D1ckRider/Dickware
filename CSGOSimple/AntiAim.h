
#include "singleton.hpp"
#include "valve_sdk/csgostructs.hpp"
#pragma once

enum class YawAntiAims : int
{
    NONE,
    BACKWARDS,
    SPINBOT,
    LOWER_BODY,
    RANDOM,
    FREESTANDING,
    CUSTOM
};

enum class YawAddAntiAims : int
{
    NONE,
    JITTER,
    SWITCH,
    SPIN,
    RANDOM
};

enum class PitchAntiAims : int
{
    NONE,
    EMOTION,
    DOWN,
    UP,
    ZERO,
    JITTER,
    DOWN_JITTER,
    UP_JITTER,
    ZERO_JITTER,
    SPIN,
    SPIN_UP,
    SPIN_DOWN,
    RANDOM,
    SWITCH,
    DOWN_SWITCH,
    UP_SWITCH,
    FAKE_UP,
    FAKE_DOWN,
    CUSTOM
};

class AntiAim : public Singleton<AntiAim>
{
public:
	// Ghetto code
	QAngle DesyncAngles;
	bool DesyncFlip;

    void OnCreateMove(CUserCmd* cmd, bool& bSendPacket);
    void LbyBreakerPrediction(CUserCmd* cmd, bool& bSendPacket);
    void ResetLbyPrediction();

    void Fakewalk(CUserCmd* cmd, bool& bSendPackets);

    bool GetEdgeDetectAngle(C_BasePlayer* entity, float& yaw);

    void SlideWalk(CUserCmd* cmd);
private:
    void DoAntiAim(CUserCmd* cmd, bool& bSendPacket);
    void Pitch(CUserCmd* cmd);
    void Yaw(CUserCmd* cmd, bool fake);
    void YawAdd(CUserCmd* cmd, bool fake);
    int GetFPS();
	int GetTickBase(CUserCmd* cmd);


    //freestanding
    float fov_player(Vector ViewOffSet, QAngle View, C_BasePlayer* entity);
    int GetNearestPlayerToCrosshair();
    bool Freestanding(C_BasePlayer* player, float& ang);
    bool FreestandingLbyBreak(float& ang);

    bool InLbyUpdate = false;
    bool NextTickInLbyUpdate = false;
	bool BreakLowerBody = false;
    float NextLbyUpdate = 0.f;
	float_t m_flSpawnTime = 0.f;

    CBaseHandle* m_ulEntHandle = nullptr;
	CCSGOPlayerAnimState* m_serverAnimState = nullptr;

    bool allocate = false, change = false, reset = false;

	int m_iRotate = 0;
	int m_iRotateIteration = 0;

	float m_flCurrentFeetYaw = 0.0f;
	float m_flPreviousFeetYaw = 0.0f;

	bool m_bAutomaticDir = false;
	int m_iAutoDirection = 0;
};