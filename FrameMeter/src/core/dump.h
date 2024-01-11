#pragma once

#include <filesystem>
#include <fstream>
#include <map>
#include <vector>

#include "core/battle.h"

class DumpWriter
{
public:
	DumpWriter(const std::filesystem::path &path);

	void add_frame(Battle *battle);
	int32_t get_num_frames() const { return num_frames; }
	void close();

private:
	int32_t num_frames;
	std::ofstream file;
};

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
