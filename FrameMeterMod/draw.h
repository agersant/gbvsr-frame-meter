#pragma once

#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>

using namespace RC::Unreal;

int32_t multiply_color(int32_t color, float multiplier);

class UFont : public UObject
{
};

enum class Typeface : uint8_t
{
	Roboto,
	SkipStd,
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

struct GetTextSizeParams
{
	FString text;
	float out_width;
	float out_height;
	UFont *font;
	float scale;
};

struct TextSize
{
	float width = 0.f;
	float height = 0.f;
};

struct DrawContext
{

public:
	DrawContext(UObject *hud);

	static inline const float ui_width = 1920.f;
	static inline const float ui_height = 1080.f;

	void draw_rect(int32_t color, float x, float y, float width, float height) const;
	void draw_text(int32_t color, float x, float y, const std::wstring &text, Typeface typeface, float size) const;
	void draw_outlined_text(int32_t color, int32_t outline_color, float x, float y, const std::wstring &text, Typeface typeface, float size) const;
	TextSize get_text_size(const std::wstring &text, Typeface typeface, float size) const;

protected:
	static inline UFunction *draw_rect_internal = nullptr;
	static inline UFunction *draw_text_internal = nullptr;
	static inline UFunction *get_text_size_internal = nullptr;
	static inline UFunction *get_player_controller = nullptr;
	static inline UFunction *get_viewport_size = nullptr;
	static inline std::map<Typeface, UFont *> fonts = {
		{Typeface::Roboto, nullptr},
		{Typeface::SkipStd, nullptr},
	};

	UObject *hud = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;
	float scaling_factor = 1.f;
};
