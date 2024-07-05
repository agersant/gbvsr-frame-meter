#pragma once

#include <filesystem>

#include "core/hitbox.h"
#include "core/meter.h"

struct Snapshot
{
	bool read_meter(const std::filesystem::path &path);
	bool read_hitboxes(const std::filesystem::path &path);

	std::string get_meter_string() const;
	std::string diff_hitboxes_against_expected(const Snapshot &other) const;

	FrameMeterCapture meter;
	HitboxCapture hitboxes;
};
