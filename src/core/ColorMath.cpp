#include "ColorMath.h"

#include <algorithm>
#include <cmath>

namespace akari {

const int32_t kMinKelvin = 3000;
const int32_t kMaxKelvin = 6500;
const int32_t kDefaultKelvin = 6500;
const int32_t kSliderMin = 0;
const int32_t kSliderMax = 100;
const uint8_t kMaxOverlayAlpha = 110;
const uint8_t kMinOverlayAlpha = 0;

int32_t
ClampKelvin(int32_t kelvin)
{
	return std::clamp(kelvin, kMinKelvin, kMaxKelvin);
}

int32_t
SliderToKelvin(int32_t slider)
{
	slider = std::clamp(slider, kSliderMin, kSliderMax);
	int32_t kelvin = kMaxKelvin
		- (slider * (kMaxKelvin - kMinKelvin)) / (kSliderMax - kSliderMin);
	return ClampKelvin(kelvin);
}

int32_t
KelvinToSlider(int32_t kelvin)
{
	kelvin = ClampKelvin(kelvin);
	int32_t slider = ((kMaxKelvin - kelvin) * (kSliderMax - kSliderMin))
		/ (kMaxKelvin - kMinKelvin);
	return std::clamp(slider, kSliderMin, kSliderMax);
}

Rgb
KelvinToRgb(int32_t kelvin)
{
	kelvin = ClampKelvin(kelvin);
	double t = kelvin / 100.0;

	double red;
	if (t <= 66.0)
		red = 255.0;
	else
		red = 329.698727446 * std::pow(t - 60.0, -0.1332047592);

	double green;
	if (t <= 66.0)
		green = 99.4708025861 * std::log(t) - 161.1195681661;
	else
		green = 288.1221695283 * std::pow(t - 60.0, -0.0755148492);

	double blue;
	if (t >= 66.0)
		blue = 255.0;
	else if (t <= 19.0)
		blue = 0.0;
	else
		blue = 138.5177312585 * std::log(t - 10.0) - 305.0447927786;

	Rgb rgb;
	rgb.red = static_cast<uint8_t>(std::clamp(red, 0.0, 255.0) + 0.5);
	rgb.green = static_cast<uint8_t>(std::clamp(green, 0.0, 255.0) + 0.5);
	rgb.blue = static_cast<uint8_t>(std::clamp(blue, 0.0, 255.0) + 0.5);
	return rgb;
}

uint8_t
WarmthToAlpha(int32_t kelvin)
{
	kelvin = ClampKelvin(kelvin);
	int32_t range = kMaxKelvin - kMinKelvin;
	int32_t warmth = kMaxKelvin - kelvin;
	int32_t alpha = (warmth * kMaxOverlayAlpha) / range;
	return static_cast<uint8_t>(std::clamp(alpha, 0, static_cast<int32_t>(kMaxOverlayAlpha)));
}

}
