#pragma once

#include "game.h"

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
	int32_t span_start_index;
	std::optional<size_t> span_length;
};

struct Page
{
public:
	static constexpr uint8_t SIZE = 80;
	std::array<std::array<Frame, SIZE>, 2> players;
	uint8_t num_frames;

	void clear();
	void add_frame(CharacterState state_1, CharacterState state_2);
	void commit_span(std::array<Frame, SIZE> &player);
	void add_player_frame(std::array<Frame, SIZE> &player, CharacterState state);
};

struct FrameMeter
{
public:
	std::optional<Page> previous_page;
	Page current_page;
	void update(AREDGameState_Battle *battle);

protected:
	static CharacterState get_character_state(AREDGameState_Battle *battle, ASW::Character *character);
	bool pending_reset;
};
