
#include "singleton.hpp"
#include "valve_sdk\csgostructs.hpp"

#pragma once
class ClantagChanger : public Singleton<ClantagChanger>
{
public:
    void OnCreateMove();
	void SetCustomSlide(std::string clantag);
private:
	void DynamicClantag();
	void CustomSilde();
    void Marquee(std::string& clantag);
};