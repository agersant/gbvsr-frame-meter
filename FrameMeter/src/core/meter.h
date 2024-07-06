#pragma once

#include <array>
#include <iostream>
#include <optional>
#include <vector>

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

private:
	bool pending_reset;

	static CharacterState get_character_state(const Battle *battle, Character *character);
	std::optional<int32_t> compute_advantage() const;
};

#if UE_BUILD_TEST || FRAME_METER_AUTOMATED_TESTS
struct FrameMeterCapturePlayer
{
	CharacterState state;
	bool highlight;

	bool operator==(FrameMeterCapturePlayer const &) const = default;
};

struct FrameMeterCaptureFrame
{
	std::array<FrameMeterCapturePlayer, 2> players;

	bool operator==(FrameMeterCaptureFrame const &) const = default;
};

struct FrameMeterCapture
{

public:
	static void begin_capture();
	static void update(const FrameMeter &viewer, bool is_in_combat);
	static void reset();

	void serialize(std::ostream &stream) const;
	static FrameMeterCapture deserialize(std::istream &stream);

	bool operator==(FrameMeterCapture const &) const = default;

	std::vector<FrameMeterCaptureFrame> frames;

private:
	void record_frame(const FrameMeter &viewer);
	void write_to_disk() const;
};

// For console printing
std::ostream &operator<<(std::ostream &stream, const FrameMeterCapture &capture);

#endif
