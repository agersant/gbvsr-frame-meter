#pragma once

#include "game.h"

enum CharacterState
{
	IDLE,
	ACTIVE_HITBOX,
	COUNTER,
	PUNISH_COUNTER,
	STUN,
};

struct Page
{
	static constexpr uint8_t SIZE = 80;
	std::array<CharacterState, SIZE> player_1;
	std::array<CharacterState, SIZE> player_2;
	uint8_t num_frames;
};

struct FrameMeter
{
public:
	void update(AREDGameState_Battle *battle);
	Page current_page;
};
