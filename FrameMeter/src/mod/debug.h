#pragma once

#include "core/battle.h"
#include "mod/game.h"

#if UE_BUILD_TEST

namespace Debug
{
	void on_battle_reset(Battle *battle);
	void on_battle_update(AREDGameState_Battle *game_state);
}

#endif