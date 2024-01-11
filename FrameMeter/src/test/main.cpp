#include <Windows.h>

#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "core/dump.h"
#include "core/meter.h"
#include "test/meter_snapshot.h"

namespace fs = std::filesystem;

bool run_test(std::shared_ptr<const Dump> dump, std::shared_ptr<const MeterSnapshot> snapshot)
{
	if (dump->frames.size() != snapshot->frames.size())
	{
		return false;
	}

	FrameMeter frame_meter = {};

	for (int i = 0; i < dump->frames.size(); i++)
	{
		frame_meter.update(&dump->frames[i]);
		const uint8_t num_frames = frame_meter.players[0].current_page.num_frames;
		const Frame &actual_1 = frame_meter.players[0].current_page.frames[num_frames - 1];
		const Frame &actual_2 = frame_meter.players[1].current_page.frames[num_frames - 1];
		const MeterFrame &expected_1 = snapshot->frames[i][0];
		const MeterFrame &expected_2 = snapshot->frames[i][1];
		const bool states_match = actual_1.state == expected_1.state && actual_2.state == expected_2.state;
		const bool highlights_match = actual_1.highlight == expected_1.highlight && actual_2.highlight == expected_2.highlight;
		if (!states_match || !highlights_match)
		{
			return false;
		}
	}

	return true;
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	std::map<std::string, bool> results;
	uint32_t num_success = 0;
	uint32_t num_failure = 0;

	std::cout << std::format("\nrunning tests:\n");

	const fs::path path = R"(FrameMeter\test_data)";
	for (const auto &entry : fs::directory_iterator(path))
	{
		const std::string name = entry.path().stem().string();
		if (results.contains(name))
		{
			continue;
		}
		const fs::path dump_path = fs::path(entry.path()).replace_extension(".dump");
		const fs::path meter_path = fs::path(entry.path()).replace_extension(".meter");
		std::shared_ptr<const Dump> dump = std::shared_ptr<const Dump>(Dump::read_from_disk(dump_path));
		std::shared_ptr<const MeterSnapshot> snapshot = std::shared_ptr<const MeterSnapshot>(MeterSnapshot::read_from_disk(meter_path));
		results[name] = dump && snapshot && run_test(dump, snapshot);
		if (results[name])
		{
			num_success += 1;
		}
		else
		{
			num_failure += 1;
		}
		const std::string result_text = results[name] ? "\x1B[32mok\x1B[0m" : "\x1B[31mFAILED\x1B[0m";
		std::cout << format("{} ... {}\n", name, result_text);
	}

	if (num_failure > 0)
	{
		std::cout << "\nfailures:\n";
		for (auto &entry : results)
		{
			if (!entry.second)
			{
				std::cout << "    " << entry.first << "\n";
			}
		}
	}

	const std::string result_text = num_failure == 0 ? "\x1B[32mok\x1B[0m" : "\x1B[31mFAILED\x1B[0m";
	std::cout << std::format("\ntest result: {}. {} passed; {} failed\n", result_text, num_success, num_failure);

	return 0;
}
