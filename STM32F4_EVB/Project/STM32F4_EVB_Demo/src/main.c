/**
  ******************************************************************************
  * @file    Project/STM32F4_EVB_Demo/main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-March-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "stdio.h"
//#include "usbd_cdc_core.h"
//#include "usbd_usr.h"
//#include "usb_conf.h"
//#include "usbd_desc.h"
//#include "stm32f4x7_eth.h"
//#include "netconf.h"
//#include "lwip/tcp.h"
#include "FreeRTOS.h"
#include "task.h"


/** @addtogroup STM32F4xx_StdPeriph_Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if defined (USE_STM324xG_EVAL)
	#define MESSAGE1   "     STM32F40xx     "
	#define MESSAGE2   " Device running on  "
	#define MESSAGE3   "   STM324xG-EVAL    "
#elif defined (USE_STM32F4_EVB)
	#define MESSAGE1   "       STM32F407xx       "
	#define MESSAGE2   "    Device running on    "
	#define MESSAGE3   "       STM32F4-EVB       "
#else /* USE_STM324x7I_EVAL */
	#define MESSAGE1   "     STM32F427x     "
	#define MESSAGE2   " Device running on  "
	#define MESSAGE3   "  STM324x7I-EVAL    "
#endif
#define SYSTEMTICK_PERIOD_MS 	10
#define AUDIO_FILE_SZE			990000
#define AUIDO_START_ADDRESS		58 /* Offset relative to audio file header size */



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint32_t cid, revid;
//USB_OTG_CORE_HANDLE	USB_OTG_dev;
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
__IO uint32_t uwVolume = 70;
static __IO uint32_t uwTimingDelay;
uint32_t testdata[sEE_PAGESIZE / 4], readdata[sEE_PAGESIZE / 4];
uint16_t uhNumDataRead;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void LED_task(void *pvParameters)
{
  RCC_ClocksTypeDef RCC_Clocks;
  uint8_t togglecounter = 0x00;
	STM_EVAL_LEDInit(LED1);
	STM_EVAL_LEDInit(LED2);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	
  while(1)
  {    
      /* Toggle LED5 */
      STM_EVAL_LEDToggle(LED1);
      vTaskDelay(100);
			STM_EVAL_LEDToggle(LED2);
      vTaskDelay(100);
			STM_EVAL_LEDToggle(LED3);
      vTaskDelay(100);
			STM_EVAL_LEDToggle(LED4);
      vTaskDelay(100);
      /* Toggle LED6 */
      //STM_EVAL_LEDOff(LED1);
      //vTaskDelay(100);
  }
}

static void LCD_task(void *pvParameters){
	//LCD_Init();
	/* Display message on LCD *************************************************/
	LCD_Init();
	while(1){
		LCD_Clear(LCD_COLOR_WHITE);
		LCD_Colorbar();
		LCD_SetBackColor(LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_BLUE);
		LCD_DisplayStringLine(LCD_LINE_0, (uint8_t *)MESSAGE1);
		LCD_DisplayStringLine(LCD_LINE_1, (uint8_t *)MESSAGE2);
		LCD_DisplayStringLine(LCD_LINE_2, (uint8_t *)MESSAGE3);
		vTaskDelay(1000);
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_Colorbar();
		LCD_SetBackColor(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_WHITE);
		LCD_DisplayStringLine(LCD_LINE_0, (uint8_t *)MESSAGE1);
		LCD_DisplayStringLine(LCD_LINE_1, (uint8_t *)MESSAGE2);
		LCD_DisplayStringLine(LCD_LINE_2, (uint8_t *)MESSAGE3);
		vTaskDelay(1000);
	}
}

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
 	RCC_ClocksTypeDef RCC_Clocks;

	/* SysTick end of count event each 10ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	PSRAM_Init();
	SystemInit();
	
	xTaskCreate(LED_task,(signed portCHAR *) "Implement LED",512 /* stack size */, NULL, tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(LCD_task,(signed portCHAR *) "Implement LED",512 /* stack size */, NULL, tskIDLE_PRIORITY + 2, NULL);
	
	vTaskStartScheduler(); 
	return 0;
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
	uwTimingDelay = nTime;

	while (uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
	if (uwTimingDelay != 0x00)
	{
		uwTimingDelay--;
	}
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
	LocalTime += SYSTEMTICK_PERIOD_MS;
}

/*--------------------------------
       Callbacks implementation:
           the callbacks prototypes are defined in the stm32f4_evb_audio_codec.h file
           and their implementation should be done in the user coed if they are needed.
           Below some examples of callback implementations.
                                     --------------------------------------------------------*/
/**
  * @brief  Calculates the remaining file size and new position of the pointer.
  * @param  None
  * @retval None
  */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
#ifdef AUDIO_MAL_MODE_NORMAL  
	/* Replay from the beginning */
#else /* #ifdef AUDIO_MAL_MODE_CIRCULAR */
	/* Display message on the LCD screen */
	LCD_DisplayStringLine(Line8, " All Buffer Reached ");   
#endif /* AUDIO_MAL_MODE_CIRCULAR */
}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @param  None
  * @retval None
  */
void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size)
{  
#ifdef AUDIO_MAL_MODE_CIRCULAR
	/* Display message on the LCD screen */
	LCD_DisplayStringLine(Line8, " 1/2 Buffer Reached "); 
#endif /* AUDIO_MAL_MODE_CIRCULAR */
}

/**
  * @brief  Manages the DMA FIFO error interrupt.
  * @param  None
  * @retval None
  */
void EVAL_AUDIO_Error_CallBack(void* pData)
{
	/* Display message on the LCD screen */
	LCD_SetBackColor(Red);
	LCD_DisplayStringLine(Line8, (uint8_t *)"     DMA  ERROR     ");
	/* Stop the program with an infinite loop */
	while (1) {};
}

/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t Codec_TIMEOUT_UserCallback(void)
{
	/* Display message on the LCD screen */
	LCD_DisplayStringLine(Line8, (uint8_t *)"  CODEC I2C  ERROR  ");  

	/* Block communication and all processes */
	while (1) {};
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
void vApplicationMallocFailedHook(){
}
void vApplicationIdleHook(){
}
void vApplicationTickHook(){
}