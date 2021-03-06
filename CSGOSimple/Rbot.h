
#include "singleton.hpp"
#include "valve_sdk/csgostructs.hpp"

struct PointScanStruct
{
    Vector pos;
    bool center = false;
};

struct CanHitStruct
{
    float damage = 0.f;
    bool CanHit = false;
    Vector pos = Vector ( 0, 0, 0 );
};

enum class BaimMode : int
{
    NONE,
    BAIM,
    FORCE_BAIM
};

struct RbotMatrixData
{
    matrix3x4_t matrix[MAXSTUDIOBONES];
    bool gotMatrix = false;
    studiohdr_t* StudioHdr;
    mstudiohitboxset_t* StudioSet;
};

#pragma once
class Rbot : public Singleton<Rbot>
{
public:
    void CreateMove ( CUserCmd* cmd, bool& bSendPacket );
	int GetTickbase(CUserCmd* cmd = nullptr);
    void OnFireEvent ( IGameEvent* event );
    bool GetBestHitboxPoint ( C_BasePlayer* entity, float& damage, Vector& hitbox, BaimMode baim, bool& WillKill, matrix3x4_t matrix[MAXSTUDIOBONES] = nullptr, mstudiohitboxset_t* StudioSet = nullptr, bool NoPointscale = false );
    void PrecacheShit();
    bool HitChance ( QAngle angles, C_BasePlayer* ent, float chance );
	bool HitChance(QAngle angles, C_BasePlayer* ent);
	void AccuracyBoost(CUserCmd* cmd);
	BaimMode* GetBAimStatus();
private:
	void UpdateWeaponConfig(C_BaseCombatWeapon* weapon);
    bool InFakeLag ( C_BasePlayer* player );
    float Simtimes[128];


	// Gladiatorz paste
	
	bool CheckTarget(int i);
	void TargetEntities(CUserCmd* cmd);
	bool TargetSpecificEnt(C_BasePlayer* pEnt);

	bool CockRevolver ( CUserCmd* cmd, C_BaseCombatWeapon* weapon );
	void FakeDuck	  ( CUserCmd* cmd, bool &bSendPackets);
	void FakeDuck	  ( CUserCmd* cmd );
    void AutoStop	  ( CUserCmd* cmd );
    void AutoCrouch   ( CUserCmd* cmd );
	void SlowWalk	  ( CUserCmd* cmd, float speed );
	void SlowWalk	  ( CUserCmd* cmd );

	
	
    CUserCmd* CurrentCmd = nullptr;
	C_BaseCombatWeapon* LocalWeapon = nullptr;
	int PrevAimtarget = NULL;
	bool CanFireWeapon = false;
	float CurTime = 0.f;


    void ZeusBot ( CUserCmd* cmd, C_BaseCombatWeapon* weapon );

    int LastRbotEnemyIndex = -1;
    //bool LastMissedDueToSpread = false;
    //bool HitChance(QAngle angles, C_BasePlayer *ent, float chance, TickRecord record);
    int FindBestEntity ( CUserCmd* cmd, C_BaseCombatWeapon* weapon, Vector& hitpos, bool& bBacktrack );
    std::vector<PointScanStruct> GetPointsForScan ( C_BasePlayer* entity, int hitbox, mstudiohitboxset_t* hitset, matrix3x4_t matrix[MAXSTUDIOBONES], float pointscale );

    //bool PrecachePointers(C_BasePlayer * entity);
    //matrix3x4_t PrecachedMatrix[128];
    //studiohdr_t* PrecachedHdr;
    //mstudiohitboxset_t *PrecachedSet;

    void UpdateConfigData();

	BaimMode baim;

	/* Configuration values */
    bool  HitboxHead          = false;
    bool  HitboxNeck          = false;
    bool  HitboxChest         = false;
    bool  HitboxPelvis        = false;
    bool  HitboxStomach       = false;
    bool  HitboxArm           = false;
    bool  HitboxLeg           = false;
    bool  HitboxFoot          = false;
	float Hitchance			 = 0.f;
	float MinDmg             = FLT_MAX;
	int   BAimAfter			 = 0;
	int   ForceBAimAfter	 = 0;
	bool  MovingBAim		 = false;
	
    float HitboxHeadScale    = 0.f;
    float HitboxNeckScale    = 0.f;
    float HitboxChestScale   = 0.f;
    float HitboxPelvisScale  = 0.f;
    float HitboxStomachScale = 0.f;
    float HitboxArmScale     = 0.f;
    float HitboxLegScale     = 0.f;
    float HitboxFootScale    = 0.f;
};

extern RbotMatrixData matrixData[128];