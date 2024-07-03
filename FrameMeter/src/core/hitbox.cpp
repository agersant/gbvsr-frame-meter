#include "core/battle.h"
#include "core/hitbox.h"

void Multibox::add_box(const AABB &new_box)
{
	const Vec2 a = new_box[0];
	const Vec2 b = {new_box[1].x, new_box[0].y};
	const Vec2 c = new_box[1];
	const Vec2 d = {new_box[0].x, new_box[1].y};
	std::vector<Line> box_lines = {{a, b}, {b, c}, {c, d}, {d, a}};
	boxes.push_back({new_box, box_lines});
}

void clip_line_against_box(const Multibox::Line &line, const Multibox::AABB &box, std::vector<Multibox::Line> &out_segments)
{
	const float box_x_min = std::min(box[0].x, box[1].x);
	const float box_x_max = std::max(box[0].x, box[1].x);
	const float box_y_min = std::min(box[0].y, box[1].y);
	const float box_y_max = std::max(box[0].y, box[1].y);

	if (line[0].y == line[1].y)
	{
		const float line_x_min = std::min(line[0].x, line[1].x);
		const float line_x_max = std::max(line[0].x, line[1].x);
		const float line_y = line[0].y;
		if (line_y <= box_y_min || line_y >= box_y_max || line_x_max <= box_x_min || line_x_min >= box_x_max)
		{
			out_segments.push_back(line);
		}
		else
		{
			if (line_x_min < box_x_min)
			{
				out_segments.push_back({Vec2{line_x_min, line_y}, Vec2{box_x_min, line_y}});
			}
			if (line_x_max > box_x_max)
			{
				out_segments.push_back({Vec2{box_x_max, line_y}, Vec2{line_x_max, line_y}});
			}
		}
	}
	else if (line[0].x == line[1].x)
	{
		const float line_y_min = std::min(line[0].y, line[1].y);
		const float line_y_max = std::max(line[0].y, line[1].y);
		const float line_x = line[0].x;
		if (line_x <= box_x_min || line_x >= box_x_max || line_y_max <= box_y_min || line_y_min >= box_y_max)
		{
			out_segments.push_back(line);
		}
		else
		{
			if (line_y_min < box_y_min)
			{
				out_segments.push_back({Vec2{line_x, line_y_min}, Vec2{line_x, box_y_min}});
			}
			if (line_y_max > box_y_max)
			{
				out_segments.push_back({Vec2{line_x, box_y_max}, Vec2{line_x, line_y_max}});
			}
		}
	}
}

void Multibox::clip()
{
	for (int i = 0; i < boxes.size(); i++)
	{
		for (int j = 0; j < boxes.size(); j++)
		{
			if (i != j)
			{
				std::vector<Line> old_lines = boxes[i].second;
				boxes[i].second.clear();
				for (const Line &old_line : old_lines)
				{
					clip_line_against_box(old_line, boxes[j].first, boxes[i].second);
				}
			}
		}
	}
}

std::vector<Multibox::Line> Multibox::get_lines() const
{
	std::vector<Multibox::Line> all_lines;
	for (auto &[aabb, lines] : boxes)
	{
		all_lines.insert(all_lines.end(), lines.begin(), lines.end());
	}
	return all_lines;
}

Multibox::AABB entity_box_to_aabb(Entity *entity, Box *box)
{
	const int32_t sx = -entity->scale_x * (entity->facing_left ? -1 : 1);
	const int32_t sy = -entity->scale_y;
	const float x0 = entity->get_position_x() + sx * box->x;
	const float y0 = entity->get_position_y() + sy * box->y;
	const float x1 = x0 + sx * box->w;
	const float y1 = y0 + sy * box->h;
	return Multibox::AABB{{{x0, y0}, {x1, y1}}};
}

void HitboxViewer::update(const Battle *battle)
{
	if (battle->is_freeze_frame())
	{
		return;
	}

	box_data.clear();

	for (int32_t entity_index = 0; entity_index < battle->num_entities; entity_index++)
	{
		Entity *entity = battle->entities[entity_index];

		const bool is_active = entity->is_active();
		const bool is_parent_active = entity->attach_parent && entity->attach_parent->is_active();
		if (is_active || is_parent_active)
		{
			for (uint32_t box_index = 0; box_index < entity->num_hitboxes; box_index++)
			{
				Box *box = (entity->hitboxes + box_index);
				box_data[entity][box->type].add_box(entity_box_to_aabb(entity, box));
			}
		}

		for (uint32_t box_index = 0; box_index < entity->num_hurtboxes; box_index++)
		{
			Box *box = (entity->hurtboxes + box_index);
			box_data[entity][box->type].add_box(entity_box_to_aabb(entity, box));
		}

		if (box_data.contains(entity))
		{
			for (auto &[box_type, multibox] : box_data[entity])
			{
				multibox.clip();
			}
		}
	}
}

Vec3 battle_to_ue_coords(const Vec2 &p)
{
	const float BATTLE_TO_UE_SCALE = 0.00043f;
	return {
		.x = BATTLE_TO_UE_SCALE * p.x,
		.y = 0.f,
		.z = BATTLE_TO_UE_SCALE * p.y,
	};
}

std::vector<Hitbox> HitboxViewer::get_hitboxes() const
{
	std::vector<Hitbox> hitboxes = {};

	for (auto &[entity, multiboxes] : box_data)
	{
		for (auto &[box_type, multibox] : multiboxes)
		{
			Hitbox &hitbox = hitboxes.emplace_back(Hitbox{
				.type = box_type,
				.lines = {},
			});
			for (auto &line : multibox.get_lines())
			{
				hitbox.lines.push_back({battle_to_ue_coords(line[0]), battle_to_ue_coords(line[1])});
			}
		}
	}

	return hitboxes;
}
