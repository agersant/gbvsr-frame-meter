#include <DynamicOutput/DynamicOutput.hpp>

#include "meter.h"

using namespace RC;

void Page::clear()
{
	player_1.fill(CharacterState::IDLE);
	player_2.fill(CharacterState::IDLE);
	num_frames = 0;
}

void FrameMeter::update(AREDGameState_Battle *battle)
{
	Output::send<LogLevel::Warning>(STR("p1: {}, p2: {}\n"), (void *)battle->engine->player_1.entity, (void *)battle->engine->player_2.entity);

	CharacterState p1 = get_player_state(battle->engine->player_1.entity);
	CharacterState p2 = get_player_state(battle->engine->player_2.entity);

	if (p1 == CharacterState::IDLE && p2 == CharacterState::IDLE)
	{
		pending_reset = true;
	}
	else
	{
		if (pending_reset)
		{
			current_page.clear();
			previous_page.reset();
			pending_reset = false;
		}
		else if (current_page.num_frames == Page::SIZE)
		{
			previous_page = current_page;
			current_page.clear();
		}

		current_page.player_1[current_page.num_frames] = p1;
		current_page.player_2[current_page.num_frames] = p2;
		current_page.num_frames += 1;
	}
}

CharacterState FrameMeter::get_player_state(ASW::Entity *entity)
{
	if (entity->has_active_hitboxes())
	{
		return CharacterState::ACTIVE_HITBOX;
	}
	return CharacterState::IDLE;
}
