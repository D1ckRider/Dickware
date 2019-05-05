
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
	void DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
};

