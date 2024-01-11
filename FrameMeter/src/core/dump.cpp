#include <iostream>
#include <format>
#include <map>

#include "core/dump.h"

const char frame_marker[] = "FRAME_END";
const uint32_t max_objects_per_frame = 500;
const size_t max_object_size = 0xFFFFF;

#if UE_BUILD_TEST

static DumpWriter *dump_writer = nullptr;

DumpWriter::DumpWriter() : num_frames(0)
{
	file.open("rename_me.dump", std::ios::binary | std::ios::out | std::ios::trunc);
}

void DumpWriter::begin_dump()
{
	if (dump_writer)
	{
		delete dump_writer;
		dump_writer = nullptr;
	}
	dump_writer = new DumpWriter();
}

void DumpWriter::update(const Battle *battle, const FrameMeter &frame_meter)
{
	if (!dump_writer)
	{
		return;
	}
	if (!frame_meter.is_at_rest())
	{
		dump_writer->dump_frame(battle);
	}
	else if (dump_writer->num_frames > 0)
	{
		delete dump_writer;
		dump_writer = nullptr;
	}
}

void DumpWriter::dump_frame(const Battle *battle)
{
	file.write((char *)battle, sizeof(Battle));
	for (int i = 0; i < battle->NUM_ENTITIES; i++)
	{
		const Entity *entity = battle->entities[i];
		if (entity == nullptr)
		{
			continue;
		}
		const int32_t size = entity->is_player ? sizeof(Character) : sizeof(Entity);
		file.write((char *)(&entity), sizeof(char *));
		file.write((char *)(&size), sizeof(size));
		file.write((char *)entity, size);
	}
	file << frame_marker;
	num_frames += 1;
}

#endif

#if FRAME_METER_AUTOMATED_TESTS

Dump *Dump::read_from_disk(const std::filesystem::path &path)
{
	Dump *dump = new Dump();

	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::in);
	if (!file.is_open())
	{
		return dump;
	}

	while (true)
	{
		Battle &battle = dump->frames.emplace_back();
		file.read((char *)&battle, sizeof(Battle));

		std::map<char *, std::pair<char *, size_t>> object_map;
		std::map<char *, char *> pointer_map;
		while (true)
		{
			char buffer[sizeof(frame_marker)] = {0};
			file.read(buffer, sizeof(buffer) - 1);
			const bool is_frame_over = std::memcmp(buffer, frame_marker, sizeof(frame_marker)) == 0;
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
