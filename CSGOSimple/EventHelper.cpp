#include "EventHelper.h"
#include "helpers\math.hpp"

void EventHelper::listener::Start()
{
	if (!g_GameEvents->AddListener(this, "player_hurt", false))
		throw std::exception("Failed to register the event");
	if (!g_GameEvents->AddListener(this, "bullet_impact", false))
		throw std::exception("Failed to register the event");

	if (!g_GameEvents->AddListener(this, "item_purchase", false))
		throw std::exception("Failed to register the event");

	if (!g_GameEvents->AddListener(this, "player_footstep", false))
		throw std::exception("Failed to register the event");

	if (!g_GameEvents->AddListener(this, "inferno_startburn", false))
		throw std::exception("Failed to register the event");
	if (!g_GameEvents->AddListener(this, "inferno_expire", false))
		throw std::exception("Failed to register the event");

	if (!g_GameEvents->AddListener(this, "smokegrenade_detonate", false))
		throw std::exception("Failed to register the event");
	if (!g_GameEvents->AddListener(this, "smokegrenade_expired", false))
		throw std::exception("Failed to register the event");

	if (!g_GameEvents->AddListener(this, "round_end", false))
		throw std::exception("Failed to register the event");

	if (!g_GameEvents->AddListener(this, "player_footstep", false))
		throw std::exception("Failed to register the event");
	if (!g_GameEvents->AddListener(this, "player_jump", false))
		throw std::exception("Failed to register the event");

	if (!g_GameEvents->AddListener(this, "bomb_beginplant", false))
		throw std::exception("Failed to register the event");
	if (!g_GameEvents->AddListener(this, "bomb_begindefuse", false))
		throw std::exception("Failed to register the event");
	if (!g_GameEvents->AddListener(this, "round_prestart", false))
		throw std::exception("Failed to register the event");
	if (!g_GameEvents->AddListener(this, "round_freeze_end", false))
		throw std::exception("Failed to register the event");
}

void EventHelper::listener::Stop()
{
    g_GameEvents->RemoveListener(this);
}

void EventHelper::listener::FireGameEvent(IGameEvent* event)
{
    EventHelper::Get().on_fire_event(event);
}

int EventHelper::listener::GetEventDebugID(void)
{
    return EVENT_DEBUG_ID_INIT;
}

void EventHelper::ShotTracer(Vector shot_pos, Vector hit_pos)
{
	if (!g_LocalPlayer || !Settings::Visual::BulletTracers)
		return;

    Color clr = Color(Math::RandomInt(0, 255), Math::RandomInt(0, 255), Math::RandomInt(0, 255));

	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = "sprites/physbeam.vmt";
	beamInfo.m_nModelIndex = -1;
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = 0.8f;
	beamInfo.m_flWidth = 1.5f;
	beamInfo.m_flEndWidth = 1.5f;
	beamInfo.m_flFadeLength = 0.0f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = clr.r();
	beamInfo.m_flGreen = clr.g();
	beamInfo.m_flBlue = clr.b();
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

	beamInfo.m_vecStart = shot_pos; //g_LocalPlayer->GetEyePos();
	beamInfo.m_vecEnd = hit_pos; //current_impact.m_vecHitPos;

	auto beam = g_RenderBeams->CreateBeamPoints(beamInfo);
	if (beam)
		g_RenderBeams->DrawBeam(beam);

	//g_DebugOverlay->AddBoxOverlay(hit_pos, Vector(-3, -3, -3), Vector(3, 3, 3), QAngle(0, 0, 0), clr.r(), clr.g(), clr.b(), clr.a(), 0.8f);
}
