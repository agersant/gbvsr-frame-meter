#include <DynamicOutput/DynamicOutput.hpp>

#include "meter.h"

using namespace RC;

void FrameMeter::update(AREDGameState_Battle *battle)
{
	Output::send<LogLevel::Warning>(STR("p1: {}, p2: {}\n"), (void *)battle->engine->player_1.entity, (void *)battle->engine->player_2.entity);
}
