#include "core/battle.h"
#include "core/hitbox.h"

Vec3 battle_to_ue_coords(float x, float y)
{
	const float BATTLE_TO_UE_SCALE = 0.00043f;
	return {
		.x = BATTLE_TO_UE_SCALE * x,
		.y = 0.f,
		.z = BATTLE_TO_UE_SCALE * y,
	};
}

Hitbox::Hitbox(const Entity *entity, const Box *box)
	: type(box->type)
{
	const int32_t sx = -entity->scale_x * (entity->facing_left ? -1 : 1);
	const int32_t sy = -entity->scale_y;
	const float x1 = entity->get_position_x() + sx * box->x;
	const float y1 = entity->get_position_y() + sy * box->y;
	const float x2 = x1 + sx * box->w;
	const float y2 = y1 + sy * box->h;

	const float left = std::min(x1, x2);
	const float top = std::min(y1, y2);
	const float right = std::max(x1, x2);
	const float bottom = std::max(y1, y2);

	top_left = battle_to_ue_coords(left, top);
	top_right = battle_to_ue_coords(right, top);
	bottom_left = battle_to_ue_coords(left, bottom);
	bottom_right = battle_to_ue_coords(right, bottom);
}

void HitboxViewer::update(const Battle *battle)
{
	hitboxes.clear();

	for (int32_t entity_index = 0; entity_index < battle->num_entities; entity_index++)
	{
		Entity *entity = battle->entities[entity_index];

		if (entity->attack_hit_connecting || entity->is_in_active_frames())
		{
			for (uint32_t box_index = 0; box_index < entity->num_hitboxes; box_index++)
			{
				Box *box = (entity->hitboxes + box_index);
				hitboxes.push_back(Hitbox(entity, box));
			}
		}

		for (uint32_t box_index = 0; box_index < entity->num_hurtboxes; box_index++)
		{
			Box *box = (entity->hurtboxes + box_index);
			hitboxes.push_back(Hitbox(entity, box));
		}
	}
}
