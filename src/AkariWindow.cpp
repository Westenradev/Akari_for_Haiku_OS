#include "AkariWindow.h"

#include <Alert.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <MessageRunner.h>
#include <Slider.h>
#include <StringView.h>
#include <TextControl.h>

#include <cstdio>

#include "App.h"
#include "TintController.h"
#include "core/ColorMath.h"

namespace akari {

static void
FormatTime(const DayTime& time, char* buffer, size_t size)
{
	std::snprintf(buffer, size, "%02d:%02d", time.hour, time.minute);
}

static bool
ParseTime(const char* text, DayTime& out)
{
	int hour = 0;
	int minute = 0;
	if (std::sscanf(text, "%d:%d", &hour, &minute) != 2)
		return false;
	out.hour = hour;
	out.minute = minute;
	return IsValidDayTime(out);
}


AkariWindow::AkariWindow(TintController* controller, const AppSettings& settings)
	:
	BWindow(settings.frameValid ? settings.windowFrame
			: BRect(80, 80, 400, 360),
		"Akari", B_FLOATING_WINDOW,
		B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS | B_CLOSE_ON_ESCAPE,
		B_CURRENT_WORKSPACE),
	fController(controller),
	fEnabledBox(NULL),
	fScheduleBox(NULL),
	fOverrideBox(NULL),
	fWarmthSlider(NULL),
	fKelvinLabel(NULL),
	fOnTime(NULL),
	fOffTime(NULL),
	fStatus(NULL)
{
	BuildLayout(settings);

	BMessage tick(kMsgTick);
	new BMessageRunner(BMessenger(this), tick, 30 * 1000000);

	UpdateStatus();
	Show();
}


AkariWindow::~AkariWindow()
{
}


void
AkariWindow::BuildLayout(const AppSettings& settings)
{
	fEnabledBox = new BCheckBox("enabled", "Enabled",
		new BMessage(kMsgEnabledChanged));
	fEnabledBox->SetValue(settings.enabled ? B_CONTROL_ON : B_CONTROL_OFF);

	fScheduleBox = new BCheckBox("schedule", "Schedule",
		new BMessage(kMsgScheduleEnabledChanged));
	fScheduleBox->SetValue(settings.schedule.enabled ? B_CONTROL_ON
		: B_CONTROL_OFF);

	fOverrideBox = new BCheckBox("override", "Override schedule now",
		new BMessage(kMsgOverrideChanged));
	fOverrideBox->SetValue(settings.manualOverride && settings.overrideActive
		? B_CONTROL_ON : B_CONTROL_OFF);

	fWarmthSlider = new BSlider("warmth", "Warmth",
		new BMessage(kMsgWarmthChanged), kSliderMin, kSliderMax, B_HORIZONTAL);
	fWarmthSlider->SetValue(settings.warmthSlider);
	fWarmthSlider->SetLimitLabels("6500K", "3000K");
	fWarmthSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fWarmthSlider->SetModificationMessage(new BMessage(kMsgWarmthChanged));

	fKelvinLabel = new BStringView("kelvin", "");

	char onText[8];
	char offText[8];
	FormatTime(settings.schedule.onTime, onText, sizeof(onText));
	FormatTime(settings.schedule.offTime, offText, sizeof(offText));
	fOnTime = new BTextControl("on", "On at", onText,
		new BMessage(kMsgScheduleTimeChanged));
	fOffTime = new BTextControl("off", "Off at", offText,
		new BMessage(kMsgScheduleTimeChanged));

	fStatus = new BStringView("status", "");

	BMenuBar* menuBar = new BMenuBar("menu_bar");
	BMenu* appMenu = new BMenu("Akari");
	appMenu->AddItem(new BMenuItem("About Akari...", new BMessage(kMsgShowAbout)));
	appMenu->AddSeparatorItem();
	appMenu->AddItem(new BMenuItem("Quit", new BMessage(kMsgQuit), 'Q'));
	menuBar->AddItem(appMenu);

	BView* content = new BView("content", B_WILL_DRAW);
	BLayoutBuilder::Group<>(content, B_VERTICAL, 0.0f)
		.SetInsets(8, 8, 8, 8)
		.Add(fEnabledBox)
		.Add(fWarmthSlider)
		.Add(fKelvinLabel)
		.Add(fScheduleBox)
		.AddGroup(B_HORIZONTAL, 8.0f)
			.Add(fOnTime)
			.Add(fOffTime)
			.End()
		.Add(fOverrideBox)
		.Add(fStatus)
		.End();

	BLayoutBuilder::Group<>(this)
		.Add(menuBar)
		.Add(content)
		.End();
}


void
AkariWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kMsgWarmthChanged:
		{
			int32_t value = fWarmthSlider->Value();
			fController->SetWarmth(value);
			UpdateStatus();
			break;
		}
		case kMsgEnabledChanged:
		{
			fController->SetEnabled(fEnabledBox->Value() == B_CONTROL_ON);
			UpdateStatus();
			break;
		}
		case kMsgScheduleEnabledChanged:
		{
			PushSchedule();
			fController->SetSchedule(fController->Settings().schedule);
			UpdateStatus();
			break;
		}
		case kMsgScheduleTimeChanged:
		{
			PushSchedule();
			fController->SetSchedule(fController->Settings().schedule);
			UpdateStatus();
			break;
		}
		case kMsgOverrideChanged:
		{
			fController->SetOverride(fOverrideBox->Value() == B_CONTROL_ON);
			UpdateStatus();
			break;
		}
		case kMsgTick:
		{
			fController->Tick();
			UpdateStatus();
			break;
		}
		case kMsgShowAbout:
			ShowAbout();
			break;
		case kMsgQuit:
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
}


bool
AkariWindow::QuitRequested()
{
	fController->SetWindowFrame(Frame());
	fController->SetWindowFrameValid(true);
	fController->SaveSettings();
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


void
AkariWindow::FrameMoved(BPoint newPosition)
{
	BWindow::FrameMoved(newPosition);
	fController->SetWindowFrame(Frame());
}


void
AkariWindow::PushSchedule()
{
	Schedule schedule = fController->Settings().schedule;
	schedule.enabled = fScheduleBox->Value() == B_CONTROL_ON;
	DayTime on = schedule.onTime;
	DayTime off = schedule.offTime;
	ParseTime(fOnTime->Text(), on);
	ParseTime(fOffTime->Text(), off);
	if (!IsValidDayTime(on))
		on = schedule.onTime;
	if (!IsValidDayTime(off))
		off = schedule.offTime;
	schedule.onTime = on;
	schedule.offTime = off;
	fController->SetSchedule(schedule);
}


void
AkariWindow::UpdateStatus()
{
	char buffer[256];
	int32_t kelvin = fController->Kelvin();
	std::snprintf(buffer, sizeof(buffer),
		"%dK%s | method: %s", kelvin,
		fController->IsTintActive() ? " (tint active)" : " (tint off)",
		fController->MethodName());
	fStatus->SetText(buffer);

	char kelvinText[32];
	std::snprintf(kelvinText, sizeof(kelvinText), "current: %d K", kelvin);
	fKelvinLabel->SetText(kelvinText);
}


void
AkariWindow::ShowAbout()
{
	BAlert* alert = new BAlert("About Akari",
		"Akari 0.1\n"
		"A native night-light for Haiku OS.\n\n"
		"Warms the screen color in the evening.\n\n"
		"Copyright (c) 2026 Westenra Softworks.\n"
		"MIT License - see LICENSE.",
		"OK");
	alert->Go();
}

}
