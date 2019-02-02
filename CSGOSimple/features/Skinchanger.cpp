#include "Skinchanger.h"
#include <filesystem>
#include "..\Logger.h"

std::unordered_map<int, EconomyItemCfg>  g_SkinchangerCfg;
std::unordered_map<int, const char*> g_ViewModelCfg;
std::unordered_map<const char*, const char*> g_KillIconCfg;



namespace fs = std::filesystem;

void Skinchanger::OnFrameStageNotify()
{

	// Don't change anything if we're not alive.
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	// Get handles to weapons we're carrying.
	CHandle<C_BaseCombatWeapon>* hWeapons = g_LocalPlayer->m_hMyWeapons();

	if (!hWeapons)
		return;

	// Loop through weapons and run our skin function on them.
	for (int nIndex = 0; hWeapons[nIndex]; nIndex++)
	{
		C_BaseAttributableItem* pWeapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(hWeapons[nIndex]));

		if (!pWeapon)
			continue;

		if (g_ViewModelCfg.find(pWeapon->m_nModelIndex()) != g_ViewModelCfg.end())
			pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(g_ViewModelCfg[pWeapon->m_nModelIndex()]);

		ApplyCustomModel(g_LocalPlayer, pWeapon);

		ApplyCustomSkin(hWeapons[nIndex]);
	}
}

void Skinchanger::LoadAllSkinsForWeapon()
{
	DynSkin::Skins::CSkins* pSkins = new DynSkin::Skins::CSkins();
	if (pSkins->Load("C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\csgo", "csgo"))
	{
		for (auto& s : pSkins->GetSkinInfoByWeapon("weapon_deagle"))
			g_Logger.Info("SKINS", s._name.data());
	}

	delete pSkins;
}

void Skinchanger::SaveCustomSkins()
{
	/*std::string fileName = std::string(getenv("APPDATA")) + "\\DickwareBeta\\skins";

	std::ofstream o(fileName);
	json j;
	
	for (auto it = g_SkinchangerCfg.begin; it != g_SkinchangerCfg.end(); ++it)
	{
		try
		{
			j["weapon_id" + std::to_string(it->first)] = it->second;
		}
		catch (json::exception ex)
		{

		}
	}
	o << std::setw(4) << j << std::endl;*/
}

void Skinchanger::LoadCustomSkins()
{
	/*std::string fileName = std::string(getenv("APPDATA")) + "\\DickwareBeta\\skins";

	std::ifstream i(fileName);

	json j;

	
	*/
	
}

bool Skinchanger::ApplyCustomSkin(CHandle<C_BaseCombatWeapon> &WeaponHandle)
{
	C_BaseAttributableItem* pWeapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(WeaponHandle));

	if (!pWeapon)
		return false;

	int nWeaponIndex = pWeapon->m_iItemDefinitionIndex();

	// Check if this weapon has a valid override defined.
	if (g_SkinchangerCfg.find(nWeaponIndex) == g_SkinchangerCfg.end())
		return false;

	// Apply our changes to the fallback variables.
	pWeapon->m_nFallbackPaintKit() = g_SkinchangerCfg[nWeaponIndex].nFallbackPaintKit;
	//pWeapon->GetEntityQuality() = g_SkinchangerCfg[nWeaponIndex].iEntityQuality;
	pWeapon->m_nFallbackSeed() = g_SkinchangerCfg[nWeaponIndex].nFallbackSeed;
	pWeapon->m_nFallbackStatTrak() = g_SkinchangerCfg[nWeaponIndex].nFallbackStatTrak;
	pWeapon->m_flFallbackWear() = g_SkinchangerCfg[nWeaponIndex].flFallbackWear;

	// If a name is defined, write it now.
	if (g_SkinchangerCfg[nWeaponIndex].szCustomName)
		sprintf_s(pWeapon->m_szCustomName(), 32, "%s", g_SkinchangerCfg[nWeaponIndex].szCustomName);


	// Edit "m_iItemIDHigh" so fallback values will be used.
	pWeapon->m_iItemIDHigh() = -1;

	return true;
}

bool Skinchanger::ApplyCustomModel(C_BasePlayer * player, C_BaseAttributableItem * item)
{
	// Get the view model of this weapon.
	C_BaseViewModel* pViewModel = g_LocalPlayer->m_hViewModel();

	if (!pViewModel)
		return false;

	// Get the weapon belonging to this view model.
	CHandle<C_BaseCombatWeapon*> hViewModelWeapon = pViewModel->m_hWeapon();
	C_BaseAttributableItem* pViewModelWeapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(hViewModelWeapon));

	if (pViewModelWeapon != item)
		return false;

	// Check if an override exists for this view model.
	int nViewModelIndex = pViewModel->m_nViewModelIndex();

	if (g_ViewModelCfg.find(nViewModelIndex) == g_ViewModelCfg.end())
		return false;

	// Set the replacement model.
	pViewModel->m_nModelIndex() = g_MdlInfo->GetModelIndex(g_ViewModelCfg[nViewModelIndex]);

	return true;
}
