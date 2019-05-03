#include "Radar.h"
#include "../Settings.h"
#include "../imgui/imgui.h"

void Radar::Render()
{
	if (Settings::Visual::GlobalESP.RadarType != 2)
		return;

	if (!g_EngineClient->IsConnected())
		return;

	float prevAlpha = ImGui::GetStyle().Alpha;

	bool radar = Settings::Visual::GlobalESP.RadarType == 2;
	float Alpha = 90; //Settings::Visual::GlobalESP.RadarAlpha; //144; //(float)Settings::Radar::rad_Alpha / 255.f;

	ImGui::GetStyle().Alpha = Alpha;


	if (ImGui::Begin("Radar", &radar, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
	{
		ImDrawList* Draw = ImGui::GetWindowDrawList();

		ImVec2 DrawPos = ImGui::GetCursorScreenPos();
		ImVec2 DrawSize = ImGui::GetContentRegionAvail();

		Draw->AddLine(
			ImVec2(DrawPos.x + DrawSize.x / 2.f, DrawPos.y),
			ImVec2(DrawPos.x + DrawSize.x / 2.f, DrawPos.y + DrawSize.y),
			ImColor(1.f, 1.f, 1.f, Alpha));

		Draw->AddLine(
			ImVec2(DrawPos.x, DrawPos.y + DrawSize.y / 2.f),
			ImVec2(DrawPos.x + DrawSize.x, DrawPos.y + DrawSize.y / 2.f),
			ImColor(1.f, 1.f, 1.f, Alpha));

		OnRenderPlayer();

		//if (Settings::Radar::rad_Sound)
			//OnRenderSounds();

		ImGui::End();
	}

	ImGui::GetStyle().Alpha = prevAlpha;
}

void Radar::RadarRange(float* x, float* y, float range)
{
	if (fabs((*x)) > range || fabs((*y)) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}

void Radar::CalcRadarPoint(Vector vOrigin, int& screenx, int& screeny)
{
	QAngle vAngle;

	g_EngineClient->GetViewAngles(vAngle);

	float dx = vOrigin.x - g_LocalPlayer->GetEyePos().x;
	float dy = vOrigin.y - g_LocalPlayer->GetEyePos().y;

	float fYaw = float(vAngle.yaw * (M_PI / 180.0));

	float fsin_yaw = sin(fYaw);
	float fminus_cos_yaw = -cos(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = Settings::Visual::GlobalESP.RadarRange; //(float)Settings::Radar::rad_Range;

	RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImGui::GetCursorScreenPos();
	ImVec2 DrawSize = ImGui::GetContentRegionAvail();

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}

void Radar::OnRenderPlayer()
{
	float Alpha = 140; //Settings::Visual::GlobalESP.RadarAlpha; //144.f; //(float)Settings::Radar::rad_Alpha / 255.f;

	for (size_t i = 0; i < g_EntityList->GetHighestEntityIndex(); ++i)
	{
		if (!g_EngineClient->IsInGame())
			continue;

		C_BasePlayer* player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i)); 

		if (!player || !player->IsPlayer() || player == g_LocalPlayer || player->IsDormant() ||!player->IsAlive() || !player->IsEnemy())
			continue;

		int screenx = 0, screeny = 0;
		float Color_R = 255;
		float Color_G = 255;
		float Color_B = 255;

		CalcRadarPoint(player->GetRenderOrigin(), screenx, screeny);

		ImDrawList* Draw = ImGui::GetWindowDrawList();

		Draw->AddRectFilled(ImVec2((float)screenx, (float)screeny),
			ImVec2((float)screenx + 5, (float)screeny + 5),
			ImColor(Color_R, Color_G, Color_B, Alpha));
	}
}

void Radar::OnRenderSounds()
{

}
