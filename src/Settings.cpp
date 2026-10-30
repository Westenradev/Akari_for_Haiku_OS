#include "Settings.h"

#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>

#include "core/ColorMath.h"

namespace akari {

static const char* kSettingsDirName = "Akari";
static const char* kSettingsFileName = "settings";

static void
Defaults(AppSettings& data)
{
	data.windowFrame = BRect(80, 80, 380, 320);
	data.frameValid = false;
	data.warmthSlider = 0;
	data.enabled = true;
	data.schedule.enabled = false;
	data.schedule.onTime = {20, 0};
	data.schedule.offTime = {7, 0};
	data.manualOverride = false;
	data.overrideActive = false;
	data.lastMethod = 0;
}

Settings::Settings()
{
	Defaults(fData);
}

status_t
Settings::Load()
{
	Defaults(fData);

	BPath path;
	if (find_directory(B_USER_SETTINGS_PATH, &path) != B_OK)
		return B_ERROR;
	path.Append(kSettingsDirName);
	path.Append(kSettingsFileName);

	BFile file(path.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return B_ENTRY_NOT_FOUND;

	BMessage message;
	if (message.Unflatten(&file) != B_OK)
		return B_ERROR;

	BRect frame;
	if (message.FindRect("windowFrame", &frame) == B_OK) {
		fData.windowFrame = frame;
		fData.frameValid = true;
	}
	int32_t value;
	if (message.FindInt32("warmth", &value) == B_OK)
		fData.warmthSlider = value;
	bool flag;
	if (message.FindBool("enabled", &flag) == B_OK)
		fData.enabled = flag;
	if (message.FindBool("scheduleEnabled", &flag) == B_OK)
		fData.schedule.enabled = flag;
	if (message.FindInt32("onHour", &value) == B_OK)
		fData.schedule.onTime.hour = value;
	if (message.FindInt32("onMinute", &value) == B_OK)
		fData.schedule.onTime.minute = value;
	if (message.FindInt32("offHour", &value) == B_OK)
		fData.schedule.offTime.hour = value;
	if (message.FindInt32("offMinute", &value) == B_OK)
		fData.schedule.offTime.minute = value;
	if (message.FindBool("manualOverride", &flag) == B_OK)
		fData.manualOverride = flag;
	if (message.FindBool("overrideActive", &flag) == B_OK)
		fData.overrideActive = flag;
	if (message.FindInt32("lastMethod", &value) == B_OK)
		fData.lastMethod = value;

	fData.schedule.onTime.hour = fData.schedule.onTime.hour < 0 ? 0
		: (fData.schedule.onTime.hour > 23 ? 23 : fData.schedule.onTime.hour);
	fData.schedule.onTime.minute = fData.schedule.onTime.minute < 0 ? 0
		: (fData.schedule.onTime.minute > 59 ? 59 : fData.schedule.onTime.minute);
	fData.schedule.offTime.hour = fData.schedule.offTime.hour < 0 ? 0
		: (fData.schedule.offTime.hour > 23 ? 23 : fData.schedule.offTime.hour);
	fData.schedule.offTime.minute = fData.schedule.offTime.minute < 0 ? 0
		: (fData.schedule.offTime.minute > 59 ? 59 : fData.schedule.offTime.minute);

	return B_OK;
}

status_t
Settings::Save() const
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_PATH, &path) != B_OK)
		return B_ERROR;
	path.Append(kSettingsDirName);

	BDirectory dir(path.Path());
	if (dir.InitCheck() != B_OK) {
		BPath parent;
		if (find_directory(B_USER_SETTINGS_PATH, &parent) != B_OK)
			return B_ERROR;
		BDirectory parentDir(parent.Path());
		if (parentDir.CreateDirectory(kSettingsDirName, &dir) != B_OK)
			return B_ERROR;
	}

	BFile file;
	if (dir.CreateFile(kSettingsFileName, &file,
			B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE) != B_OK)
		return B_ERROR;

	BMessage message('AKST');
	message.AddRect("windowFrame", fData.windowFrame);
	message.AddInt32("warmth", fData.warmthSlider);
	message.AddBool("enabled", fData.enabled);
	message.AddBool("scheduleEnabled", fData.schedule.enabled);
	message.AddInt32("onHour", fData.schedule.onTime.hour);
	message.AddInt32("onMinute", fData.schedule.onTime.minute);
	message.AddInt32("offHour", fData.schedule.offTime.hour);
	message.AddInt32("offMinute", fData.schedule.offTime.minute);
	message.AddBool("manualOverride", fData.manualOverride);
	message.AddBool("overrideActive", fData.overrideActive);
	message.AddInt32("lastMethod", fData.lastMethod);

	return message.Flatten(&file);
}

}
