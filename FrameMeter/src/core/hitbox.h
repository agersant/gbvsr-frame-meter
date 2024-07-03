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
🔲 Setup tests, including at least Zeta S (ranged and close), Charlotta 5U and Katalina 2S+U (whiff)
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
