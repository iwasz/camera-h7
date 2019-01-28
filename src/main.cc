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
static void EXTI15_10_IRQHandler_Config ();
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

        /* Configure the system clock to 400 MHz */
        SystemClock_Config ();

        /* -1- Initialize LEDs mounted on STM32H743ZI-NUCLEO board */
        //        BSP_LED_Init (LED1);

        /* Infinite loop */
        volatile int i = 0;
        while (1) {
                ++i;
        }
}

/*****************************************************************************/

static void SystemClock_Config (void)
{
        RCC_ClkInitTypeDef RCC_ClkInitStruct;
        RCC_OscInitTypeDef RCC_OscInitStruct;
        HAL_StatusTypeDef ret = HAL_OK;

        /*!< Supply configuration update enable */
        MODIFY_REG (PWR->CR3, PWR_CR3_SCUEN, 0);

        /* The voltage scaling allows optimizing the power consumption when the device is
           clocked below the maximum system frequency, to update the voltage scaling value
           regarding system frequency refer to product datasheet.  */
        __HAL_PWR_VOLTAGESCALING_CONFIG (PWR_REGULATOR_VOLTAGE_SCALE1);

        while (!__HAL_PWR_GET_FLAG (PWR_FLAG_VOSRDY)) {
        }

        /* Enable HSE Oscillator and activate PLL with HSE as source */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
        RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
        RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

        RCC_OscInitStruct.PLL.PLLM = 4;
        RCC_OscInitStruct.PLL.PLLN = 400;
        RCC_OscInitStruct.PLL.PLLP = 2;
        RCC_OscInitStruct.PLL.PLLR = 2;
        RCC_OscInitStruct.PLL.PLLQ = 4;

        RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
        RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
        ret = HAL_RCC_OscConfig (&RCC_OscInitStruct);
        if (ret != HAL_OK) {
                Error_Handler ();
        }

        /* Select PLL as system clock source and configure  bus clocks dividers */
        RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1
                                       | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1);

        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
        RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
        ret = HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_4);
        if (ret != HAL_OK) {
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
