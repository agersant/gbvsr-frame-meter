#include "ui.h"

void UI::draw(const DrawContext &context, const FrameMeter &frame_meter)
{
	float x = 100.f;
	float y = 100.f;
	const float frame_width = 10.f;
	const float frame_height = 20.f;
	const float frame_spacing = 2.f;

	for (int i = 0; i < frame_meter.current_page.num_frames; i++)
	{
		context.draw_rect(0x222222, x, y, frame_width, frame_height);
		x += frame_width + frame_spacing;
	}
}
