#include <iostream>
#include <format>
#include <map>

#include "miniz.h"

#include "core/dump.h"

const char archived_file_name[] = "dump";
const char frame_marker[] = "FRAME_END";
const uint32_t max_objects_per_frame = 500;
const size_t max_object_size = 0xFFFFF;

#if UE_BUILD_TEST

static DumpWriter *dump_writer = nullptr;

void DumpWriter::reset()
{
	if (dump_writer)
	{
		delete dump_writer;
		dump_writer = nullptr;
	}
}

void DumpWriter::begin_dump()
{
	reset();
	dump_writer = new DumpWriter();
}

void DumpWriter::update(const Battle *battle, bool is_in_combat)
{
	if (!dump_writer)
	{
		return;
	}
	if (is_in_combat)
	{
		dump_writer->dump_frame(battle);
	}
	else if (dump_writer->num_frames > 0)
	{
		dump_writer->finalize();
		reset();
	}
}

void DumpWriter::dump_frame(const Battle *battle)
{
	dump.write((char *)battle, sizeof(Battle));
	for (int i = 0; i < battle->num_entities; i++)
	{
		const Entity *entity = battle->entities[i];
		if (entity == nullptr)
		{
			continue;
		}
		const int32_t size = entity->is_player ? sizeof(Character) : sizeof(Entity);
		dump.write((char *)(&entity), sizeof(char *));
		dump.write((char *)(&size), sizeof(size));
		dump.write((char *)entity, size);
	}
	dump.write(frame_marker, sizeof(frame_marker) - 1);
	num_frames += 1;
}

void DumpWriter::finalize()
{
	const char *zip_filename = "dump.zip";
	remove(zip_filename);

	dump.flush();
	std::string data = std::move(dump).str();
	mz_zip_add_mem_to_archive_file_in_place(zip_filename, archived_file_name, data.c_str(), data.size(), "", 0, MZ_BEST_COMPRESSION);
}

#endif

#if FRAME_METER_AUTOMATED_TESTS

Dump *Dump::read_from_disk(const std::filesystem::path &path)
{
	Dump *dump = new Dump();

	size_t dump_size;
	mz_zip_error err;
	void *data = mz_zip_extract_archive_file_to_heap_v2(path.string().c_str(), archived_file_name, nullptr, &dump_size, 0, &err);
	if (!data)
	{
		std::cout << std::format("Zip error code {} while reading {}\n", (int32_t)err, path.string());
		delete dump;
		return nullptr;
	}
	std::string data_string = std::string((char *)data, dump_size);
	free(data);
	std::istringstream file(data_string);

	while (true)
	{
		Battle &battle = dump->frames.emplace_back();
		file.read((char *)&battle, sizeof(Battle));

		std::map<char *, std::pair<char *, size_t>> object_map;
		std::map<char *, char *> pointer_map;
		while (true)
		{
			char buffer[sizeof(frame_marker) - 1] = {0};
			file.read(buffer, sizeof(buffer));
			const bool is_frame_over = std::memcmp(buffer, frame_marker, sizeof(buffer)) == 0;
			if (is_frame_over)
			{
				break;
			}

			file.seekg(-file.gcount(), std::ios_base::cur);

			char *address;
			file.read((char *)&address, sizeof(address));
			int32_t size;
			file.read((char *)&size, sizeof(size));

			if (object_map.size() >= max_objects_per_frame)
			{
				std::cout << "Too many objects in frame\n";
				delete dump;
				return nullptr;
			}

			if (size > max_object_size)
			{
				std::cout << std::format("Oversized object in dump ({} bytes)\n", size);
				delete dump;
				return nullptr;
			}

			object_map[address] = {(char *)malloc(size), size};
			dump->objects.push_back(object_map[address].first);
			file.read(object_map[address].first, size);
			pointer_map[address] = object_map[address].first;
		}

		remap_pointers((char *)&battle, sizeof(Battle), pointer_map);
		for (auto &entry : object_map)
		{
			remap_pointers(entry.second.first, entry.second.second, pointer_map);
		}

		if (file.peek() == EOF)
		{
			break;
		}
	}

	return dump;
}

Dump::~Dump()
{
	for (char *object : objects)
	{
		free(object);
	}
}

void Dump::remap_pointers(char *target, size_t target_size, const std::map<char *, char *> &pointer_map)
{
	char *cursor = target;
	while (cursor < target + target_size)
	{
		char *data = *(char **)cursor;
		if (pointer_map.contains(data))
		{
			*(char **)cursor = pointer_map.at(data);
		}
		cursor += sizeof(char *);
	}
}

#endif
