#include "DiverMode.h"
#include "WavePlusLUT.h"

DiverMode* banks[] = { 
	new WavePlusLUT(
		WavePlusLUT::Options {
			.deinterlace_mode = 0,
			.updateStyle = WavePlusLUT::UpdateStyle::Constant
		},
		[](WavePlusLUT::Lookup& l, DiverUIState& state) {
			return ((l.i * DAC_MAX_VALUE) / l.vres) & 1023;
		}
	),
	new WavePlusLUT(
		WavePlusLUT::Options {
			.deinterlace_mode = 0,
			.updateStyle = WavePlusLUT::UpdateStyle::Constant
		},
		[](WavePlusLUT::Lookup& l, DiverUIState& state) {
			float a = float(l.i) / float(l.vres);
			float b = (a*a) * 1023.0;
			float c = (a * 1023.0 * 2.0) - (b);
			return uint16_t(c) & 1023;
		}
	),
	new WavePlusLUT(
		WavePlusLUT::Options {
			.deinterlace_mode = 0,
			.updateStyle = WavePlusLUT::UpdateStyle::Constant
		},
		[](WavePlusLUT::Lookup& l, DiverUIState& state) {
			float a = float(l.i) / float(l.vres);
        	float b = (a*a) * 1023.0;
        	return uint16_t(b) & 1023;
		}
	),
	new WavePlusLUT(
		WavePlusLUT::Options {
			.deinterlace_mode = 0,
			.updateStyle = WavePlusLUT::UpdateStyle::Constant
		},
		[](WavePlusLUT::Lookup& l, DiverUIState& state) {
			return ((l.i * DAC_MAX_VALUE) / l.vres) & 0b1111000000;
		}
	),
	new WavePlusLUT(
		WavePlusLUT::Options {
			.deinterlace_mode = 0,
			.updateStyle = WavePlusLUT::UpdateStyle::Constant
		},
		[](WavePlusLUT::Lookup& l, DiverUIState& state) {
			return ((l.i * DAC_MAX_VALUE) / l.vres) & 0b1110000000;
		}
	),
	new WavePlusLUT(
		WavePlusLUT::Options {
			.deinterlace_mode = 0,
			.updateStyle = WavePlusLUT::UpdateStyle::Constant
		},
		[](WavePlusLUT::Lookup& l, DiverUIState& state) {
			return ((l.i * DAC_MAX_VALUE) / l.vres) & 0b1100000000;
		}
	),
	new WavePlusLUT(
		WavePlusLUT::Options {
			.deinterlace_mode = 0,
			.updateStyle = WavePlusLUT::UpdateStyle::Constant
		},
		nullptr
	),
	new WavePlusLUT(
		WavePlusLUT::Options {
			.deinterlace_mode = 1,
			.updateStyle = WavePlusLUT::UpdateStyle::Constant
		},
		nullptr
	),
};