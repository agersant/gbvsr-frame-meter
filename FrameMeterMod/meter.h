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
};

struct Page
{
public:
	static constexpr uint8_t SIZE = 80;
	std::array<CharacterState, SIZE> player_1;
	std::array<CharacterState, SIZE> player_2;
	uint8_t num_frames;

	void clear();
};

struct FrameMeter
{
public:
	std::optional<Page> previous_page;
	Page current_page;
	void update(AREDGameState_Battle *battle);

protected:
	static CharacterState get_character_state(ASW::Character *character);
	bool pending_reset;
};
