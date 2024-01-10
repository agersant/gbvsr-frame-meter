#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "core/battle.h"
#include "core/meter.h"

namespace fs = std::filesystem;

struct Dump
{
	Dump(){};
	Dump(const Dump &) = delete;
	void operator=(const Dump &) = delete;

	std::vector<Battle> frames;
};

struct MeterFrame
{
	CharacterState state;
	bool highlight;
};

struct MeterSnapshot
{
	std::vector<std::pair<MeterFrame, MeterFrame>> frames;
};

std::shared_ptr<Dump> read_game_dump(const std::wstring &path)
{
	std::shared_ptr<Dump> dump(new Dump());
	return dump;
}

std::shared_ptr<MeterSnapshot> read_meter_snapshot(const std::wstring &path)
{
	std::shared_ptr<MeterSnapshot> snapshot(new MeterSnapshot());
	return snapshot;
}

struct TestResult
{
	bool success;
	std::wstring error_message;
};

TestResult run_test(std::shared_ptr<Dump> dump, std::shared_ptr<MeterSnapshot> expected)
{
	return {.success = false, .error_message = L"oopsie"};
}

int main()
{
	std::map<std::wstring, TestResult> results;
	uint32_t num_success = 0;
	uint32_t num_failure = 0;

	const std::string path = "FrameMeter\\test_data";
	const int64_t num_tests = std::distance(fs::directory_iterator(path), fs::directory_iterator{});
	std::wcout << std::format(L"\nrunning {} tests:\n", num_tests);

	for (const auto &entry : fs::directory_iterator(path))
	{
		std::wstring name = entry.path().stem();
		if (results.contains(name))
		{
			continue;
		}
		std::shared_ptr<Dump> dump = read_game_dump(name + L".dump");
		std::shared_ptr<MeterSnapshot> snapshot = read_meter_snapshot(name + L".meter");
		results[name] = run_test(dump, snapshot);
		if (results[name].success)
		{
			num_success += 1;
		}
		else
		{
			num_failure += 1;
		}
		const std::wstring result_text = results[name].success ? L"\x1B[32mok\x1B[0m" : L"\x1B[31mFAILED\x1B[0m";
		std::wcout << std::format(L"{} ... {}\n", name, result_text);
	}

	if (num_failure > 0)
	{
		std::wcout << L"\nfailures:\n";
		for (auto &entry : results)
		{
			if (!entry.second.success)
			{
				std::wcout << std::format(L"\n---- {} ----\n{}\n", entry.first, entry.second.error_message);
			}
		}

		std::wcout << L"\nfailures:\n";
		for (auto &entry : results)
		{
			if (!entry.second.success)
			{
				std::wcout << L"    " << entry.first << L"\n";
			}
		}
	}

	const std::wstring result_text = num_failure == 0 ? L"\x1B[32mok\x1B[0m" : L"\x1B[31mFAILED\x1B[0m";
	std::wcout << std::format(L"\ntest result: {}. {} passed; {} failed\n", result_text, num_success, num_failure);

	return 0;
}
