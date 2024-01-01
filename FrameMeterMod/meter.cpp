#include <DynamicOutput/DynamicOutput.hpp>

#include "meter.h"

using namespace RC;

void Page::clear()
{
	player_1.fill(CharacterState::IDLE);
	player_2.fill(CharacterState::IDLE);
	num_frames = 0;
}

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

	const bool cinematic_freeze = character_1->cinematic_freeze || character_2->cinematic_freeze;
	const uint32_t hitstop = std::min(character_1->hitstop, character_2->hitstop);
	const bool skip_frame = cinematic_freeze || hitstop > 0;
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
	if (!character->hit_connecting && !character->guard_connecting)
	{
		if (character->can_walk() || (character->can_attack() && character->action_id != ASW::ActionID::Jump))
		{
			return CharacterState::IDLE;
		}
	}

	if (character->is_in_blockstun() || character->is_in_hitstun())
	{
		return CharacterState::STUN;
	}

	if (character->is_recovering())
	{
		return CharacterState::PUNISH_COUNTER;
	}

	if (character->is_in_active_frames())
	{
		return CharacterState::ACTIVE_HITBOX;
	}

	if (character->is_invincible())
	{
		return CharacterState::INVINCIBLE;
	}

	if (character->is_counterable())
	{
		return CharacterState::COUNTER;
	}

	if (character->is_maneuvering())
	{
		return CharacterState::MOVEMENT;
	}

	return CharacterState::IDLE;
}
