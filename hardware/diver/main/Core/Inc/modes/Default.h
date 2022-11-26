#include "main.h"
#include "globals.h"

#include "DiverUI.h"
#include "modes/DiverMode.h"

struct ModeDefault : DiverMode
{

    void OnActivate()
    {

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
            switch (ui->selected_bank)
            {
            case 6:		
            case 7:	 sample = samples_wave[sampleReadPtr][sample_index]; break;	
            default: sample = sampleplusramp&1023; break;
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
            switch (ui->selected_bank)
            {
            case 6:		
            case 7:	 sample = samples_wave[sampleReadPtr][sample_index]; break;	
            default: sample = sampleplusramp & 1023; break;
            }				
            vwave[waveWritePtr][i] = sample;
            hphase_cv[waveWritePtr][i] = sample_hphase;
            vwave[waveWritePtr][vres + vres - i] = sample;
            hphase_cv[waveWritePtr][vres + vres - i] = sample_hphase;
        }


        switch (ui->selected_bank)
        {
        case 7:	deinterlace_mode = 1; interlace_mode = 0; hphase_interlace_mode = 0; break;
        default:	deinterlace_mode = 0; interlace_mode = 1; hphase_interlace_mode = 1; break;
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
};


void GenerateLUT(uint8_t waveform)
{
	//uint16_t fib_width = vres >> 2;
	//uint16_t fib[5] = {  55 - 55, 89 - 55, 144 - 55, 233 - 55, 377 - 55};

	for (uint32_t i = 0; i < vres; i++)
	{
		float a = float(i) / float(vres);
		float b = (a*a) * 1023.0;
		float c = (a * 1023.0 * 2.0) - (b);

		//uint16_t cur_fib_segment = i / fib_width;
		//uint16_t cur_fib_interpolate_a = ((i % fib_width)*DAC_MAX_VALUE) / fib_width; 
		//uint16_t fib_result = ((fib[cur_fib_segment]*1023)/322) + ((cur_fib_interpolate_a*(((fib[cur_fib_segment+1]-fib[cur_fib_segment])*DAC_MAX_VALUE)/322))/DAC_MAX_VALUE);

		switch (waveform)
		{
			case 0: lut[i] = ((i*DAC_MAX_VALUE) / vres) & 1023; break;
			case 1: lut[i] = uint16_t(c) & 1023; break;
			case 2: lut[i] = uint16_t(b) & 1023; break;		
			case 3: lut[i] = ((i*DAC_MAX_VALUE) / vres) & 0b1111000000; break;
			case 4: lut[i] = ((i*DAC_MAX_VALUE) / vres) & 0b1110000000; break;
			case 5: lut[i] = ((i*DAC_MAX_VALUE) / vres) & 0b1100000000; break;

		}
	}
}
