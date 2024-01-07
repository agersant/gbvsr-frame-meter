#include "game.h"

bool ASW::Character::can_walk()
{
	return enable_flag & EnableFlag::ForwardWalk;
}

bool ASW::Character::can_attack()
{
	return enable_flag & EnableFlag::NormalAttack;
}

bool ASW::Character::is_counterable()
{
	return attacking && !recovery;
}

bool ASW::Character::is_in_active_frames()
{
	return attacking && active_frames && num_hitboxes > 0;
}

bool ASW::Character::is_recovering()
{
	return attacking && recovery;
}

bool ASW::Character::is_invincible()
{
	return full_invincible || strike_invincible || throw_invincible;
}

bool ASW::Character::is_in_blockstun()
{
	if (defense_guard_connecting)
	{
		return true;
	}
	return action_id >= ActionID::MidGuardPre && action_id <= ActionID::AirGuardEnd;
}

bool ASW::Character::is_maneuvering()
{
	switch (action_id)
	{
	case ActionID::JumpPre:
	case ActionID::Jump:
	case ActionID::JumpLanding:
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

bool ASW::Character::is_in_hitstun()
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
		return true;
	default:
		return false;
	}
}
