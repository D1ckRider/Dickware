#include "Settings.h"
#include <filesystem>
#include "Logger.h"

namespace fs = std::filesystem;


/* Configs Stuff */
std::deque<std::string> Settings::Configs;
std::string Settings::AppDataFolder = "";

void Settings::Initialize()
{
	//C:\Users\TOR Exit Node\AppData\Roaming
	AppDataFolder = std::string(getenv("APPDATA")) + "\\DickwareBeta\\";

	if (!fs::exists(AppDataFolder))
	{
		fs::create_directory(AppDataFolder);
		//MessageBox ( NULL, L"An important folder did not exist!", L"", 0 );
		//std::exit ( 1 );
	}

	if (!fs::exists(AppDataFolder + "configs\\"))
		fs::create_directory(AppDataFolder + "configs\\");

	RefreshConfigList();
}

void Settings::CreateConfig(std::string fileName)
{
	std::string orgfile = fileName;

	if (fs::exists(AppDataFolder + "configs\\" + fileName))
		return;

	std::ofstream ofs(AppDataFolder + "configs\\" + fileName);
	ofs << "";
	ofs.close();

	g_Logger.Success("CONFIG", "config " + orgfile + " created");

	RefreshConfigList();
}

void Settings::RefreshConfigList()
{
	Configs.clear();

	for (auto& p : fs::directory_iterator(AppDataFolder + "configs\\"))
	{
		fs::path path = fs::path(p);
		Configs.push_back(path.filename().string());
	}

	g_Logger.Info("CONFIG", "refreshed config list");
}

void Settings::SaveSettings(std::string fileName)
{
	std::string orgfile = fileName;
	fileName = AppDataFolder + "configs\\" + fileName;

	std::ofstream o(fileName);
	json j;

	/* Write aimbot settings */
	j["legitbot_enabled"] = Aimbot::Enabled;
	for ( int i = 0; i < 9; i++ )
	{
		j["legit_weapon " + std::to_string(i)]["FOV"] = Aimbot::WeaponAimSetting[i].FOV;
		j["legit_weapon " + std::to_string(i)]["Smooth"] = Aimbot::WeaponAimSetting[i].Smooth;
	}
	o << std::setw(4) << j << std::endl;
}

void Settings::LoadSettings(std::string fileName)
{
	std::string orgfile = fileName;
	fileName = AppDataFolder + "configs\\" + fileName;

	std::ifstream i(fileName);
	json j;
	i >> j;
	
	Aimbot::Enabled = j["legitbot_enabled"];
	for (int i = 0; i < 9; i++)
	{
		Aimbot::WeaponAimSetting[i].FOV = j["legit_weapon " + std::to_string(i)]["FOV"];
		Aimbot::WeaponAimSetting[i].Smooth = j["legit_weapon " + std::to_string(i)]["Smooth"];
	}
}


/* Init settings vars */
namespace Settings::Aimbot
{
	bool Enabled = false;
	WeaponSetting WeaponAimSetting[9] = { 0 };
	bool Backtrack = false;
	bool BacktrackAtAim = false;
	float BacktrackTick = false;
}