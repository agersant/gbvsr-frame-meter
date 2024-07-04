#pragma once

#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "core/battle.h"
#include "core/math.h"

/*

TODO LIST

✅ Find entity positions in 2D
✅ Find hitbox positions in 2D
✅ 2D -> 3D coordinate conversion
✅ 3D -> screen coordinate conversion
✅ Draw some boxes!
✅ Hurtboxes support w/ color distinction
✅ Disable during cinematic camera
✅ Add on/off keybind
✅ Box merging
✅ Hide hurtboxes while invulnerable
🔲 Throw box support?
🔲 Pushbox support?
✅ Investigate pitch -1 multiplier
✅ Test Charlotta 5U box
✅ Support Cag traps
✅ Support Nier puppet
🚧 Support Katalina 2S+U
🚧 Support Zeta S
🚧 Fine-tune logic on which hitboxes get collected
🚧 Setup tests, including at least Zeta S (ranged and close), Charlotta 5U and Katalina 2S+U (whiff)
🔲 Automate test suite recording
*/

enum class HitboxType : uint8_t
{
	HURT,
	HIT,
};

#if FRAME_METER_AUTOMATED_TESTS
HitboxType deserialize_hitbox_type(const std::string &type);
#endif

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

	void update(const Battle *battle);
	std::vector<Line> get_lines() const;
};

#if UE_BUILD_TEST || FRAME_METER_AUTOMATED_TESTS
class HitboxCapture
{
public:
	static void begin_capture();
	static void update(const HitboxViewer &viewer, bool is_in_combat);
	static void reset();

	bool operator==(HitboxCapture const &) const = default;

	std::vector<std::set<HitboxViewer::Line>> frames;

private:
	void record_frame(const HitboxViewer &viewer);
	void finalize();
};
#endif
