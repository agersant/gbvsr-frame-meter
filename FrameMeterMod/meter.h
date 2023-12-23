#include <polyhook2/Detour/x64Detour.hpp>

#include <Unreal/AActor.hpp>

class AGameState : public RC::Unreal::AActor
{
};

class AREDGameState_Battle : public AGameState
{
};

class Meter
{
public:
	static Meter *get_instance();
	void init();

	Meter(Meter &other) = delete;
	void operator=(const Meter &) = delete;

protected:
	static Meter *instance;
	Meter();

	std::unique_ptr<PLH::x64Detour> update_battle_detour = nullptr;
	uint64_t update_battle_original;
	static void update_battle_callback(AREDGameState_Battle *battle, float delta_time);
	void update_battle(AREDGameState_Battle *battle, float delta_time);

	bool enabled = false;
};
