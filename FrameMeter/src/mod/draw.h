#pragma once

#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>

#include "core/math.h"

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

struct LinearColor
{
	float r;
	float g;
	float b;
	float a;

	static LinearColor from_srgb(int32_t srgb);
};

LinearColor &operator*=(LinearColor &color, const float multiplier);
LinearColor operator*(const LinearColor &color, float multiplier);
LinearColor operator*(float multiplier, const LinearColor &color);

struct TextSize
{
	float width = 0.f;
	float height = 0.f;
};

struct DrawContext
{

public:
	DrawContext(UObject *hud, Camera camera);

	static inline const float ui_width = 1920.f;
	static inline const float ui_height = 1080.f;

	void draw_rect(const LinearColor &color, float x, float y, float width, float height) const;
	void draw_text(const LinearColor &color, float x, float y, const std::wstring &text, Typeface typeface, float size) const;
	void draw_line(const LinearColor &color, float x1, float y1, float x2, float y2, float thickness) const;
	void draw_outlined_text(const LinearColor &color, const LinearColor &outline_color, float x, float y, const std::wstring &text, Typeface typeface, float size) const;
	TextSize get_text_size(const std::wstring &text, Typeface typeface, float size) const;

	Vec2 project(Vec3 point) const;

private:
	static inline UFunction *draw_line_internal = nullptr;
	static inline UFunction *draw_rect_internal = nullptr;
	static inline UFunction *draw_text_internal = nullptr;
	static inline UFunction *get_text_size_internal = nullptr;
	static inline UFunction *get_player_controller = nullptr;
	static inline UFunction *get_viewport_size = nullptr;
	static inline std::map<Typeface, UFont *> fonts = {
		{Typeface::Roboto, nullptr},
		{Typeface::SkipStd, nullptr},
	};

	Camera camera;
	UObject *hud = nullptr;
	float scaling_factor = 1.f;
};
