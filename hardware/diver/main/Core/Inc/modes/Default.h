#include "main.h"
#include "globals.h"

#include <functional>

#include "DiverUI.h"
#include "modes/DiverMode.h"

uint16_t lut[MAX_BUFFER_SIZE];

struct ModeDefault : DiverMode
{
    uint8_t deinterlace_mode;
    std::function<uint16_t(uint32_t, uint16_t)> lookupTableBuilder;

    ModeDefault(uint8_t deinterlace_mode, std::function<uint16_t(uint32_t, uint16_t)> lookupTableBuilder)
    : deinterlace_mode(deinterlace_mode)
    , lookupTableBuilder(lookupTableBuilder)
    {
    }

    void OnActivate()
    {
    	GenerateLUT();
    }

    void OnInterruptHSync()
    {

    }

    void OnInterruptFrameStart()
    {
        if (waveRenderComplete)
		{
			if (captureEnable)
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

    void OnOddField()
    {
        //Waveform generation here
        for(uint32_t i = 0 ; i < hres ; i++)
        {
            uint32_t sample_index;
            sample_index = i;

            if (ui->state_scrollx)
            {
                sample_index = (sample_index + (hres-hphasecnt) + HPHASE_OFFSET) % (hres);	
            }
            else
            {
                sample_index = (sample_index + (hres-hphase_slider) + HPHASE_OFFSET) % (hres);
            }
            
            if (ui->state_mirrorx) { 
                if (sample_index >= (hres >> 1))
                {
                    sample_index = ((hres >> 1) - (sample_index + 1 - (hres >> 1))) << 1; 	
                }
                else
                {
                    sample_index = sample_index << 1;
                }
            }

            if (ui->state_invert)	{ sample_index = hres - 1 - sample_index; }

            uint32_t sample;
            if (lookupTableBuilder != nullptr)
            {
                uint32_t sampleplusramp;
                if ((lut[sample_index] + samples_wave[sampleReadPtr][sample_index]-512) > DAC_MAX_VALUE)
                {
                    sampleplusramp = DAC_MAX_VALUE;
                } 
                else if ((lut[sample_index] + samples_wave[sampleReadPtr][sample_index]) < 512)
                {
                    sampleplusramp = 0;
                }
                else
                {
                    sampleplusramp = (lut[sample_index] + samples_wave[sampleReadPtr][sample_index]-512);
                }
                sample = sampleplusramp & 1023;
            }
            else 
            {	
                sample = samples_wave[sampleReadPtr][sample_index];
            }

            hwave[waveWritePtr][i] = sample;
            hwave[waveWritePtr][hres+i] = sample;
        }

        vphase_slider = vphase_slider & 0b1111111111110;
        vphase_cv = vphase_cv & 0b1111111111110;

        for (uint32_t i = 0; i < vres; i++)
        {
            uint32_t sample_index;	
            sample_index = i;
            if (ui->state_scrolly)
            {
                sample_index = (sample_index + (vres - vphasecnt) + (vres - vphase_cv) + VPHASE_OFFSET) % (vres);	
            }
            else
            {
                sample_index = (sample_index + (vres-vphase_slider) + (vres-vphase_cv) + VPHASE_OFFSET) % (vres);
            }
            if (ui->state_mirrory) { 
                if (sample_index >= (vres >> 1))
                {
                    sample_index = ((vres >> 1) - (sample_index + 1 - (vres >> 1))) << 1; 	
                }
                else
                {
                    sample_index = sample_index << 1;
                }
            }
            if (ui->state_invert)	{ sample_index = vres - 1 - sample_index; }

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
            } else {
                sample = samples_wave[sampleReadPtr][sample_index];
            }

            vwave[waveWritePtr][i] = sample;
            hphase_cv[waveWritePtr][i] = sample_hphase;
            vwave[waveWritePtr][vres + vres - i] = sample;
            hphase_cv[waveWritePtr][vres + vres - i] = sample_hphase;
        }

        uint8_t hphase_interlace_mode = 1;  // 0 = video sampling, 1 = audio sampling
        uint8_t interlace_mode = 1;         // 0 = video sampling, 1 = audio sampling

        if (deinterlace_mode) {
            interlace_mode = 0; 
            hphase_interlace_mode = 0;
        }
                
        //Interlacing/deinterlacing here
        if(interlace_mode)
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
        if (deinterlace_mode)
        {
            uint16_t samples_deinterlaced[hres];
            for (uint32_t i = 0; i < hres; i++)
            {
                if (i &0b1) //even pixel
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

    void GenerateLUT()
    {
        if (lookupTableBuilder != nullptr)
        {
            for (uint32_t i = 0; i < vres; i++)
            {
                lut[i] = lookupTableBuilder(i, vres);
            }
        }
    }
};