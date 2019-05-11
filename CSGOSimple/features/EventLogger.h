#pragma once
#include "../valve_sdk/sdk.hpp"
#include "../singleton.hpp"

#include <deque>

class EventLogger : public Singleton<EventLogger>
{
public:
	void PaintTraverse();
	void OnFireEvent(IGameEvent* event);
	void AddEvent(std::string pre, std::string text, Color color);
private:
	struct LogInfo
	{
		float LogTime;
		std::string Type;
		std::string Message;
		Color Color;
		float X, Y;
	};

	std::deque<LogInfo> Log;
};