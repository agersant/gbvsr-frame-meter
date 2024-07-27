#include <format>
#include <fstream>
#include <regex>
#include <string>

#include "core/battle.h"
#include "core/hitbox.h"

const float BATTLE_TO_UE_SCALE = 0.00043f;

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
	const float w = box->w;
	const float h = box->h;
	const int32_t sx = -entity->scale_x * (entity->facing_left ? -1 : 1);
	const int32_t sy = -entity->scale_y;

	// TODO Unclear if we should rotate each corner of the box or just the center.
	// Rotating each corner would require supporting hitboxes that are not axis-aligned :(.
	const Vec2 center = Vec2 {
		sx * (box->x + w / 2.f),
		sy * (box->y + h / 2.f)
	}.rotate(entity->get_rotation());

	const float x0 = entity->get_position_x() + center.x - sx * w / 2.f;
	const float y0 = entity->get_position_y() + center.y - sy * h / 2.f;
	const float x1 = x0 + sx * w;
	const float y1 = y0 + sy * h;
	return Multibox::AABB{{{x0, y0}, {x1, y1}}};
}

Multibox::AABB make_throw_box(Entity *entity)
{
	float x1, x2, y1, y2;

	const float entity_x = static_cast<float>(entity->get_position_x());
	const float entity_y = static_cast<float>(entity->get_position_y());

	// What the game checks to tell if p1 can throw p2, on the X axis:
	// - There is at most `throw_range` units between the 2 players' pushboxes
	// - (p1.x + throw_box_offset_left) <= p2.x <= (p1.x + throw_box_offset_right)
	//
	// What we draw instead:
	// - Box starts at p1.x since you can never throw behind you anyway
	// - Box ends at the most restrictive position, either throw range or throw_box_offset_right.
	//	 When picking throw range, we extend it so that the middle of p2 pushbox is what needs to be visually in the box.

	x1 = entity_x;

	const float direction_multiplier = entity->facing_left ? -1.f : 1.f;
	const float x2_offset_based = entity_x + direction_multiplier * entity->attack_parameters.throw_box_right;

	float x2_range_based;
	{
		float throw_range = static_cast<float>(entity->attack_parameters.throw_range);
		if (entity->opponent && entity->opponent->pushbox_enabled)
		{
			throw_range += std::abs(entity->opponent->pushbox_left - entity->opponent->pushbox_right) / 2.f;
		}

		if (entity->facing_left)
		{
			x2_range_based = static_cast<float>(entity->pushbox_left - throw_range);
		}
		else
		{
			x2_range_based = static_cast<float>(entity->pushbox_right + throw_range);
		}
	}

	if (std::abs(entity_x - x2_offset_based) < std::abs(entity_x - x2_range_based))
	{
		x2 = x2_offset_based;
	}
	else
	{
		x2 = x2_range_based;
	}

	// What the game checks to tell if p1 can throw p2, on the Y axis:
	// - If throw_offset_* are configured sensically (ie. top > bottom): (p1.y + throw_box_offset_bottom) <= p2.y <= (p1.y + throw_box_offset_top)
	// - Otherwise: no vertical check at all
	//
	// What we draw instead:
	// - If throw_offset_* are configured sensically, we use those and add offset so that grabs connect when the drawn box overlaps the middle of p2 pushbox
	// - Othwerwise: we use p1 pushbox heights, with an arbitrary offset to avoid likely overlaps

	if (entity->attack_parameters.throw_box_bottom < entity->attack_parameters.throw_box_top)
	{
		y1 = entity_y + entity->attack_parameters.throw_box_top;
		y2 = entity_y + entity->attack_parameters.throw_box_bottom;
		if (entity->opponent && entity->opponent->pushbox_enabled)
		{
			const float pushbox_middle = std::abs(entity->opponent->pushbox_bottom + entity->opponent->pushbox_top) / 2.f;
			const int32_t opponent_y = entity->opponent->get_position_y();
			const float visual_aid = pushbox_middle - opponent_y;
			y1 += visual_aid;
			y2 += visual_aid;
		}
	}
	else
	{
		const float arbitrary_offset = 20000;
		y1 = static_cast<float>(entity->pushbox_top + arbitrary_offset);
		y2 = static_cast<float>(entity->pushbox_bottom - arbitrary_offset);
	}

	const Vec2 top_left = {std::min(x1, x2), std::min(y1, y2)};
	const Vec2 bottom_right = {std::max(x1, x2), std::max(y1, y2)};
	return Multibox::AABB{top_left, bottom_right};
}

void HitboxViewer::cycle_display_mode()
{
	display_mode = (HitboxDisplayMode)(((uint8_t)display_mode + 1) % (uint8_t)HitboxDisplayMode::Count);
}

bool HitboxViewer::update(const Battle *battle)
{
	if (battle->is_freeze_frame())
	{
		return false;
	}

	lines.clear();

	EntityHitboxes raw_data;
	for (int32_t entity_index = 0; entity_index < battle->num_entities; entity_index++)
	{
		Entity *entity = battle->entities[entity_index];
		add_entity(battle, entity, entity->is_active(), raw_data);
	}

	for (auto &[entity, multiboxes] : raw_data)
	{
		for (auto &[box_type, multibox] : multiboxes)
		{
			multibox.clip();
		}
	}

	auto battle_to_ue_coords = [](const Vec2 &p) -> Vec3
	{
		return {
			.x = BATTLE_TO_UE_SCALE * p.x,
			.y = 0.f,
			.z = BATTLE_TO_UE_SCALE * p.y,
		};
	};

	for (auto &[entity, multiboxes] : raw_data)
	{
		for (auto &[box_type, multibox] : multiboxes)
		{
			for (auto &line_2d : multibox.get_lines())
			{
				Line &line = lines.emplace_back();
				line.type = box_type;
				if (entity == battle->get_player1() || entity->parent_character == battle->get_player1())
				{
					line.owner = HitboxOwner::P1;
				}
				else
				{
					line.owner = HitboxOwner::P2;
				}
				line.vertices = {battle_to_ue_coords(line_2d[0]), battle_to_ue_coords(line_2d[1])};
			}
		}
	}

	return true;
}

void HitboxViewer::add_entity(const Battle *battle, Entity *entity, bool is_active, EntityHitboxes &out_hitboxes)
{
	is_active |= entity->is_active();

	const bool collected_strike_boxes = out_hitboxes[entity].contains(HitboxType::STRIKE);
	const bool collected_grab_boxes = out_hitboxes[entity].contains(HitboxType::GRAB);
	if (is_active && !collected_strike_boxes && !collected_grab_boxes)
	{
		if (!entity->attack_parameters.is_grab)
		{
			for (uint32_t box_index = 0; box_index < entity->num_hitboxes; box_index++)
			{
				Box *box = (entity->hitboxes + box_index);
				out_hitboxes[entity][HitboxType::STRIKE].add_box(entity_box_to_aabb(entity, box));
			}
		}
		else if (entity->attack_parameters.throw_range >= 0)
		{
			out_hitboxes[entity][HitboxType::GRAB].add_box(make_throw_box(entity));
		}
	}

	if (!out_hitboxes[entity].contains(HitboxType::HURT))
	{
		if (!entity->is_strike_invincible() && !entity->on_the_floor)
		{
			for (uint32_t box_index = 0; box_index < entity->num_hurtboxes; box_index++)
			{
				Box *box = (entity->hurtboxes + box_index);
				out_hitboxes[entity][HitboxType::HURT].add_box(entity_box_to_aabb(entity, box));
			}
		}
	}

	if (entity->pushbox_enabled && !out_hitboxes[entity].contains(HitboxType::PUSH))
	{
		Multibox::AABB box = {{
			{static_cast<float>(entity->pushbox_left), static_cast<float>(entity->pushbox_top)},
			{static_cast<float>(entity->pushbox_right), static_cast<float>(entity->pushbox_bottom)},
		}};
		out_hitboxes[entity][HitboxType::PUSH].add_box(box);
	}

	if (entity->attached && battle->is_entity_valid(entity->attached))
	{
		add_entity(battle, entity->attached, is_active, out_hitboxes);
	}
}

const std::vector<HitboxViewer::Line> &HitboxViewer::get_lines() const
{
	return lines;
}

#if UE_BUILD_TEST || FRAME_METER_AUTOMATED_TESTS

static const std::map<HitboxType, std::string> hitbox_type_to_string = {
	{HitboxType::PUSH, std::string("push")},
	{HitboxType::HURT, std::string("hurt")},
	{HitboxType::STRIKE, std::string("strike")},
	{HitboxType::GRAB, std::string("grab")},
};
static_assert((int32_t)HitboxType::COUNT == 4, "Update hitbox serialization");

static HitboxCapture *capture = nullptr;

void HitboxCapture::begin_capture()
{
	reset();
	capture = new HitboxCapture();
}

void HitboxCapture::update(const HitboxViewer &viewer, bool is_in_combat)
{
	if (!capture)
	{
		return;
	}
	if (is_in_combat)
	{
		capture->record_frame(viewer);
	}
	else if (capture->frames.size() > 0)
	{
		capture->write_to_disk();
		reset();
	}
}

void HitboxCapture::reset()
{
	if (capture)
	{
		delete capture;
		capture = nullptr;
	}
}

void HitboxCapture::record_frame(const HitboxViewer &viewer)
{
	const std::vector<HitboxViewer::Line> lines = viewer.get_lines();
	frames.emplace_back(lines.begin(), lines.end());
}

const std::string &serialize_hitbox_type(HitboxType type)
{
	return hitbox_type_to_string.at(type);
}

HitboxType deserialize_hitbox_type(const std::string &type)
{
	for (const auto &[k, v] : hitbox_type_to_string)
	{
		if (v == type)
		{
			return k;
		}
	}
	return HitboxType::STRIKE;
}

void HitboxCapture::serialize(std::ostream &stream) const
{
	int32_t frame_index = 1;
	for (const auto &frame : frames)
	{
		stream << std::format("# Frame {}\n", frame_index);
		for (const HitboxViewer::Line &line : frame)
		{
			const std::string &box_type = serialize_hitbox_type(line.type);
			const Vec3 &from = line.vertices[0];
			const Vec3 &to = line.vertices[1];
			stream << std::format("p{} {} ({}, {}, {}) -> ({}, {}, {})\n", (uint8_t)line.owner, box_type, from.x, from.y, from.z, to.x, to.y, to.z);
		}
		stream << "\n";
		frame_index++;
	}
}

HitboxCapture HitboxCapture::deserialize(std::istream &stream)
{
	HitboxCapture capture;

	const std::regex frame_header("# Frame \\d+");
	const std::regex hitbox_line("p(\\d) (\\w+) \\((.*), (.*), (.*)\\) -> \\((.*), (.*), (.*)\\)");

	for (std::string line; std::getline(stream, line);)
	{
		std::smatch match;
		if (std::regex_search(line, match, frame_header))
		{
			capture.frames.emplace_back();
		}
		else if (std::regex_search(line, match, hitbox_line))
		{
			if (!capture.frames.empty())
			{
				const HitboxOwner owner = static_cast<HitboxOwner>(std::stoi(match[1]));
				const HitboxType type = deserialize_hitbox_type(match[2]);
				const Vec3 from = Vec3{std::stof(match[3]), std::stof(match[4]), std::stof(match[5])};
				const Vec3 to = Vec3{std::stof(match[6]), std::stof(match[7]), std::stof(match[8])};
				capture.frames.back().emplace(HitboxViewer::Line{
					.owner = owner,
					.type = type,
					.vertices = {from, to},
				});
			}
		}
	}

	return capture;
}

void HitboxCapture::write_to_disk() const
{
	std::ofstream output;
	output.open("capture.hitboxes");
	serialize(output);
}
#endif
