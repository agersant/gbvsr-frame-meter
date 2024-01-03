#pragma once

#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>

using namespace RC::Unreal;

class UFont;

struct FLinearColor
{
	float r;
	float g;
	float b;
	float a;
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
	DrawContext(UObject *hud, UFunction *draw_rect, UFunction *draw_text)
		: hud(hud),
		  draw_rect_internal(draw_rect),
		  draw_text_internal(draw_text)
	{
	}

	void draw_rect(int32_t color, float x, float y, float width, float height) const;
	void draw_text(int32_t color, float x, float y, const std::wstring &text, UFont *font = nullptr, float scale = 1.f) const;

protected:
	UObject *hud;
	UFunction *draw_rect_internal;
	UFunction *draw_text_internal;
};
