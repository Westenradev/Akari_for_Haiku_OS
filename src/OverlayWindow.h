#ifndef AKARI_OVERLAY_WINDOW_H
#define AKARI_OVERLAY_WINDOW_H

#include <GraphicsDefs.h>
#include <Window.h>

class BView;

namespace akari {

class OverlayWindow : public BWindow {
public:
						OverlayWindow();
	virtual				~OverlayWindow();

	void				SetTint(rgb_color color, uint8_t alpha);
	void				HideTint();

	virtual void		ScreenChanged(BRect screenSize, color_space depth);

private:
	void				ResizeToScreen();

	BView*				fView;
	rgb_color			fTintColor;
	uint8_t				fTintAlpha;
	bool				fActive;
};

}

#endif
