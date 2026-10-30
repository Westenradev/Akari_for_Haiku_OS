#include "TintController.h"

#include <ctime>

#include <Screen.h>

#include "OverlayWindow.h"
#include "core/ColorMath.h"

namespace akari {

static bool
GammaSupported()
{
	BScreen screen;
	return screen.IsValid();
}


TintController::TintController()
	:
	fOverlay(NULL),
	fActive(false),
	fMethod(kMethodNone)
{
}


TintController::~TintController()
{
	if (fOverlay != NULL) {
		fOverlay->Lock();
		fOverlay->Quit();
		fOverlay = NULL;
	}
}


void
TintController::Init()
{
	fMethod = GammaSupported() ? kMethodGamma : kMethodOverlay;
	if (fMethod == kMethodOverlay) {
		fOverlay = new OverlayWindow();
		fOverlay->Hide();
	}
}


void
TintController::SetWarmth(int32_t slider)
{
	fSettings.warmthSlider = slider;
	Apply();
}


void
TintController::SetEnabled(bool enabled)
{
	fSettings.enabled = enabled;
	if (!enabled) {
		fSettings.manualOverride = false;
		fSettings.overrideActive = false;
	}
	Apply();
}


void
TintController::SetSchedule(const Schedule& schedule)
{
	fSettings.schedule = schedule;
	Apply();
}


void
TintController::SetOverride(bool active)
{
	fSettings.manualOverride = true;
	fSettings.overrideActive = active;
	Apply();
}


void
TintController::Tick()
{
	Apply();
}


bool
TintController::IsTintActive() const
{
	return fActive;
}


int32_t
TintController::Kelvin() const
{
	return SliderToKelvin(fSettings.warmthSlider);
}


TintMethod
TintController::Method() const
{
	return fMethod;
}


const char*
TintController::MethodName() const
{
	switch (fMethod) {
		case kMethodGamma:
			return "gamma ramps";
		case kMethodOverlay:
			return "overlay window";
		default:
			return "none";
	}
}


AppSettings
TintController::Settings() const
{
	return fSettings;
}


void
TintController::SetWindowFrameValid(bool valid)
{
	fSettings.frameValid = valid;
}


void
TintController::SetWindowFrame(const BRect& frame)
{
	fSettings.windowFrame = frame;
}


void
TintController::SaveSettings()
{
	Settings settings;
	settings.fData = fSettings;
	settings.Save();
}


bool
TintController::ComputeActive()
{
	if (!fSettings.enabled)
		return false;
	if (fSettings.manualOverride)
		return fSettings.overrideActive;
	if (fSettings.schedule.enabled) {
		tm local;
		time_t now = time(NULL);
		localtime_r(&now, &local);
		int32_t minutes = local.tm_hour * 60 + local.tm_min;
		return IsWithinSchedule(fSettings.schedule, minutes);
	}
	return true;
}


void
TintController::Apply()
{
	bool active = ComputeActive();
	fActive = active;

	if (!active || fMethod != kMethodOverlay || fOverlay == NULL) {
		if (fOverlay != NULL)
			fOverlay->HideTint();
		return;
	}

	int32_t kelvin = Kelvin();
	Rgb rgb = KelvinToRgb(kelvin);
	uint8_t alpha = WarmthToAlpha(kelvin);
	rgb_color color;
	color.red = rgb.red;
	color.green = rgb.green;
	color.blue = rgb.blue;
	color.alpha = 255;
	fOverlay->SetTint(color, alpha);
}

}
