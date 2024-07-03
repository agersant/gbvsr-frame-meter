#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>

#include <map>
#include <regex>

#include "test/snapshot.h"

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

static const std::map<CharacterState, std::string> state_to_string = {
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

std::string Snapshot::get_meter_string() const
{
	std::string out;
	for (int i = 0; i < 2; i++)
	{
		for (const SnapshotMeterFrame &frame : meter)
		{
			out += state_to_string.at(frame.players[i].state);
			if (frame.players[i].highlight)
			{
				out += "]";
			}
		}
		out += "\n";
	}
	return out;
}

std::string Snapshot::diff_hitboxes_against_expected(const Snapshot &other) const
{
	std::string out;
	const std::vector<std::set<HitboxViewer::Line>> &actual = hitboxes;
	const std::vector<std::set<HitboxViewer::Line>> &expected = other.hitboxes;

	if (actual.size() != expected.size())
	{
		out += std::format("Expected {} frames but found {}.\n", other.hitboxes.size(), hitboxes.size());
		return out;
	}

	for (int32_t i = 0; i < expected.size(); i++)
	{
		if (expected[i] == actual[i])
		{
			continue;
		}

		out += std::format("Frame #{}:", i + 1);

		{
			std::vector<HitboxViewer::Line> missing_lines;
			std::set_difference(expected[i].begin(), expected[i].end(), actual[i].begin(), actual[i].end(), std::back_inserter(missing_lines));
			if (missing_lines.size() > 0)
			{
				out += std::format(" {} missing lines.", missing_lines.size());
			}
		}

		{
			std::vector<HitboxViewer::Line> unexpected_lines;
			std::set_difference(actual[i].begin(), actual[i].end(), expected[i].begin(), expected[i].end(), std::back_inserter(unexpected_lines));
			if (unexpected_lines.size() > 0)
			{
				out += std::format(" {} unexpected lines.", unexpected_lines.size());
			}
		}

		out += "\n";
	}

	return out;
}

bool Snapshot::read_meter(const std::filesystem::path &path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return false;
	}

	int32_t frame = 0;
	uint8_t player = 0;
	char32_t codepoint;
	while ((codepoint = read_utf8_codepoint(file), !file.eof()))
	{
		if (codepoint == U']' && frame > 0)
		{
			meter[frame - 1].players[player].highlight = true;
		}
		if (codepoint == U'\n')
		{
			player = 1;
			frame = 0;
		}
		if (emoji_to_state.contains(codepoint))
		{
			if (meter.size() <= frame)
			{
				meter.emplace_back();
			}
			meter[frame].players[player].state = emoji_to_state.at(codepoint);
			frame += 1;
		}
	}

	return true;
}

bool Snapshot::read_hitboxes(const std::filesystem::path &path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return false;
	}

	const std::regex frame_header("# Frame \\d+");
	const std::regex hitbox_line("(\\w+) \\((.*), (.*), (.*)\\) -> \\((.*), (.*), (.*)\\)");

	for (std::string line; std::getline(file, line);)
	{
		std::smatch match;
		if (std::regex_search(line, match, frame_header))
		{
			hitboxes.emplace_back();
		}
		else if (std::regex_search(line, match, hitbox_line))
		{
			if (!hitboxes.empty())
			{
				const HitboxType type = deserialize_hitbox_type(match[1]);
				const Vec3 from = Vec3{std::stof(match[2]), std::stof(match[3]), std::stof(match[4])};
				const Vec3 to = Vec3{std::stof(match[5]), std::stof(match[6]), std::stof(match[7])};
				hitboxes.back().emplace(HitboxViewer::Line{type, {from, to}});
			}
		}
	}

	return true;
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
