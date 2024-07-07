#include "mod/draw.h"
#include "mod/sigscan.h"

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

FLinearColor &operator*=(FLinearColor &color, const float multiplier)
{
	color.r *= multiplier;
	color.g *= multiplier;
	color.b *= multiplier;
	return color;
}

FLinearColor operator*(FLinearColor color, float multiplier)
{
	return color *= multiplier;
}

FLinearColor operator*(float multiplier, FLinearColor color)
{
	return color *= multiplier;
}

DrawContext::DrawContext(UObject *hud, Camera camera) : hud(hud), camera(camera)
{
	if (!canvas_triangle_item_draw)
	{
		canvas_triangle_item_draw = find_function("48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8D 6C 24 C9 48 81 EC 00 01 00 00 48 83 79 40 00 48 8B DA");
	}

	if (TObjectPtr<UCanvas> *canvas_ptr = (TObjectPtr<UCanvas> *)hud->GetValuePtrByPropertyNameInChain(STR("Canvas")))
	{
		canvas = (*canvas_ptr).UnderlyingObjectPointer;
	}

	if (fonts.at(Typeface::Roboto) == nullptr)
	{
		fonts.at(Typeface::Roboto) = UObjectGlobals::FindObject<UFont>(nullptr, STR("/Engine/EngineFonts/Roboto.Roboto"));
	}

	if (fonts.at(Typeface::SkipStd) == nullptr)
	{
		fonts.at(Typeface::SkipStd) = UObjectGlobals::FindObject<UFont>(nullptr, STR("/Game/Shared/Font/JPN/FOT-SkipStd-D_Font.FOT-SkipStd-D_Font"));
	}

	if (!draw_line_internal)
	{
		draw_line_internal = hud->GetFunctionByNameInChain(FName(STR("DrawLine")));
	}

	if (!draw_rect_internal)
	{
		draw_rect_internal = hud->GetFunctionByNameInChain(FName(STR("DrawRect")));
	}

	if (!draw_text_internal)
	{
		draw_text_internal = hud->GetFunctionByNameInChain(FName(STR("DrawText")));
	}

	if (!get_text_size_internal)
	{
		get_text_size_internal = hud->GetFunctionByNameInChain(FName(STR("GetTextSize")));
	}

	if (!get_player_controller)
	{
		get_player_controller = hud->GetFunctionByNameInChain(FName(STR("GetOwningPlayerController")));
	}

	if (get_player_controller && !get_viewport_size)
	{
		UObject *player_controller = nullptr;
		hud->ProcessEvent(get_player_controller, &player_controller);
		if (player_controller)
		{
			get_viewport_size = player_controller->GetFunctionByNameInChain(FName(STR("GetViewportSize")));
		}
	}

	if (get_player_controller && get_viewport_size)
	{
		UObject *player_controller = nullptr;
		hud->ProcessEvent(get_player_controller, &player_controller);
		if (player_controller)
		{
			struct Size
			{
				int32_t width;
				int32_t height;
			} viewport_size;
			player_controller->ProcessEvent(get_viewport_size, &viewport_size);
			scaling_factor = viewport_size.height / ui_height;
		}
	}
}

void DrawContext::draw_line(const FLinearColor &color, float x1, float y1, float x2, float y2, float thickness) const
{
	struct DrawLineParams
	{
		float x1;
		float y1;
		float x2;
		float y2;
		FLinearColor color;
		float thickness;
	} params = {
		.x1 = x1 * scaling_factor,
		.y1 = y1 * scaling_factor,
		.x2 = x2 * scaling_factor,
		.y2 = y2 * scaling_factor,
		.color = color,
		.thickness = thickness,
	};

	if (hud && draw_line_internal)
	{
		hud->ProcessEvent(draw_line_internal, &params);
	}
}

void DrawContext::draw_rect(const FLinearColor &color, float x, float y, float width, float height) const
{
	struct DrawRectParams
	{
		FLinearColor color;
		float x;
		float y;
		float w;
		float h;
	} params = {
		.color = color,
		.x = x * scaling_factor,
		.y = y * scaling_factor,
		.w = width * scaling_factor,
		.h = height * scaling_factor,
	};

	if (hud && draw_rect_internal)
	{
		hud->ProcessEvent(draw_rect_internal, &params);
	}
}

void DrawContext::draw_triangles(const FLinearColor &color, const std::vector<std::array<Vec2, 3>> &triangles) const
{
	// TODO apply ui scale

	struct FCanvasUVTri
	{
		FVector2D v0_pos;
		FVector2D v0_uv;
		FLinearColor v0_color;

		FVector2D v1_pos;
		FVector2D v1_uv;
		FLinearColor v1_color;

		FVector2D v2_pos;
		FVector2D v2_uv;
		FLinearColor v2_color;
	};

	struct FCanvasItem
	{
		void *vptr;
		FVector2D position;
		uint32_t stereo_depth;
		ESimpleElementBlendmode blend_mode;
		bool freeze_time;
		void *batched_element_parameters;
		FLinearColor color;
	};

	struct FCanvasTriangleItem : public FCanvasItem
	{
		void *texture;
		void *material_render_proxy;
		void *batched_element_parameters;
		TArray<FCanvasUVTri> triangle_list;
	};

	FCanvasTriangleItem triangle_item = {};
	triangle_item.blend_mode = ESimpleElementBlendmode::SE_BLEND_Opaque;

	TObjectPtr<UObject> *texture_ptr = canvas->GetValuePtrByPropertyName<TObjectPtr<UObject>>(STR("DefaultTexture"));
	if (texture_ptr)
	{
		triangle_item.texture = texture_ptr->UnderlyingObjectPointer;
	}

	for (const auto &triangle : triangles)
	{
		FCanvasUVTri &canvas_triangle = triangle_item.triangle_list.AddDefaulted_GetRef();
		canvas_triangle.v0_pos = {triangle[0].x, triangle[0].y};
		canvas_triangle.v1_pos = {triangle[1].x, triangle[1].y};
		canvas_triangle.v2_pos = {triangle[2].x, triangle[2].y};
		canvas_triangle.v0_color = color;
		canvas_triangle.v1_color = color;
		canvas_triangle.v2_color = color;
	}

	if (canvas_triangle_item_draw && canvas && canvas->inner_canvas)
	{
		using CanvasTriangleItemDraw_sig = void (*)(FCanvasTriangleItem *, FCanvas *);
		// TODO This causes a crash while rendering the frame, seemingly in `SetTextureParameter` from ShaderParameterUtils
		// Dissassembly doesn't seem to match stock UE :(
		((CanvasTriangleItemDraw_sig)canvas_triangle_item_draw)(&triangle_item, canvas->inner_canvas);
	}
}

void DrawContext::draw_text(const FLinearColor &color, float x, float y, const std::wstring &text, Typeface typeface, float size) const
{
	UFont *font = fonts.at(typeface);
	if (font == nullptr)
	{
		return;
	}

	int32_t *size_prop = font->GetValuePtrByPropertyName<int32_t>(STR("LegacyFontSize"));
	const int32_t original_size = *size_prop;
	*size_prop = int32_t(std::max(1.f, ceil(scaling_factor * size)));
	const float scale = (scaling_factor * size) / *size_prop;

	struct DrawTextParams
	{
		FString text;
		FLinearColor color;
		float x;
		float y;
		UFont *font;
		float scale;
		bool scale_position;
	} params = {
		.text = FString::FString(text.c_str()),
		.color = color,
		.x = x * scaling_factor,
		.y = y * scaling_factor,
		.font = font,
		.scale = scale,
		.scale_position = false,
	};

	if (hud && draw_text_internal)
	{
		hud->ProcessEvent(draw_text_internal, &params);
	}

	*size_prop = original_size;
}

void DrawContext::draw_outlined_text(const FLinearColor &color, const FLinearColor &outline_color, float x, float y, const std::wstring &text, Typeface typeface, float size) const
{
	draw_text(outline_color, x - 1, y, text, typeface, size);
	draw_text(outline_color, x + 1, y, text, typeface, size);
	draw_text(outline_color, x, y - 1, text, typeface, size);
	draw_text(outline_color, x, y + 1, text, typeface, size);
	draw_text(color, x, y, text, typeface, size);
}

TextSize DrawContext::get_text_size(const std::wstring &text, Typeface typeface, float size) const
{
	TextSize text_size;
	UFont *font = fonts.at(typeface);
	if (font == nullptr)
	{
		return text_size;
	}

	int32_t *size_prop = font->GetValuePtrByPropertyName<int32_t>(STR("LegacyFontSize"));
	const int32_t original_size = *size_prop;
	*size_prop = int32_t(std::max(1.f, ceil(size)));
	const float scale = size / *size_prop;

	struct GetTextSizeParams
	{
		FString text;
		float out_width;
		float out_height;
		UFont *font;
		float scale;
	} params = {
		.text = FString::FString(text.c_str()),
		.out_width = 0.f,
		.out_height = 0.f,
		.font = font,
		.scale = scale,
	};

	if (hud && get_text_size_internal)
	{
		hud->ProcessEvent(get_text_size_internal, &params);
		text_size.width = params.out_width;
		text_size.height = params.out_height;
	}

	*size_prop = original_size;

	return text_size;
}

Vec2 DrawContext::project(Vec3 point) const
{
	Vec2 projection = camera.project(point);
	projection.x *= ui_width;
	projection.y *= ui_height;
	return projection;
}
