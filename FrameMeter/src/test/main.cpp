#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "core/dump.h"
#include "core/meter.h"
#include "test/output_sink.h"
#include "test/snapshot.h"

namespace fs = std::filesystem;

bool run_test(const fs::path &test_file)
{
	const fs::path dump_path = fs::path(test_file).replace_extension(".zip");
	std::shared_ptr<const Dump> dump = std::unique_ptr<const Dump>(Dump::read_from_disk(dump_path));
	if (!dump)
	{
		std::cout << "Failed to read dump file\n";
		return false;
	}

	const fs::path snapshot_path = fs::path(test_file).replace_extension(".meter");
	std::shared_ptr<const Snapshot> expected = std::unique_ptr<const Snapshot>(Snapshot::read_from_disk(snapshot_path));
	if (!expected)
	{
		std::cout << "Failed to read snapshot file\n";
		return false;
	}

	FrameMeter meter = {};
	Snapshot actual;

	for (int i = 0; i < dump->frames.size(); i++)
	{
		if (!meter.update(&dump->frames[i]))
		{
			continue;
		}
		std::array<SnapshotFrame, 2> &frame = actual.frames.emplace_back();

		const uint8_t num_frames = meter.players[0].current_page.num_frames;
		const Frame &p1 = meter.players[0].current_page.frames[num_frames - 1];
		const Frame &p2 = meter.players[1].current_page.frames[num_frames - 1];
		frame[0].state = p1.state;
		frame[0].highlight = p1.highlight;
		frame[1].state = p2.state;
		frame[1].highlight = p2.highlight;
	}

	if (actual != *expected)
	{
		std::cout << std::format("Expected:\n{}\nActual:\n{}\n", expected->string(), actual.string());
		return false;
	}

	return true;
}

int main(int32_t argc, char *argv[])
{
	SetConsoleOutputCP(CP_UTF8);

	std::optional<std::string> test_name;
	for (int i = 1; i < argc; i++)
	{
		if (i + 1 < argc && _stricmp(argv[i], "--only") == 0)
		{
			test_name = std::string(argv[i + 1]);
			i += 1;
		}
	}

	struct TestResult
	{
		bool success;
		std::string output;
	};

	std::map<std::string, TestResult> results;
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
		if (test_name.has_value() && _stricmp(name.c_str(), test_name->c_str()) != 0)
		{
			continue;
		}
		{
			OutputSink sink;
			TestResult &result = results[name];
			result.success = run_test(entry.path());
			result.output = sink.content();
		}
		if (results[name].success)
		{
			num_success += 1;
		}
		else
		{
			num_failure += 1;
		}
		const std::string result_text = results[name].success ? "\x1B[32mok\x1B[0m" : "\x1B[31mFAILED\x1B[0m";
		std::cout << format("{} ... {}\n", name, result_text);
	}

	if (num_failure > 0)
	{
		std::cout << "\nfailures:\n";
		for (auto &entry : results)
		{
			if (!entry.second.success)
			{
				std::cout << "\n---- " << entry.first << " ----\n";
				std::cout << entry.second.output;
			}
		}

		std::cout << "\nfailures:\n";
		for (auto &entry : results)
		{
			if (!entry.second.success)
			{
				std::cout << "    " << entry.first << "\n";
			}
		}
	}

	const std::string result_text = num_failure == 0 ? "\x1B[32mok\x1B[0m" : "\x1B[31mFAILED\x1B[0m";
	std::cout << std::format("\ntest result: {}. {} passed; {} failed\n", result_text, num_success, num_failure);

	return 0;
}
