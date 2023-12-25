#pragma once

#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>

using namespace RC::Unreal;

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

struct DrawContext
{

public:

	DrawContext(UObject *hud, UFunction *draw_rect) : hud(hud), draw_rect_internal(draw_rect)
	{
	}

	void draw_rect(int32_t color, float x, float y, float width, float height) const;

protected:

	UObject *hud;
	UFunction *draw_rect_internal;
};
