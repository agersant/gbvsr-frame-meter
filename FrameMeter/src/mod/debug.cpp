#include <DynamicOutput/Output.hpp>

#include "mod/debug.h"

using namespace RC;

#if UE_BUILD_TEST

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

void Debug::on_battle_reset(Battle *battle)
{
	Output::send<LogLevel::Warning>(
		STR("Battle Reset. Game Instance: {}, Battle: {}, P1: {}, P2: {}\n"),
		(void *)get_game_instance(),
		(void *)battle,
		(void *)battle->teams[0].main_player_object,
		(void *)battle->teams[1].main_player_object);
}

void Debug::on_battle_update(AREDGameState_Battle *game_state)
{
}

#endif
