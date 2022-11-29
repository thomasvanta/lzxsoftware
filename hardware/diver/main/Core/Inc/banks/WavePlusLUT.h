#include "globals.h"
#include "main.h"

#include <functional>

#include "DiverBankBase.h"
#include "DiverUI.h"

uint16_t lut[MAX_BUFFER_SIZE];

struct WavePlusLUT : public DiverBankBase
{
    typedef enum
    {
        Constant = 0,
        PerFrame,
        PerLine
    } UpdateStyle;

    struct Options
    {
        float altA_default;
        float altB_default;
        uint8_t deinterlace_mode;
        UpdateStyle update_style;
    };

    struct Lookup
    {
        uint16_t i;
        uint16_t linecnt;
        uint16_t hres;
        uint16_t vres;
    };

    Options options;
    typedef std::function<uint16_t(Lookup&, DiverUIState&)> LookupTableBuilder;
    LookupTableBuilder lookupTableBuilder;

    WavePlusLUT(const Options& options, LookupTableBuilder lookupTableBuilder)
        : options(options), lookupTableBuilder(lookupTableBuilder)
    {
    }

    void Init()
    {
    }

    void OnActivate(DiverUIState& state)
    {
        state.param_altA = options.altA_default;
        state.param_altB = options.altB_default;

        if (options.update_style == UpdateStyle::Constant)
        {
            GenerateLUT(state);
        }
    }

    void OnInterruptHSync(DiverUIState& state)
    {
        if (options.update_style == UpdateStyle::PerLine)
        {
            GenerateLUT(state);
        }
    }

    void OnInterruptFrameStart(DiverUIState& state)
    {
        if (waveRenderComplete)
        {
            if (state.captureEnable)
            {
                sampleReadPtr = sampleWritePtr;
                sampleWritePtr = (sampleWritePtr + 1) % 4;
            }

            waveReadPtr = waveWritePtr;
            waveWritePtr = (waveWritePtr + 1) % 4;
            waveRenderComplete = 0;
        }
        else
        {
            dropped_frames++;
        }
    }

    void OnOddField(DiverUIState& state)
    {
        if (options.update_style == UpdateStyle::PerFrame)
        {
            GenerateLUT(state);
        }

        // Waveform generation here
        for (uint32_t i = 0; i < hres; i++)
        {
            uint32_t sample_index;
            sample_index = i;

            if (state.scrollx)
            {
                sample_index = (sample_index + (hres - state.hphasecnt) + HPHASE_OFFSET) % (hres);
            }
            else
            {
                sample_index = (sample_index + (hres - state.hphase_slider) + HPHASE_OFFSET) % (hres);
            }

            if (state.mirrorx)
            {
                if (sample_index >= (hres >> 1))
                {
                    sample_index = ((hres >> 1) - (sample_index + 1 - (hres >> 1))) << 1;
                }
                else
                {
                    sample_index = sample_index << 1;
                }
            }

            if (state.invert)
            {
                sample_index = hres - 1 - sample_index;
            }

            uint32_t sample;
            if (lookupTableBuilder != nullptr)
            {
                uint32_t sampleplusramp;
                if ((lut[sample_index] + samples_wave[sampleReadPtr][sample_index] - 512) > DAC_MAX_VALUE)
                {
                    sampleplusramp = DAC_MAX_VALUE;
                }
                else if ((lut[sample_index] + samples_wave[sampleReadPtr][sample_index]) < 512)
                {
                    sampleplusramp = 0;
                }
                else
                {
                    sampleplusramp = (lut[sample_index] + samples_wave[sampleReadPtr][sample_index] - 512);
                }
                sample = sampleplusramp & 1023;
            }
            else
            {
                sample = samples_wave[sampleReadPtr][sample_index];
            }

            hwave[waveWritePtr][i] = sample;
            hwave[waveWritePtr][hres + i] = sample;
        }

        for (uint32_t i = 0; i < vres; i++)
        {
            uint32_t sample_index;
            sample_index = i;
            if (state.scrolly)
            {
                sample_index = (sample_index + (vres - state.vphasecnt) + (vres - state.vphase_cv) + VPHASE_OFFSET) % (vres);
            }
            else
            {
                sample_index = (sample_index + (vres - state.vphase_slider) + (vres - state.vphase_cv) + VPHASE_OFFSET) % (vres);
            }

            if (state.mirrory)
            {
                if (sample_index >= (vres >> 1))
                {
                    sample_index = ((vres >> 1) - (sample_index + 1 - (vres >> 1))) << 1;
                }
                else
                {
                    sample_index = sample_index << 1;
                }
            }
            if (state.invert)
            {
                sample_index = vres - 1 - sample_index;
            }

            uint32_t sample;
            uint32_t sample_hphase;
            sample_hphase = samples_hphase_cv[sampleReadPtr][sample_index];

            if (lookupTableBuilder != nullptr)
            {
                uint32_t sampleplusramp;
                if ((lut[sample_index] + samples_wave[sampleReadPtr][sample_index] - 512) > DAC_MAX_VALUE)
                {
                    sampleplusramp = DAC_MAX_VALUE;
                }
                else if ((lut[sample_index] + samples_wave[sampleReadPtr][sample_index]) < 512)
                {
                    sampleplusramp = 0;
                }
                else
                {
                    sampleplusramp = (lut[sample_index] + samples_wave[sampleReadPtr][sample_index] - 512);
                }

                sample = sampleplusramp & 1023;
            }
            else
            {
                sample = samples_wave[sampleReadPtr][sample_index];
            }

            vwave[waveWritePtr][i] = sample;
            hphase_cv[waveWritePtr][i] = sample_hphase;
            vwave[waveWritePtr][vres + vres - i] = sample;
            hphase_cv[waveWritePtr][vres + vres - i] = sample_hphase;
        }

        uint8_t hphase_interlace_mode = 1; // 0 = video sampling, 1 = audio sampling
        uint8_t interlace_mode = 1;        // 0 = video sampling, 1 = audio sampling

        if (options.deinterlace_mode)
        {
            interlace_mode = 0;
            hphase_interlace_mode = 0;
        }

        // Interlacing/deinterlacing here
        if (interlace_mode)
        {
            uint16_t samples_interlaced[vres];
            for (uint32_t i = 0; i < vres; i++)
            {
                if (i < (vres >> 1))
                {
                    samples_interlaced[i] = vwave[waveWritePtr][i << 1];
                }
                else
                {
                    samples_interlaced[i] = vwave[waveWritePtr][((i - (vres >> 1)) << 1) + 1];
                }
            }

            for (uint32_t i = 0; i < vres; i++)
            {
                vwave[waveWritePtr][i] = samples_interlaced[i];
            }
        }
        if (hphase_interlace_mode)
        {
            uint16_t samples_interlaced[vres];
            for (uint32_t i = 0; i < vres; i++)
            {
                if (i < (vres >> 1))
                {
                    samples_interlaced[i] = hphase_cv[waveWritePtr][i << 1];
                }
                else
                {
                    samples_interlaced[i] = hphase_cv[waveWritePtr][((i - (vres >> 1)) << 1) + 1];
                }
            }

            for (uint32_t i = 0; i < vres; i++)
            {
                hphase_cv[waveWritePtr][i] = samples_interlaced[i];
            }
        }
        if (options.deinterlace_mode)
        {
            uint16_t samples_deinterlaced[hres];
            for (uint32_t i = 0; i < hres; i++)
            {
                if (i & 0b1) // even pixel
                {
                    samples_deinterlaced[i] = hwave[waveWritePtr][(i >> 1) + (hres >> 1)];
                }
                else
                {
                    samples_deinterlaced[i] = hwave[waveWritePtr][(i >> 1)];
                }
            }

            for (uint32_t i = 0; i < hres; i++)
            {
                hwave[waveWritePtr][i] = samples_deinterlaced[i];
            }
        }

        waveRenderComplete = 1;
    }

  protected:
    void GenerateLUT(DiverUIState& state)
    {
        if (lookupTableBuilder != nullptr)
        {
            Lookup pixel{
                .i = 0,
                .linecnt = linecnt,
                .hres = hres,
                .vres = vres,
            };
            for (uint32_t i = 0; i < vres; i++)
            {
                pixel.i = i;
                lut[i] = lookupTableBuilder(pixel, state);
            }
        }
    }
};
