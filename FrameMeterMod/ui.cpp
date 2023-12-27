#include "ui.h"

static std::map<CharacterState, int32_t> palette = {
	{CharacterState::IDLE, 0x222222},
	{CharacterState::COUNTER, 0x00FF00},
	{CharacterState::ACTIVE_HITBOX, 0xFF0000},
	{CharacterState::PUNISH_COUNTER, 0x0000FF},
	{CharacterState::STUN, 0xFFFF00},
	{CharacterState::MOVEMENT, 0x00FFFF},
	{CharacterState::FULL_INVINCIBLE, 0xFF00FF},
};

static constexpr float frame_width = 10.f;
static constexpr float frame_height = 20.f;
static constexpr float frame_spacing = 2.f;
static constexpr float player_spacing = 10.f;

void draw_page_player(const DrawContext &context, float x, float y, const std::array<CharacterState, Page::SIZE> &frames);

void UI::draw(const DrawContext &context, const FrameMeter &frame_meter)
{
	float x = 100.f;
	float y = 100.f;

	draw_page_player(context, x, y, frame_meter.current_page.player_1);
	y += frame_height + player_spacing;
	draw_page_player(context, x, y, frame_meter.current_page.player_2);
}

void draw_page_player(const DrawContext &context, float x, float y, const std::array<CharacterState, Page::SIZE> &frames)
{

	for (int i = 0; i < frames.size(); i++)
	{
		const CharacterState state = frames[i];
		context.draw_rect(palette[state], x, y, frame_width, frame_height);
		x += frame_width + frame_spacing;
	}
}
