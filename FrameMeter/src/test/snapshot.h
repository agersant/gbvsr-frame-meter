#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>

#include "core/meter.h"

struct SnapshotFrame
{
	CharacterState state;
	bool highlight;

	bool operator==(SnapshotFrame const &) const = default;
};

struct Snapshot
{
	static Snapshot *read_from_disk(const std::filesystem::path &path);

	std::string string() const;
	bool operator==(Snapshot const &) const = default;

	std::vector<std::array<SnapshotFrame, 2>> frames;

private:
	static std::optional<CharacterState> state_from_codepoint(char32_t codepoint);
	static std::optional<char32_t> codepoint_from_state(CharacterState state);
	static std::string string_from_state(CharacterState state);
	static char32_t read_utf8_codepoint(std::ifstream &input);
};
