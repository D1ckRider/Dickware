
#include "BuyBot.h"
#include "ConfigSystem.h"
#include "Logger.h"
#include "ConsoleHelper.h"
#include "Settings.h"

void BuyBot::OnRoundStart()
{
    //static float LastBuy = 0.f;
    //if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() || !g_Config.GetBool("misc_buybot"))
	if (!g_LocalPlayer || !g_EngineClient->IsConnected() || !g_EngineClient->IsInGame() || !Settings::Misc::BuyBot)
        return;

    LastRoundStartTime = g_GlobalVars->curtime;
    ShouldBuy = true;
}

void BuyBot::OnCreateMove()
{
    if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || !ShouldBuy)
        return;
    ShouldBuy = false;

    std::string buycommand = "";
	BuyBotHvHWeapons weapon = (BuyBotHvHWeapons)Settings::Misc::BuyBotWeapon;
    switch (weapon)
    {
        case BuyBotHvHWeapons::SG_AUG:
            buycommand += "buy sg556; buy aug; ";
            break;
        case BuyBotHvHWeapons::SSG:
            buycommand += "buy ssg08; ";
            break;
        case BuyBotHvHWeapons::AUTO:
            buycommand += "buy scar20; buy g3sg1; ";
            break;
        case BuyBotHvHWeapons::MAC_10:
            buycommand += "buy mac10; ";
            break;
        case BuyBotHvHWeapons::P90:
            buycommand += "buy p90; ";
            break;
        case BuyBotHvHWeapons::PP_BIZON:
            buycommand += "buy bizon; ";
            break;
        case BuyBotHvHWeapons::AK_47:
            buycommand += "buy ak47; buy m4a4; buy m4a1_silencer; ";
            break;
        case BuyBotHvHWeapons::AWP:
            buycommand += "buy awp; ";
            break;
    }

	BuyBotPistols pistol = (BuyBotPistols)Settings::Misc::BuyBotPistol;
    switch (pistol)
    {
        case BuyBotPistols::GLOCK_18__USP_S__P2000:
            buycommand += "buy glock; buy usp_silencer; buy hkp2000; ";
            break;
        case BuyBotPistols::DUAL_BERETTAS:
            buycommand += "buy elite; ";
            break;
        case BuyBotPistols::P250: //p250
            buycommand += "buy p250; ";
            break;
        case BuyBotPistols::TEC_9__FIVE_SEVEN:
            buycommand += "buy tec9; buy fiveseven; ";
            break;
        case BuyBotPistols::DESERT_EAGLE__R8:
            buycommand += "buy deagle; buy revolver; ";
            break;
    }
	/* Nades */
    if (g_Config.GetBool("misc_buybot_grenade_molotov"))
        buycommand += "buy molotov; ";

    if (g_Config.GetBool("misc_buybot_grenade_grenade"))
        buycommand += "buy hegrenade; ";

    if (g_Config.GetBool("misc_buybot_grenade_smoke"))
            buycommand += "buy smokegrenade; ";

    if (g_Config.GetBool("misc_buybot_grenade_flash"))
        buycommand += "buy flashbang; ";
    if (g_Config.GetBool("misc_buybot_grenade_decoy"))
        buycommand += "buy decoy; ";

	/* Etc */
	if ( Settings::Misc::BuyBotArmor )
        buycommand += "buy vest; buy vesthelm; ";

	if ( Settings::Misc::BuyBotZeus )
        buycommand += "buy taser; ";

	if ( Settings::Misc::BuyBotDefuser )
        buycommand += "buy defuser; ";

    g_Logger.Info("BUYBOT", buycommand.c_str());

    g_EngineClient->ExecuteClientCmd(buycommand.c_str());
}
