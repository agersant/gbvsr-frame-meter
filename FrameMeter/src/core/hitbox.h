#pragma once

#include <array>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "core/battle.h"
#include "core/math.h"

enum class HitboxDisplayMode : uint8_t
{
	None,
	All,
	OnlyP1,
	OnlyP2,
	Count,
};

enum class HitboxOwner : uint8_t
{
	P1 = 1,
	P2 = 2,
};

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
	struct Line
	{
		HitboxOwner owner;
		HitboxType type;
		std::array<Vec3, 2> vertices;

		bool operator==(Line const &) const = default;
		auto operator<=>(const Line &) const = default;
	};

	HitboxDisplayMode display_mode = HitboxDisplayMode::None;

	void cycle_display_mode();
	bool update(const Battle *battle);
	const std::vector<Line> &get_lines() const;

private:
	typedef std::map<Entity *, std::map<HitboxType, Multibox>> EntityHitboxes;

	std::vector<Line> lines;

	void add_entity(const Battle *battle, Entity *entity, bool is_active, EntityHitboxes &out_hitboxes);
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
