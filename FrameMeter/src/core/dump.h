#pragma once

#include <filesystem>
#include <sstream>
#include <map>
#include <vector>

#include "core/battle.h"

#if UE_BUILD_TEST

class DumpWriter
{
public:
	static void reset();
	static void begin_dump();
	static void update(const Battle *battle, bool is_in_combat);

private:
	void dump_frame(const Battle *battle);
	void finalize();

	int32_t num_frames;
	std::ostringstream dump;
};

#endif

#if FRAME_METER_AUTOMATED_TESTS

class Dump
{
public:
	static Dump *read_from_disk(const std::filesystem::path &path);

	Dump(){};
	Dump(const Dump &) = delete;
	void operator=(const Dump &) = delete;
	~Dump();

	std::vector<Battle> frames;
	std::vector<char *> objects;

private:
	static void remap_pointers(char *target, size_t target_size, const std::map<char *, char *> &pointer_map);
};

#endif
