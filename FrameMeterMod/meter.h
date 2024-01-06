#pragma once

#include "game.h"

constexpr size_t PAGE_SIZE = 80;

enum CharacterState
{
	IDLE,
	ACTIVE_HITBOX,
	COUNTER,
	PUNISH_COUNTER,
	STUN,
	MOVEMENT,
	INVINCIBLE,
	PROJECTILE,
};

struct Frame
{
	CharacterState state;
	int32_t span_start_index; // Relative to owning page (0 = start of page, negative values for span started in prior page)
	std::optional<size_t> span_length;
};

struct Player
{
public:
	std::array<Frame, PAGE_SIZE> frames;
	std::optional<std::pair<CharacterState, int32_t>> prior_span;
	uint8_t num_frames;

	void commit_span();
	void add_frame(CharacterState state);
};

struct Page
{
public:
	std::array<Player, 2> players;

	void clear();
	void add_player_frame(size_t player_index, CharacterState state);
};

struct FrameMeter
{
public:
	Page current_page;
	std::optional<Page> previous_page;
	std::optional<int32_t> advantage;
	bool continuous;
	bool advantage_enabled;

	void reset();
	void update(AREDGameState_Battle *battle);

protected:
	bool pending_reset;

	static CharacterState get_character_state(AREDGameState_Battle *battle, ASW::Character *character);
	std::optional<int32_t> compute_advantage() const;
};
