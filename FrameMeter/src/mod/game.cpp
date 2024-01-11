#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/World.hpp>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "mod/game.h"

static class UREDGameCommon *game_instance = nullptr;
static UFunction *get_world_settings_func = nullptr;
static UFunction *get_scalar_parameter_value_func = nullptr;
static std::pair<UWorld *, UObject *> hud_material = {};
static std::map<int32_t, bool> pressed_keys = {};

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

bool just_pressed(int32_t key)
{
	bool was_pressed = pressed_keys[key];
	pressed_keys[key] = GetAsyncKeyState(key) & 0x8000;
	return pressed_keys[key] && !was_pressed;
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

bool is_paused(AREDGameState_Battle *game_state)
{
	if (AActor *world_settings = get_world_settings(game_state))
	{
		TObjectPtr<AActor> *pauser = (TObjectPtr<AActor> *)world_settings->GetValuePtrByPropertyNameInChain(STR("PauserPlayerState"));
		return pauser != nullptr && pauser->UnderlyingObjectPointer != nullptr;
	}
	return true;
}

UObject *get_hud_material(AActor *actor)
{
	UWorld *world = actor->GetWorld();
	if (hud_material.first != world || hud_material.second == nullptr)
	{
		if (UObject *battle_hud_top_actor = UObjectGlobals::FindFirstOf(STR("BattleHudTop_C")))
		{
			UClass *static_mesh_component_class = UObjectGlobals::StaticFindObject<UClass *>(nullptr, nullptr, STR("/Script/Engine.StaticMeshComponent"));
			UObject *static_mesh_component = UObjectGlobals::FindObject(static_mesh_component_class, battle_hud_top_actor, STR("StaticMeshComponent0"));
			TArray<TObjectPtr<UObject>> *materials = (TArray<TObjectPtr<UObject>> *)static_mesh_component->GetValuePtrByPropertyNameInChain(STR("OverrideMaterials"));
			hud_material.first = world;
			hud_material.second = (*materials)[0].UnderlyingObjectPointer;
		}
	}
	return hud_material.first == world ? hud_material.second : nullptr;
}

bool is_hud_visible(AActor *actor)
{
	UObject *hud_material = get_hud_material(actor);
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
