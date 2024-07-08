#pragma once

#include "core/meter.h"
#include "core/hitbox.h"
#include "mod/draw.h"

namespace UI
{
	void draw_frame_meter(const DrawContext &context, const FrameMeter &frame_meter);
	void draw_hitbox_viewer(const DrawContext &context, const HitboxViewer &hitbox_viewer);
}
