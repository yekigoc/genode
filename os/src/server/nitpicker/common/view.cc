/*
 * \brief  Nitpicker view implementation
 * \author Norman Feske
 * \date   2006-08-09
 */

/*
 * Copyright (C) 2006-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/printf.h>

#include "view.h"
#include "clip_guard.h"
#include "session.h"
#include "draw_label.h"


/***************
 ** Utilities **
 ***************/

/**
 * Draw rectangle
 */
static void draw_rect(Canvas &canvas, int x, int y, int w, int h, Color color)
{
	canvas.draw_box(Rect(Point(x, y),         Area(w, 1)), color);
	canvas.draw_box(Rect(Point(x, y),         Area(1, h)), color);
	canvas.draw_box(Rect(Point(x + w - 1, y), Area(1, h)), color);
	canvas.draw_box(Rect(Point(x, y + h - 1), Area(w, 1)), color);
}


/**
 * Draw outlined frame with black outline color
 */
static void draw_frame(Canvas &canvas, Rect r, Color color, int frame_size)
{
	/* draw frame around the view */
	int d = frame_size;
	draw_rect(canvas, r.x1() - d, r.y1() - d, r.w() + 2*d, r.h() + 2*d, BLACK);
	while (--d > 1)
		draw_rect(canvas, r.x1() - d, r.y1() - d, r.w() + 2*d, r.h() + 2*d, color);
	draw_rect(canvas, r.x1() - d, r.y1() - d, r.w() + 2*d, r.h() + 2*d, BLACK);
}


/**********
 ** View **
 **********/

void View::title(const char *title)
{
	Nitpicker::strncpy(_title, title, TITLE_LEN);

	/* calculate label size, the position is defined by the view stack */
	_label_rect = Rect(Point(0, 0), label_size(_session.label(), _title));
}


void View::frame(Canvas &canvas, Mode const &mode) const
{
	/* do not draw frame in flat mode */
	if (mode.flat()) return;

	draw_frame(canvas, *this, _session.color(), frame_size(mode));
}


void View::draw(Canvas &canvas, Mode const &mode) const
{
	/* is this the currently focused view? */
	bool const view_is_focused = mode.focused_view()
	                          && mode.focused_view()->belongs_to(_session);

	Color const frame_color = _session.color();

	/*
	 * Use dimming in x-ray and kill mode, but do not dim the focused view in
	 * x-ray mode.
	 */
	Canvas::Mode const op = mode.flat() || (mode.xray() && view_is_focused)
	                      ? Canvas::SOLID : Canvas::MIXED;

	/*
	 * The view content and label should never overdraw the
	 * frame of the view in non-flat Nitpicker modes. The frame
	 * is located outside the view area. By shrinking the
	 * clipping area to the view area, we protect the frame.
	 */
	Clip_guard clip_guard(canvas, *this);

	/*
	 * If the clipping area shrinked to zero, we do not process drawing
	 * operations.
	 */
	if (!canvas.clip_valid() || !&_session) return;

	/* allow alpha blending only in flat mode */
	bool allow_alpha = mode.flat();

	/* draw view content */
	Color const mix_color = mode.kill() ? KILL_COLOR
	                      : Color(_session.color().r >> 1,
	                              _session.color().g >> 1,
	                              _session.color().b >> 1);

	canvas.draw_texture(_session.texture(), mix_color, _buffer_off + p1(),
	                    op, allow_alpha);

	if (mode.flat()) return;

	/* draw label */
	draw_label(canvas, _label_rect.p1(), _session.label(), WHITE, _title, frame_color);
}
