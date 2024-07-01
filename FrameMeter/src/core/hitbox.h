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
🔲 Draw some boxes!
✅ Hurtboxes support w/ color distinction
🔲 Disable during cinematic camera (RedCamera_Battle->ActiveAnims for super, but doesnt work for throws)
🔲 Add on/off keybind
🔲 Hide hurtboxes while invuln
🔲 Charlotta 5U box?
🔲 Throw box support?
🔲 Pushbox support?
🔲 Investigate pitch -1 multiplier
🔲 Proof read logic when to collect/not-collect hitboxes
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
