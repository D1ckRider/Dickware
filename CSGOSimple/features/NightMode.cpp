#include "NightMode.h"
#include <string>
#include "..\valve_sdk\csgostructs.hpp"
#include "..\Settings.h"

void NightMode::Apply(bool ForceUpdate)
{
	static bool perfomed = false, bLastSetting;

	static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
	sv_skyname->m_nFlags &= ~FCVAR_CHEAT; // something something dont force convars

	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
		return;

	// Add revert option
	if (!Active || ForceUpdate)
	{
		for (int i = 0; i < g_EntityList->GetMaxEntities(); i++)
		{
			C_BaseEntity* ent = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));

			if (ent)
			{
				if (ent->GetClientClass()->m_ClassID == ClassId::CEnvTonemapController)
				{
					sv_skyname->SetValue("sky_csgo_night02");
					CEnvTonemapController* tonemapper = static_cast<CEnvTonemapController*>(ent);
					SavedValues.AutoExposureMax = tonemapper->m_flCustomAutoExposureMax();
					SavedValues.AutoExposureMin = tonemapper->m_flCustomAutoExposureMin();
					tonemapper->m_bUseCustomAutoExposureMin() = 1;
					tonemapper->m_bUseCustomAutoExposureMax() = 1;
					tonemapper->m_flCustomAutoExposureMax() = 0.075f;
					tonemapper->m_flCustomAutoExposureMin() = 0.075f;
					Active = true;
				}
			}
		}
	}

	if (bLastSetting != Settings::Visual::NightMode)
		bLastSetting = Settings::Visual::NightMode;

	/*static bool bPerformed = false, bLastSetting;

	static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
	sv_skyname->m_nFlags &= ~FCVAR_CHEAT; // something something dont force convars

	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
		return;

	if (!bPerformed)
	{
		for (auto i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
		{
			static IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

			if (!pMaterial || pMaterial->IsErrorMaterial())
				continue;

			if ( strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp") )
			{
				if (bLastSetting)
				{
					sv_skyname->SetValue("sky_csgo_night02");
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false); // walls were translucent for me for some odd reason, probably p100 codenz :/
					pMaterial->ColorModulate(0.3, 0.3, 0.3);
				}
				else
				{
					sv_skyname->SetValue("vertigoblue_hdr"); // fixme: i was too lazy to backup old value
					pMaterial->ColorModulate(1.00, 1.00, 1.00);
				}
			}
		}
	}

	if (bLastSetting != Settings::bVisualNightmode)
	{
		bLastSetting = Settings::bVisualNightmode;

	}*/

}

void NightMode::Revert()
{
	if (Active)
	{
		static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
		sv_skyname->m_nFlags &= ~FCVAR_CHEAT; // something something dont force convars


		for (int i = 0; i < g_EntityList->GetMaxEntities(); i++)
		{
			C_BaseEntity* ent = static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(i));

			if (ent)
			{
				if (ent->GetClientClass()->m_ClassID == ClassId::CEnvTonemapController)
				{
					sv_skyname->SetValue(SavedValues.sv_skybox);
					CEnvTonemapController* tonemapper = static_cast<CEnvTonemapController*>(ent);
					tonemapper->m_bUseCustomAutoExposureMin() = 1;
					tonemapper->m_bUseCustomAutoExposureMax() = 1;
					tonemapper->m_flCustomAutoExposureMax() = SavedValues.AutoExposureMax;
					tonemapper->m_flCustomAutoExposureMin() = SavedValues.AutoExposureMin;
					Active = false;
				}
			}
		}
	}
}
