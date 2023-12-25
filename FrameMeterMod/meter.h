#include <polyhook2/Detour/x64Detour.hpp>

#include <Unreal/AActor.hpp>

#define FIELD(OFFSET, TYPE, NAME)                                                \
	void set_##OFFSET(std::add_const_t<std::add_lvalue_reference_t<TYPE>> value) \
	{                                                                            \
		*(std::add_pointer_t<TYPE>)((char *)this + OFFSET) = value;              \
	}                                                                            \
                                                                                 \
	void set_##OFFSET(std::add_rvalue_reference_t<TYPE> value)                   \
	{                                                                            \
		*(std::add_pointer_t<TYPE>)((char *)this + OFFSET) = std::move(value);   \
	}                                                                            \
                                                                                 \
	std::add_lvalue_reference_t<TYPE> get_##OFFSET() const                       \
	{                                                                            \
		return *(std::add_pointer_t<TYPE>)((char *)this + OFFSET);               \
	}                                                                            \
	__declspec(property(get = get_##OFFSET, put = set_##OFFSET)) TYPE NAME

class AGameState : public RC::Unreal::AActor
{
};

class AREDGameState_Battle : public AGameState
{
public:
	FIELD(0xA10, class ASWEngine *, engine);
};

class Player
{
public:
	FIELD(0x08, class Entity *, entity);
};

class ASWEngine
{
public:
	FIELD(0x10, class Player, player_1);
	FIELD(0x88, class Player, player_2);
};

class Entity
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
