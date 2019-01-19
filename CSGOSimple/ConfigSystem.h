
#include "valve_sdk/csgostructs.hpp"
#include <map>
#include <string>
#include <deque>
#pragma once

struct SkinSaverData
{
    bool Enabled = false;
    int Model = 0;
    int m_nFallbackPaintKit = 0;
    int m_iEntityQuality = 0;
    float m_flFallbackWear = 0.f;
    std::string m_szCustomName = "";
};

class ConfigSystem
{
public:
    //get
    int GetInt ( std::string name );
    float GetFloat ( std::string name );
    bool GetBool ( std::string name );
    Color GetColor ( std::string name );
    //set
    void Set ( std::string name, int value );
    void Set ( std::string name, float value );
    void Set ( std::string name, bool value );
    void Set ( std::string name, Color value );
    void Set ( int i, SkinSaverData value );
    //save-load
    void Save ( std::string file );
    void Load ( std::string file );
    void RefreshConfigList();
    void CreateConfig ( std::string file );
    void ResetConfig()
    {
        Setup();
    };
    ConfigSystem()
    {
        Setup();
    };
    std::deque<std::string> Configs;
    std::string AppdataFolder = "";
    std::array<SkinSaverData, 33> skinOptions;
	std::string LoadGameConfig();
    void LoadSkins();
    void SaveSkins();
private:
    void Setup();
    void SetupVar ( std::string name, int value );
    void SetupVar ( std::string name, float value );
    void SetupVar ( std::string name, bool value );
    void SetupVar ( std::string name, Color value );
    std::unordered_map<std::string, int> intOptions; //prev map
    std::unordered_map<std::string, bool> boolOptions; //prev map
    std::unordered_map<std::string, float> floatOptions; //prev map
    std::unordered_map<std::string, Color> colorOptions; //prev map
};

extern ConfigSystem g_Config;