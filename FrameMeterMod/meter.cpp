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

	current_page.player_1[current_page.num_frames] = get_player_state(battle->engine->player_1.entity);
	current_page.player_2[current_page.num_frames] = get_player_state(battle->engine->player_2.entity);
	current_page.num_frames += 1;
}

CharacterState FrameMeter::get_player_state(ASW::Entity *entity)
{
	if (entity->has_active_hitboxes())
	{
		return CharacterState::ACTIVE_HITBOX;
	}
	return CharacterState::IDLE;
}
