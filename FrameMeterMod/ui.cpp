#include "ui.h"

static std::map<CharacterState, int32_t> palette = {
	{CharacterState::IDLE, 0x222222},
	{CharacterState::COUNTER, 0x00FF00},
	{CharacterState::ACTIVE_HITBOX, 0xFF0000},
	{CharacterState::PUNISH_COUNTER, 0x0000FF},
	{CharacterState::STUN, 0xFFFF00},
};

void UI::draw(const DrawContext &context, const FrameMeter &frame_meter)
{
	float x = 100.f;
	float y = 100.f;
	const float frame_width = 10.f;
	const float frame_height = 20.f;
	const float frame_spacing = 2.f;

	for (int i = 0; i < Page::SIZE; i++)
	{
		const CharacterState state = frame_meter.current_page.player_1[i];
		context.draw_rect(palette[state], x, y, frame_width, frame_height);
		x += frame_width + frame_spacing;
	}
}
