#include "mod/ui.h"

static const FLinearColor black = FLinearColor::from_srgb(0x000000);
static const FLinearColor white = FLinearColor::from_srgb(0xFFFFFF);

static const FLinearColor advantage_plus_color = FLinearColor::from_srgb(0x7FD1FA);
static const FLinearColor advantage_minus_color = FLinearColor::from_srgb(0xFC9594);

static const std::map<CharacterState, FLinearColor> palette = {
	{CharacterState::IDLE, FLinearColor::from_srgb(0x202020)}, // Should be 0x1A1A1A, but UE doesn't render these 1:1. Perceptually adjusted.
	{CharacterState::COUNTER, FLinearColor::from_srgb(0x00B796)},
	{CharacterState::ACTIVE_HITBOX, FLinearColor::from_srgb(0xCC2B67)},
	{CharacterState::RECOVERY, FLinearColor::from_srgb(0x0170BE)},
	{CharacterState::STUN, FLinearColor::from_srgb(0xFFF830)},
	{CharacterState::MOVEMENT, FLinearColor::from_srgb(0x42F8FD)},
	{CharacterState::INVINCIBLE, FLinearColor::from_srgb(0xF1F1F0)},
	{CharacterState::PROJECTILE, FLinearColor::from_srgb(0xC98002)},
	{CharacterState::ARMOR, FLinearColor::from_srgb(0x5C1E6E)},
};

static const float bottom_margin = 140.f;
static const float border = 4.f;
static const float frame_width = 14.f;
static const float frame_height = 24.f;
static const float frame_spacing = 1.f;
static const float player_spacing = 8.f;
static const float advantage_text_size = 15.f;
static const float advantage_text_margin = 5.f;

float draw_advantage(const DrawContext &context, float x, float y, const FrameMeter &frame_meter)
{
	const wchar_t *caption_text = STR("Advantage");
	const Typeface typeface = Typeface::SkipStd;
	const float size = advantage_text_size;
	TextSize computed_size = context.get_text_size(caption_text, typeface, size);
	context.draw_outlined_text(white, black, x, y, caption_text, typeface, size);
	x += computed_size.width + 8.f;

	const std::optional<int32_t> advantage = frame_meter.advantage;
	std::wstring advantage_text = STR("--");
	FLinearColor color = white;
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
	context.draw_outlined_text(color, black, x, y, advantage_text, typeface, size);

	return computed_size.height;
}

void draw_frame(const DrawContext &context, float x, float y, size_t index, const Frame *frame, bool previous_page)
{
	if (frame && frame->highlight)
	{
		context.draw_rect(white, x, y - border, frame_width + frame_spacing, frame_height + 2 * border);
	}

	const CharacterState state = frame ? frame->state : CharacterState::IDLE;
	FLinearColor text_color = white;
	FLinearColor color = palette.at(state);
	if (previous_page && state != CharacterState::IDLE)
	{
		const float darkening = 0.2f;
		text_color *= darkening;
		color *= darkening;
	}

	context.draw_rect(color, x, y, frame_width, frame_height);

	if (state != CharacterState::IDLE && frame && frame->span_length.has_value())
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
			context.draw_outlined_text(text_color, black, x + 1.f, y, digit_string, typeface, size);
		}
	}
}

void draw_player(const DrawContext &context, float x, float y, const Player &player)
{
	const float background_width = 2 * border + PAGE_SIZE * (frame_width + frame_spacing) - frame_spacing;
	const float background_height = 2 * border + frame_height;
	context.draw_rect(black, x, y, background_width, background_height);
	x += border;
	y += border;

	for (int i = 0; i < PAGE_SIZE; i++)
	{
		const Frame *frame = nullptr;
		bool is_previous_page = false;
		if (i < player.current_page.num_frames)
		{
			frame = &player.current_page.frames[i];
		}
		else if (player.previous_page.has_value() && i > player.current_page.num_frames)
		{
			is_previous_page = true;
			frame = &player.previous_page->frames[i];
		}
		draw_frame(context, x, y, i, frame, is_previous_page);
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
