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
		JumpPre = 0x6,
		Jump,
		JumpLanding,

		FDash = 0xC,
		FDashStop,
		BDash,
		BDashStop,

		// Hitstun
		NokezoriHighLv1 = 0x12,
		NokezoriHighLv2,
		NokezoriHighLv3,
		NokezoriHighLv4,
		NokezoriHighLv5,
		NokezoriLowLv1,
		NokezoriLowLv2,
		NokezoriLowLv3,
		NokezoriLowLv4,
		NokezoriLowLv5,
		NokezoriCrouchLv1,
		NokezoriCrouchLv2,
		NokezoriCrouchLv3,
		NokezoriCrouchLv4,
		NokezoriCrouchLv5,

		// Knockdowns
		BDownUpper = 0x21,
		BDownUpperEnd,
		BDownDown,
		BDownBound,
		BDownLoop,
		BDown2Stand,
		FDownUpper,
		FDownUpperEnd,
		FDownDown,
		FDownBound,
		FDownLoop,
		FDown2Stand,
		VDownUpper,
		VDownUpperEnd,
		VDownDown,
		VDownBound,
		VDownLoop,
		Blowoff,
		WallBound = 0x3A,
		WallBoundDown,
		Ukemi = 0x46,
		UkemiLeap = 0x48,
		UkemiLeap2,

		// Blockstun
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

		LockWait = 0x63,   // Throw receive
		Shirimochi = 0x67, // Guard crush
		ShirimochiCrouch,

		ExDamage = 0x76, // Super receive

		Dodge = 0x85,
		EvasiveMove,
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
		FIELD(0x3BC, uint32_t, flags_2);
		FIELD(0x3C0, uint32_t, flags_3);
		BIT_FIELD(0x3B8, 0x40000000, recovery);
		BIT_FIELD(0x3BC, 0x10, dodge_invincible);
		BIT_FIELD(0x3BC, 0x20, full_invincible);
		BIT_FIELD(0x3C0, 0x080000, attacking);
		BIT_FIELD(0x3C0, 0x0100, active_frames);
		ARRAY_FIELD(0x3EC0, char[20], action_name);
	};

	class Character : public Entity
	{
	public:
		FIELD(0xD020, EnableFlag, enable_flag);
		FIELD(0xDA54, ActionID, action_id);

		bool can_walk();
		bool can_attack();
		bool is_invincible();
		bool is_in_blockstun();
		bool is_in_hitstun();
		bool is_maneuvering();
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
