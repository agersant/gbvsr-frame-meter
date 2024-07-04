#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <optional>
#include <vector>
#include <set>

#include "core/hitbox.h"
#include "core/meter.h"

struct SnapshotMeterPlayer
{
	CharacterState state;
	bool highlight;

	bool operator==(SnapshotMeterPlayer const &) const = default;
};

struct SnapshotMeterFrame
{
	std::array<SnapshotMeterPlayer, 2> players;

	bool operator==(SnapshotMeterFrame const &) const = default;
};

struct Snapshot
{
	bool read_meter(const std::filesystem::path &path);
	bool read_hitboxes(const std::filesystem::path &path);

	std::string get_meter_string() const;
	std::string diff_hitboxes_against_expected(const Snapshot &other) const;

	std::vector<SnapshotMeterFrame> meter;
	HitboxCapture hitboxes;

private:
	static std::optional<CharacterState> state_from_codepoint(char32_t codepoint);
	static std::optional<char32_t> codepoint_from_state(CharacterState state);
	static std::string string_from_state(CharacterState state);
	static char32_t read_utf8_codepoint(std::ifstream &input);
};
