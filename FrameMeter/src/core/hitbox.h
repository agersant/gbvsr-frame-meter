#pragma once

#include <array>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "core/battle.h"
#include "core/math.h"

enum class HitboxType : uint8_t
{
	PUSH,
	HURT,
	STRIKE,
	GRAB,
	COUNT,
};

struct Multibox
{
	typedef std::array<Vec2, 2> Line;
	typedef std::array<Vec2, 2> AABB;

	struct Entry
	{
		AABB aabb;
		std::vector<Line> lines;
		std::vector<AABB> fills;
	};

	void add_box(const AABB &new_box);
	void clip();
	std::vector<Line> get_lines() const;
	std::vector<AABB> get_fills() const;

private:
	std::vector<Entry> boxes;
};

struct Hitbox
{
	HitboxType type;
	std::vector<std::array<Vec3, 2>> lines;
	std::vector<std::array<Vec3, 4>> fills;

	bool operator==(Hitbox const &) const = default;
	auto operator<=>(const Hitbox &) const = default;
};

struct HitboxViewer
{
	std::map<Entity *, std::map<HitboxType, Multibox>> box_data;

	bool update(const Battle *battle);
	std::vector<Hitbox> get_hitboxes() const;

private:
	void add_entity(const Battle *battle, Entity *entity, bool is_active);
};

#if UE_BUILD_TEST || FRAME_METER_AUTOMATED_TESTS
class HitboxCapture
{
public:
	static void begin_capture();
	static void update(const HitboxViewer &viewer, bool is_in_combat);
	static void reset();

	void serialize(std::ostream &stream) const;
	static HitboxCapture deserialize(std::istream &stream);

	bool operator==(HitboxCapture const &) const = default;

	std::vector<std::set<Hitbox>> frames;

private:
	void record_frame(const HitboxViewer &viewer);
	void write_to_disk() const;
};
#endif
