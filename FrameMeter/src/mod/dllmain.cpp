#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"

// Included here as workaround for conflict between UE4SS macro named S and polyhook source
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Virtuals/VFuncSwapHook.hpp>

#include <DynamicOutput/Output.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>

#include "core/battle.h"
#include "core/dump.h"
#include "core/meter.h"
#include "mod/debug.h"
#include "mod/draw.h"
#include "mod/game.h"
#include "mod/ui.h"

using namespace RC;
using namespace RC::Unreal;

static std::unique_ptr<PLH::x64Detour> update_battle_detour = nullptr;
static uint64_t update_battle_original;

static std::unique_ptr<PLH::x64Detour> reset_battle_detour = nullptr;
static uint64_t reset_battle_original;

const uint8_t HUD_VTABLE_INDEX_POST_RENDER = 0x6C0 / 8;
static std::unique_ptr<PLH::VFuncSwapHook> hud_post_render_hook = nullptr;
static PLH::VFuncMap hud_original_functions = {};

static FrameMeter frame_meter = {};
static bool meter_visible = true;

void update_battle(AREDGameState_Battle *game_state, float delta_time)
{
	using UpdateBattle_sig = void (*)(AREDGameState_Battle *, float);
	((UpdateBattle_sig)update_battle_original)(game_state, delta_time);
	if (is_meter_allowed())
	{
		if (!is_paused(game_state))
		{
			frame_meter.update(game_state->battle);
#if UE_BUILD_TEST
			print_battle_data(game_state);
			if (just_pressed(VK_F8))
			{
				DumpWriter::begin_dump();
			}
			DumpWriter::update(game_state->battle, frame_meter);
#endif
		}
		if (is_hud_visible(game_state) && just_pressed(VK_F4))
		{
			meter_visible = !meter_visible;
		}
	}
}

void reset_battle(Battle *battle, int32_t *param)
{
	using ResetBattle_sig = void (*)(Battle *, int32_t *);
	((ResetBattle_sig)reset_battle_original)(battle, param);
	if (is_meter_allowed())
	{
		frame_meter.reset();
		frame_meter.continuous = is_replay_mode();
		frame_meter.advantage_enabled = is_training_mode();
	}
}

void post_render(AActor *hud)
{
	if (hud_original_functions.contains(HUD_VTABLE_INDEX_POST_RENDER))
	{
		using HUDPostRender_sig = void (*)(uintptr_t);
		((HUDPostRender_sig)hud_original_functions.at(HUD_VTABLE_INDEX_POST_RENDER))((uintptr_t)hud);
	}

	if (is_meter_allowed() && is_hud_visible(hud) && meter_visible)
	{
		DrawContext draw_context(hud);
		draw_frame_meter(draw_context, frame_meter);
	}
}

std::unique_ptr<PLH::x64Detour> setup_detour(uint64_t callback, uint64_t *trampoline, const char *signature)
{
	std::unique_ptr<PLH::x64Detour> detour = nullptr;

	SignatureContainer signature_container{
		{{signature}},
		[&](const SignatureContainer &self)
		{
			detour = std::make_unique<PLH::x64Detour>(
				(uint64_t)self.get_match_address(),
				callback,
				trampoline);
			detour->hook();
			return true;
		},
		[](SignatureContainer &self) {},
	};
	SinglePassScanner::SignatureContainerMap signature_containers = {
		{ScanTarget::MainExe, {signature_container}},
	};
	SinglePassScanner::start_scan(signature_containers);

	return detour;
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

			UClass *hud_class = UObjectGlobals::StaticFindObject<UClass *>(nullptr, nullptr, STR("/Script/RED.REDHUD_Battle"));
			if (!hud_class)
			{
				return;
			}

			UObject *default_hud = hud_class->GetClassDefaultObject();
			if (!default_hud)
			{
				return;
			}

			hud_post_render_hook = std::make_unique<PLH::VFuncSwapHook>(
				(uintptr_t)default_hud,
				PLH::VFuncMap({{HUD_VTABLE_INDEX_POST_RENDER, (uintptr_t)&post_render}}),
				&hud_original_functions);

			hud_post_render_hook->hook();
		});
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
		update_battle_detour = setup_detour((uint64_t)&update_battle, &update_battle_original, "40 57 41 54 41 55 48 83 EC 70 80 B9 C0 0A 00 00 01 48 8B F9 44 0F 29 44 24");
		reset_battle_detour = setup_detour((uint64_t)&reset_battle, &reset_battle_original, "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8D 6C 24 C9 48 81 EC C0 00 00 00 48 8B 05 5D 18 91 04 48 33 C4");
		hook_ui_render();
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
		reset_battle_detour.reset();
		hud_post_render_hook.reset();
		delete mod;
	}
}
