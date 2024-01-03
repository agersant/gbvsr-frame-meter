#include "ui.h"

static std::map<CharacterState, int32_t> palette = {
	{CharacterState::IDLE, 0x222222},
	{CharacterState::COUNTER, 0x00FF00},
	{CharacterState::ACTIVE_HITBOX, 0xFF0000},
	{CharacterState::PUNISH_COUNTER, 0x0000FF},
	{CharacterState::STUN, 0xFFFF00},
	{CharacterState::MOVEMENT, 0x00FFFF},
	{CharacterState::INVINCIBLE, 0xFFFFFF},
	{CharacterState::PROJECTILE, 0xFF8800},
};

static constexpr float frame_width = 10.f;
static constexpr float frame_height = 20.f;
static constexpr float frame_spacing = 2.f;
static constexpr float player_spacing = 10.f;

void draw_page_player(const DrawContext &context, float x, float y, const FrameMeter &frame_meter, const Page &page, size_t player_index);

void UI::draw(const DrawContext &context, const FrameMeter &frame_meter)
{
	float x = 100.f;
	float y = 100.f;

	draw_page_player(context, x, y, frame_meter, frame_meter.current_page, 0);
	y += frame_height + player_spacing;
	draw_page_player(context, x, y, frame_meter, frame_meter.current_page, 1);
}

void draw_page_player(const DrawContext &context, float x, float y, const FrameMeter &frame_meter, const Page &page, size_t player_index)
{
	for (int i = 0; i < Page::SIZE; i++)
	{
		const Frame &frame = page.players[player_index][i];
		context.draw_rect(palette[frame.state], x, y, frame_width, frame_height);

		if (frame.state != CharacterState::IDLE && i < page.num_frames && frame.span_length.has_value())
		{
			const int32_t span_length = frame.span_length.value();
			const int32_t num_digits = int32_t(log10(span_length) + 1);
			const int32_t available_space = span_length + std::min(0, frame.span_start_index);
			const int32_t index_in_span = i - frame.span_start_index;
			if (span_length >= 4 && num_digits <= available_space && index_in_span + num_digits >= span_length)
			{
				const int32_t power = (span_length - 1) - index_in_span;
				const int32_t digit = (span_length / int32_t(pow(10, power))) % 10;
				const std::wstring digit_string = std::to_wstring(digit);
				context.draw_text(0x000000, x - 1, y, digit_string);
				context.draw_text(0x000000, x + 1, y, digit_string);
				context.draw_text(0x000000, x, y - 1, digit_string);
				context.draw_text(0x000000, x, y + 1, digit_string);
				context.draw_text(0xFFFFFF, x, y, digit_string);
			}
		}

		x += frame_width + frame_spacing;
	}
}
