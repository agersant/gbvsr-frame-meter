#include "draw.h"

void DrawContext::draw_rect(int32_t color, float x, float y, float width, float height) const
{
	FLinearColor linear_color = {
		.r = ((color >> 16) & 0xFF) / 255.f,
		.g = ((color >> 8) & 0xFF) / 255.f,
		.b = ((color >> 0) & 0xFF) / 255.f,
		.a = 1.f,
	};

	DrawRectParams params = {
		.color = linear_color,
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

void DrawContext::draw_text(int32_t color, float x, float y, const std::wstring &text, class UFont *font, float scale) const
{
	FLinearColor linear_color = {
		.r = ((color >> 16) & 0xFF) / 255.f,
		.g = ((color >> 8) & 0xFF) / 255.f,
		.b = ((color >> 0) & 0xFF) / 255.f,
		.a = 1.f,
	};

	DrawTextParams params = {
		.text = FString::FString(text.c_str()),
		.color = linear_color,
		.x = x,
		.y = y,
		.font = font,
		.scale = scale,
		.scale_position = false,
	};

	if (hud && draw_text_internal)
	{
		hud->ProcessEvent(draw_text_internal, &params);
	}
}