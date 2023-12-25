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
	ASW::Entity *player_1 = battle->engine->player_1.entity;
	ASW::Entity *player_2 = battle->engine->player_2.entity;

	if (player_1->hitstop > 0 || player_2->hitstop > 0)
	{
		if (in_hitstop)
		{
			return;
		}
		in_hitstop = true;
	}
	else
	{
		in_hitstop = false;
	}

	CharacterState state_1 = get_player_state(player_1);
	CharacterState state_2 = get_player_state(player_2);

	if (state_1 == CharacterState::IDLE && state_1 == CharacterState::IDLE)
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

		current_page.player_1[current_page.num_frames] = state_1;
		current_page.player_2[current_page.num_frames] = state_2;
		current_page.num_frames += 1;
	}
}

CharacterState FrameMeter::get_player_state(ASW::Entity *entity)
{
	if (entity->attacking)
	{
		if (entity->recovery)
		{
			return CharacterState::PUNISH_COUNTER;
		}
		else if (entity->active_frames && entity->num_hitboxes > 0)
		{
			return CharacterState::ACTIVE_HITBOX;
		}
		else
		{
			return CharacterState::COUNTER;
		}
	}

	return CharacterState::IDLE;
}
