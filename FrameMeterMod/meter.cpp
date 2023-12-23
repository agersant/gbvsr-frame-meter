#include <polyhook2/Detour/x64Detour.hpp>

#include <DynamicOutput/DynamicOutput.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>

#include "meter.h"

using namespace RC;
using namespace RC::Unreal;

Meter::Meter()
{
}

Meter *Meter::instance = nullptr;
Meter *Meter::get_instance()
{
	if (instance == nullptr)
	{
		instance = new Meter();
	}
	return instance;
}

void Meter::init()
{
	SignatureContainer update_battle_signature{
		{{"40 57 41 54 41 55 48 83 EC 70 80 B9 C0 0A 00 00 01 48 8B F9 44 0F 29 44 24"}},
		[&](const SignatureContainer &self)
		{
			update_battle_detour = std::make_unique<PLH::x64Detour>(
				(uint64_t)self.get_match_address(),
				(uint64_t)&Meter::update_battle_callback,
				&update_battle_original);
			update_battle_detour->hook();
			Output::send<LogLevel::Warning>(STR("Detoured update battle\n"));
			return true;
		},
		[](SignatureContainer &self) {},
	};

	SinglePassScanner::SignatureContainerMap signature_containers = {
		{ScanTarget::MainExe, {update_battle_signature}},
	};
	SinglePassScanner::start_scan(signature_containers);
}

void Meter::update_battle_callback(AREDGameState_Battle *battle, float delta_time)
{
	get_instance()->update_battle(battle, delta_time);
}

void Meter::update_battle(AREDGameState_Battle *battle, float delta_time)
{
	Output::send<LogLevel::Warning>(STR("UPDATE BATTLE {}\n"), delta_time);
	using UpdateBattle_sig = void (*)(AREDGameState_Battle *, float);
	((UpdateBattle_sig)update_battle_original)(battle, delta_time);
}
