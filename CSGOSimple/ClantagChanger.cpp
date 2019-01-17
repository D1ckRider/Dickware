
#include "ClantagChanger.h"

void ClantagChanger::Marquee(std::string& clantag)
{
    std::string temp = clantag;
    clantag.erase(0, 1);
    clantag += temp[0];
}

void ClantagChanger::OnCreateMove()
{
    if (!g_LocalPlayer)
    {
        return;
    }
    static std::string text = " DickWare ";
    //static int OrgTextLength = text.size();
    //static int CurrentTextLength = OrgTextLength;

    static float LastChangeTime = 0.f;
    if (g_GlobalVars->realtime - LastChangeTime < 0.5f)
    {
        return;
    }

    LastChangeTime = g_GlobalVars->realtime;
    Marquee(text);

    Utils::SetClantag(text.data());
}
