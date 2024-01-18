#pragma once

#include <array>
#include <optional>

#include "core/battle.h"

const size_t PAGE_SIZE = 80;

enum CharacterState
{
	IDLE,
	ACTIVE_HITBOX,
	COUNTER,
	RECOVERY,
	STUN,
	MOVEMENT,
	INVINCIBLE,
	PROJECTILE,
	ARMOR,
};

struct Frame
{
	CharacterState state;
	int32_t span_start_index; // Relative to owning page (0 = start of page, negative values for span started in prior page)
	std::optional<size_t> span_length;
	bool highlight;
};

struct Page
{
public:
	std::array<Frame, PAGE_SIZE> frames;
	uint8_t num_frames;

	void add_frame(const Frame &frame);
	void commit_span();
};

struct Player
{
public:
	Page current_page;
	std::optional<Page> previous_page;

	void end_page();
	void add_frame(CharacterState state, bool can_highlight);
};

struct FrameMeter
{
public:
	std::array<Player, 2> players;
	std::optional<int32_t> advantage;
	bool continuous;
	bool advantage_enabled;

	void reset();
	bool is_at_rest() const;
	bool update(const Battle *battle);

protected:
	bool pending_reset;

	static CharacterState get_character_state(const Battle *battle, Character *character);
	std::optional<int32_t> compute_advantage() const;
};
