/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "usbd_customhid.h"
#include "bsp_key.h"
#include "u8g2.h"
#include "u8g2_test.h"
#include "stm32_u8g2.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// typedef  struct FCUPanelDataReceive {
////    struct {
////        uint16_t ap_master: 1;
////        uint16_t ap1: 1;
////        uint16_t ap2: 1;
////        uint16_t ap_throttle: 1;
////        uint16_t loc_mode_active: 1;
////        uint16_t exped_mode_active: 1;
////        uint16_t appr_mode_active: 1;
////        uint16_t alt_inc: 1;
////        uint16_t spd_dashes: 1;
////        uint16_t spd_dot: 1;
////        uint16_t spd_mach: 1;
////        uint16_t hdg_dashes: 1;
////        uint16_t hdg_dot: 1;
////        uint16_t alt_dot: 1;
////        uint16_t vs_dashes: 1;
////        uint16_t trk_fpa_mode: 1;
////    };
//    uint8_t flag1;
//    uint8_t flag2;
//    float spd_selected;
//    int16_t hdg_selected;
//    int16_t fpa_selected;
//    uint16_t alt_selected;
//    int8_t vs_selected;
//} panel_receive_t;

uint32_t usb_send_buf = {0};
unsigned char usb_receive_buf[13];    // USB接收缓存
unsigned char USB_Received_Count = 0; // USB接收数据计数
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

//extern USBD_HandleTypeDef hUsbDeviceFS;
//
//static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
//{
//    return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
//}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* 使用状态机来完成数据维护 */
typedef struct FCU_Panel_State {

    struct {
        uint16_t ap_master : 1;
        uint16_t ap1_active : 1;
        uint16_t ap2_active : 1;
        uint16_t ap_throttle_active : 1;
        uint16_t loc_mode_active : 1;
        uint16_t exped_mode_active : 1;
        uint16_t appr_mode_active : 1;
        uint16_t spd_mach_mode : 1;
        uint16_t trk_fpa_mode : 1;
        uint16_t metric_mode : 1;

        uint16_t spd_dashes : 1;
        uint16_t spd_dot : 1;
        uint16_t hdg_dashes : 1;
        uint16_t hdg_dot : 1;
        uint16_t alt_dot : 1;
        uint16_t alt_increment : 1; // 0|1=>100|1000
        uint16_t vs_dashes : 1;
    };
    float spd_selected;
    uint16_t hdg_selected;
    int16_t fpa_selected;
    uint16_t alt_selected;
    int16_t vs_selected;
} panel_state_t;

void panel_led_update(panel_state_t *panel)
{
    HAL_GPIO_WritePin(AP1_LED_GPIO_Port, AP1_LED_Pin, panel->ap1_active);
    HAL_GPIO_WritePin(AP2_LED_GPIO_Port, AP2_LED_Pin, panel->ap2_active);
    HAL_GPIO_WritePin(ATHR_LED_GPIO_Port, ATHR_LED_Pin, panel->ap_throttle_active);
    HAL_GPIO_WritePin(LOC_LED_GPIO_Port, LOC_LED_Pin, panel->loc_mode_active);
    HAL_GPIO_WritePin(EXPED_LED_GPIO_Port, EXPED_LED_Pin, panel->exped_mode_active);
    HAL_GPIO_WritePin(APPR_LED_GPIO_Port, APPR_LED_Pin, panel->appr_mode_active);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

    u8g2_t screen_left;
    u8g2_t screen_right;
    u8g2_init_hi2c2(&screen_left);
    u8g2_init_hi2c1(&screen_right);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//    HAL_GPIO_WritePin(ATHR_LED_GPIO_Port, ATHR_LED_Pin, panel->ap_throttle_active);
    HAL_GPIO_TogglePin(ATHR_LED_GPIO_Port, ATHR_LED_Pin);

//        u8g2_FirstPage(&screen_left);
//        do {
//            draw(&screen_left);
//
//            u8g2DrawTest(&screen_left);
//        } while (u8g2_NextPage(&screen_left));
//        bsp_key_init();
//        matrix_key_scan();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
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
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
