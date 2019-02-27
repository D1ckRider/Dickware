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
    beamInfo.m_flLife = 2.5f;
    beamInfo.m_flWidth = 2.5f;
    beamInfo.m_flEndWidth = 2.5f;
    beamInfo.m_flFadeLength = 0.0f;
    beamInfo.m_flAmplitude = 3.0f;
    beamInfo.m_flBrightness = clr.a();
    beamInfo.m_flSpeed = 0.3f;
    beamInfo.m_nStartFrame = 0;
    beamInfo.m_flFrameRate = 0.f;
    beamInfo.m_flRed = clr.r();
    beamInfo.m_flGreen = clr.g();
    beamInfo.m_flBlue = clr.b();
    beamInfo.m_nSegments = 2;
    beamInfo.m_bRenderable = true;
    beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

    beamInfo.m_vecStart = shot_pos - Vector(0, -0.1f, -0.1f);
    beamInfo.m_vecEnd = hit_pos;

    auto beam = g_RenderBeams->CreateBeamPoints(beamInfo);
    if (beam)
        g_RenderBeams->DrawBeam(beam);
}
