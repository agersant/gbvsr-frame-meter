#include <cassert>

#include "core/meter.h"

void FrameMeter::reset()
{
	players.fill(Player{});
	advantage = std::nullopt;
	pending_reset = false;
}

bool FrameMeter::is_at_rest() const
{
	return pending_reset;
}

bool FrameMeter::update(const Battle *battle)
{
	Character *character_1 = battle->teams[0].main_player_object;
	Character *character_2 = battle->teams[1].main_player_object;

	const bool is_cinematic_freeze = character_1->cinematic_freeze || character_2->cinematic_freeze;
	const bool is_slowdown_bonus_frame = character_1->slowdown_bonus_frame || character_2->slowdown_bonus_frame;
	const bool is_hitstop = character_1->hitstop > 0 && character_2->hitstop > 0;
	const bool skip_frame = is_cinematic_freeze || is_slowdown_bonus_frame || is_hitstop;
	if (skip_frame)
	{
		return false;
	}

	CharacterState state_1 = get_character_state(battle, character_1);
	CharacterState state_2 = get_character_state(battle, character_2);

	if (!continuous && state_1 == CharacterState::IDLE && state_2 == CharacterState::IDLE)
	{
		if (!pending_reset)
		{
			players[0].current_page.commit_span();
			players[1].current_page.commit_span();
			advantage = compute_advantage();
			pending_reset = true;
		}
		return false;
	}

	if (pending_reset)
	{
		reset();
	}
	else if (players[0].current_page.num_frames == PAGE_SIZE)
	{
		players[0].end_page();
		players[1].end_page();
	}

	// can_act() catches all situations except when defender presses a button on wake-up frame 1 and gets counter hit
	// -> attacking catches this counter hit situation but somehow also picks up every frame during throw tech
	// -> defense_hit_connecting distinguishes throw techs from the counter hit scenario
	const bool can_highlight_1 = character_1->can_act() || (character_1->attacking && character_1->defense_hit_connecting);
	const bool can_highlight_2 = character_2->can_act() || (character_2->attacking && character_2->defense_hit_connecting);

	players[0].add_frame(state_1, can_highlight_1);
	players[1].add_frame(state_2, can_highlight_2);
	advantage = std::nullopt;

	return true;
}

std::optional<int32_t> FrameMeter::compute_advantage() const
{
	const int32_t num_frames = players[0].current_page.num_frames;
	if (num_frames <= 0)
	{
		return std::nullopt;
	}

	int32_t p1_free_at;
	{
		const Frame &last_frame = players[0].current_page.frames[num_frames - 1];
		const bool is_idle = last_frame.state == CharacterState::IDLE;
		p1_free_at = is_idle ? last_frame.span_start_index : num_frames;
	}

	int32_t p2_free_at;
	{
		const Frame &last_frame = players[1].current_page.frames[num_frames - 1];
		const bool is_idle = last_frame.state == CharacterState::IDLE;
		p2_free_at = is_idle ? last_frame.span_start_index : num_frames;
	}

	return p2_free_at - p1_free_at;
}

CharacterState FrameMeter::get_character_state(const Battle *battle, Character *character)
{
	for (int i = 0; i < battle->num_entities; i++)
	{
		Entity *entity = battle->entities[i];
		if (!entity || entity == character || entity->parent_character != character)
		{
			continue;
		}
		if (!entity->attack_hit_connecting)
		{
			if (entity->recovery || !entity->active_frames || entity->num_hitboxes <= 0)
			{
				continue;
			}
		}
		return CharacterState::PROJECTILE;
	}

	if (character->can_act() && !character->defense_hit_connecting && !character->defense_guard_connecting)
	{
		return CharacterState::IDLE;
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

void Player::end_page()
{
	previous_page = current_page;
	for (int i = 0; i < PAGE_SIZE; i++)
	{
		previous_page->frames[i].highlight = false;
	}
	current_page.frames.fill(Frame{});
	current_page.num_frames = 0;
}

void Player::add_frame(CharacterState state, bool can_highlight)
{
	Frame *previous_frame = nullptr;
	if (current_page.num_frames > 0)
	{
		previous_frame = &current_page.frames[current_page.num_frames - 1];
	}
	else if (previous_page.has_value())
	{
		previous_frame = &previous_page->frames.back();
	}

	bool highlight_previous = can_highlight && state == CharacterState::STUN && previous_frame && previous_frame->state == CharacterState::STUN;
	if (highlight_previous)
	{
		previous_frame->highlight = true;
	}

	const bool is_new_span = !previous_frame || previous_frame->state != state || highlight_previous;
	if (is_new_span)
	{
		current_page.commit_span();
	}

	int32_t span_start;
	if (is_new_span)
	{
		span_start = current_page.num_frames;
	}
	else if (current_page.num_frames > 0)
	{
		span_start = previous_frame->span_start_index;
	}
	else
	{
		span_start = previous_frame->span_start_index - PAGE_SIZE;
	}

	assert(current_page.num_frames < PAGE_SIZE);
	current_page.add_frame(Frame{
		.state = state,
		.span_start_index = span_start,
		.highlight = false,
	});
}

void Page::add_frame(const Frame &frame)
{
	frames[num_frames] = frame;
	num_frames += 1;
}

void Page::commit_span()
{
	if (num_frames <= 0)
	{
		return;
	}
	const int32_t first = frames[num_frames - 1].span_start_index;
	const int32_t last = num_frames - 1;
	for (size_t i = std::max(0, first); i <= last; i++)
	{
		frames[i].span_length = 1 + last - first;
	}
}
