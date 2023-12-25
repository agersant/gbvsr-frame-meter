#include "ui.h"

void UI::draw(const DrawContext &context, const FrameMeter &frame_meter)
{
	context.draw_rect(0xFF0000, 100, 100, 100, 100);
}
