#include "OverlayWindow.h"

#include <Screen.h>
#include <View.h>

namespace akari {

class OverlayView : public BView {
public:
						OverlayView(BRect frame, rgb_color color, uint8_t alpha);

	virtual void		Draw(BRect updateRect);

	void				SetTint(rgb_color color, uint8_t alpha);

private:
	rgb_color			fColor;
	uint8_t				fAlpha;
};


OverlayView::OverlayView(BRect frame, rgb_color color, uint8_t alpha)
	:
	BView(frame, "AkariOverlay", B_FOLLOW_ALL_SIDES, B_WILL_DRAW),
	fColor(color),
	fAlpha(alpha)
{
	SetDrawingMode(B_OP_ALPHA);
	SetViewColor(B_TRANSPARENT_COLOR);
	SetLowColor(B_TRANSPARENT_COLOR);
}


void
OverlayView::Draw(BRect updateRect)
{
	SetHighColor(fColor.red, fColor.green, fColor.blue, fAlpha);
	FillRect(Bounds(), B_SOLID_HIGH);
}


void
OverlayView::SetTint(rgb_color color, uint8_t alpha)
{
	fColor = color;
	fAlpha = alpha;
	Invalidate();
}


OverlayWindow::OverlayWindow()
	:
	BWindow(BScreen().Frame(), "Akari Overlay", B_NO_BORDER_WINDOW_LOOK,
		B_NORMAL_WINDOW_FEEL,
		B_NOT_MOVABLE | B_NOT_CLOSABLE | B_NOT_RESIZABLE | B_NOT_MINIMIZABLE
			| B_NOT_ZOOMABLE | B_AVOID_FOCUS | B_AVOID_FRONT
			| B_NO_WORKSPACE_ACTIVATION | B_SAME_POSITION_IN_ALL_WORKSPACES,
		B_ALL_WORKSPACES),
	fTintColor(),
	fTintAlpha(0),
	fActive(false)
{
	fView = new OverlayView(Bounds(), fTintColor, 0);
	AddChild(fView);
}


OverlayWindow::~OverlayWindow()
{
}


void
OverlayWindow::SetTint(rgb_color color, uint8_t alpha)
{
	fTintColor = color;
	fTintAlpha = alpha;
	fActive = true;
	static_cast<OverlayView*>(fView)->SetTint(color, alpha);
	if (IsHidden())
		Show();
}


void
OverlayWindow::HideTint()
{
	fActive = false;
	if (!IsHidden())
		Hide();
}

}
