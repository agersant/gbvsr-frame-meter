#include "ui.h"

static const int32_t advantage_plus_color = 0x7FD1FA;
static const int32_t advantage_minus_color = 0xFC9594;
static const std::map<CharacterState, int32_t> current_page_palette = {
	{CharacterState::IDLE, 0x202020}, // Should be 0x1A1A1A, but UE doesn't render these 1:1. Perceptually adjusted.
	{CharacterState::COUNTER, 0x00B796},
	{CharacterState::ACTIVE_HITBOX, 0xCC2B67},
	{CharacterState::PUNISH_COUNTER, 0x0170BE},
	{CharacterState::STUN, 0xFFF830},
	{CharacterState::MOVEMENT, 0x42F8FD},
	{CharacterState::INVINCIBLE, 0xF1F1F0},
	{CharacterState::PROJECTILE, 0xC98002},
};

static const float darkening = 0.6f;
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

static constexpr float bottom_margin = 140.f;
static constexpr float border = 4.f;
static constexpr float frame_width = 14.f;
static constexpr float frame_height = 24.f;
static constexpr float frame_spacing = 1.f;
static constexpr float player_spacing = 8.f;
static constexpr float advantage_text_size = 15.f;
static constexpr float advantage_text_margin = 5.f;

float draw_advantage(const DrawContext &context, float x, float y, const FrameMeter &frame_meter)
{
	const wchar_t *caption_text = STR("Advantage");
	const Typeface typeface = Typeface::SkipStd;
	const float size = advantage_text_size;
	TextSize computed_size = context.get_text_size(caption_text, typeface, size);
	context.draw_outlined_text(0xFFFFFF, 0x000000, x, y, caption_text, typeface, size);
	x += computed_size.width + 8.f;

	const std::optional<int32_t> advantage = frame_meter.advantage;
	std::wstring advantage_text = STR("--");
	int32_t color = 0xFFFFFF;
	if (advantage.has_value())
	{
		advantage_text = std::to_wstring(abs(*advantage)) + STR("F");
		if (*advantage > 0)
		{
			color = advantage_plus_color;
		}
		else if (*advantage < 0)
		{
			color = advantage_minus_color;
		}
	}
	context.draw_outlined_text(color, 0x000000, x, y, advantage_text, typeface, size);

	return computed_size.height;
}

void draw_frame(const DrawContext &context, float x, float y, size_t index, const Frame *frame, const std::map<CharacterState, int32_t> *palette)
{
	if (frame && frame->highlight)
	{
		context.draw_rect(0xFFFFFF, x, y - border, frame_width + frame_spacing, frame_height + 2 * border);
	}

	const int32_t color = palette && frame ? palette->at(frame->state) : current_page_palette.at(CharacterState::IDLE);
	context.draw_rect(color, x, y, frame_width, frame_height);

	if (frame && frame->state != CharacterState::IDLE && frame->span_length.has_value())
	{
		const int32_t span_length = frame->span_length.value();
		const int32_t num_digits = int32_t(log10(span_length) + 1);
		const int32_t available_space = span_length + std::min(0, frame->span_start_index);
		const int32_t index_in_span = index - frame->span_start_index;
		if (span_length >= 4 && num_digits <= available_space && index_in_span + num_digits >= span_length)
		{
			const int32_t power = (span_length - 1) - index_in_span;
			const int32_t digit = (span_length / int32_t(pow(10, power))) % 10;
			const std::wstring digit_string = std::to_wstring(digit);
			const Typeface typeface = Typeface::Roboto;
			const float size = 16.f;
			context.draw_outlined_text(0xFFFFFF, 0x000000, x + 1.f, y, digit_string, typeface, size);
		}
	}
}

void draw_player(const DrawContext &context, float x, float y, const Player &player)
{
	const float background_width = 2 * border + PAGE_SIZE * (frame_width + frame_spacing) - frame_spacing;
	const float background_height = 2 * border + frame_height;
	context.draw_rect(0x000000, x, y, background_width, background_height);
	x += border;
	y += border;

	for (int i = 0; i < PAGE_SIZE; i++)
	{
		const Frame *frame = nullptr;
		const std::map<CharacterState, int32_t> *palette = nullptr;
		if (i < player.current_page.num_frames)
		{
			palette = &current_page_palette;
			frame = &player.current_page.frames[i];
		}
		else if (player.previous_page.has_value() && i > player.current_page.num_frames)
		{
			palette = &previous_page_palette;
			frame = &player.previous_page->frames[i];
		}
		draw_frame(context, x, y, i, frame, palette);
		x += frame_width + frame_spacing;
	}
}

void draw_frame_meter(const DrawContext &context, const FrameMeter &frame_meter)
{
	const float meter_width = 2 * border + PAGE_SIZE * (frame_width + frame_spacing) - frame_spacing;
	const float meter_height = 2 * (2 * border + frame_height) + player_spacing;
	float x = round((context.ui_width - meter_width) / 2.f);
	float y = context.ui_height - bottom_margin - meter_height;

	if (frame_meter.advantage_enabled)
	{
		y -= advantage_text_size + advantage_text_margin;
		y += draw_advantage(context, x, y, frame_meter) + advantage_text_margin;
	}

	draw_player(context, x, y, frame_meter.players[0]);
	y += 2 * border + frame_height + player_spacing;
	draw_player(context, x, y, frame_meter.players[1]);
}
