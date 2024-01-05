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
	DrawContext(UObject *hud);

	static inline const float ui_width = 1920.f;
	static inline const float ui_height = 1080.f;

	void draw_rect(int32_t color, float x, float y, float width, float height) const;
	void draw_text(int32_t color, float x, float y, const std::wstring &text, float text_size) const;

protected:
	static inline UFunction *draw_rect_internal = nullptr;
	static inline UFunction *draw_text_internal = nullptr;
	static inline UFunction *get_player_controller = nullptr;
	static inline UFunction *get_viewport_size = nullptr;
	static inline UFont *small_font = nullptr;

	UObject *hud = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;
	float scaling_factor = 1.f;
};
