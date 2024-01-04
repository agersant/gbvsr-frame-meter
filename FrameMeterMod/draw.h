#pragma once

#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>

using namespace RC::Unreal;

int32_t multiply_color(int32_t color, float multiplier);

class UFont : public UObject
{
};

struct FLinearColor
{
	float r;
	float g;
	float b;
	float a;

	static FLinearColor from_srgb(int32_t srgb);
};

struct DrawRectParams
{
	FLinearColor color;
	float x;
	float y;
	float w;
	float h;
};

struct DrawTextParams
{
	FString text;
	FLinearColor color;
	float x;
	float y;
	UFont *font;
	float scale;
	bool scale_position;
};

struct DrawContext
{

public:
	DrawContext(UObject *hud, UFunction *draw_rect, UFunction *draw_text, UFont *small_font)
		: hud(hud),
		  draw_rect_internal(draw_rect),
		  draw_text_internal(draw_text),
		  small_font(small_font)
	{
	}

	void draw_rect(int32_t color, float x, float y, float width, float height) const;
	void draw_text(int32_t color, float x, float y, const std::wstring &text, float scale = 1.f) const;

protected:
	UObject *hud;
	UFunction *draw_rect_internal;
	UFunction *draw_text_internal;
	UFont *small_font;
};
