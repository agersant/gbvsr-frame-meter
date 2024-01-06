#include "meter.h"

void FrameMeter::reset()
{
	previous_page.reset();
	current_page.clear();
	pending_reset = false;
}

void FrameMeter::update(AREDGameState_Battle *battle)
{
	ASW::Character *character_1 = battle->engine->player_1.character;
	ASW::Character *character_2 = battle->engine->player_2.character;

	const bool is_cinematic_freeze = character_1->cinematic_freeze || character_2->cinematic_freeze;
	const bool is_shared_hitstop = character_1->hitstop > 0 && character_2->hitstop > 0;
	const bool skip_frame = is_cinematic_freeze || is_shared_hitstop;
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
			current_page.players[0].commit_span();
			current_page.players[1].commit_span();
			pending_reset = true;
		}
	}
	else
	{
		if (pending_reset)
		{
			reset();
		}
		else if (current_page.players[0].num_frames == PAGE_SIZE)
		{
			previous_page = current_page;
			current_page.clear();
			const Frame &last_frame_1 = previous_page->players[0].frames.back();
			const Frame &last_frame_2 = previous_page->players[1].frames.back();
			current_page.players[0].prior_span = {last_frame_1.state, last_frame_1.span_start_index};
			current_page.players[1].prior_span = {last_frame_2.state, last_frame_2.span_start_index};
		}

		current_page.players[0].add_frame(state_1);
		current_page.players[1].add_frame(state_2);
	}
	assert(current_page.players[0].num_frames == current_page.players[1].num_frames);

	advantage = compute_advantage();
}

std::optional<int32_t> FrameMeter::compute_advantage() const
{
	const int32_t num_frames = current_page.players[0].num_frames;
	if (!pending_reset || num_frames <= 0)
	{
		return std::nullopt;
	}

	int32_t p1_free_at;
	{
		const Frame &last_frame = current_page.players[0].frames[num_frames - 1];
		const bool is_idle = last_frame.state == CharacterState::IDLE;
		p1_free_at = is_idle ? last_frame.span_start_index : num_frames;
	}

	int32_t p2_free_at;
	{
		const Frame &last_frame = current_page.players[1].frames[num_frames - 1];
		const bool is_idle = last_frame.state == CharacterState::IDLE;
		p2_free_at = is_idle ? last_frame.span_start_index : num_frames;
	}

	return p2_free_at - p1_free_at;
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
		const bool can_walk = character->can_walk();
		const bool is_mid_jump = character->action_id == ASW::ActionID::Jump;
		const bool is_mid_dash = character->action_id == ASW::ActionID::FDash;
		const bool can_use_normal = character->can_attack() && !is_mid_jump && !is_mid_dash;
		if (can_walk || can_use_normal)
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

void Page::clear()
{
	players.fill(Player{});
}

void Player::add_frame(CharacterState state)
{
	bool is_new_span;
	int32_t span_start;
	if (num_frames > 0)
	{
		const Frame &previous_frame = frames[num_frames - 1];
		is_new_span = state != previous_frame.state;
		span_start = is_new_span ? num_frames : previous_frame.span_start_index;
	}
	else if (prior_span.has_value() && state == prior_span->first)
	{
		is_new_span = false;
		span_start = prior_span->second - PAGE_SIZE;
	}
	else
	{
		is_new_span = true;
		span_start = 0;
	}

	if (is_new_span)
	{
		commit_span();
	}

	assert(num_frames < PAGE_SIZE);
	frames[num_frames] = Frame{.state = state, .span_start_index = span_start};

	num_frames += 1;
}

void Player::commit_span()
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
