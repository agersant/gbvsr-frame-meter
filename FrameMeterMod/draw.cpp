#include "draw.h"

FLinearColor FLinearColor::from_srgb(int32_t srgb)
{
	const float r = ((srgb >> 16) & 0xFF) / 255.f;
	const float g = ((srgb >> 8) & 0xFF) / 255.f;
	const float b = ((srgb >> 0) & 0xFF) / 255.f;
	return FLinearColor{
		.r = pow(r, 2.2f),
		.g = pow(g, 2.2f),
		.b = pow(b, 2.2f),
		.a = 1.f,
	};
}

void DrawContext::draw_rect(int32_t color, float x, float y, float width, float height) const
{
	DrawRectParams params = {
		.color = FLinearColor::from_srgb(color),
		.x = x,
		.y = y,
		.w = width,
		.h = height,
	};

	if (hud && draw_rect_internal)
	{
		hud->ProcessEvent(draw_rect_internal, &params);
	}
}

void DrawContext::draw_text(int32_t color, float x, float y, const std::wstring &text, float scale) const
{
	DrawTextParams params = {
		.text = FString::FString(text.c_str()),
		.color = FLinearColor::from_srgb(color),
		.x = x,
		.y = y,
		.font = small_font,
		.scale = scale,
		.scale_position = false,
	};

	if (hud && draw_text_internal)
	{
		hud->ProcessEvent(draw_text_internal, &params);
	}
}