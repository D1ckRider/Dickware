#include "EventLogger.h"
#include "visuals.hpp"

void EventLogger::PaintTraverse()
{
	if (Log.empty())
		return;

	auto last_y = 6;
	
	for (size_t i = 0; i < Log.size(); i++)
	{
		auto& msg = Log.at(i);

		if (Utils::EpochTime() - msg.LogTime > 2700)
		{
			float factor = (msg.LogTime + 3100) - Utils::EpochTime();
			factor /= 1000;

			auto opacity = int(255 * factor);

			if (opacity < 2) 
			{
				Log.erase(Log.begin() + i);
				continue;
			}

			msg.Color.SetAlpha(opacity);

			msg.X += 1 * (factor * 1.25);
			msg.Y += 1 * (factor * 1.25);
		}

		const auto pre = msg.Type.c_str();
		const auto text = msg.Message.c_str();
		auto name_size = VGSHelper::Get().GetSize(msg.Type);
		
		VGSHelper::Get().DrawLogHeader(pre, msg.X, last_y + msg.Y, msg.Color);
		VGSHelper::Get().DrawLogBase(text, msg.X + 10 + name_size.x, last_y + msg.Y, Color::White);

		last_y += 14;
	}
}

void EventLogger::OnFireEvent(IGameEvent* event)
{
	static auto get_hitgroup_name = [](int hitgroup) -> std::string {
		switch (hitgroup) 
		{
		case HITBOX_HEAD:
			return "head";
		case HITBOX_LEFT_CALF:
			return "left leg";
		case HITBOX_RIGHT_CALF:
			return "right leg";
		case HITBOX_STOMACH:
			return "stomach";
		case HITBOX_LEFT_UPPER_ARM:
			return "left arm";
		case HITBOX_RIGHT_UPPER_ARM:
			return "right arm";
		default:
			return "body";
		}
	};

	constexpr char* hasdefusekit[2] = { "without defuse kit.","with defuse kit." };
	constexpr char* hasbomb[2] = { "without the bomb.","with the bomb." };
	constexpr char* hitgroups[7] = { "head" };

	if(strstr(event->GetName(), "player_hurt"))
	{
		auto
			userid = event->GetInt("userid"),
			attacker = event->GetInt("attacker");

		if (!userid || !attacker)
			return;

		auto
			userid_id = g_EngineClient->GetPlayerForUserID(userid), 
			attacker_id = g_EngineClient->GetPlayerForUserID(attacker);

		player_info_t userid_info, attacker_info;
		if(!g_EngineClient->GetPlayerInfo(userid_id, &userid_info))
			return;

		if(!g_EngineClient->GetPlayerInfo(attacker_id, &attacker_info))
			return;

		auto m_victim = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(userid_id));

		std::stringstream ss;
		if (attacker_id == g_EngineClient->GetLocalPlayer()) 
		{
			ss << "You hurt " << userid_info.szName << " in the " << get_hitgroup_name(event->GetInt("hitgroup")) << " for " << event->GetInt("dmg_health");
			ss << " ( " << Math::Clamp(m_victim->m_iHealth() - event->GetInt("dmg_health"), 0, 100) << " health remaining )";

			AddEvent("HIT", ss.str(), Color(255, 100, 0));
		}
		else if (userid_id == g_EngineClient->GetLocalPlayer()) 
		{
			ss << "You took " << event->GetInt("dmg_health") << " damage from " << attacker_info.szName << "in the " << get_hitgroup_name(event->GetInt("hitgroup"));
			ss << " ( " << Math::Clamp(m_victim->m_iHealth() - event->GetInt("dmg_health"), 0, 100) << " health remaining )";

			AddEvent("HIT", ss.str(), Color(255, 100, 0));
		}
	}

	if(strstr(event->GetName(), "item_purchase"))
	{
		auto userid = event->GetInt("userid");

		if (!userid)
			return;

		auto userid_id = g_EngineClient->GetPlayerForUserID(userid);

		player_info_t userid_info;
		if(!g_EngineClient->GetPlayerInfo(userid_id, &userid_info))
			return;

		auto m_player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(userid_id));

		if (!g_LocalPlayer || !m_player)
			return;

		if (m_player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			return;

		std::stringstream ss;
		ss << userid_info.szName << " purchased a(n) " << event->GetString("weapon");

		AddEvent("BUY", ss.str(), Color(0, 185, 255));
	}

	if(strstr(event->GetName(), "bomb_beginplant"))
	{
		auto userid = event->GetInt("userid");

		if (!userid)
			return;

		auto userid_id = g_EngineClient->GetPlayerForUserID(userid); 

		player_info_t userid_info;
		if(!g_EngineClient->GetPlayerInfo(userid_id, &userid_info))
			return;

		auto m_player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(userid_id)); 

		if (!m_player)
			return;

		std::stringstream ss;
		ss << userid_info.szName << " has began planting the bomb at site " << event->GetInt("site") << ".";

		AddEvent("INFO", ss.str(), Color(0, 185, 255));
	}

	if(strstr(event->GetName(), "bomb_begindefuse"))
	{
		auto userid = event->GetInt("userid");

		if (!userid)
			return;

		auto userid_id = g_EngineClient->GetPlayerForUserID(userid); 

		player_info_t userid_info;
		if (!g_EngineClient->GetPlayerInfo(userid_id, &userid_info))
			return;

		auto m_player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(userid_id));

		if (!m_player)
			return;

		std::stringstream ss;
		ss << userid_info.szName << " has began defusing the bomb " << hasdefusekit[event->GetBool("haskit")];

		AddEvent("INFO", ss.str(), Color(0, 185, 255));
	}
}

void EventLogger::AddEvent(std::string pre, std::string text, Color color)
{
	Log.push_front(LogInfo { static_cast<float>(Utils::EpochTime()), pre, text, color });
}
