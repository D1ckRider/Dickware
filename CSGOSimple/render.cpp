
#include "render.hpp"

#include <mutex>

#include "features/visuals.hpp"
#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "menu_helpers.hpp"
#include "options.hpp"
#include "droid.hpp"
#include "Settings.h"
#include "helpers/math.hpp"
#include <ctime>
#include <chrono>

ImFont* g_pDefaultFont;
ImFont* g_pC4Font;
ImFont* g_pIconFont;
ImFont* g_ManualAAFont;


//CRITICAL_SECTION render_cs;

Render::Render()
{
    //InitializeCriticalSection(&render_cs);
}

ImDrawListSharedData _data;

std::mutex render_mutex;


int GetFPS()
{
	static int fps = 0;
	static int count = 0;
	using namespace std::chrono;
	auto now = high_resolution_clock::now();
	static auto last = high_resolution_clock::now();
	count++;

	if (duration_cast<milliseconds> (now - last).count() > 1000)
	{
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

void Render::DrawWatermark()
{
	// Watermark
	int x, y;
	std::time_t cur_sec = std::time(0);
	std::tm* now = std::localtime(&cur_sec);
	std::string watermark = "DickWare | fps: " + std::to_string(GetFPS()) + " | time: " + std::to_string(now->tm_hour) + ":" + std::to_string(now->tm_min) + ":" + std::to_string(now->tm_sec);
	g_EngineClient->GetScreenSize(x, y);
	ImVec2 t = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, watermark.data());

	Render::Get().RenderBoxFilled(x - 187, y / 2 - 354, x - 173 + t.x, y / 2 - 346 + t.y, Color(13, 9, 9, 50));
	//Render::Get().RenderBox(x - 185.f, y / 2 - 352.f, x - 175 + t.x, y / 2 - 348 + t.y, Color::White);
	Render::Get().RenderBoxFilled(x - 185, y / 2 - 352, x - 175 + t.x, y / 2 - 348 + t.y, Color(13, 9, 9, 150));
	Render::Get().RenderTextNoOutline(watermark.data(), ImVec2(x - 180, y / 2 - 350.f), 12.f, Color::White);
}

void Render::Initialize()
{
    ImGui::CreateContext();

    ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);

    _data = ImDrawListSharedData();

    draw_list = new ImDrawList(&_data);
    draw_list_act = new ImDrawList(&_data);
    draw_list_rendering = new ImDrawList(&_data);

    GetFonts();
}

void Render::GetFonts()
{
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 14.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

    g_ManualAAFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 42.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
    g_pDefaultFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 18.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
    g_pC4Font = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 32.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

    g_pIconFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Droid_compressed_data, Droid_compressed_size, 18.f);
}

void Render::ClearDrawList()
{
    render_mutex.lock();
    draw_list_act->Clear();
    render_mutex.unlock();
}

void Render::BeginScene()
{
    draw_list->Clear();
    draw_list->PushClipRectFullScreen();

	if(Settings::Misc::WatermarkEnabled)
		DrawWatermark();
    Visuals::Get().AddToDrawList();

	if (g_EngineClient->IsInGame() && g_LocalPlayer && Settings::Aimbot::DrawFov)
		Visuals::Get().DrawFOV();

	if (g_EngineClient->IsInGame() && g_LocalPlayer && Settings::Aimbot::LegitAA > 0)
	{
		int PosDesyncY;

		//if (g_Options.misc_watermark)
			//PosDesyncY = 25;
		//else PosDesyncY = 5;
		PosDesyncY = 5;

		Render::Get().RenderText(Settings::Aimbot::AaSide > 0.0f ? "LEFT" : "RIGHT", 8, PosDesyncY, 18.0f, Color::White);
	}

    render_mutex.lock();
    *draw_list_act = *draw_list;
    render_mutex.unlock();
}

ImDrawList* Render::RenderScene()
{

    if (render_mutex.try_lock())
    {
        *draw_list_rendering = *draw_list_act;
        render_mutex.unlock();
    }

    return draw_list_rendering;
    /*
    if (!draw_list_rendering->VtxBuffer.empty()) {
    draw_data.Valid = true;
    draw_data.CmdLists = &draw_list_rendering;
    draw_data.CmdListsCount = 1;
    draw_data.TotalVtxCount = draw_list_rendering->VtxBuffer.Size;
    draw_data.TotalIdxCount = draw_list_rendering->IdxBuffer.Size;
    }

    if (draw_data.Valid)
    ImGui_ImplDX9_RenderDrawData(&draw_data);*/
}


float Render::RenderText(const std::string& text, const ImVec2& pos, float size, Color color, bool center, ImFont* pFont)
{
    ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());

    if (!pFont->ContainerAtlas)
    {
        return 0.f;
    }

    draw_list->PushTextureID(pFont->ContainerAtlas->TexID);

    if (center)
    {
        draw_list->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y) + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
        draw_list->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y) - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
        draw_list->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y) - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
        draw_list->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y) + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());

        draw_list->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y), GetU32(color), text.c_str());
    }
    else
    {
        draw_list->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y) + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
        draw_list->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y) - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
        draw_list->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y) - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
        draw_list->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y) + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());

        draw_list->AddText(pFont, size, ImVec2(pos.x, pos.y), GetU32(color), text.c_str());
    }

    draw_list->PopTextureID();

    return pos.y + textSize.y;
}

float Render::RenderTextNoOutline(const std::string& text, const ImVec2& pos, float size, Color color, bool center, ImFont* pFont)
{
    ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());

    if (!pFont->ContainerAtlas)
    {
        return 0.f;
    }

    draw_list->PushTextureID(pFont->ContainerAtlas->TexID);

    if (center)
    {
        draw_list->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y), GetU32(color), text.c_str());
    }
    else
    {
        draw_list->AddText(pFont, size, ImVec2(pos.x, pos.y), GetU32(color), text.c_str());
    }

    draw_list->PopTextureID();

    return pos.y + textSize.y;
}

void Render::RenderCircle3D(Vector position, float points, float radius, Color color)
{
    float step = (float)M_PI * 2.0f / points;

    for (float a = 0; a < (M_PI * 2.0f); a += step)
    {
        Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
        Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

        Vector start2d, end2d;
        if (g_DebugOverlay->ScreenPosition(start, start2d) || g_DebugOverlay->ScreenPosition(end, end2d))
        {
            return;
        }

        RenderLine(start2d.x, start2d.y, end2d.x, end2d.y, color);
    }
}
