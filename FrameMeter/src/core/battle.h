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

	Character *get_player1() const;
	Character *get_player2() const;
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
	BIT_FIELD(0x14, 0x8000, is_grab);
	FIELD(0x18, int32_t, flags_3); // APFLG3
	BIT_FIELD(0x18, 0x10, opponent_must_be_grounded);
	BIT_FIELD(0x18, 0x20, opponent_must_be_airborne);
	FIELD(0x1C, int32_t, flags_4); // APFLG4
	FIELD(0x20, int32_t, flags_5); // APFLG5
	FIELD(0x38, int32_t, enemy_guard_hitstop);
	FIELD(0x40, int32_t, enemy_blockstun);
	FIELD(0x4C, int32_t, throw_box_right);
	FIELD(0x50, int32_t, throw_box_top);
	FIELD(0x54, int32_t, throw_box_left);
	FIELD(0x58, int32_t, throw_box_bottom);
	FIELD(0x5C, int32_t, throw_range);
};

struct Entity
{
	FIELD(0x20, bool, is_player);
	FIELD(0x68, FrameInfo *, frame_info);
	FIELD(0x80, Box *, hurtboxes);
	FIELD(0x88, Box *, hitboxes);
	FIELD(0x114, uint32_t, num_hurtboxes);
	FIELD(0x118, uint32_t, num_hitboxes);
	BIT_FIELD(0x1B3, 0x04, cinematic_freeze);
	BIT_FIELD(0x1B4, 0x200000, slowdown_bonus_frame);
	BIT_FIELD(0x1C4, 0x01, attack_hit_connecting);
	BIT_FIELD(0x1C4, 0x040000, pushbox_enabled);
	FIELD(0x264, uint32_t, hitstop);
	FIELD(0x288, Character *, parent_character);
	FIELD(0x298, Entity *, opponent);
	FIELD(0x2B0, Entity *, attach_parent);
	FIELD(0x2F8, Entity *, attached);
	FIELD(0x310, Entity *, puppet);
	FIELD(0x3C0, uint32_t, flags_1);
	FIELD(0x3C4, uint32_t, flags_2);
	FIELD(0x3C8, uint32_t, flags_3);
	FIELD(0x3CC, uint32_t, flags_4);  // ?
	BIT_FIELD(0x3C0, 0x01, airborne); // ?
	BIT_FIELD(0x3C0, 0x40000000, recovery);
	BIT_FIELD(0x3C4, 0x10, strike_invincible); // except crossups
	BIT_FIELD(0x3C4, 0x20, throw_invincible);
	BIT_FIELD(0x3C4, 0x40, full_invincible); // except crossups
	BIT_FIELD(0x3C8, 0x02, defense_hit_connecting);
	BIT_FIELD(0x3C8, 0x0100, active_frames);
	BIT_FIELD(0x3C8, 0x0400, on_the_floor);
	BIT_FIELD(0x3C8, 0x040800, temporarily_attached); // These 2 get checked together by game code, unclear what distinction is
	BIT_FIELD(0x3C8, 0x080000, attacking);
	BIT_FIELD(0x3C8, 0x10000000, defense_guard_connecting); // Other bits in same byte also good candidates
	FIELD(0x3D4, bool, facing_left);
	FIELD(0x3D8, int32_t, offfset_x);
	FIELD(0x3DC, int32_t, offfset_y);
	FIELD(0x3E4, int32_t, rotation);
	FIELD(0x3FC, int32_t, scale_x);
	FIELD(0x400, int32_t, scale_y);
	BIT_FIELD(0x464, 0x04, cinematic_attack);
	FIELD(0x790, Entity *, temporarily_attached_to);
	FIELD(0x7A8, AttackParameters, attack_parameters);
	BIT_FIELD(0xEB0, 0x02, has_hit_handler);
	FIELD(0xEB4, HitHandlerType, hit_handler_type);
	FIELD(0xF10, Bitmask<BBScriptInterrupt::MAX>, bbscript_interrupts);
	FIELD(0x449C, int32_t, pushbox_left);
	FIELD(0x44A0, int32_t, pushbox_right);
	FIELD(0x44A4, int32_t, pushbox_top);
	FIELD(0x44A8, int32_t, pushbox_bottom);
	ARRAY_FIELD(0x3FF0, char[20], action_name);

	int32_t get_position_x() const;
	int32_t get_position_y() const;
	float get_rotation() const; // in radians
	bool is_active() const;
	bool has_armor() const;
	bool is_strike_invincible() const;

private:
	char pad[0xD220];
};

struct Character : public Entity
{
	FIELD(0xD240, EnableFlag, enable_flag);
	FIELD(0xD258, uint32_t, blockstun_duration);
	FIELD(0xD280, uint32_t, hitstun_duration);
	FIELD(0xDCE0, ActionID, action_id);
	FIELD(0xE9C4, int32_t, slowdown_remaining);

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
	char pad[0xE9C4 + sizeof(int32_t) - sizeof(Entity)];
};
