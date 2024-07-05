#pragma once

#include "core/bitmask.h"
#include "core/struct_util.h"

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
	JumpLandingStiff,

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

	ExDamage = 0x76,	 // Super receive
	ExDamageLand = 0x77, // Beatrix 5U receive

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

struct Team
{
	FIELD(0x08, struct Character *, main_player_object);

private:
	char pad[0x78];
};
static_assert(sizeof(Team) == 0x78);

struct Battle
{
private:
	static const int32_t MAX_ENTITIES = 131;
	char pad[0x14C0 + Battle::MAX_ENTITIES * sizeof(void *)];

public:
	ARRAY_FIELD(0x10, Team[2], teams);
	FIELD(0x1018, int32_t, num_entities);
	ARRAY_FIELD(0x14C0, struct Entity *[MAX_ENTITIES], entities);

	bool is_freeze_frame() const;
	bool is_entity_valid(Entity *entity) const;
};

struct FrameInfo
{
public:
	FIELD(0x2D, uint8_t, num_hurtboxes);
	FIELD(0x2F, uint8_t, num_hitboxes);
};

enum class BoxType : uint8_t
{
	HURTBOX,
	HITBOX,
};

struct Box
{
public:
	BoxType type;
	float x;
	float y;
	float w;
	float h;
	char pad[4];
};

enum class AttackType : int32_t
{
	Unknown,
	Normal,
	Special,
	Super,
};

enum class HitHandlerType : int32_t
{
	Ignore,
	Parry,
};

struct AttackParameters
{
	FIELD(0x0, AttackType, attack_type);
	FIELD(0x0C, int32_t, damage);
	FIELD(0x10, int32_t, flags_1); // APFLG
	FIELD(0x14, int32_t, flags_2); // APFLG2
	BIT_FIELD(0x14, 0x04, is_grab_cinematic);
	BIT_FIELD(0x14, 0x80, is_grab);
	FIELD(0x18, int32_t, flags_3); // APFLG3
	BIT_FIELD(0x18, 0x10, opponent_must_be_grounded);
	BIT_FIELD(0x18, 0x20, opponent_must_be_airborne);
	FIELD(0x1C, int32_t, flags_4); // APFLG4
	FIELD(0x20, int32_t, flags_5); // APFLG5
	FIELD(0x38, int32_t, enemy_guard_hitstop);
	FIELD(0x40, int32_t, enemy_blockstun);
};

struct Entity
{
	FIELD(0x20, bool, is_player);
	FIELD(0x60, FrameInfo *, frame_info);
	FIELD(0x78, Box *, hurtboxes);
	FIELD(0x80, Box *, hitboxes);
	FIELD(0x10C, uint32_t, num_hurtboxes);
	FIELD(0x110, uint32_t, num_hitboxes);
	BIT_FIELD(0x1AB, 0x04, cinematic_freeze);
	BIT_FIELD(0x1AC, 0x00200000, slowdown_bonus_frame);
	BIT_FIELD(0x1BC, 0x01, attack_hit_connecting);
	FIELD(0x25C, uint32_t, hitstop);
	FIELD(0x280, Character *, parent_character);
	FIELD(0x2A8, Entity *, attach_parent);
	FIELD(0x2F0, Entity *, attached);
	FIELD(0x308, Entity *, puppet);
	FIELD(0x3B8, uint32_t, flags_1);
	FIELD(0x3BC, uint32_t, flags_2);
	FIELD(0x3C0, uint32_t, flags_3);
	FIELD(0x3C4, uint32_t, flags_4);  // ?
	BIT_FIELD(0x3B8, 0x01, airborne); // ?
	BIT_FIELD(0x3B8, 0x40000000, recovery);
	BIT_FIELD(0x3BC, 0x00000010, strike_invincible); // except crossups
	BIT_FIELD(0x3BC, 0x00000020, throw_invincible);
	BIT_FIELD(0x3BC, 0x00000040, full_invincible); // except crossups
	BIT_FIELD(0x3C0, 0x00080000, attacking);
	BIT_FIELD(0x3C0, 0x00000100, active_frames);
	BIT_FIELD(0x3C0, 0x00000400, on_the_floor);
	BIT_FIELD(0x3C0, 0x00000002, defense_hit_connecting);
	BIT_FIELD(0x3C0, 0x10000000, defense_guard_connecting); // Other bits in same byte also good candidates
	FIELD(0x3CC, bool, facing_left);
	FIELD(0x3D0, int32_t, offfset_x);
	FIELD(0x3D4, int32_t, offfset_y);
	FIELD(0x3F4, int32_t, scale_x);
	FIELD(0x3F8, int32_t, scale_y);
	BIT_FIELD(0x45C, 0x04, cinematic_attack);
	FIELD(0x7A0, AttackParameters, attack_parameters);
	BIT_FIELD(0xEA8, 0x02, has_hit_handler);
	FIELD(0xEAC, HitHandlerType, hit_handler_type);
	FIELD(0xF08, Bitmask<BBScriptInterrupt::MAX>, bbscript_interrupts);
	ARRAY_FIELD(0x3EE0, char[20], action_name);

	int32_t get_position_x() const;
	int32_t get_position_y() const;
	bool is_active() const;
	bool has_armor() const;
	bool is_any_invincible() const;
	bool is_strike_invincible() const;

private:
	char pad[0xD110];
};

struct Character : public Entity
{
	FIELD(0xD130, EnableFlag, enable_flag);
	FIELD(0xD148, uint32_t, blockstun_duration);
	FIELD(0xD170, uint32_t, hitstun_duration);
	FIELD(0xDB84, ActionID, action_id);
	FIELD(0xE82C, int32_t, slowdown_remaining);

	bool can_walk() const;
	bool can_attack() const;
	bool is_air_blocking() const;
	bool is_idle() const;
	bool is_counterable() const;
	bool is_recovering() const;
	bool is_in_blockstun() const;
	bool is_in_hitstun() const;
	bool is_maneuvering() const;

private:
	char pad[0xE82C + sizeof(int32_t) - sizeof(Entity)];
};
