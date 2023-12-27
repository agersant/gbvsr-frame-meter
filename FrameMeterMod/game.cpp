#include "game.h"

bool ASW::Character::can_walk()
{
	return enable_flag & EnableFlag::ForwardWalk;
}

bool ASW::Character::is_in_blockstun()
{
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
	if (action_id >= ActionID::NokezoriHighLv1 && action_id <= ActionID::NokezoriCrouchLv5)
	{
		return true;
	}

	if (action_id >= ActionID::BDownUpper && action_id <= ActionID::Blowoff)
	{
		return true;
	}

	switch (action_id)
	{
	case ActionID::Ukemi:
	case ActionID::UkemiLeap:
	case ActionID::WallBound:
	case ActionID::WallBoundDown:
	case ActionID::LockWait:
	case ActionID::ExDamage:
		return true;
	default:
		return false;
	}
}
