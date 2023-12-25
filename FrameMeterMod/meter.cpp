#include <DynamicOutput/DynamicOutput.hpp>

#include "meter.h"

using namespace RC;

void FrameMeter::update(AREDGameState_Battle *battle)
{
	Output::send<LogLevel::Warning>(STR("p1: {}, p2: {}\n"), (void *)battle->engine->player_1.entity, (void *)battle->engine->player_2.entity);

	if (current_page.num_frames == Page::SIZE)
	{
		current_page.num_frames = 0;
	}

	current_page.player_1[current_page.num_frames] = CharacterState::IDLE;
	current_page.player_2[current_page.num_frames] = CharacterState::IDLE;
	current_page.num_frames += 1;
}
