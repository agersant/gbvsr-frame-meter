#include "core/battle.h"

bool Battle::is_freeze_frame() const
{
	Character *character_1 = teams[0].main_player_object;
	Character *character_2 = teams[1].main_player_object;

	const bool is_cinematic_freeze = character_1->cinematic_freeze || character_2->cinematic_freeze;
	const bool is_slowdown_bonus_frame = character_1->slowdown_bonus_frame || character_2->slowdown_bonus_frame;
	const bool is_hitstop = character_1->hitstop > 0 && character_2->hitstop > 0;
	return is_cinematic_freeze || is_slowdown_bonus_frame || is_hitstop;
}

int32_t Entity::get_position_x() const
{
	int32_t position = offfset_x;
	if (attach_parent != nullptr)
	{
		position += attach_parent->get_position_x();
	}
	return position;
}

int32_t Entity::get_position_y() const
{
	int32_t position = offfset_y;
	if (attach_parent != nullptr)
	{
		position += attach_parent->get_position_y();
	}
	return position;
}

bool Entity::is_active() const
{
	if (attack_hit_connecting)
	{
		return true;
	}

	if (attack_parameters.opponent_must_be_airborne && attack_parameters.opponent_must_be_grounded)
	{
		return false;
	}

	return active_frames && !recovery;
}

bool Entity::has_armor() const
{
	return has_hit_handler && !attack_parameters.is_grab_cinematic && num_hurtboxes > 0;
}

bool Character::can_walk() const
{
	return enable_flag & EnableFlag::ForwardWalk;
}

bool Character::can_attack() const
{
	return enable_flag & EnableFlag::NormalAttack;
}

bool Character::is_air_blocking() const
{
	return action_id >= ActionID::AirGuardPre && action_id <= ActionID::AirGuardEnd;
}

bool Character::is_idle() const
{
	const bool is_mid_jump = action_id == ActionID::Jump;
	const bool is_mid_dash = action_id == ActionID::FDash;
	return (can_walk() || can_attack()) && !is_air_blocking() && !is_mid_jump && !is_mid_dash;
}

bool Character::is_counterable() const
{
	return attacking && !recovery;
}

bool Character::is_recovering() const
{
	return attacking && recovery;
}

bool Character::is_invincible() const
{
	return full_invincible || strike_invincible || throw_invincible;
}

bool Character::is_in_blockstun() const
{
	if (defense_guard_connecting)
	{
		return true;
	}
	if (is_air_blocking() && can_attack())
	{
		return false;
	}
	return action_id >= ActionID::MidGuardPre && action_id <= ActionID::AirGuardEnd;
}

bool Character::is_maneuvering() const
{
	switch (action_id)
	{
	case ActionID::JumpPre:
	case ActionID::Jump:
	case ActionID::JumpLanding:
	case ActionID::JumpLandingStiff:
	case ActionID::AirGuardPre:
	case ActionID::AirGuardLoop:
	case ActionID::AirGuardEnd:
	case ActionID::FDash:
	case ActionID::FDashStop:
	case ActionID::BDash:
	case ActionID::BDashStop:
	case ActionID::Dodge:
	case ActionID::EvasiveMove:
		return true;
	default:
		return false;
	}
}

bool Character::is_in_hitstun() const
{
	if (defense_hit_connecting)
	{
		return true;
	}

	if (action_id >= ActionID::NokezoriHighLv1 && action_id <= ActionID::NokezoriCrouchLv5)
	{
		return true;
	}

	if (action_id >= ActionID::BDownUpper && action_id <= ActionID::KirimomiUpper)
	{
		return true;
	}

	switch (action_id)
	{
	case ActionID::Hizakuzure:
	case ActionID::Ukemi:
	case ActionID::UkemiLeap:
	case ActionID::UkemiLeap2:
	case ActionID::WallBound:
	case ActionID::WallBoundDown:
	case ActionID::HajikareStand:
	case ActionID::HajikareCrouch:
	case ActionID::HajikareAir:
	case ActionID::LockWait:
	case ActionID::LockReject:
	case ActionID::Shirimochi:
	case ActionID::ShirimochiCrouch:
	case ActionID::ExDamage:
	case ActionID::ExDamageLand:
		return true;
	default:
		return false;
	}
}
