#pragma once

#include <string>
#include "valve_sdk/Misc/Color.hpp"
#include "helpers\KitParser.h"
#include "helpers\ItemDefinitions.h"
#include "json.hpp"

using json = nlohmann::json;

struct item_setting
{
	char name[32] = "Default";
	bool enabled = false;
	int definition_vector_index = 0;
	int definition_index = 1;
	int paint_kit_vector_index = 0;
	int paint_kit_index = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	int stat_trak = 0;
	float wear = 0.f; //std::numeric_limits<float>::min();
	char custom_name[32] = "";
};


class Options {

};

extern bool g_Unload;
extern bool g_IsAllowed;