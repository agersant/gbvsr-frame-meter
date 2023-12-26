#include "game.h"

bool ASW::Character::can_act()
{
	return enable_flag & EnableFlag::ForwardWalk || enable_flag & EnableFlag::NormalAttack;
}

bool ASW::Character::is_in_blockstun()
{
	return action_id >= ActionID::MidGuardPre && action_id <= ActionID::AirGuardEnd;
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
