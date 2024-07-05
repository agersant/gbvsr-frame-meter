#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>

#include <cassert>
#include <format>
#include <fstream>
#include <locale>
#include <map>

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
	if (battle->is_freeze_frame())
	{
		return false;
	}

	Character *character_1 = battle->teams[0].main_player_object;
	Character *character_2 = battle->teams[1].main_player_object;
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
	const bool can_highlight_1 = character_1->is_idle() || (character_1->attacking && character_1->defense_hit_connecting);
	const bool can_highlight_2 = character_2->is_idle() || (character_2->attacking && character_2->defense_hit_connecting);

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
		if (!entity->is_active() || entity->num_hitboxes <= 0)
		{
			continue;
		}
		return CharacterState::PROJECTILE;
	}

	if (character->is_idle() && !character->defense_hit_connecting && !character->defense_guard_connecting)
	{
		return CharacterState::IDLE;
	}

	if (character->is_in_blockstun() || character->is_in_hitstun())
	{
		return CharacterState::STUN;
	}

	if (character->is_recovering())
	{
		return CharacterState::RECOVERY;
	}

	const bool is_active = character->is_active();
	const bool has_hitbox = character->num_hitboxes > 0;
	const bool attached_has_hitbox = character->attached && character->attached->num_hitboxes > 0;
	if (character->attacking && is_active && (has_hitbox || attached_has_hitbox))
	{
		return CharacterState::ACTIVE_HITBOX;
	}

	if (character->is_any_invincible())
	{
		return CharacterState::INVINCIBLE;
	}

	if (character->has_armor() || (character->puppet && character->puppet->has_armor()))
	{
		return CharacterState::ARMOR;
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

#if UE_BUILD_TEST || FRAME_METER_AUTOMATED_TESTS

static const std::map<char32_t, CharacterState> emoji_to_state = {
	{0x02B1B /*⬛*/, CharacterState::IDLE},
	{0x02B1C /*⬜*/, CharacterState::INVINCIBLE},
	{0x1F537 /*🔷*/, CharacterState::MOVEMENT},
	{0x1F7E5 /*🟥*/, CharacterState::ACTIVE_HITBOX},
	{0x1F7E6 /*🟦*/, CharacterState::RECOVERY},
	{0x1F7E7 /*🟧*/, CharacterState::PROJECTILE},
	{0x1F7E8 /*🟨*/, CharacterState::STUN},
	{0x1F7E9 /*🟩*/, CharacterState::COUNTER},
	{0x1F7EA /*🟪*/, CharacterState::ARMOR},
};

static const std::map<CharacterState, std::string> state_to_console_string = {
	{CharacterState::IDLE, "\x1B[38;5;234m■\x1B[0m "},
	{CharacterState::INVINCIBLE, "\x1B[38;5;252m■\x1B[0m "},
	{CharacterState::MOVEMENT, "\x1B[38;5;44m■\x1B[0m "},
	{CharacterState::ACTIVE_HITBOX, "\x1B[38;5;160m■\x1B[0m "},
	{CharacterState::RECOVERY, "\x1B[38;5;26m■\x1B[0m "},
	{CharacterState::PROJECTILE, "\x1B[38;5;172m■\x1B[0m "},
	{CharacterState::STUN, "\x1B[38;5;220m■\x1B[0m "},
	{CharacterState::COUNTER, "\x1B[38;5;35m■\x1B[0m "},
	{CharacterState::ARMOR, "\x1B[38;5;54m■\x1B[0m "},
};

static FrameMeterCapture *capture = nullptr;

void FrameMeterCapture::begin_capture()
{
	reset();
	capture = new FrameMeterCapture();
}

void FrameMeterCapture::update(const FrameMeter &meter, bool is_in_combat)
{
	if (!capture)
	{
		return;
	}
	if (is_in_combat)
	{
		capture->record_frame(meter);
	}
	else if (capture->frames.size() > 0)
	{
		capture->write_to_disk();
		reset();
	}
}

void FrameMeterCapture::reset()
{
	if (capture)
	{
		delete capture;
		capture = nullptr;
	}
}

void FrameMeterCapture::record_frame(const FrameMeter &meter)
{
	const auto capture_player = [](const Player &player) -> FrameMeterCapturePlayer
	{
		const Frame &frame = player.current_page.frames[player.current_page.num_frames - 1];
		return FrameMeterCapturePlayer{
			.state = frame.state,
			.highlight = frame.highlight,
		};
	};
	FrameMeterCapturePlayer p1 = capture_player(meter.players[0]);
	FrameMeterCapturePlayer p2 = capture_player(meter.players[1]);
	frames.emplace_back(FrameMeterCaptureFrame{p1, p2});
}

char32_t read_utf8_codepoint(std::istream &input)
{
	const char8_t c1 = input.get();
	if (c1 < 0b10000000U)
	{
		return c1;
	}
	else if (c1 < 0b11100000U)
	{
		const char8_t c2 = input.get();
		return (c1 & 0b00011111U) << 6U |
			   (c2 & 0b00111111U);
	}
	else if (c1 < 0b11110000U)
	{
		const char8_t c2 = input.get();
		const char8_t c3 = input.get();
		return (c1 & 0b00001111U) << 12U |
			   (c2 & 0b00111111U) << 6U |
			   (c3 & 0b00111111U);
	}
	else if (c1 < 0b11111000U)
	{
		const char8_t c2 = input.get();
		const char8_t c3 = input.get();
		const char8_t c4 = input.get();
		return (c1 & 0b00000111U) << 18UL |
			   (c2 & 0b00111111U) << 12U |
			   (c3 & 0b00111111U) << 6U |
			   (c4 & 0b00111111U);
	}
	return 0;
}

std::ostream &operator<<(std::ostream &stream, const FrameMeterCapture &capture)
{
	for (int32_t player_index = 0; player_index < 2; player_index++)
	{
		for (const FrameMeterCaptureFrame &frame : capture.frames)
		{
			const FrameMeterCapturePlayer &player = frame.players[player_index];
			stream << state_to_console_string.at(player.state);
			if (player.highlight)
			{
				stream << "]";
			}
		}
		stream << "\n";
	}

	return stream;
}

void FrameMeterCapture::serialize(std::ostream &stream) const
{
	for (int32_t player_index = 0; player_index < 2; player_index++)
	{
		for (const FrameMeterCaptureFrame &frame : frames)
		{
			const FrameMeterCapturePlayer &player = frame.players[player_index];
			for (const auto &[emoji, state] : emoji_to_state)
			{
				if (state == player.state)
				{
					std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
					stream << convert.to_bytes(&emoji, &emoji + 1);
					break;
				}
			}
			if (player.highlight)
			{
				stream << "]";
			}
		}
		stream << "\n";
	}
}

FrameMeterCapture FrameMeterCapture::deserialize(std::istream &stream)
{
	FrameMeterCapture capture;

	int32_t frame = 0;
	uint8_t player = 0;
	char32_t codepoint;

	while ((codepoint = read_utf8_codepoint(stream), !stream.eof()))
	{
		if (codepoint == U']' && frame > 0)
		{
			capture.frames[frame - 1].players[player].highlight = true;
		}
		if (codepoint == U'\n')
		{
			player = 1;
			frame = 0;
		}
		if (emoji_to_state.contains(codepoint))
		{
			if (capture.frames.size() <= frame)
			{
				capture.frames.emplace_back();
			}
			capture.frames[frame].players[player].state = emoji_to_state.at(codepoint);
			frame += 1;
		}
	}

	return capture;
}

void FrameMeterCapture::write_to_disk() const
{
	std::ofstream output;
	output.open("capture.meter");
	serialize(output);
}
#endif