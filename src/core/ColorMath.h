#ifndef AKARI_COLOR_MATH_H
#define AKARI_COLOR_MATH_H

#include <cstdint>

namespace akari {

struct Rgb {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

extern const int32_t kMinKelvin;
extern const int32_t kMaxKelvin;
extern const int32_t kDefaultKelvin;
extern const int32_t kSliderMin;
extern const int32_t kSliderMax;
extern const uint8_t kMaxOverlayAlpha;
extern const uint8_t kMinOverlayAlpha;

int32_t ClampKelvin(int32_t kelvin);

int32_t SliderToKelvin(int32_t slider);

int32_t KelvinToSlider(int32_t kelvin);

Rgb KelvinToRgb(int32_t kelvin);

uint8_t WarmthToAlpha(int32_t kelvin);

}

#endif
