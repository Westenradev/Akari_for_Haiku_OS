#ifndef AKARI_SETTINGS_H
#define AKARI_SETTINGS_H

#include <GraphicsDefs.h>
#include <Message.h>
#include <Rect.h>

#include "core/Schedule.h"

namespace akari {

struct AppSettings {
	BRect windowFrame;
	bool frameValid;
	int32_t warmthSlider;
	bool enabled;
	Schedule schedule;
	bool manualOverride;
	bool overrideActive;
	int32_t lastMethod;
};

class Settings {
public:
						Settings();

	status_t			Load();
	status_t			Save() const;

	AppSettings			fData;
};

}

#endif
