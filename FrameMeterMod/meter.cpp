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
			STR("{} {:x} {} {:x} {:x} {:x}    ||    {} {:x} {} {:x} {:x} {:x}\n"),
			(void *)character_1,
			(uint32_t)character_1->action_id,
			action_1,
			character_1->flags_1,
			character_1->flags_2,
			character_1->flags_3,
			(void *)character_2,
			(uint32_t)character_2->action_id,
			action_2,
			character_2->flags_1,
			character_2->flags_2,
			character_2->flags_3);
	}

	// TODO Skip while game is paused
	const uint32_t hitstop = std::max(character_1->hitstop, character_2->hitstop);
	const bool skip_frame = hitstop > 0 && hitstop < previous_hitstop;
	previous_hitstop = hitstop;
	if (skip_frame)
	{
		return;
	}

	CharacterState state_1 = get_character_state(character_1);
	CharacterState state_2 = get_character_state(character_2);

	if (state_1 == CharacterState::IDLE && state_2 == CharacterState::IDLE)
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

CharacterState FrameMeter::get_character_state(ASW::Character *character)
{
	if (character->can_walk() || (character->can_attack() && character->action_id != ASW::ActionID::Jump))
	{
		return CharacterState::IDLE;
	}

	if (character->is_in_blockstun() || character->is_in_hitstun())
	{
		return CharacterState::STUN;
	}

	if (character->attacking)
	{
		if (character->recovery)
		{
			return CharacterState::PUNISH_COUNTER;
		}
		else if (character->active_frames && character->num_hitboxes > 0)
		{
			return CharacterState::ACTIVE_HITBOX;
		}
	}

	if (character->is_invincible())
	{
		return CharacterState::INVINCIBLE;
	}

	if (character->attacking)
	{
		return CharacterState::COUNTER;
	}

	if (character->is_maneuvering())
	{
		return CharacterState::MOVEMENT;
	}

	return CharacterState::IDLE;
}
