#include "Schedule.h"

#include <algorithm>

namespace akari {

bool
IsValidDayTime(DayTime time)
{
	return time.hour >= 0 && time.hour < 24
		&& time.minute >= 0 && time.minute < 60;
}

int32_t
DayTimeToMinutes(DayTime time)
{
	return time.hour * 60 + time.minute;
}

DayTime
MinutesToDayTime(int32_t minutes)
{
	minutes = ((minutes % 1440) + 1440) % 1440;
	DayTime time;
	time.hour = minutes / 60;
	time.minute = minutes % 60;
	return time;
}

bool
IsWithinSchedule(const Schedule& schedule, int32_t minutesOfDay)
{
	if (!schedule.enabled)
		return false;
	if (!IsValidDayTime(schedule.onTime) || !IsValidDayTime(schedule.offTime))
		return false;

	int32_t now = ((minutesOfDay % 1440) + 1440) % 1440;
	int32_t on = DayTimeToMinutes(schedule.onTime);
	int32_t off = DayTimeToMinutes(schedule.offTime);

	if (on == off)
		return true;
	if (on < off)
		return now >= on && now < off;
	return now >= on || now < off;
}

}
