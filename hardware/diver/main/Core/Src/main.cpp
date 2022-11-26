

/* Includes ------------------------------------------------------------------*/

#define MAIN_CPP

#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
//#include "usb_device.h"
#include "gpio.h"

#include "globals.h"
#include "DiverUI.h"
#include "modes/BankLayout.h"

#include <math.h>
#include <string>
#include <vector>
#include <stdio.h>

void SystemClock_Config(void);
void data_transmitted_handler(DMA_HandleTypeDef *hdma);
void transmit_error_handler(DMA_HandleTypeDef *hdma);


template <class T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept
{
    return N;
}

DiverUIState state(size(banks));
DiverUI ui(state);

extern "C"
{
	void on_framestart()
	{
		banks[state.selected_bank]->OnInterruptFrameStart(state);
	}

	void on_hsync()
	{
		ui.OnInterruptHSync();
		banks[state.selected_bank]->OnInterruptHSync(state);
	}

	void on_bank_changed()
	{
		banks[state.selected_bank]->OnActivate(state);
	}
}


int main(void)
{
	HAL_Init();
	HAL_Delay(250);
	SystemClock_Config();
	HAL_Delay(250);
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	MX_ADC1_Init();
	//MX_USB_DEVICE_Init();

	MX_TIM1_Init();
	HAL_TIM_Base_MspInit(&htim1);
	
	ui.Display_Init();
	for (DiverMode* bank : banks) {
		bank->OnInit();
	}

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);	
	//HAL_NVIC_SetPriority(SysTick_IRQn, 4,0);
	HAL_NVIC_SetPriority(ADC_IRQn, 4, 0);
	HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 1, 0); 
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);

	//HAL_NVIC_EnableIRQ(SysTick_IRQn);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
	HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn); 
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	HAL_Delay(250);

	if (lines_per_frame > 575)
	{
		vres = 624; 		
	}
	else
	{
		vres = 524;
	}

	hres = 524;

	on_bank_changed();

	htim1.hdma[TIM_DMA_ID_UPDATE]->XferCpltCallback = data_transmitted_handler;
	htim1.hdma[TIM_DMA_ID_UPDATE]->XferErrorCallback = transmit_error_handler;
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(DAC_RW_GPIO_Port, DAC_RW_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DAC_RW_GPIO_Port, DAC_AB_Pin, GPIO_PIN_RESET);
	HAL_DMA_Start_IT(htim1.hdma[TIM_DMA_ID_UPDATE], (uint32_t)&hwave, (uint32_t)&GPIOC->ODR, HBLANK + HRES + 2);
	__HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);
	__HAL_TIM_ENABLE(&htim1);
	HAL_ADC_Start(&hadc1);
	
	while (1)
	{
		if (evenfield_event && oddfield_event == 0) {
			evenfield_event = 0;         	// new even field
		}
								
		if (oddfield_event)
		{
			oddfield_event = 0;            // new odd field

			ui.Buttons_Poll();
			banks[state.selected_bank]->OnOddField(state);
			ui.Display_Refresh();
		}		
	}	
}

void data_transmitted_handler(DMA_HandleTypeDef *hdma)
{
	TIM1->CR1 = 0; 
	hwave[waveReadPtr][hres + HBLANK] = vwave[waveReadPtr][linecnt] | 0b10000000000;
	HAL_DMA_Start_IT(htim1.hdma[TIM_DMA_ID_UPDATE], (uint32_t)&hwave[waveReadPtr][hphase_cv[waveReadPtr][linecnt]%hres], (uint32_t)&GPIOC->ODR, hres + HBLANK + 3);
}

extern "C"
{
	void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
	{
	}
 
	void ADC_IRQHandler()
	{
		HAL_ADC_IRQHandler(&hadc1);
	}
}
 
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Configure the main internal regulator output voltage 
	*/
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 27;
	//RCC_OscInitStruct.PLL.PLLN = 192;
	RCC_OscInitStruct.PLL.PLLN = 216;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		//_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time 
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick 
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	//HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */





void transmit_error_handler(DMA_HandleTypeDef *hdma)
{
}

void _Error_Handler(char *file, int line)
{
	while (1)
	{
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
