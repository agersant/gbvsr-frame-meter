#pragma once

#include <Unreal/AActor.hpp>

#include "bitmask.h"
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
		KirimomiUpper = 0x39,
		WallBound = 0x3A,
		WallBoundDown,
		Hizakuzure = 0x41,
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

		// Triggering parry
		HajikareStand = 0x5F,
		HajikareCrouch,
		HajikareAir,

		LockWait = 0x63,   // Throw receive
		LockReject = 0x64, // Throw tech
		Shirimochi = 0x67, // Guard crush
		ShirimochiCrouch,

		ExDamage = 0x76, // Super receive

		Dodge = 0x85,
		EvasiveMove,
	};

	// ON_XXXX_INTRPT
	enum BBScriptInterrupt
	{
		ON_INITIALIZE = 0,
		ON_RECEIVE_HIT_1 = 41,
		ON_RECEIVE_HIT_2 = 64,
		ON_RECEIVE_HIT_3 = 96,
		MAX = 104,
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
		static constexpr size_t NUM_ENTITIES = 132;
		ARRAY_FIELD(0x1030, class Entity *[NUM_ENTITIES], entities);
	};

	class Entity
	{
	public:
		FIELD(0x110, uint32_t, num_hitboxes);
		BIT_FIELD(0x1AB, 0x04, cinematic_freeze);
		BIT_FIELD(0x1BC, 0x01, attack_hit_connecting);
		FIELD(0x25C, uint32_t, hitstop);
		FIELD(0x280, Character *, parent_character);
		FIELD(0x3B8, uint32_t, flags_1);
		FIELD(0x3BC, uint32_t, flags_2);
		FIELD(0x3C0, uint32_t, flags_3);
		FIELD(0x3C4, uint32_t, flags_4); // ?
		BIT_FIELD(0x3B8, 0x40000000, recovery);
		BIT_FIELD(0x3BC, 0x00000010, dodge_invincible);
		BIT_FIELD(0x3BC, 0x00000020, full_invincible);
		BIT_FIELD(0x3C0, 0x00080000, attacking);
		BIT_FIELD(0x3C0, 0x00000100, active_frames);
		BIT_FIELD(0x3C0, 0x00000002, defense_hit_connecting);
		BIT_FIELD(0x3C0, 0x10000000, defense_guard_connecting); // Other bits in same byte also good candidates
		FIELD(0x3D0, int32_t, position_x);
		FIELD(0x3D4, int32_t, position_y);
		BIT_FIELD(0x45C, 0x04, cinematic_attack);
		FIELD(0xEE8, Bitmask<BBScriptInterrupt::MAX>, bbscript_interrupts);
		ARRAY_FIELD(0x3EC0, char[20], action_name);
	};

	class Character : public Entity
	{
	public:
		FIELD(0xD020, EnableFlag, enable_flag);
		FIELD(0xD038, uint32_t, blockstun_duration);
		FIELD(0xD060, uint32_t, hitstun_duration);
		FIELD(0xDA54, ActionID, action_id);

		bool can_walk();
		bool can_attack();
		bool is_counterable();
		bool is_in_active_frames();
		bool is_recovering();
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

class UGameInstance : public RC::Unreal::UObject
{
};

enum class GameMode : uint8_t
{
	ARCADE = 0x02,
	TRAINING = 0x06,
	MENU = 0x0A,
	STORY = 0x14,
};

class UREDGameCommon : public UGameInstance
{
public:
	FIELD(0x318, GameMode, game_mode);
};
