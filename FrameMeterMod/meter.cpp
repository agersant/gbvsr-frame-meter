#include <algorithm>
#include <DynamicOutput/DynamicOutput.hpp>

#include "meter.h"
#include <windows.h>

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
		MultiByteToWideChar(CP_UTF8, 0, &character_1->action_name[0], -1, action_1, 64);
		MultiByteToWideChar(CP_UTF8, 0, &character_2->action_name[0], -1, action_2, 64);

		Output::send<LogLevel::Warning>(
			STR("actions {:x} {}, {:x} {}\n"),
			(uint32_t)character_1->action_id,
			action_1,
			(uint32_t)character_2->action_id,
			action_2
			);
	}

	const uint32_t hitstop = (std::max)(character_1->hitstop, character_2->hitstop);
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
	if (character->can_act())
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
		return CharacterState::COUNTER;
	}

	return CharacterState::IDLE;
}
