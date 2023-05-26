/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : usbd_custom_hid_if.c
 * @version        : v2.0_Cube
 * @brief          : USB Device Custom HID interface file.
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
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
 * @brief Usb device.
 * @{
 */

/** @addtogroup USBD_CUSTOM_HID
 * @{
 */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
 * @brief Private types.
 * @{
 */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
 * @}
 */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
 * @brief Private defines.
 * @{
 */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
 * @}
 */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
 * @brief Private macros.
 * @{
 */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
 * @}
 */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
 * @brief Private variables.
 * @{
 */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
    {
        /* USER CODE BEGIN 0 */
        /* 告诉PC我是桌面类的 */
        0x05, 0x01, // USAGE_PAGE (Generic Desktop)
        /* 告诉PC我是干什么的，他也可以定义成鼠标或者键盘，具体根据自己需求*/
        //                0x09, 0x04, // USAGE (Joystick)
        0x09, 0x05, // USAGE (Game Pad)

        /* 他告诉PC端我的物理逻辑，我们属于输入设备，以外部物理逻辑判断我们的行为再向PC端输入 */
        0xa1, 0x01, // COLLECTION (Application)
                    //        0xa1, 0x02, //   COLLECTION (Logical)

        /* 告诉PC我要添加按键属性 */
        0x05, 0x09, // USAGE_PAGE (Button)
        // 按键最小是1最大是32，这里PC最多只能添加32个按键
        0x19, 0x01, // USAGE_MINIMUM (Button 1)
        0x29, 0x20, // USAGE_MAXIMUM (Button 32)
        // 按键逻辑最小为0，最大为1，按键就0和1的逻辑，按下和抬起
        0x15, 0x00, // LOGICAL_MINIMUM (0)
        0x25, 0x01, // LOGICAL_MAXIMUM (1)
        // 按键报告是1个bit，共报告25次，所以占用4个字节
        0x95, 0x08, // REPORT_COUNT (4bytes)
        0x75, 0x04, // REPORT_SIZE (8bit)
        0x81, 0x02, // INPUT (Data,Var,Abs)

        /*需要接受的数据*/
        /* Byte0: LED亮灭 AP_Master AP1 AP2 A/THR LOC EXPED APPR   ALT_INC(0为100 1为1000) */
        /* Byte1: OLED显示类型 SPD_DASHES SPD_DOT SPD_MACH HDG_DASHES HDG_DOT ALT_DOT VS_DASHES TRK_FPA */
        /* Byte2: OLED显示数据 SPD_SELECTED(float) */
        /* Byte3: OLED显示数据 SPD_SELECTED(float) */
        /* Byte4: OLED显示数据 SPD_SELECTED(float) */
        /* Byte5: OLED显示数据 SPD_SELECTED(float) */
        /* Byte6: OLED显示数据 HDG_SELECTED(int16_t) */
        /* Byte7: OLED显示数据 HDG_SELECTED(int16_t) */
        /* Byte8: OLED显示数据 FPA_SELECTED(int8_t) */
        /* Byte9: OLED显示数据 VS_SELECTED(1%)(int8_t) */
        /* Byte10: OLED显示数据 ALT_SELECTED(uint16_t) */
        /* Byte11: OLED显示数据 ALT_SELECTED(uint16_t) */
        0x09, 0x04,                  //   USAGE (Undefined)
        0x15, 0x00,                  //   LOGICAL_MINIMUM (0)
        0x26, 0xff, 0x00,            //   LOGICAL_MAXIMUM (255)
        0x75, 0x08,                  //   REPORT_SIZE (8)
        0x95, CUSTOM_HID_EPOUT_SIZE, //   REPORT_COUNT (12)
        0x91, 0x02,                  //   OUTPUT (Data,Var,Abs)

        /* 结束，前面用到几个COLLECTION结尾就用到几个结束。
         * 上述报告描述符共用到3个字节: 按键24个共3个字节
         * 报告描述符修改完了 接下来修改设备描述符 */
        //        0xc0, // END_COLLECTION
        /* USER CODE END 0 */
        0xC0 /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
 * @}
 */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
 * @brief Public variables.
 * @{
 */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
 * @}
 */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
 * @brief Private functions declaration.
 * @{
 */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
 * @}
 */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
    {
        CUSTOM_HID_ReportDesc_FS,
        CUSTOM_HID_Init_FS,
        CUSTOM_HID_DeInit_FS,
        CUSTOM_HID_OutEvent_FS};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
 * @brief Private functions.
 * @{
 */

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the CUSTOM HID media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CUSTOM_HID_Init_FS(void)
{
    /* USER CODE BEGIN 4 */
    return (USBD_OK);
    /* USER CODE END 4 */
}

/**
 * @brief  DeInitializes the CUSTOM HID media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
    /* USER CODE BEGIN 5 */
    return (USBD_OK);
    /* USER CODE END 5 */
}

/**
 * @brief  Manage the CUSTOM HID class events
 * @param  event_idx: Event index
 * @param  state: Event state
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
    /* USER CODE BEGIN 6 */
    return (USBD_OK);
    /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
 * @brief  Send the report to the Host
 * @param  report: The report to be sent
 * @param  len: The report length
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */