#ifndef AKARI_SCHEDULE_H
#define AKARI_SCHEDULE_H

#include <cstdint>

namespace akari {

struct DayTime {
	int32_t hour;
	int32_t minute;
};

struct Schedule {
	bool enabled;
	DayTime onTime;
	DayTime offTime;
};

bool IsValidDayTime(DayTime time);

int32_t DayTimeToMinutes(DayTime time);

DayTime MinutesToDayTime(int32_t minutes);

bool IsWithinSchedule(const Schedule& schedule, int32_t minutesOfDay);

}

#endif
