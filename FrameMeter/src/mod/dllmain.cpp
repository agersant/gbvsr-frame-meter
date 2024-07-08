#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "psapi.h"

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
#include "core/hitbox.h"
#include "core/meter.h"
#include "mod/debug.h"
#include "mod/draw.h"
#include "mod/game.h"
#include "mod/ui.h"

using namespace RC;
using namespace RC::Unreal;

static const wchar_t supported_version[] = STR("Version:2023/01/03 Revision:13549:17447M");

static std::unique_ptr<PLH::x64Detour> update_battle_detour = nullptr;
static uint64_t update_battle_original;

static std::unique_ptr<PLH::x64Detour> reset_battle_detour = nullptr;
static uint64_t reset_battle_original;

const uint8_t HUD_VTABLE_INDEX_POST_RENDER = 0x6C0 / 8;
static std::unique_ptr<PLH::VFuncSwapHook> hud_post_render_hook = nullptr;
static PLH::VFuncMap hud_original_functions = {};

static FrameMeter frame_meter = {};
static HitboxViewer hitbox_viewer = {};

static bool frame_by_frame = false;

void update_battle(AREDGameState_Battle *game_state, float delta_time)
{
	const bool play_frame = !is_training_mode() || !frame_by_frame || just_pressed(VK_F6);
	const bool accept_input = is_mod_allowed() && !is_paused(game_state) && is_hud_visible(game_state->GetWorld());
	const bool read_battle_data = is_mod_allowed() && !is_paused(game_state) && play_frame;

	if (play_frame)
	{
		using UpdateBattle_sig = void (*)(AREDGameState_Battle *, float);
		((UpdateBattle_sig)update_battle_original)(game_state, delta_time);
	}

	if (accept_input)
	{
		if (just_pressed(VK_F3))
		{
			hitbox_viewer.cycle_display_mode();
		}
		if (just_pressed(VK_F4))
		{
			frame_meter.visible = !frame_meter.visible;
		}
#if UE_BUILD_TEST
		if (is_training_mode())
		{
			if (just_pressed(VK_F5))
			{
				frame_by_frame = !frame_by_frame;
			}
			if (just_pressed(VK_F8))
			{
				DumpWriter::begin_dump();
				HitboxCapture::begin_capture();
				FrameMeterCapture::begin_capture();
			}
		}
#endif
	}

	if (read_battle_data)
	{
		frame_meter.update(game_state->battle);
		hitbox_viewer.update(game_state->battle);

#if UE_BUILD_TEST
		Debug::on_battle_update(game_state);

		const bool is_in_combat = !frame_meter.is_at_rest();
		DumpWriter::update(game_state->battle, is_in_combat);

		if (!game_state->battle->is_freeze_frame())
		{
			HitboxCapture::update(hitbox_viewer, is_in_combat);
			FrameMeterCapture::update(frame_meter, is_in_combat);
		}
#endif
	}
}

void reset_battle(Battle *battle, int32_t *param)
{
	using ResetBattle_sig = void (*)(Battle *, int32_t *);
	((ResetBattle_sig)reset_battle_original)(battle, param);
	frame_by_frame = false;
#if UE_BUILD_TEST
	Debug::on_battle_reset(battle);
	DumpWriter::reset();
	HitboxCapture::reset();
	FrameMeterCapture::reset();
#endif
	frame_meter.reset();
	frame_meter.continuous = is_replay_mode();
	frame_meter.advantage_enabled = is_training_mode();
}

void post_render(AActor *hud)
{
	if (hud_original_functions.contains(HUD_VTABLE_INDEX_POST_RENDER))
	{
		using HUDPostRender_sig = void (*)(uintptr_t);
		((HUDPostRender_sig)hud_original_functions.at(HUD_VTABLE_INDEX_POST_RENDER))((uintptr_t)hud);
	}

	if (is_mod_allowed() && is_hud_visible(hud->GetWorld()))
	{
		DrawContext draw_context(hud, get_camera(hud->GetWorld()));
		if (is_2d_view(hud->GetWorld()))
		{
			UI::draw_hitbox_viewer(draw_context, hitbox_viewer);
		}
		UI::draw_frame_meter(draw_context, frame_meter);
	}
}

void post_init_game_state(AGameModeBase *GameMode)
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

bool check_game_version()
{
	MODULEINFO module_info;
	K32GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &module_info, sizeof(MODULEINFO));
	wchar_t game_version[512] = {0};
	wcscpy_s(game_version, sizeof(game_version), (wchar_t *)((char *)module_info.lpBaseOfDll + 0x0459FEC0));

	const bool is_expected_version = wcscmp(game_version, supported_version) == 0;
	if (!is_expected_version)
	{
		Output::send<LogLevel::Error>(STR("FrameMeterMod failed game version check. Expected '{}' but found '{}'.\n"), supported_version, game_version);
	}

	return is_expected_version;
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
		if (!check_game_version())
		{
			return;
		}
		update_battle_detour = setup_detour((uint64_t)&update_battle, &update_battle_original, "40 57 41 54 41 55 48 83 EC 70 80 B9 F8 0A 00 00 01 48 8B F9 44 0F 29 44 24");
		reset_battle_detour = setup_detour((uint64_t)&reset_battle, &reset_battle_original, "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 55 41 54 41 55 41 56 41 57 48 8D 6C 24 C9 48 81 EC C0 00 00 00 48 8B 05 FD CF 9C 04 48 33 C4 48 89 45 2F 45");
		Hook::RegisterInitGameStatePostCallback(&post_init_game_state);
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
