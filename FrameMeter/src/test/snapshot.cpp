#include <fstream>

#include "test/snapshot.h"

std::string Snapshot::get_meter_string() const
{
	std::ostringstream out;
	meter.serialize(out);
	return out.str();
}

std::string Snapshot::diff_hitboxes_against_expected(const Snapshot &other) const
{
	std::string out;
	const HitboxCapture &actual = hitboxes;
	const HitboxCapture &expected = other.hitboxes;

	if (actual.frames.size() != expected.frames.size())
	{
		out += std::format("Expected {} frames but found {}.\n", other.hitboxes.frames.size(), hitboxes.frames.size());
		return out;
	}

	for (int32_t i = 0; i < expected.frames.size(); i++)
	{
		if (expected.frames[i] == actual.frames[i])
		{
			continue;
		}

		out += std::format("Frame #{}:", i + 1);

		{
			std::vector<HitboxViewer::Line> missing_lines;
			std::set_difference(expected.frames[i].begin(), expected.frames[i].end(), actual.frames[i].begin(), actual.frames[i].end(), std::back_inserter(missing_lines));
			if (missing_lines.size() > 0)
			{
				out += std::format(" {} missing lines.", missing_lines.size());
			}
		}

		{
			std::vector<HitboxViewer::Line> unexpected_lines;
			std::set_difference(actual.frames[i].begin(), actual.frames[i].end(), expected.frames[i].begin(), expected.frames[i].end(), std::back_inserter(unexpected_lines));
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
	meter = FrameMeterCapture::deserialize(file);
	return true;
}

bool Snapshot::read_hitboxes(const std::filesystem::path &path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return false;
	}
	hitboxes = HitboxCapture::deserialize(file);
	return true;
}
