#include "core/battle.h"

bool Entity::is_in_active_frames()
{
	if (attack_parameters.opponent_must_be_airborne && attack_parameters.opponent_must_be_grounded)
	{
		return false;
	}
	return active_frames && !recovery && (num_hitboxes > 0 || (attached && attached->num_hitboxes > 0));
}

bool Entity::has_armor()
{
	return has_hit_handler && !attack_parameters.is_grab_cinematic && num_hurtboxes > 0;
}

bool Character::can_walk()
{
	return enable_flag & EnableFlag::ForwardWalk;
}

bool Character::can_attack()
{
	return enable_flag & EnableFlag::NormalAttack;
}

bool Character::is_air_blocking()
{
	return action_id >= ActionID::AirGuardPre && action_id <= ActionID::AirGuardEnd;
}

bool Character::is_idle()
{
	const bool is_mid_jump = action_id == ActionID::Jump;
	const bool is_mid_dash = action_id == ActionID::FDash;
	return (can_walk() || can_attack()) && !is_air_blocking() && !is_mid_jump && !is_mid_dash;
}

bool Character::is_counterable()
{
	return attacking && !recovery;
}

bool Character::is_recovering()
{
	return attacking && recovery;
}

bool Character::is_invincible()
{
	return full_invincible || strike_invincible || throw_invincible;
}

bool Character::is_in_blockstun()
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

bool Character::is_maneuvering()
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

bool Character::is_in_hitstun()
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
