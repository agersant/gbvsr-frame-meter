#pragma once

#include <Unreal/AActor.hpp>

#include "core/battle.h"

using namespace RC::Unreal;

class AWorldSettings : public AActor
{
};

class AGameState : public AActor
{
};

class AREDGameState_Battle : public AGameState
{
public:
	FIELD(0xA10, class Battle *, battle);
};

class UGameInstance : public RC::Unreal::UObject
{
};

enum class GameMode : uint8_t
{
	ARCADE = 0x02,
	TRAINING = 0x06,
	MENU = 0x0A,
	ONLINE_MATCH = 0x11,
	REPLAY = 0x12,
	STORY = 0x14,
};

class UREDGameCommon : public UGameInstance
{
public:
	FIELD(0x318, GameMode, game_mode);
};
