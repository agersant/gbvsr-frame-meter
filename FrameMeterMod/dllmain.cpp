// Included here as workaround for conflict between UE4SS macro named S and polyhook source
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Virtuals/VFuncSwapHook.hpp>

#include <SigScanner/SinglePassSigScanner.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>

#include "draw.h"
#include "game.h"
#include "meter.h"
#include "ui.h"

using namespace RC;
using namespace RC::Unreal;

static std::unique_ptr<PLH::x64Detour> update_battle_detour = nullptr;
static uint64_t update_battle_original;

const uint8_t HUD_VTABLE_INDEX_POST_RENDER = 0x6C0 / 8;
static std::unique_ptr<PLH::VFuncSwapHook> hud_post_render_hook = nullptr;
static PLH::VFuncMap hud_original_functions = {};

static FrameMeter frame_meter = {};

static bool is_training_mode()
{
	UREDGameCommon *game_instance = (UREDGameCommon *)UObjectGlobals::FindFirstOf(L"REDGameCommon");
	return game_instance && game_instance->game_mode == GameMode::TRAINING;
}

static void update_battle(AREDGameState_Battle *battle, float delta_time)
{
	using UpdateBattle_sig = void (*)(AREDGameState_Battle *, float);
	((UpdateBattle_sig)update_battle_original)(battle, delta_time);
	if (is_training_mode())
	{
		frame_meter.update(battle);
	}
}

static void post_render(uintptr_t hud_ptr)
{
	if (hud_original_functions.contains(HUD_VTABLE_INDEX_POST_RENDER))
	{
		using HUDPostRender_sig = void (*)(uintptr_t);
		((HUDPostRender_sig)hud_original_functions.at(HUD_VTABLE_INDEX_POST_RENDER))((uintptr_t)hud_ptr);
	}

	if (is_training_mode())
	{
		UObject *hud = (UObject *)hud_ptr;
		UFunction *draw_rect_original = hud->GetFunctionByNameInChain(FName(STR("DrawRect")));
		UFunction *draw_text_original = hud->GetFunctionByNameInChain(FName(STR("DrawText")));

		DrawContext draw_context(hud, draw_rect_original, draw_text_original);
		UI::draw(draw_context, frame_meter);
	}
}

class FrameMeterMod : public RC::CppUserModBase
{
public:
	FrameMeterMod() : RC::CppUserModBase()
	{
		ModName = STR("FrameMeterMod");
		ModVersion = STR("1.0");
		ModDescription = STR("Adds a frame meter tool in training mode.");
		ModAuthors = STR("agersant");
	}

	void on_unreal_init() override
	{
		hook_battle_updates();
		hook_ui_render();
	}

	void hook_battle_updates()
	{
		SignatureContainer update_battle_signature{
			{{"40 57 41 54 41 55 48 83 EC 70 80 B9 C0 0A 00 00 01 48 8B F9 44 0F 29 44 24"}},
			[&](const SignatureContainer &self)
			{
				update_battle_detour = std::make_unique<PLH::x64Detour>(
					(uint64_t)self.get_match_address(),
					(uint64_t)&update_battle,
					&update_battle_original);
				update_battle_detour->hook();
				return true;
			},
			[](SignatureContainer &self) {},
		};
		SinglePassScanner::SignatureContainerMap signature_containers = {
			{ScanTarget::MainExe, {update_battle_signature}},
		};
		SinglePassScanner::start_scan(signature_containers);
	}

	void hook_ui_render()
	{
		Hook::RegisterInitGameStatePostCallback(
			[&](AGameModeBase *GameMode)
			{
				if (hud_post_render_hook != nullptr)
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

				hud_post_render_hook = std::make_unique<PLH::VFuncSwapHook>(
					(uintptr_t)default_hud,
					PLH::VFuncMap({{HUD_VTABLE_INDEX_POST_RENDER, (uintptr_t)&post_render}}),
					&hud_original_functions);

				if (!hud_post_render_hook->hook())
				{
					Output::send<LogLevel::Warning>(STR("Failed to install HUD hook\n"));
				}
			});
	}

	~FrameMeterMod() override
	{
	}
};

extern "C"
{
	__declspec(dllexport) RC::CppUserModBase *start_mod()
	{
		return new FrameMeterMod();
	}

	__declspec(dllexport) void uninstall_mod(RC::CppUserModBase *mod)
	{
		update_battle_detour.reset();
		hud_post_render_hook.reset();
		delete mod;
	}
}
