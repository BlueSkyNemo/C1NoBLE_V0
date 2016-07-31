/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "wwdg.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "fpe.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "console.h"
#include "Thread.h"
#include "Tick.h"
#include "Button.h"
#include "DelayCall.h"
#include "lcd.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//UART_HandleTypeDef huart2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
#define SYSTEM_ATUOPOFF_MS 600000 //10min
enum BUTTON_ID
{
	BUTTON_DOME = 1,
	BUTTON_TOUCH = 2,
	BUTTON_NUMBER,
};

enum STATUS_ID
{
	STATUS_INIT = 0,	
	STATUS_SHOWMPM2D5,
	STATUS_POWEROFF,
};

enum LCD_DIRECTION
{
	LCD_DIR_UP,
	LCD_DIR_LEFT,
	LCD_DIR_DOWN,
	LCD_DIR_RIGHT,
};
console_t* g_main_console = NULL;
//uint32_t g_main_dustValue = 0;
uint8_t g_main_status = STATUS_INIT;
uint8_t g_lcd_scanDir = LCD_DIR_UP;
uint8_t g_dir = 0;
#define MAIN_STATUE_ADDR 0x0100
#define LCD_SCANDIR_ADDR 0x0108


#define UART2_BUFFER_SIZE 35
#define UART2_BUFFER_MASK (UART2_BUFFER_SIZE - 1)
#define DUST_BUFFER_SIZE 128
UART_HandleTypeDef* g_main_dustUart = NULL;
uint8_t g_main_dustBuffer[DUST_BUFFER_SIZE]= {0};
uint16_t g_uart2Index = 0;
uint32_t	g_main_dustValue1 = 0;
uint32_t	g_main_dustValue = 0;
uint32_t	g_main_dustValuec = 0;
uint32_t	g_main_dustValue10 = 0;


_Bool g_isIniting = 1;
_Bool g_isJustShowPm25 = 0;
uint32_t g_main_boot_tick = 0;
uint8_t g_main_refresh = 0;




uint8_t isButtonPressed(uint16_t id);

void onPowerButtonPressed(void);
void onModeButtonPressed(void);
void onMode2ButtonPressed(void);
void onTouchButtonPressed(void);

void lcd_update(void);
void dust_update(void);
void touchButton(void);

void dust_init(UART_HandleTypeDef* huart,uint8_t* buffer);

static void sensorPowerOff(void)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);//B2 power off
}
static void sensorPowerOn(void)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
}
static void hw_deinit(void)
{
	
	sensorPowerOff();
	__DMA1_CLK_DISABLE();
  __GPIOA_CLK_DISABLE();
  __GPIOB_CLK_DISABLE();
  __GPIOC_CLK_DISABLE();
  __GPIOD_CLK_DISABLE();
  __GPIOF_CLK_DISABLE();
}
static void loadFlashData(void)
{
	uint32_t value;
	value = fpe_read(0x0108);
	if (value != UINT32_MAX)
	{
		g_dir = value;
		lcd_setDir(g_dir);
	}	
}
static void hw_init()
{
	MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
	HAL_DMA_Init(&hdma_usart2_rx);
	HAL_Delay(100);
	dust_init(&huart2,g_main_dustBuffer);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
	HAL_Delay(50);
//	MX_USART2_UART_Init();
	lcd_init();
	lcd_clear_all(g_dir);
	HAL_Delay(50);
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
//	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
	HAL_Delay(50);
	dust_init(&huart2,g_main_dustBuffer);
	
//	g_isIniting = 1;
	
}

void test()
{
	uint32_t count = 0;
	count ++;
	if(count > 22222)
	{
		count = 1;
	}
	lcd_update_pm25(count,g_isJustShowPm25,g_dir);
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_WWDG_Init();

  /* USER CODE BEGIN 2 */
	  /* System interrupt init*/
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

//	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
//  {
 //   __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
//  }
	
	
//lcd_init();
	tick_init(&htim1);
	thread_init(0,tick_ms());
	
	button_init(BUTTON_DOME,isButtonPressed);
//	button_init(BUTTON_TOUCH,isButtonPressed);
	button_addListener(BUTTON_DOME,BUTTON_STATUS_HOLD,onPowerButtonPressed,0);
	button_addListener(BUTTON_DOME,BUTTON_STATUS_CLICKED_NO_NEXT,onModeButtonPressed,0);
	fpe_init(62,63);
	
	loadFlashData();
	hw_init();
	thread_quickAdd(0,delayCall_run,10,0,0);
//	thread_quickAdd(0,console_run,10,0,0);
	thread_quickAdd(0,button_run,10,0,0);
	thread_quickAdd(0,dust_update,10,1,0);
	#if DEBUG_ALL_UI
	thread_quickAdd(0,lcd_update,2000,10,0);
	#else
	thread_quickAdd(0,lcd_update,500,10,0);
	#endif
	
	//* tick
//	tick_add(delayCall_tick,10);
//	tick_add(console_tick,100);
	tick_add(button_tick,10);	
	tick_start();
	HAL_Delay(100);
	
	g_main_boot_tick = tick_ms() + SYSTEM_ATUOPOFF_MS;
	lcd_setDir(g_dir);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
//	HAL_WWDG_Start(&hwwdg);
//	lcd_showTest();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	
//		HAL_Delay(100);
	thread_run(0,tick_ms(),0,0);
//	HAL_WWDG_Refresh(&hwwdg, 0x7e);	
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

#define max(a,b) (a>b?a:b)
#define min(a,b) (a>b?b:a)
uint8_t isButtonPressed(uint16_t id)
{
	switch (id)
	{
		case BUTTON_DOME:
			return !HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_7);
//		case BUTTON_TOUCH:
//			return !HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2);
		default:
			return 0;
	}
}

void StandbyMode_Measure(void)
{
  /* Enable Power Clock*/
  __PWR_CLK_ENABLE();

  /* Allow access to Backup */
  HAL_PWR_EnableBkUpAccess();

  /* Reset RTC Domain */
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();
  
  /* The Following Wakeup sequence is highly recommended prior to each Standby mode entry
     mainly  when using more than one wakeup source this is to not miss any wakeup event.
       - Disable all used wakeup sources,
       - Clear all related wakeup flags, 
       - Re-enable all used wakeup sources,
       - Enter the Standby mode.
  */
  
  /*#### Disable all used wakeup sources: WKUP pin ###########################*/
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  
  /*#### Clear all related wakeup flags ######################################*/
  /* Clear PWR wake up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  
  /* Enable WKUP pin */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

  /* Request to enter STANDBY mode */
  HAL_PWR_EnterSTANDBYMode();
}


void onPowerButtonPressed(void)
{
//	console_printf(g_main_console,"Power off\r\n");
//	
	if (g_main_status == STATUS_POWEROFF)
	{
		g_main_status = STATUS_INIT;
//		fpe_write(0x0100,g_main_status);
		HAL_NVIC_SystemReset();
	}
	g_main_status = STATUS_POWEROFF;
	fpe_write(0x0100,g_main_status);
	lcd_clear_all(g_dir);
//	esp8266_deinit();
//	sensorPowerOff();
//	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_4,GPIO_PIN_RESET);//power
//	HAL_Delay(100);
//	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_4,GPIO_PIN_SET);//power
	//debug


}

void enterSleep(void)
{
	
//	hw_deinit();
	
	HAL_Delay(100);
//	{
		GPIO_InitTypeDef GPIO_InitStruct;
		__GPIOB_CLK_ENABLE();
		__SYSCFG_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* Enable and set Button EXTI Interrupt to the lowest priority */
		HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0x03, 0x00);
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
//		__GPIOB_CLK_DISABLE();
//	}
//	g_main_status = STATUS_POWEROFF;
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
//	hw_init();
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	
	g_main_status = STATUS_INIT;	
	g_isIniting = 1;
	//StandbyMode_Measure();
//	hw_init();
	HAL_NVIC_SystemReset();
	
}

uint8_t times;
static uint8_t history_status = 0;
void onModeButtonPressed(void)
{
//	g_isJustPlug = 0;
	g_isIniting = 0;
	if (g_main_status != STATUS_POWEROFF)
	{
		if(g_main_status == STATUS_INIT)
		{
			g_main_status = STATUS_SHOWMPM2D5;
		}
		if(g_dir <3)
		{
			g_dir++;
		}
		else
		{
			g_dir  = 0;
		}
		lcd_setDir(g_dir);
		fpe_write(0x0108,g_dir);
		lcd_clear_all(g_dir);
		lcd_showStaticImage(0,g_dir);
		__disable_irq();
		lcd_update_pm25(g_main_dustValue,1,g_dir);
		__enable_irq();
//		fpe_write(0x0100,g_main_status);
//		HAL_NVIC_SystemReset();
	}
	else
	{

		g_main_status = STATUS_INIT;	
		lcd_clear_all(g_dir);
		g_isJustShowPm25 = 1;
		fpe_write(0x0100,g_main_status);
		g_isIniting = 1;
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
	}
	g_main_boot_tick = tick_ms() + SYSTEM_ATUOPOFF_MS;
//	console_printf(g_main_console,"Status = %d\r\n",g_main_status);
}
void onMode2ButtonPressed(void)
{
	
	g_main_boot_tick = tick_ms() + SYSTEM_ATUOPOFF_MS;
}
//void onTouchButtonPressed(void)
//{
//	
//	onModeButtonPressed();
//	
//}
#define abs(a) (a<0?-a:a)
void lcd_update(void)
{
	static uint8_t status = STATUS_INIT;
	static uint32_t initCounter = 0;
	static uint32_t Counter = 0;
//	Counter ++;
	if(g_isIniting)
	{
		initCounter ++;
	}
	if (g_isIniting && initCounter == 3)
	{
		g_main_status = STATUS_SHOWMPM2D5;
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
		g_isJustShowPm25 = 1;
		g_isIniting = 0;
		initCounter = 0;
	}
	switch (g_main_status)
	{

		case STATUS_INIT:
		{
			lcd_showStaticImage(1,g_dir);
		  break;
		}
		case STATUS_SHOWMPM2D5:
		{
			if(g_isJustShowPm25 == 1)
			{
				lcd_clear_all(g_dir);
				lcd_showStaticImage(0,g_dir);
				__disable_irq();
				lcd_update_pm25(g_main_dustValue,g_isJustShowPm25,g_dir);
				__enable_irq();
				g_isJustShowPm25 = 0;
			}
			__disable_irq();
			lcd_update_pm25(g_main_dustValue,g_isJustShowPm25,g_dir);
			__enable_irq();
		break;
		}
		case STATUS_POWEROFF:
		{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
		break;
		}
		default:
			break;
	}

}

		
void dust_init(UART_HandleTypeDef* huart,uint8_t* buffer)
{
	//HAL_GPIO_WritePin(GPIOF,GPIO_PIN_5,GPIO_PIN_SET);
//	memset(g_uart2Buffer,UINT8_MAX-1,sizeof(g_uart2Buffer));
//	memset(g_uart2Buffer,254,sizeof(g_uart2Buffer));
	__IO uint8_t status = HAL_UART_Receive_DMA(huart,buffer, DUST_BUFFER_SIZE);
	g_main_dustUart = huart;
}
void dust_send(char* str)
{
	HAL_UART_Transmit(g_main_dustUart,(uint8_t*)str, strlen(str),10);
}
static inline uint8_t dustBuffer(uint16_t index)
{
	return g_main_dustBuffer[index&(DUST_BUFFER_SIZE-1)];
}
static inline uint32_t dustValue(uint16_t index)
{
	uint32_t toRet = 0;
	for (uint8_t i=0; i<4; i++)
	{
		toRet |=  ((uint32_t)g_main_dustBuffer[(index+i)&(DUST_BUFFER_SIZE-1)])<<(i<<3);
	}
	return toRet;
}


void dust_update(void)
{
	uint8_t match = 0;
	for(uint8_t i=0; i<DUST_BUFFER_SIZE; i++)
	{
		if(dustBuffer(i)==0x5a && dustBuffer(i+1)==0xa5 && dustBuffer(i+2)==0x34 &&dustBuffer(i+3)==0x00)
		{
			uint16_t sum = 0;
			for(uint8_t j=0; j<14; j++)
			{
				sum += dustBuffer((j<<2) + i);
				sum += dustBuffer((j<<2) + 1 + i);
				sum += dustBuffer((j<<2) + 2 + i)<<8;
//				sum += dustBuffer((j<<2) + 3 + i);
			}
			if ((((uint16_t)dustBuffer(i+58))<<8) + (dustBuffer(i+59)) == sum)
			{
				match = 1;
				g_main_dustValue1 = dustValue(i+8);
				g_main_dustValue = dustValue(i+16);
				g_main_dustValuec = dustValue(i+20);
				g_main_dustValue10 = dustValue(i+24);
				break;
			}
		}
	}
	if (match == 0)
	{
		dust_send("AT+outtypef=4\r\n");
	}
	dust_send("AT+fco\r\n");

#if DEBUG_RAND
	g_main_dustValue = rand()%4000;
#endif
}
/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
