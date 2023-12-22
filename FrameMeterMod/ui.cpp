#include <DynamicOutput/DynamicOutput.hpp>
#include <polyhook2/Virtuals/VFuncSwapHook.hpp>
#include <Unreal/AActor.hpp>
#include <Unreal/AGameModeBase.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>

#include "ui.h"

using namespace RC;
using namespace RC::Unreal;

const uint8_t HUD_VTABLE_INDEX_POST_RENDER = 0x6C0 / 8;

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

UI::UI()
{
}

UI *UI::instance = nullptr;
UI *UI::get_instance()
{
	if (instance == nullptr)
	{
		instance = new UI();
	}
	return instance;
}

void UI::init()
{
	Hook::RegisterInitGameStatePostCallback(
		[&](AGameModeBase *GameState)
		{
			if (hud_hook != nullptr)
			{
				return;
			}

			UClass *hud_class = UObjectGlobals::StaticFindObject<UClass *>(nullptr, nullptr, L"/Script/RED.REDHUD_Battle");
			if (!hud_class)
			{
				Output::send<LogLevel::Warning>(STR("REDHUD_Battle not found\n"));
				return;
			}

			UObject *default_hud = hud_class->GetClassDefaultObject();
			if (!default_hud)
			{
				Output::send<LogLevel::Warning>(STR("HUD DefaultObject not found\n"));
				return;
			}

			draw_rect = default_hud->GetFunctionByNameInChain(FName(STR("DrawRect")));
			if (!draw_rect)
			{
				Output::send<LogLevel::Warning>(STR("Could not find DrawRect() function\n"));
				return;
			}

			hud_hook = std::make_unique<PLH::VFuncSwapHook>(
				(uintptr_t)default_hud,
				PLH::VFuncMap({{HUD_VTABLE_INDEX_POST_RENDER, (uintptr_t)&UI::draw_callback}}),
				&hud_original_functions);

			if (!hud_hook->hook())
			{
				Output::send<LogLevel::Warning>(STR("Failed to install HUD hook\n"));
			}
		});
}

void UI::draw_callback(uintptr_t hud)
{
	if (hud)
	{
		instance->draw((AActor *)hud);
	}
}

void UI::draw(AActor *hud)
{
	if (hud_original_functions.contains(HUD_VTABLE_INDEX_POST_RENDER))
	{
		using HUDPostRender_sig = void (*)(uintptr_t);
		((HUDPostRender_sig)hud_original_functions.at(HUD_VTABLE_INDEX_POST_RENDER))((uintptr_t)hud);
	}

	if (draw_rect)
	{
		FLinearColor color = {.r = 1.f, .a = 1.f};
		DrawRectParams rect = {.color = color, .x = 100, .y = 100, .w = 100, .h = 100};
		hud->ProcessEvent(draw_rect, &rect);
	}
}
