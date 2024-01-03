#include <DynamicOutput/DynamicOutput.hpp>

#include "meter.h"

using namespace RC;

void Page::clear()
{
	players[0].fill(Frame{});
	players[1].fill(Frame{});
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

	CharacterState state_1 = get_character_state(battle, character_1);
	CharacterState state_2 = get_character_state(battle, character_2);

	if (state_1 == CharacterState::IDLE && state_2 == CharacterState::IDLE)
	{
		if (!pending_reset)
		{
			current_page.commit_span(current_page.players[0]);
			current_page.commit_span(current_page.players[1]);
			pending_reset = true;
		}
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

		current_page.add_frame(state_1, state_2);
	}
}

void Page::add_frame(CharacterState state_1, CharacterState state_2)
{
	assert(num_frames < Page::SIZE);
	add_player_frame(players[0], state_1);
	add_player_frame(players[1], state_2);
	num_frames += 1;
}

void Page::commit_span(std::array<Frame, SIZE> &player)
{
	if (num_frames <= 0)
	{
		return;
	}
	const int32_t first = player[num_frames - 1].span_start_index;
	const int32_t last = num_frames - 1;
	for (size_t i = std::max(0, first); i <= last; i++)
	{
		player[i].span_length = 1 + last - first;
	}
}

void Page::add_player_frame(std::array<Frame, SIZE> &player, CharacterState state)
{
	const bool is_new_span = num_frames == 0 || state != player[num_frames - 1].state;
	if (is_new_span)
	{
		commit_span(player);
	}

	const int32_t start_index = is_new_span ? num_frames : player[num_frames - 1].span_start_index;
	player[num_frames] = Frame{.state = state, .span_start_index = start_index};
}

CharacterState FrameMeter::get_character_state(AREDGameState_Battle *battle, ASW::Character *character)
{
	for (size_t i = 0; i < ASW::Engine::NUM_ENTITIES; i++)
	{
		ASW::Entity *entity = battle->engine->entities[i];
		if (!entity || entity == character || entity->parent_character != character)
		{
			continue;
		}
		if (!entity->active_frames || entity->num_hitboxes <= 0)
		{
			continue;
		}
		if (entity->recovery && !entity->attack_hit_connecting)
		{
			continue;
		}
		return CharacterState::PROJECTILE;
	}

	if (!character->defense_hit_connecting && !character->defense_guard_connecting)
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

	if (character->cinematic_attack)
	{
		return CharacterState::INVINCIBLE;
	}

	return CharacterState::IDLE;
}
