#pragma once

#include <Unreal/AActor.hpp>

#include "struct_util.h"

namespace ASW
{
	class Player
	{
	public:
		FIELD(0x08, class Entity *, entity);
	};

	class Engine
	{
	public:
		FIELD(0x10, class Player, player_1);
		FIELD(0x88, class Player, player_2);
	};

	class Entity
	{
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
