// Included here as workaround for conflict between UE4SS macro named S and polyhook source
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/Virtuals/VFuncSwapHook.hpp>

#include <DynamicOutput/Output.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/World.hpp>

#include "debug.h"
#include "draw.h"
#include "game.h"
#include "meter.h"
#include "ui.h"

using namespace RC;
using namespace RC::Unreal;

static UFunction *get_world_settings_func = nullptr;
static UFunction *get_scalar_parameter_value_func = nullptr;

static UREDGameCommon *game_instance = nullptr;
static std::map<UWorld *, UObject *> hud_material = {};

static std::unique_ptr<PLH::x64Detour> update_battle_detour = nullptr;
static uint64_t update_battle_original;

static std::unique_ptr<PLH::x64Detour> reset_battle_detour = nullptr;
static uint64_t reset_battle_original;

const uint8_t HUD_VTABLE_INDEX_POST_RENDER = 0x6C0 / 8;
static std::unique_ptr<PLH::VFuncSwapHook> hud_post_render_hook = nullptr;
static PLH::VFuncMap hud_original_functions = {};

static FrameMeter frame_meter = {};

class AWorldSettings : public AActor
{
};

UREDGameCommon *get_game_instance()
{
	if (!game_instance)
	{
		game_instance = (UREDGameCommon *)UObjectGlobals::FindFirstOf(L"REDGameCommon");
	}
	return game_instance;
}

AWorldSettings *get_world_settings(AActor *actor)
{
	UWorld *world = actor->GetWorld();
	if (!get_world_settings_func)
	{
		get_world_settings_func = world->GetFunctionByNameInChain(FName(STR("K2_GetWorldSettings")));
	}

	AWorldSettings *world_settings = nullptr;
	world->ProcessEvent(get_world_settings_func, &world_settings);
	return world_settings;
}

bool is_training_mode()
{
	UREDGameCommon *game_instance = get_game_instance();
	return game_instance && game_instance->game_mode == GameMode::TRAINING;
}

bool is_replay_mode()
{
	UREDGameCommon *game_instance = get_game_instance();
	return game_instance && game_instance->game_mode == GameMode::REPLAY;
}

bool is_meter_allowed()
{
	return is_training_mode() || is_replay_mode();
}

bool is_paused(AREDGameState_Battle *battle)
{
	if (AActor *world_settings = get_world_settings(battle))
	{
		TObjectPtr<AActor> *pauser = (TObjectPtr<AActor> *)world_settings->GetValuePtrByPropertyNameInChain(STR("PauserPlayerState"));
		return pauser != nullptr && pauser->UnderlyingObjectPointer != nullptr;
	}
	return true;
}

UObject *get_hud_material(AActor *hud)
{
	UWorld *world = hud->GetWorld();
	if (hud_material[world] == nullptr)
	{
		if (UObject *battle_hud_top_actor = UObjectGlobals::FindFirstOf(L"BattleHudTop_C"))
		{
			UClass *static_mesh_component_class = UObjectGlobals::StaticFindObject<UClass *>(nullptr, nullptr, STR("/Script/Engine.StaticMeshComponent"));
			UObject *static_mesh_component = UObjectGlobals::FindObject(static_mesh_component_class, battle_hud_top_actor, STR("StaticMeshComponent0"));
			TArray<TObjectPtr<UObject>> *materials = (TArray<TObjectPtr<UObject>> *)static_mesh_component->GetValuePtrByPropertyNameInChain(STR("OverrideMaterials"));
			hud_material[world] = (*materials)[0].UnderlyingObjectPointer;
		}
	};
	return hud_material[world];
}

bool is_hud_visible(AActor *hud)
{
	UObject *hud_material = get_hud_material(hud);
	if (!hud_material)
	{
		return false;
	}

	if (!get_scalar_parameter_value_func)
	{
		get_scalar_parameter_value_func = hud_material->GetFunctionByNameInChain(FName(STR("K2_GetScalarParameterValue")));
	}

	struct FGetScalarParameterValueParams
	{
		FName parameter_name;
		float value;
	} params;
	params.parameter_name = FName(STR("DrawFlag"));
	hud_material->ProcessEvent(get_scalar_parameter_value_func, &params);
	return params.value > 0.f;
}

void update_battle(AREDGameState_Battle *battle, float delta_time)
{
	using UpdateBattle_sig = void (*)(AREDGameState_Battle *, float);
	((UpdateBattle_sig)update_battle_original)(battle, delta_time);
	if (is_meter_allowed() && !is_paused(battle))
	{
		frame_meter.update(battle);
		// print_battle_data(battle);
	}
}

void reset_battle(ASW::Engine *engine, int32_t *param)
{
	using ResetBattle_sig = void (*)(ASW::Engine *, int32_t *);
	((ResetBattle_sig)reset_battle_original)(engine, param);
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

	if (is_meter_allowed() && is_hud_visible(hud))
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
			hud_material.clear();

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
