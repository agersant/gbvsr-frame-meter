#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>

#include <map>

#include "test/snapshot.h"

static const std::map<char32_t, CharacterState> emoji_to_state = {
	{0x02B1B /*⬛*/, CharacterState::IDLE},
	{0x02B1C /*⬜*/, CharacterState::INVINCIBLE},
	{0x1F537 /*🔷*/, CharacterState::MOVEMENT},
	{0x1F7E5 /*🟥*/, CharacterState::ACTIVE_HITBOX},
	{0x1F7E6 /*🟦*/, CharacterState::PUNISH_COUNTER},
	{0x1F7E7 /*🟧*/, CharacterState::PROJECTILE},
	{0x1F7E8 /*🟨*/, CharacterState::STUN},
	{0x1F7E9 /*🟩*/, CharacterState::COUNTER},
};

static const std::map<CharacterState, std::string> state_to_string = {
	{CharacterState::IDLE, "\x1B[38;5;234m■\x1B[0m "},
	{CharacterState::INVINCIBLE, "\x1B[38;5;252m■\x1B[0m "},
	{CharacterState::MOVEMENT, "\x1B[38;5;44m■\x1B[0m "},
	{CharacterState::ACTIVE_HITBOX, "\x1B[38;5;160m■\x1B[0m "},
	{CharacterState::PUNISH_COUNTER, "\x1B[38;5;26m■\x1B[0m "},
	{CharacterState::PROJECTILE, "\x1B[38;5;172m■\x1B[0m "},
	{CharacterState::STUN, "\x1B[38;5;220m■\x1B[0m "},
	{CharacterState::COUNTER, "\x1B[38;5;35m■\x1B[0m "},
};

std::string Snapshot::string() const
{
	std::string out;
	for (int i = 0; i < 2; i++)
	{
		for (auto &frame : frames)
		{
			out += state_to_string.at(frame[i].state);
			if (frame[i].highlight)
			{
				out += "]";
			}
		}
		out += "\n";
	}
	return out;
}

Snapshot *Snapshot::read_from_disk(const std::filesystem::path &path)
{
	Snapshot *snapshot = new Snapshot();

	std::ifstream file(path);
	if (!file.is_open())
	{
		return snapshot;
	}

	int32_t frame = 0;
	uint8_t player = 0;
	char32_t codepoint;
	while ((codepoint = read_utf8_codepoint(file), !file.eof()))
	{
		if (codepoint == U']' && frame > 0)
		{
			snapshot->frames[frame - 1][player].highlight = true;
		}
		if (codepoint == U'\n')
		{
			player = 1;
			frame = 0;
		}
		if (emoji_to_state.contains(codepoint))
		{
			if (snapshot->frames.size() <= frame)
			{
				snapshot->frames.emplace_back();
			}
			snapshot->frames[frame][player].state = emoji_to_state.at(codepoint);
			frame += 1;
		}
	}

	return snapshot;
}

char32_t Snapshot::read_utf8_codepoint(std::ifstream &input)
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
