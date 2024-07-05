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
	const int32_t sx = -entity->scale_x * (entity->facing_left ? -1 : 1);
	const int32_t sy = -entity->scale_y;
	const float x0 = entity->get_position_x() + sx * box->x;
	const float y0 = entity->get_position_y() + sy * box->y;
	const float x1 = x0 + sx * box->w;
	const float y1 = y0 + sy * box->h;
	return Multibox::AABB{{{x0, y0}, {x1, y1}}};
}

bool HitboxViewer::update(const Battle *battle)
{
	if (battle->is_freeze_frame())
	{
		return false;
	}

	box_data.clear();

	for (int32_t entity_index = 0; entity_index < battle->num_entities; entity_index++)
	{
		Entity *entity = battle->entities[entity_index];
		add_entity(battle, entity, entity->is_active());
	}

	for (auto &[entity, multiboxes] : box_data)
	{
		for (auto &[box_type, multibox] : multiboxes)
		{
			multibox.clip();
		}
	}

	return true;
}

void HitboxViewer::add_entity(const Battle *battle, Entity *entity, bool is_active)
{
	is_active |= entity->is_active();

	const bool collected_strike_boxes = box_data[entity].contains(HitboxType::STRIKE);
	const bool collected_grab_boxes = box_data[entity].contains(HitboxType::GRAB);
	if (is_active && !collected_strike_boxes && !collected_grab_boxes)
	{
		const HitboxType type = entity->attack_parameters.is_grab ? HitboxType::GRAB : HitboxType::STRIKE;
		for (uint32_t box_index = 0; box_index < entity->num_hitboxes; box_index++)
		{
			Box *box = (entity->hitboxes + box_index);
			box_data[entity][type].add_box(entity_box_to_aabb(entity, box));
		}
	}

	if (!box_data[entity].contains(HitboxType::HURT))
	{
		if (!entity->is_strike_invincible() && !entity->on_the_floor)
		{
			for (uint32_t box_index = 0; box_index < entity->num_hurtboxes; box_index++)
			{
				Box *box = (entity->hurtboxes + box_index);
				box_data[entity][HitboxType::HURT].add_box(entity_box_to_aabb(entity, box));
			}
		}
	}

	if (entity->pushbox_enabled && !box_data[entity].contains(HitboxType::PUSH))
	{
		Multibox::AABB box = {{
			{static_cast<float>(entity->pushbox_left), static_cast<float>(entity->pushbox_top)},
			{static_cast<float>(entity->pushbox_right), static_cast<float>(entity->pushbox_bottom)},
		}};
		box_data[entity][HitboxType::PUSH].add_box(box);
	}

	if (entity->attached && battle->is_entity_valid(entity->attached))
	{
		add_entity(battle, entity->attached, is_active);
	}
}

std::vector<HitboxViewer::Line> HitboxViewer::get_lines() const
{
	std::vector<Line> lines = {};

	auto battle_to_ue_coords = [](const Vec2 &p) -> Vec3
	{
		return {
			.x = BATTLE_TO_UE_SCALE * p.x,
			.y = 0.f,
			.z = BATTLE_TO_UE_SCALE * p.y,
		};
	};

	for (auto &[entity, multiboxes] : box_data)
	{
		for (auto &[box_type, multibox] : multiboxes)
		{
			for (auto &line : multibox.get_lines())
			{
				lines.push_back(Line{box_type, {battle_to_ue_coords(line[0]), battle_to_ue_coords(line[1])}});
			}
		}
	}

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
		for (const auto &[type, segment] : frame)
		{
			const std::string &box_type = serialize_hitbox_type(type);
			const Vec3 &from = segment[0];
			const Vec3 &to = segment[1];
			stream << std::format("{} ({}, {}, {}) -> ({}, {}, {})\n", box_type, from.x, from.y, from.z, to.x, to.y, to.z);
		}
		stream << "\n";
		frame_index++;
	}
}

HitboxCapture HitboxCapture::deserialize(std::istream &stream)
{
	HitboxCapture capture;

	const std::regex frame_header("# Frame \\d+");
	const std::regex hitbox_line("(\\w+) \\((.*), (.*), (.*)\\) -> \\((.*), (.*), (.*)\\)");

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
				const HitboxType type = deserialize_hitbox_type(match[1]);
				const Vec3 from = Vec3{std::stof(match[2]), std::stof(match[3]), std::stof(match[4])};
				const Vec3 to = Vec3{std::stof(match[5]), std::stof(match[6]), std::stof(match[7])};
				capture.frames.back().emplace(HitboxViewer::Line{type, {from, to}});
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
