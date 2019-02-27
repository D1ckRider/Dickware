#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../Deps/lodepng.h"
#include "../RuntimeSaver.h"

std::uint8_t* load_image(unsigned short id, int width, int height, const std::string &type = "PNG")
{
	// csgo->module is the module you get from creating a thread in DLLMain
	// static DWORD __stdcall thread( const LPVOID parameter )
	// csgo->module = HMODULE( parameter )
	std::wstring stemp = std::wstring(type.begin(), type.end());
	LPCWSTR sw = stemp.c_str();

	auto resource = FindResource(g_Saver.csgo, MAKEINTRESOURCE(id), sw);
	auto loaded_resource = LoadResource(g_Saver.csgo, resource);
	auto resource_ptr = LockResource(loaded_resource);
	auto resource_size = SizeofResource(g_Saver.csgo, resource);

	std::vector< std::uint8_t > image;

	auto w = std::uint32_t(width);
	auto h = std::uint32_t(height);

	if (const auto error = lodepng::decode(image, w, h, (unsigned char*)resource_ptr, resource_size))
		exit(0);

	const auto data = new std::uint8_t[image.size()];
	std::copy(image.begin(), image.end(), data);

	return data;
}

class C_Texture
{
	// raw image data
	unsigned char *data_{ };

	// ISurface texture id
	unsigned int texture_id_{ 0 };

	// size
	int width_{ };
	int height_{ };

public:
	C_Texture() = default;

	// create texture
	C_Texture(unsigned short id, int width, int height, const std::string &type = "PNG")
		: data_{ load_image(id, width, height, type) }
		, width_(width)
		, height_(height)
	{ }

	// draw texture
	void paint(int x, int y)
	{
		if (!g_VGuiSurface->IsTextureIDValid(texture_id_))
		{
			texture_id_ = g_VGuiSurface->CreateNewTextureID(true);
			g_VGuiSurface->DrawSetTextureRGBA(texture_id_, data_, uint32_t(width_), uint32_t(height_));
		}

		g_VGuiSurface->DrawSetColor(255, 255, 255, 255);
		g_VGuiSurface->DrawSetTexture(texture_id_);
		g_VGuiSurface->DrawTexturedRect(x, y, x + width_, y + height_);
	}
};

