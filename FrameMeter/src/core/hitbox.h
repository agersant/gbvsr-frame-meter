#pragma once

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
🔲 Box merging
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

struct Hitbox
{
	BoxType type;
	Vec3 top_left;
	Vec3 top_right;
	Vec3 bottom_left;
	Vec3 bottom_right;

	Hitbox(const Entity *entity, const Box *box);
};

struct HitboxViewer
{
	std::vector<Hitbox> hitboxes;

	void update(const Battle *battle);
};
