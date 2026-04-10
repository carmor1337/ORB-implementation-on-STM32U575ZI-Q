/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cordic.h"
#include "crc.h"
#include "dcache.h"
#include "flash.h"
#include "fmac.h"
#include "icache.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "test_image.h"
#include "ORB.h"
#include "FAST.h"
#include <string.h>
#include "Benchmarking.h"
#include "Benchmarking_map.h"
#include "config.h"
#include "common_includes.h"
#include "output.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DEBUG_VAR(x) (void)(x)
//#define UNUSED __attribute__((unused))
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Makes it so printf gets to the debugger terminal
	int _write(int fd, char *ptr, int len) {
		(void)fd;
	    for (int i = 0; i < len; i++) {
	        while (ITM->PORT[0].u32 == 0);  // wait until ready
	        ITM->PORT[0].u8 = (uint8_t)ptr[i];
	    }
	    return (len);
	}

// In main.c, add these two functions
void stack_paint(void) {
	extern uint32_t _estack;
	extern uint32_t _Min_Stack_Size;
	uint32_t stack_bottom = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
	uint32_t *p = (uint32_t*)stack_bottom;
	while (p < (uint32_t*)__get_MSP())
		*p++ = 0xDEADBEEF;
}

uint32_t stack_usage(void) {
	extern uint32_t _estack;
	extern uint32_t _Min_Stack_Size;
	uint32_t stack_bottom = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
	uint32_t *p = (uint32_t*)stack_bottom;
	while (*p == 0xDEADBEEF) p++;
	return (uint32_t)&_estack - (uint32_t)p;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	stack_paint();


	//CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	//DWT->CYCCNT = 0;
	//DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


	DWT_init();
	DWT_MapInit();
	int8_t idx_ORB_total =   DWT_register("ORB");
#if ORB_PROFILING
	int8_t idx_FAST_total=   DWT_register("FAST");
	int8_t idx_HARRIS_total = DWT_register("Harris");
	int8_t idx_Centroid_total = DWT_register("Centroid");
	int8_t idx_rBRIEF_total = DWT_register("rBRIEF");

	DEBUG_VAR(idx_FAST_total);
    DEBUG_VAR(idx_HARRIS_total);
    DEBUG_VAR(idx_Centroid_total);
    DEBUG_VAR(idx_rBRIEF_total);
#endif
	// Fast profiles
#if FAST_PROFILING
	int8_t idx_FAST_HSP=   DWT_register("FAST: HST");
	DEBUG_VAR(idx_FAST_HSP);
	int8_t idx_FAST_get_circle=   DWT_register("FAST: Circle");
	DEBUG_VAR(idx_FAST_get_circle);
#endif

	// Harris profiles

#if HARRIS_PROFILING
	int8_t idx_HARRIS_compute_matrix = DWT_register("HARRIS:matrix");
	DEBUG_VAR(idx_HARRIS_compute_matrix);
#endif

	// Centroid Profiles

#if CENTROID_PROFILING
	int8_t idx_Centroid_momentumsl = DWT_register("Centroid:m01,m10");
	int8_t idx_Centroid_atan2 = DWT_register("Centroid:atan2");

	DEBUG_VAR(idx_Centroid_momentumsl);
	DEBUG_VAR(idx_Centroid_atan2);
#endif

	// rBRIEF profiles

#if rBRIEF_PROFILING
	int8_t idx_rBRIEF_rotation = DWT_register("rBRIEF:rotation");
	int8_t idx_rBRIEF_sample = DWT_register("rBRIEF:sample");

	DEBUG_VAR(idx_rBRIEF_rotation);
	DEBUG_VAR(idx_rBRIEF_sample);
#endif


	// Trying to get the image into ram
	static uint8_t image_ram[IMAGE_WIDTH * IMAGE_HEIGTH];
	memcpy(image_ram, test_image, sizeof(image_ram));

	static const int num_pixels = IMAGE_HEIGTH * IMAGE_WIDTH;
	// Kitty dataset has 1241 x 376
	static const int32_t kitti_num_pixels = 2341 * 376;



  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  volatile uint32_t clock1 = HAL_RCC_GetHCLKFreq();

   ORB_init(image_ram);
  /* USER CODE END Init */

  /* Configure the System Power */
  SystemPower_Config();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CORDIC_Init();
  MX_CRC_Init();
  MX_DCACHE1_Init();
  MX_FLASH_Init();
  MX_FMAC_Init();
  MX_ICACHE_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */
  // To update the system clock
  SystemCoreClockUpdate();

  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_RED);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //HAL_StatusTypeDef osc_result = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  //HAL_StatusTypeDef clk_result = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  //printf(osc_result);
  volatile uint32_t clock2 = HAL_RCC_GetHCLKFreq();



  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  BSP_LED_Toggle(LED_GREEN);
	  HAL_Delay(LED_BLINK_WAIT);
	  BSP_LED_Toggle(LED_RED);
	  HAL_Delay(LED_BLINK_WAIT);

	  DWT_start(idx_ORB_total);

	  ORB_extract_and_match();

	  DWT_stop(idx_ORB_total);

	  DWT_convert_all_profiles_to_timed();
	  DWT_Profile_t *ORB_profile = DWT_get(idx_ORB_total);
	  DWT_timed_pair_t *Orb_profile_timed =DWT_get_timed(idx_ORB_total);

#if ORB_PROFILING
	  DWT_timed_pair_t *FAST_timed =DWT_get_timed(idx_FAST_total);
	  DWT_timed_pair_t *HARRIS_timed =DWT_get_timed(idx_HARRIS_total);
	  DWT_timed_pair_t *Centroid_timed =DWT_get_timed(idx_Centroid_total);
	  DWT_timed_pair_t *rBRIEF_timed =DWT_get_timed(idx_rBRIEF_total);
	  DWT_timed_pair_t *ORB_all_profiles_timed =DWT_get_timed_registry();

	  DEBUG_VAR(ORB_all_profiles_timed);
	  DEBUG_VAR(FAST_timed);
	  DEBUG_VAR(HARRIS_timed);
	  DEBUG_VAR(Centroid_timed);
	  DEBUG_VAR(rBRIEF_timed);
#endif
	  double us_per_pixel = (int)ORB_profile->avg/num_pixels;
	  us_per_pixel = us_per_pixel/(SystemCoreClock / 1000000.0);
	  double kitti_time_us = us_per_pixel*kitti_num_pixels;
	  const char feature_msg[] = "None";
	  const char performance_msg[] = "Improved the consecutive check by AND:ing the bits when right shifted";
	  output_commit_message(feature_msg,performance_msg );

	  uint32_t used = stack_usage();
	  DWT_aggregate_reset_all();

	  DEBUG_VAR(idx_ORB_total);


	  DEBUG_VAR(ORB_profile);
	  DEBUG_VAR(Orb_profile_timed);


	  DEBUG_VAR(num_pixels);
	  DEBUG_VAR(kitti_num_pixels);
	  DEBUG_VAR(kitti_time_us);


	  DEBUG_VAR(clock1);
	  DEBUG_VAR(clock2);
	  DEBUG_VAR(used);
	  DEBUG_VAR(Orb_profile_timed);
	  DEBUG_VAR(Orb_profile_timed);
	  DEBUG_VAR(ORB_profile);
	  DEBUG_VAR(Orb_profile_timed);
	  DEBUG_VAR(Orb_profile_timed);

	  /*
	  uint32_t microseconds = cycles / (SystemCoreClock / 1000000U);
	  volatile double milliseconds = (double)microseconds /1000.0;

	  ORB_feature_point_t first_one = feature_points[0];
	  volatile uint16_t num_of_feature_points = FAST_get_feature_count();
	  volatile double microseconds_per_pixel = (double)microseconds/num_pixels;
	  volatile double KITII_time_Extrapolated = (1241*376) * microseconds_per_pixel/ 1000.0;

	  DEBUG_VAR(KITII_time_Extrapolated);
	  DEBUG_VAR(num_of_feature_points);
	  DEBUG_VAR(first_one);
	  DEBUG_VAR(milliseconds);
	  */



  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  /** MCO configuration
  */
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
   */
  HAL_PWREx_DisableUCPDDeadBattery();

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
