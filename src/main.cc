/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "stm32h7xx_hal.h"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>

/*****************************************************************************/

static void SystemClock_Config ();
static void Error_Handler ();

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

void Error_Handler ()
{
        while (true) {
        }
}
