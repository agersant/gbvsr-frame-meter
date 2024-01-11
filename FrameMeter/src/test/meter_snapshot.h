#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>

#include "core/meter.h"

struct MeterFrame
{
	CharacterState state;
	bool highlight;
};

struct MeterSnapshot
{
	static MeterSnapshot *read_from_disk(const std::filesystem::path &path);
	static std::string string_from_state(CharacterState state);

	std::vector<std::array<MeterFrame, 2>> frames;

private:
	static std::optional<CharacterState> state_from_codepoint(char32_t codepoint);
	static std::optional<char32_t> codepoint_from_state(CharacterState state);
	static char32_t read_utf8_codepoint(std::ifstream &input);
};
