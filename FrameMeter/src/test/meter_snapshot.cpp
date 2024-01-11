#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>

#include "test/meter_snapshot.h"

static const std::vector<std::pair<char32_t, CharacterState>> representations = {
	{0x02B1B /*⬛*/, CharacterState::IDLE},
	{0x02B1C /*⬜*/, CharacterState::INVINCIBLE},
	{0x1F537 /*🔷*/, CharacterState::MOVEMENT},
	{0x1F7E5 /*🟥*/, CharacterState::ACTIVE_HITBOX},
	{0x1F7E6 /*🟦*/, CharacterState::PUNISH_COUNTER},
	{0x1F7E7 /*🟧*/, CharacterState::PROJECTILE},
	{0x1F7E8 /*🟨*/, CharacterState::STUN},
	{0x1F7E9 /*🟩*/, CharacterState::COUNTER},
};

MeterSnapshot *MeterSnapshot::read_from_disk(const std::filesystem::path &path)
{
	MeterSnapshot *snapshot = new MeterSnapshot();

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
		std::optional<CharacterState> state = state_from_codepoint(codepoint);
		if (state.has_value())
		{
			if (snapshot->frames.size() <= frame)
			{
				snapshot->frames.emplace_back();
			}
			snapshot->frames[frame][player].state = state.value();
			frame += 1;
		}
	}

	return snapshot;
}

std::optional<CharacterState> MeterSnapshot::state_from_codepoint(char32_t codepoint)
{
	for (auto &entry : representations)
	{
		if (entry.first == codepoint)
		{
			return entry.second;
		}
	}
	return std::nullopt;
}

std::optional<char32_t> MeterSnapshot::codepoint_from_state(CharacterState state)
{
	for (auto &entry : representations)
	{
		if (entry.second == state)
		{
			return entry.first;
		}
	}
	return std::nullopt;
}

std::string MeterSnapshot::string_from_state(CharacterState state)
{
	const char32_t codepoint = codepoint_from_state(state).value_or(0);
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
	return convert.to_bytes(&codepoint, &codepoint + 1);
}

char32_t MeterSnapshot::read_utf8_codepoint(std::ifstream &input)
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
