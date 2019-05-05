#include "ClantagChanger.h"
#include "Settings.h"

using CType = Settings::Misc::ClantagsType;

std::vector<std::string> dicks = 
{
	":() C===3",
	":()C===3",
	":(===3",
	":(==3",
	":(=3",
	":(3",
	":(=3",
	":(==3",
	":(===3"
};

std::string custom_clantag;

void ClantagChanger::DynamicClantag()
{
	static bool TriggerSwitch = false;
	static bool SwitchTag = false;
	static std::string name = " DickWare ";

	//static int OrgTextLength = text.size();
	//static int CurrentTextLength = OrgTextLength;

	static float LastChangeTime = 0.f;
	static float TimeToChangeTag = 0.f;

	if (g_GlobalVars->realtime - LastChangeTime < 0.5f)
		return;

	LastChangeTime = g_GlobalVars->realtime;

	if (!TriggerSwitch)
	{
		TimeToChangeTag = g_GlobalVars->realtime;
		TriggerSwitch = true;
	}

	if (g_GlobalVars->realtime - TimeToChangeTag > 10.f)
	{
		TriggerSwitch = false;
		SwitchTag = !SwitchTag;
	}
	else
	{
		if (SwitchTag)
		{
			Marquee(name);
			Utils::SetClantag(name.data());
		}
		else
		{
			std::rotate(dicks.begin(), dicks.begin() + 1, dicks.end());
			Utils::SetClantag(dicks[0].data());
		}
	}
}

void ClantagChanger::CustomSilde()
{
	static float LastChangeTime = 0.f;
	static float TimeToChangeTag = 0.f;

	if (g_GlobalVars->realtime - LastChangeTime < 0.5f)
		return;

	LastChangeTime = g_GlobalVars->realtime;
	Marquee(custom_clantag);

	Utils::SetClantag(custom_clantag.data());
}

void ClantagChanger::Marquee(std::string& clantag)
{
    std::string temp = clantag;
    clantag.erase(0, 1);
    clantag += temp[0];
}

void ClantagChanger::OnCreateMove()
{
    if (!g_LocalPlayer)
        return;

	/*static bool TriggerSwitch = false;
	static bool SwitchTag = false;
	static std::string name = " DickWare ";

    //static int OrgTextLength = text.size();
    //static int CurrentTextLength = OrgTextLength;

    static float LastChangeTime = 0.f;
	static float TimeToChangeTag = 0.f;

    if ( g_GlobalVars->realtime - LastChangeTime < 0.5f )
        return;
	
	LastChangeTime = g_GlobalVars->realtime;

	if ( !TriggerSwitch )
	{
		TimeToChangeTag = g_GlobalVars->realtime;
		TriggerSwitch = true;
	}

	if ( g_GlobalVars->realtime - TimeToChangeTag > 10.f )
	{
		TriggerSwitch = false;
		SwitchTag = !SwitchTag;
	}
	else
	{
		if ( SwitchTag )
		{
			Marquee(name);
			Utils::SetClantag(name.data());
		}
		else
		{
			std::rotate(dicks.begin(), dicks.begin() + 1, dicks.end());
			Utils::SetClantag(dicks[0].data());
		}
	}*/

	switch (Settings::Misc::ClantagType)
	{
	case CType::None:
		return;
	case CType::Static:
		Utils::SetClantag("DickWare");
		break;
	case CType::Dynamic:
		DynamicClantag();
		break;
	case CType::CustomSlide:
		CustomSilde();
		break;
	default:
		break;
	}
}

void ClantagChanger::SetCustomSlide(std::string clantag)
{
	custom_clantag = clantag;
}
