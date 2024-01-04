#include "draw.h"

int32_t multiply_color(int32_t color, float multiplier)
{
	uint8_t r = std::clamp(int(0xFF * multiplier * ((color >> 16) & 0xFF) / 255.f), 0, 0xFF);
	uint8_t g = std::clamp(int(0xFF * multiplier * ((color >> 8) & 0xFF) / 255.f), 0, 0xFF);
	uint8_t b = std::clamp(int(0xFF * multiplier * ((color >> 0) & 0xFF) / 255.f), 0, 0xFF);
	return (r << 16) + (g << 8) + b;
}

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

DrawContext::DrawContext(UObject *hud) : hud(hud)
{
	if (!small_font)
	{
		small_font = UObjectGlobals::StaticFindObject<UFont *>(nullptr, nullptr, L"/Engine/EngineFonts/Roboto.Roboto");
	}

	if (!draw_rect_internal)
	{
		draw_rect_internal = hud->GetFunctionByNameInChain(FName(STR("DrawRect")));
	}

	if (!draw_text_internal)
	{
		draw_text_internal = hud->GetFunctionByNameInChain(FName(STR("DrawText")));
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
			width = viewport_size.width;
			height = viewport_size.height;
		}
	}

	scaling_factor = height / ui_height;
}

void DrawContext::draw_rect(int32_t color, float x, float y, float width, float height) const
{
	DrawRectParams params = {
		.color = FLinearColor::from_srgb(color),
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

void DrawContext::draw_text(int32_t color, float x, float y, const std::wstring &text, float scale) const
{
	DrawTextParams params = {
		.text = FString::FString(text.c_str()),
		.color = FLinearColor::from_srgb(color),
		.x = x * scaling_factor,
		.y = y * scaling_factor,
		.font = small_font,
		.scale = scale * scaling_factor,
		.scale_position = false,
	};

	if (hud && draw_text_internal)
	{
		hud->ProcessEvent(draw_text_internal, &params);
	}
}