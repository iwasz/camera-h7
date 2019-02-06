/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Ov3640.h"
#include "stm32h7xx_hal.h"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>

/*****************************************************************************/

static void SystemClock_Config ();
extern "C" void Error_Handler ();

/*****************************************************************************/
/* Camera                                                                    */
/*****************************************************************************/

#include "stm324x9i_eval_camera.h"

#define CAMERA_I2C_ADDRESS 0x60
I2C_HandleTypeDef hi2c1;

extern "C" void DMA2_Stream1_IRQHandler () { BSP_CAMERA_DMA_IRQHandler (); }

/**
 * @brief  DCMI interrupt handler.
 * @param  None
 * @retval None
 */
extern "C" void DCMI_IRQHandler () { BSP_CAMERA_IRQHandler (); }

constexpr uint32_t getResolutionSize (uint32_t resolution)
{
        uint32_t size = 0;

        /* Get capture size */
        switch (resolution) {
        case CAMERA_R160x120: {
                size = 0x2580;
        } break;
        case CAMERA_R320x240: {
                size = 0x9600;
        } break;
        default: {
                break;
        }
        }

        return size;
}

/*****************************************************************************/

void myCamera (uint8_t *buf, size_t size, uint32_t current_resolution)
{
        DCMI_HandleTypeDef hdcmi;

        hdcmi.Instance = DCMI;
        hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
        hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_FALLING;
        hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
        hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
        hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
        hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
        hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
        hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
        hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
        hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
        hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;

        if (HAL_DCMI_Init (&hdcmi) != HAL_OK) {
                Error_Handler ();
        }

        /*---------------------------------------------------------------------------*/

        GPIO_InitTypeDef GPIO_InitStruct = { 0 };

        __HAL_RCC_DCMI_CLK_ENABLE ();

        __HAL_RCC_DMA1_CLK_ENABLE ();

        __HAL_RCC_GPIOE_CLK_ENABLE ();
        __HAL_RCC_GPIOA_CLK_ENABLE ();
        __HAL_RCC_GPIOC_CLK_ENABLE ();
        __HAL_RCC_GPIOD_CLK_ENABLE ();
        __HAL_RCC_GPIOG_CLK_ENABLE ();
        /**DCMI GPIO Configuration
        PE4     ------> DCMI_D4
        PE5     ------> DCMI_D6
        PE6     ------> DCMI_D7
        PA4     ------> DCMI_HSYNC
        PA6     ------> DCMI_PIXCLK
        PC6     ------> DCMI_D0
        PC7     ------> DCMI_D1
        PC8     ------> DCMI_D2
        PC9     ------> DCMI_D3
        PD3     ------> DCMI_D5
        PG9     ------> DCMI_VSYNC
        */
        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOE, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOD, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOG, &GPIO_InitStruct);

        /* DCMI DMA Init */
        /* DCMI Init */
        DMA_HandleTypeDef hdma_dcmi = { 0 };
        hdma_dcmi.Instance = DMA1_Stream0;
        hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_dcmi.Init.Request = DMA_REQUEST_DCMI;
        hdma_dcmi.Init.MemInc = DMA_MINC_ENABLE;
        hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        hdma_dcmi.Init.Mode = DMA_NORMAL;
        hdma_dcmi.Init.Priority = DMA_PRIORITY_MEDIUM;
        hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

        if (HAL_DMA_Init (&hdma_dcmi) != HAL_OK) {
                Error_Handler ();
        }

        __HAL_LINKDMA (&hdcmi, DMA_Handle, hdma_dcmi);

        /*** Configure the NVIC for DCMI and DMA ***/
        /* NVIC configuration for DCMI transfer complete interrupt */
        HAL_NVIC_SetPriority (DCMI_IRQn, 0x0F, 0);
        HAL_NVIC_EnableIRQ (DCMI_IRQn);

        /* NVIC configuration for DMA2D transfer complete interrupt */
        HAL_NVIC_SetPriority (DMA2_Stream1_IRQn, 0x0F, 0);
        HAL_NVIC_EnableIRQ (DMA2_Stream1_IRQn);

        /* Configure the DMA stream */
        HAL_DMA_Init (hdcmi.DMA_Handle);

        /*---------------------------------------------------------------------------*/

        HAL_DCMI_Init (&hdcmi);

        /*---------------------------------------------------------------------------*/

        //        if (ov2640_ReadID (CAMERA_I2C_ADDRESS) == OV2640_ID) {
        //                /* Camera Init */
        //                ov2640_Init (CAMERA_I2C_ADDRESS, current_resolution);
        //        }
        Ov3640 camera;

        // Continuous frames.
        // HAL_DCMI_Start_DMA (&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)buff, GetSize (current_resolution));

        // Single frame
        HAL_DCMI_Start_DMA (&hdcmi, DCMI_MODE_SNAPSHOT, reinterpret_cast<uint32_t> (buf), size);
}

/*---------------------------------------------------------------------------*/

/**
 * @brief  Initializes Camera low level.
 */
void CAMERA_IO_Init ()
{
        GPIO_InitTypeDef GPIO_InitStruct = { 0 };

        __HAL_RCC_GPIOB_CLK_ENABLE ();
        /**I2C1 GPIO Configuration
        PB6     ------> I2C1_SCL
        PB9     ------> I2C1_SDA
        */
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init (GPIOB, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE ();

        hi2c1.Instance = I2C1;
        hi2c1.Init.Timing = 0x10B0DCFB;
        hi2c1.Init.OwnAddress1 = 0;
        hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        hi2c1.Init.OwnAddress2 = 0;
        hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
        hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

        if (HAL_I2C_Init (&hi2c1) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_I2CEx_ConfigAnalogFilter (&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
                Error_Handler ();
        }

        if (HAL_I2CEx_ConfigDigitalFilter (&hi2c1, 0) != HAL_OK) {
                Error_Handler ();
        }
}

// byte ArduCAM::wrSensorReg16_8(int regID, int regDat)
//{
//    #if defined (RASPBERRY_PI)
//        arducam_i2c_word_write(regID, regDat);
//        arducam_delay_ms(1);
//    #else
//        Wire.beginTransmission(sensor_addr >> 1);
//      Wire.write(regID >> 8);            // sends instruction byte, MSB first
//      Wire.write(regID & 0x00FF);
//      Wire.write(regDat & 0x00FF);
//      if (Wire.endTransmission())
//      {
//        return 0;
//      }
//      delay(1);
//    #endif
//    return 1;
//}

/**
 * @brief  Camera delay..
 * @param  Delay: Delay in ms
 */
void CAMERA_Delay (uint32_t Delay) { HAL_Delay (Delay); }

// void initAndShot ()
//{
//        /* Initialize the Camera */
//        BSP_CAMERA_Init (RESOLUTION_R480x272);

//        /* Wait 1s before Camera snapshot */
//        HAL_Delay (1000);

//        /* Start the Camera Capture */
//        BSP_CAMERA_SnapshotStart ((uint8_t *)CAMERA_FRAME_BUFFER);
//}

/*****************************************************************************/
/* /Camera                                                                   */
/*****************************************************************************/

int main ()
{

        /* Enable I-Cache */
        SCB_EnableICache ();

        /* Enable D-Cache */
        SCB_EnableDCache ();

        /* STM32H7xx HAL library initialization:
             - Systick timer is configured by default as source of time base, but user.
               can eventually implement his proper time base source (a general purpose.
               timer for example or other time source), keeping in mind that Time base.
               duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and.
               handled in milliseconds basis.
             - Set NVIC Group Priority to 4
             - Low Level Initialization
           */
        HAL_Init ();

        unsigned int jj = 76;
        float a = jj;

        /* Configure the system clock to 400 MHz */
        SystemClock_Config ();

        /* -1- Initialize LEDs mounted on STM32H743ZI-NUCLEO board */
        //        BSP_LED_Init (LED1);

        uint32_t resolution = RESOLUTION_R480x272;
        uint8_t buffer[getResolutionSize (resolution)];
        myCamera (buffer, sizeof (buffer), resolution);

        /* Infinite loop */
        volatile double dd = 0;
        while (1) {
                ++dd;
        }
}

/*****************************************************************************/

void SystemClock_Config (void)
{
        RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
        RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

        /**Supply configuration update enable
         */
        MODIFY_REG (PWR->CR3, PWR_CR3_SCUEN, 0);
        /**Configure the main internal regulator output voltage
         */
        __HAL_PWR_VOLTAGESCALING_CONFIG (PWR_REGULATOR_VOLTAGE_SCALE1);

        while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) {
        }

        /**Initializes the CPU, AHB and APB busses clocks
         */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_CSI;
        RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
        RCC_OscInitStruct.CSIState = RCC_CSI_ON;
        RCC_OscInitStruct.CSICalibrationValue = 16;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_CSI;
        RCC_OscInitStruct.PLL.PLLM = 1;
        RCC_OscInitStruct.PLL.PLLN = 200;
        RCC_OscInitStruct.PLL.PLLP = 2;
        RCC_OscInitStruct.PLL.PLLQ = 4;
        RCC_OscInitStruct.PLL.PLLR = 2;
        RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
        RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
        RCC_OscInitStruct.PLL.PLLFRACN = 0;

        if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
                Error_Handler ();
        }

        /**Initializes the CPU, AHB and APB busses clocks
         */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
        RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

        if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
                Error_Handler ();
        }

        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3 | RCC_PERIPHCLK_USB;
        PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
        PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

        if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInitStruct) != HAL_OK) {
                Error_Handler ();
        }

        /*activate CSI clock mondatory for I/O Compensation Cell*/
        __HAL_RCC_CSI_ENABLE ();

        /* Enable SYSCFG clock mondatory for I/O Compensation Cell */
        __HAL_RCC_SYSCFG_CLK_ENABLE ();

        /* Enables the I/O Compensation Cell */
        HAL_EnableCompensationCell ();
}

/*****************************************************************************/

namespace __gnu_cxx {
void __verbose_terminate_handler ()
{
        while (1)
                ;
}
} // namespace __gnu_cxx

extern "C" void Error_Handler ()
{
        while (true) {
        }
}
