#include "../src/core/ColorMath.h"
#include "../src/core/Schedule.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace akari;

static int sFailures = 0;
static int sChecks = 0;

static void
Check(bool condition, const char* expression, int line)
{
	++sChecks;
	if (!condition) {
		++sFailures;
		std::printf("FAIL line %d: %s\n", line, expression);
	}
}

#define CHECK(cond) Check((cond), #cond, __LINE__)

static void
TestKelvinClamping()
{
	CHECK(ClampKelvin(1000) == kMinKelvin);
	CHECK(ClampKelvin(10000) == kMaxKelvin);
	CHECK(ClampKelvin(4500) == 4500);
	CHECK(ClampKelvin(kMinKelvin) == kMinKelvin);
	CHECK(ClampKelvin(kMaxKelvin) == kMaxKelvin);
}

static void
TestKelvinRgbEndpoints()
{
	Rgb day = KelvinToRgb(kMaxKelvin);
	CHECK(day.red == 255);
	CHECK(day.green > 240);
	CHECK(day.blue > 240);

	Rgb night = KelvinToRgb(kMinKelvin);
	CHECK(night.red == 255);
	CHECK(night.green > 160 && night.green < 190);
	CHECK(night.blue > 90 && night.blue < 130);
	CHECK(night.blue < night.green);
	CHECK(night.green < day.green);
	CHECK(night.blue < day.blue);
}

static void
TestKelvinRgbMonotonicity()
{
	Rgb previous = KelvinToRgb(kMinKelvin);
	for (int32_t k = kMinKelvin + 100; k <= kMaxKelvin; k += 100) {
		Rgb current = KelvinToRgb(k);
		CHECK(current.blue >= previous.blue);
		CHECK(current.green >= previous.green);
		previous = current;
	}
}

static void
TestSliderKelvinMapping()
{
	CHECK(SliderToKelvin(kSliderMin) == kMaxKelvin);
	CHECK(SliderToKelvin(kSliderMax) == kMinKelvin);
	CHECK(SliderToKelvin(-50) == kMaxKelvin);
	CHECK(SliderToKelvin(9999) == kMinKelvin);

	int32_t mid = SliderToKelvin(50);
	CHECK(mid >= 4700 && mid <= 4850);

	CHECK(KelvinToSlider(kMaxKelvin) == kSliderMin);
	CHECK(KelvinToSlider(kMinKelvin) == kSliderMax);
	CHECK(KelvinToSlider(5000) == KelvinToSlider(ClampKelvin(5000)));

	for (int32_t s = kSliderMin; s <= kSliderMax; ++s) {
		int32_t k = SliderToKelvin(s);
		int32_t back = KelvinToSlider(k);
		CHECK(std::abs(back - s) <= 2);
	}
}

static void
TestOverlayAlphaMapping()
{
	CHECK(WarmthToAlpha(kMaxKelvin) == kMinOverlayAlpha);
	CHECK(WarmthToAlpha(kMinKelvin) == kMaxOverlayAlpha);
	CHECK(WarmthToAlpha(1000) == kMaxOverlayAlpha);
	CHECK(WarmthToAlpha(99999) == kMinOverlayAlpha);

	uint8_t previous = WarmthToAlpha(kMinKelvin);
	for (int32_t k = kMinKelvin + 100; k <= kMaxKelvin; k += 100) {
		uint8_t current = WarmthToAlpha(k);
		CHECK(current <= previous);
		previous = current;
	}

	uint8_t mid = WarmthToAlpha(4750);
	CHECK(mid > kMinOverlayAlpha && mid < kMaxOverlayAlpha);
}

static void
TestScheduleValidation()
{
	CHECK(IsValidDayTime({0, 0}));
	CHECK(IsValidDayTime({23, 59}));
	CHECK(!IsValidDayTime({24, 0}));
	CHECK(!IsValidDayTime({-1, 0}));
	CHECK(!IsValidDayTime({12, 60}));
	CHECK(!IsValidDayTime({12, -1}));
}

static void
TestScheduleDayTimeConversion()
{
	CHECK(DayTimeToMinutes({0, 0}) == 0);
	CHECK(DayTimeToMinutes({20, 0}) == 1200);
	CHECK(DayTimeToMinutes({7, 30}) == 450);
	CHECK(DayTimeToMinutes({23, 59}) == 1439);

	DayTime t = MinutesToDayTime(1200);
	CHECK(t.hour == 20 && t.minute == 0);
	t = MinutesToDayTime(0);
	CHECK(t.hour == 0 && t.minute == 0);
	t = MinutesToDayTime(1439);
	CHECK(t.hour == 23 && t.minute == 59);
	t = MinutesToDayTime(-60);
	CHECK(t.hour == 23 && t.minute == 0);
	t = MinutesToDayTime(1440 + 90);
	CHECK(t.hour == 1 && t.minute == 30);
}

static void
TestScheduleDisabled()
{
	Schedule s;
	s.enabled = false;
	s.onTime = {20, 0};
	s.offTime = {7, 0};
	CHECK(!IsWithinSchedule(s, 1200));
	CHECK(!IsWithinSchedule(s, 0));
}

static void
TestScheduleOvernight()
{
	Schedule s;
	s.enabled = true;
	s.onTime = {20, 0};
	s.offTime = {7, 0};

	CHECK(IsWithinSchedule(s, 1200));
	CHECK(IsWithinSchedule(s, 1439));
	CHECK(IsWithinSchedule(s, 0));
	CHECK(IsWithinSchedule(s, 419));
	CHECK(!IsWithinSchedule(s, 420));
	CHECK(!IsWithinSchedule(s, 700));
	CHECK(!IsWithinSchedule(s, 1100));
	CHECK(!IsWithinSchedule(s, 1199));
}

static void
TestScheduleSameDay()
{
	Schedule s;
	s.enabled = true;
	s.onTime = {9, 0};
	s.offTime = {17, 0};

	CHECK(IsWithinSchedule(s, 540));
	CHECK(!IsWithinSchedule(s, 539));
	CHECK(IsWithinSchedule(s, 1019));
	CHECK(!IsWithinSchedule(s, 1020));
	CHECK(!IsWithinSchedule(s, 0));
	CHECK(!IsWithinSchedule(s, 1300));
}

static void
TestScheduleEqualTimes()
{
	Schedule s;
	s.enabled = true;
	s.onTime = {12, 0};
	s.offTime = {12, 0};
	CHECK(IsWithinSchedule(s, 0));
	CHECK(IsWithinSchedule(s, 720));
	CHECK(IsWithinSchedule(s, 1439));
}

static void
TestScheduleInvalidTimes()
{
	Schedule s;
	s.enabled = true;
	s.onTime = {25, 0};
	s.offTime = {7, 0};
	CHECK(!IsWithinSchedule(s, 1200));

	s.onTime = {20, 0};
	s.offTime = {7, 99};
	CHECK(!IsWithinSchedule(s, 1200));
}

static void
TestScheduleBoundaryWrap()
{
	Schedule s;
	s.enabled = true;
	s.onTime = {0, 0};
	s.offTime = {0, 1};
	CHECK(IsWithinSchedule(s, 0));
	CHECK(!IsWithinSchedule(s, 1));
	CHECK(!IsWithinSchedule(s, 1439));
}

int
main()
{
	TestKelvinClamping();
	TestKelvinRgbEndpoints();
	TestKelvinRgbMonotonicity();
	TestSliderKelvinMapping();
	TestOverlayAlphaMapping();
	TestScheduleValidation();
	TestScheduleDayTimeConversion();
	TestScheduleDisabled();
	TestScheduleOvernight();
	TestScheduleSameDay();
	TestScheduleEqualTimes();
	TestScheduleInvalidTimes();
	TestScheduleBoundaryWrap();

	std::printf("%d checks, %d failures\n", sChecks, sFailures);
	if (sFailures > 0) {
		std::printf("TESTS FAILED\n");
		return 1;
	}
	std::printf("ALL TESTS PASSED\n");
	return 0;
}
