
#include "valve_sdk/csgostructs.hpp"
#include "features/MaterialManager.hpp"

enum class ChamsModes : int
{
    NORMAL,
    FLAT,
    WIREFRAME,
    GLASS,
    METALLIC,
    XQZ,
    METALLIC_XQZ,
    FLAT_XQZ
};

#pragma once
class Chams : public Singleton<Chams>
{
public:
    void OnSceneEnd();
	void OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights,
		float* flpFlexDelayedWeights, Vector& vrModelOrigin, int iFlags);
};

