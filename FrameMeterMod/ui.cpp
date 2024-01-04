#include "ui.h"

static const std::map<CharacterState, int32_t> current_page_palette = {
	{CharacterState::IDLE, 0x1A1A1A},
	{CharacterState::COUNTER, 0x00B796},
	{CharacterState::ACTIVE_HITBOX, 0xCC2B67},
	{CharacterState::PUNISH_COUNTER, 0x0170BE},
	{CharacterState::STUN, 0xFFF830},
	{CharacterState::MOVEMENT, 0x42F8FD},
	{CharacterState::INVINCIBLE, 0xF1F1F0},
	{CharacterState::PROJECTILE, 0xC98002},
};

static const float darkening = 0.7f;
static const std::map<CharacterState, int32_t> previous_page_palette = {
	{CharacterState::IDLE, current_page_palette.at(CharacterState::IDLE)},
	{CharacterState::COUNTER, multiply_color(current_page_palette.at(CharacterState::COUNTER), darkening)},
	{CharacterState::ACTIVE_HITBOX, multiply_color(current_page_palette.at(CharacterState::ACTIVE_HITBOX), darkening)},
	{CharacterState::PUNISH_COUNTER, multiply_color(current_page_palette.at(CharacterState::PUNISH_COUNTER), darkening)},
	{CharacterState::STUN, multiply_color(current_page_palette.at(CharacterState::STUN), darkening)},
	{CharacterState::MOVEMENT, multiply_color(current_page_palette.at(CharacterState::MOVEMENT), darkening)},
	{CharacterState::INVINCIBLE, multiply_color(current_page_palette.at(CharacterState::INVINCIBLE), darkening)},
	{CharacterState::PROJECTILE, multiply_color(current_page_palette.at(CharacterState::PROJECTILE), darkening)},
};

static constexpr float border = 4.f;
static constexpr float frame_width = 14.f;
static constexpr float frame_height = 24.f;
static constexpr float frame_spacing = 1.f;
static constexpr float player_spacing = 8.f;

void draw_player(const DrawContext &context, float x, float y, const FrameMeter &frame_meter, size_t player_index);
void draw_player_frames(const DrawContext &context, float x, float y, const Player &player, bool fill_row, const std::map<CharacterState, int32_t> &palette);

void UI::draw(const DrawContext &context, const FrameMeter &frame_meter)
{
	float x = 100.f;
	float y = 100.f;

	draw_player(context, x, y, frame_meter, 0);
	y += 2 * border + frame_height + player_spacing;
	draw_player(context, x, y, frame_meter, 1);
}

void draw_player(const DrawContext &context, float x, float y, const FrameMeter &frame_meter, size_t player_index)
{
	const float background_width = 2 * border + PAGE_SIZE * (frame_width + frame_spacing) - frame_spacing;
	const float background_height = 2 * border + frame_height;
	context.draw_rect(0x000000, x, y, background_width, background_height);
	x += border;
	y += border;

	const bool has_previous_page = frame_meter.previous_page.has_value();
	if (has_previous_page)
	{
		draw_player_frames(context, x, y, frame_meter.previous_page.value().players[player_index], true, previous_page_palette);
	}

	draw_player_frames(context, x, y, frame_meter.current_page.players[player_index], !has_previous_page, current_page_palette);
}

void draw_player_frames(const DrawContext &context, float x, float y, const Player &player, bool fill_row, const std::map<CharacterState, int32_t> &palette)
{
	for (int i = 0; i < PAGE_SIZE; i++)
	{
		const Frame &frame = player.frames[i];
		if (i > player.num_frames && !fill_row)
		{
			break;
		}

		context.draw_rect(palette.at(frame.state), x, y, frame_width, frame_height);

		if (frame.state != CharacterState::IDLE && i < player.num_frames && frame.span_length.has_value())
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
				const float font_scale = 1.4f;
				const float dx = 2.f;
				const float dy = 1.f;
				context.draw_text(0x000000, x + dx - 1, y + dy, digit_string, font_scale);
				context.draw_text(0x000000, x + dx + 1, y + dy, digit_string, font_scale);
				context.draw_text(0x000000, x + dx, y + dy - 1, digit_string, font_scale);
				context.draw_text(0x000000, x + dx, y + dy + 1, digit_string, font_scale);
				context.draw_text(0xFFFFFF, x + dx, y + dy, digit_string, font_scale);
			}
		}

		x += frame_width + frame_spacing;
	}
}
