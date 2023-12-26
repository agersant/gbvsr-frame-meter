#include "game.h"

bool ASW::Character::can_act()
{
	return enable_flag & EnableFlag::ForwardWalk || enable_flag & EnableFlag::NormalAttack;
}

bool ASW::Character::is_in_blockstun()
{
	switch (action_id)
	{
	case ActionID::MidGuardPre:
	case ActionID::MidGuardLoop:
	case ActionID::MidGuardEnd:
	case ActionID::HighGuardPre:
	case ActionID::HighGuardLoop:
	case ActionID::HighGuardEnd:
	case ActionID::CrouchGuardPre:
	case ActionID::CrouchGuardLoop:
	case ActionID::CrouchGuardEnd:
	case ActionID::AirGuardPre:
	case ActionID::AirGuardLoop:
	case ActionID::AirGuardEnd:
		return true;
	default:
		return false;
	}
}

bool ASW::Character::is_in_hitstun()
{
	return false;
}
