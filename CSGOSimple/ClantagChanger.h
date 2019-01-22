
#include "singleton.hpp"
#include "valve_sdk\csgostructs.hpp"

#pragma once
class ClantagChanger : public Singleton<ClantagChanger>
{
public:
    void OnCreateMove();
private:
    void Marquee(std::string& clantag);
};