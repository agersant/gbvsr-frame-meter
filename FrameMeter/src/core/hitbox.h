#pragma once

#include <array>
#include <map>
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
🔲 Hide hurtboxes while invuln
✅ Charlotta 5U box?
🔲 Throw box support?
🔲 Pushbox support?
✅ Investigate pitch -1 multiplier
✅ Test Cag traps
✅ Test Nier puppet
🚧 Test Katalina ares moves.
🚧 Proof read logic when to collect/not-collect hitboxes (result: sus)
🔲 Setup tests, including Zeta 4S, Charlotta 5U and Katalina 2S+U
*/

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

struct Hitbox
{
	BoxType type;
	std::vector<std::array<Vec3, 2>> lines;
};

struct HitboxViewer
{
	std::map<Entity *, std::map<BoxType, Multibox>> box_data;

	void update(const Battle *battle);
	std::vector<Hitbox> get_hitboxes() const;
};
