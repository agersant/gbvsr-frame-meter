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

	void add_box(const AABB &new_box);
	void clip();
	std::vector<Line> get_lines() const;

private:
	std::vector<std::pair<AABB, std::vector<Line>>> boxes;
};

struct HitboxViewer
{
	typedef std::pair<HitboxType, std::array<Vec3, 2>> Line;

	std::map<Entity *, std::map<HitboxType, Multibox>> box_data;

	bool update(const Battle *battle);
	std::vector<Line> get_lines() const;

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

	std::vector<std::set<HitboxViewer::Line>> frames;

private:
	void record_frame(const HitboxViewer &viewer);
	void write_to_disk() const;
};
#endif
