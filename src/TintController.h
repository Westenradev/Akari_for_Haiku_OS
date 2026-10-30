#ifndef AKARI_TINT_CONTROLLER_H
#define AKARI_TINT_CONTROLLER_H

#include <GraphicsDefs.h>
#include <Messenger.h>

#include "Settings.h"
#include "core/Schedule.h"

namespace akari {

enum TintMethod {
	kMethodNone = 0,
	kMethodOverlay = 1,
	kMethodGamma = 2
};

class OverlayWindow;

class TintController {
public:
						TintController();
						~TintController();

	void				Init();
	void				SetWarmth(int32_t slider);
	void				SetEnabled(bool enabled);
	void				SetSchedule(const Schedule& schedule);
	void				SetOverride(bool active);
	void				Tick();

	bool				IsTintActive() const;
	int32_t				Kelvin() const;
	TintMethod			Method() const;
	const char*			MethodName() const;

	AppSettings			Settings() const;
	void				SetWindowFrameValid(bool valid);
	void				SetWindowFrame(const BRect& frame);

	void				SaveSettings();

private:
	bool				ComputeActive();
	void				Apply();

	OverlayWindow*		fOverlay;
	AppSettings			fSettings;
	bool				fActive;
	TintMethod			fMethod;
	BMessenger			fNotifyTarget;
};

}

#endif
