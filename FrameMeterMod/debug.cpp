#include <DynamicOutput/Output.hpp>

#include "debug.h"

using namespace RC;

static std::wstring bytes_to_string(void *bytes, size_t num_bytes)
{
	std::wstring out;
	out.reserve(2 + 3 * num_bytes);
	for (int i = 0; i < num_bytes; i++)
	{
		const uint8_t value = *((uint8_t *)(bytes) + i);
		if (value)
		{
			out.append(std::format(STR("{:02X}"), value));
		}
		else
		{
			out.append(STR("xx"));
		}
		if (i + 1 < num_bytes)
		{
			out.append(STR(" "));
		}
	}
	return out;
}

void print_battle_data(AREDGameState_Battle *battle)
{
	ASW::Character *character_1 = battle->engine->player_1.character;
	ASW::Character *character_2 = battle->engine->player_2.character;

	if (character_1->action_id != 0 || character_2->action_id != 0)
	{
		wchar_t action_1[64];
		wchar_t action_2[64];
		size_t r;
		mbstowcs_s(&r, action_1, 64, &character_1->action_name[0], 64);
		mbstowcs_s(&r, action_2, 64, &character_2->action_name[0], 64);

		Output::send<LogLevel::Warning>(
			STR("{} {:#02X} [{}] {:02}   ||   {} {:#02X} [{}] {:02}  {} {}\n"),
			(void *)character_1,
			(uint32_t)character_1->action_id,
			bytes_to_string(&character_1->flags_1, 16),
			character_1->hitstop,
			(void *)character_2,
			(uint32_t)character_2->action_id,
			bytes_to_string(&character_2->flags_1, 16),
			character_2->hitstop,
			action_1,
			action_2);
	}
}