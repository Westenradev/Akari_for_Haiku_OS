#ifndef AKARI_WINDOW_H
#define AKARI_WINDOW_H

#include <Window.h>

#include "Settings.h"
#include "core/Schedule.h"

class BCheckBox;
class BSlider;
class BStringView;
class BTextControl;

namespace akari {

class TintController;

enum {
	kMsgWarmthChanged = 'wCHG',
	kMsgEnabledChanged = 'eCHG',
	kMsgScheduleEnabledChanged = 'sCHG',
	kMsgScheduleTimeChanged = 'tCHG',
	kMsgOverrideChanged = 'oCHG',
	kMsgShowAbout = 'ABOU',
	kMsgQuit = 'QUIT',
	kMsgTick = 'TICK'
};

class AkariWindow : public BWindow {
public:
						AkariWindow(TintController* controller,
							const AppSettings& settings);
	virtual				~AkariWindow();

	virtual void		MessageReceived(BMessage* message);
	virtual bool		QuitRequested();
	virtual void		FrameMoved(BPoint newPosition);

private:
	void				BuildLayout(const AppSettings& settings);
	void				UpdateStatus();
	void				PushSchedule();
	void				ShowAbout();

	TintController*		fController;
	BCheckBox*			fEnabledBox;
	BCheckBox*			fScheduleBox;
	BCheckBox*			fOverrideBox;
	BSlider*			fWarmthSlider;
	BStringView*		fKelvinLabel;
	BTextControl*		fOnTime;
	BTextControl*		fOffTime;
	BStringView*		fStatus;
};

}

#endif
