#include "NightMode.h"
#include <string>

void NightMode::Apply()
{
	static bool bPerformed = false, bLastSetting;

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

	/*if (bLastSetting != Settings::bVisualNightmode)
	{
		bLastSetting = Settings::bVisualNightmode;

	}*/

}
