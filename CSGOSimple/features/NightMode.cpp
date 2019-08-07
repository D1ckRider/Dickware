#include "NightMode.h"
#include <string>
#include "..\\RuntimeSaver.h"
#include "..\valve_sdk\csgostructs.hpp"
#include "..\Settings.h"

std::string fallback_skybox = "";

void NightMode::Apply(bool ForceUpdate)
{
	static bool perfomed = false, bLastSetting;
	bool optMode = false;
	static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
	sv_skyname->m_nFlags &= ~FCVAR_CHEAT; // something something dont force convars
	static ConVar* r_3dsky = g_CVar->FindVar("r_3dsky");

	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame())
	{
		Active = false;
		return;
	}
		
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
					fallback_skybox = sv_skyname->GetString();
					sv_skyname->SetValue("sky_csgo_night02");
					if (r_3dsky != nullptr)
					{
						if (r_3dsky->m_nFlags & FCVAR_HIDDEN)
							r_3dsky->m_nFlags &= ~FCVAR_HIDDEN;

						r_3dsky->SetValue(0);
					}

					CEnvTonemapController* tonemapper = static_cast<CEnvTonemapController*>(ent);
					tonemapper->m_bUseCustomAutoExposureMin() = 1;
					tonemapper->m_bUseCustomAutoExposureMax() = 1;
					tonemapper->m_flCustomAutoExposureMax() =   Settings::Visual::NightModeBrighthness;//0.08f;
					tonemapper->m_flCustomAutoExposureMin() =   Settings::Visual::NightModeBrighthness;//0.08f;
					Active = true;

				}
			}
		}
	}

	if (bLastSetting != Settings::Visual::NightMode)
		bLastSetting = Settings::Visual::NightMode;
}

void NightMode::Revert()
{
	if (Active)
	{
		static ConVar* sv_skyname = g_CVar->FindVar("sv_skyname");
		static ConVar* r_3dsky = g_CVar->FindVar("r_3dsky");
		sv_skyname->m_nFlags &= ~FCVAR_CHEAT; // something something dont force convars
		sv_skyname->SetValue(fallback_skybox.data());
		if (r_3dsky != nullptr)
		{
			r_3dsky->SetValue(0);
		}
		
		g_Saver.RequestForceUpdate = true;
		Active = false;
	}
}
