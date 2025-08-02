/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : STM32 project using ADC to read an LDR sensor and control an LED
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"

/* External peripheral handles */
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

/* Thresholds for light detection */
#define LED_ON_THRESH   600   // Turn LED ON below this light level
#define LED_OFF_THRESH  1000  // Turn LED OFF above this light level

/* Global variables */
uint16_t lux = 0;               // ADC value representing light intensity
char msg[40];                   // UART message buffer
static uint8_t led_on = 0;      // Tracks current LED state (0 = off, 1 = on)

/* Function Prototypes */
void SystemClock_Config(void);

/**
  * @brief  Main program entry point
  * @retval int
  */
int main(void)
{
  /* Initialize the HAL Library */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize peripherals: GPIO, ADC, UART */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();

  /* Main program loop */
  while (1)
  {
    // Start ADC conversion
    HAL_ADC_Start(&hadc1);

    // Wait until ADC conversion is complete
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

    // Read converted value (light intensity)
    lux = HAL_ADC_GetValue(&hadc1);

    // LED control logic with hysteresis to avoid flickering
    if (!led_on && lux < LED_ON_THRESH)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);   // Turn LED ON
      led_on = 1;
    }
    else if (led_on && lux > LED_OFF_THRESH)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET); // Turn LED OFF
      led_on = 0;
    }

    // Format and send serial output (e.g., "Light: 734 | LED: ON")
    sprintf(msg, "Light: %hu | LED: %s\r\n", lux, led_on ? "ON" : "OFF");
    HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

    // Small delay for stability and readability in terminal
    HAL_Delay(250);
  }
}

/**
  * @brief  Configure the system clock
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  // Configure internal oscillator and PLL
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  // Configure system, AHB, and APB clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  // Select peripheral clocks for USART2 and ADC1
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Error handler function
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    // Stay in infinite loop on critical error
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Report assert_param errors
  * @param  file: Source file name
  * @param  line: Line number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  // Optional: Add debug print here
}
#endif /* USE_FULL_ASSERT */
