#pragma once

#include <Unreal/AActor.hpp>

#include "struct_util.h"

namespace ASW
{
	enum EnableFlag : uint32_t
	{
		ForwardWalk = 0x4,
		NormalAttack = 0x1000,
	};

	enum ActionID : uint32_t
	{
		MidGuardPre = 0x53,
		MidGuardLoop,
		MidGuardEnd,
		HighGuardPre,
		HighGuardLoop,
		HighGuardEnd,
		CrouchGuardPre,
		CrouchGuardLoop,
		CrouchGuardEnd,
		AirGuardPre,
		AirGuardLoop,
		AirGuardEnd,
	};

	class Player
	{
	public:
		FIELD(0x08, class Character *, character);
	};

	class Engine
	{
	public:
		FIELD(0x10, class Player, player_1);
		FIELD(0x88, class Player, player_2);
	};

	class Entity
	{
	public:
		FIELD(0x110, uint32_t, num_hitboxes);
		FIELD(0x25C, uint32_t, hitstop);
		FIELD(0x3B8, uint32_t, flags_1);
		FIELD(0x3C0, uint32_t, flags_2);
		BIT_FIELD(0x3B8, 0x40000000, recovery);
		BIT_FIELD(0x3C0, 0x080000, attacking);
		BIT_FIELD(0x3C0, 0x0100, active_frames);
		ARRAY_FIELD(0x3EC0, char[20], action_name);
	};

	class Character : public Entity
	{
	public:
		FIELD(0xD020, EnableFlag, enable_flag);
		FIELD(0xDA54, ActionID, action_id);

		bool can_act();
		bool is_in_blockstun();
		bool is_in_hitstun();
	};
}

class AGameState : public RC::Unreal::AActor
{
};

class AREDGameState_Battle : public AGameState
{
public:
	FIELD(0xA10, class ASW::Engine *, engine);
};
