#pragma once

#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>

#include "core/math.h"
#include "core/struct_util.h"

using namespace RC::Unreal;

int32_t multiply_color(int32_t color, float multiplier);

class UFont : public UObject
{
};

enum class ECanvasDrawMode : uint8_t
{
	Deferred,
	Immediate,
};

struct FCanvas
{
};

class UCanvas : public UObject
{
public:
	FIELD(0x278, FCanvas *, inner_canvas);
};

enum class Typeface : uint8_t
{
	Roboto,
	SkipStd,
};

struct FVector2D
{
	float x;
	float y;
};

struct FLinearColor
{
	float r;
	float g;
	float b;
	float a;

	static FLinearColor from_srgb(int32_t srgb);
};

FLinearColor &operator*=(FLinearColor &color, const float multiplier);
FLinearColor operator*(const FLinearColor &color, float multiplier);
FLinearColor operator*(float multiplier, const FLinearColor &color);

enum ESimpleElementBlendmode : int
{
	SE_BLEND_Opaque = 0,
	SE_BLEND_Masked,
	SE_BLEND_Translucent,
	SE_BLEND_Additive,
	SE_BLEND_Modulate,
	SE_BLEND_MaskedDistanceField,
	SE_BLEND_MaskedDistanceFieldShadowed,
	SE_BLEND_TranslucentDistanceField,
	SE_BLEND_TranslucentDistanceFieldShadowed,
	SE_BLEND_AlphaComposite,
	SE_BLEND_AlphaHoldout,
	SE_BLEND_AlphaBlend,
	SE_BLEND_TranslucentAlphaOnly,
	SE_BLEND_TranslucentAlphaOnlyWriteAlpha,
	SE_BLEND_RGBA_MASK_START,
	SE_BLEND_RGBA_MASK_END = SE_BLEND_RGBA_MASK_START + 31,
	SE_BLEND_MAX

};

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

	void draw_line(const FLinearColor &color, float x1, float y1, float x2, float y2, float thickness) const;
	void draw_rect(const FLinearColor &color, float x, float y, float width, float height) const;
	void draw_triangles(const FLinearColor &color, const std::vector<std::array<Vec2, 3>> &triangles) const;
	void draw_text(const FLinearColor &color, float x, float y, const std::wstring &text, Typeface typeface, float size) const;
	void draw_outlined_text(const FLinearColor &color, const FLinearColor &outline_color, float x, float y, const std::wstring &text, Typeface typeface, float size) const;
	TextSize get_text_size(const std::wstring &text, Typeface typeface, float size) const;

	Vec2 project(Vec3 point) const;

private:
	static inline uint64_t canvas_triangle_item_draw = 0;
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
	UCanvas *canvas = nullptr;
	float scaling_factor = 1.f;
};
