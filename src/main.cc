/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "../../ium/ium-lite/src/StringQueue.h"
#include "Blinker.h"
#include "Debug.h"
#include "Gpio.h"
#include "Hal.h"
//#include "MQTTPacket.h"
#include "Ov3640.h"
#include "Pwm.h"
#include "Usart.h"
//#include "cn-cbor/cn-cbor.h"
//#include "communicationInterface/esp8266/Esp8266.h"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <new>

/*****************************************************************************/

static void SystemClock_Config ();
extern "C" void Error_Handler ();
extern "C" void initialise_monitor_handles ();

/*****************************************************************************/

/**
 * TODO improve. Can priority inversion occur?
 */
class MySink : public ICharacterSink {
public:
        MySink (StringQueue &g) : gsmQueue (g) {}
        virtual ~MySink () {}
        void onData (char c);
        void onError (uint32_t flags)
        {
                // silently discard all errors, because my ESP sometimes outputs gibberish after reset
        }

private:
        uint16_t rxBufferGsmPos = 0;
        char rxBufferGsm[128]; /// Bufor wejściowy na odpowiedzi z modemu. Mamy własny, gdyż kolejka może się w między czasie wyczyścić.
        StringQueue &gsmQueue;

        //        static char allData[256];
        //        uint8_t allDataCnt = 0;
};

/*****************************************************************************/
/* Camera                                                                    */
/*****************************************************************************/

I2C_HandleTypeDef hi2c1 = { 0 };
DCMI_HandleTypeDef hdcmi = { 0 };
DMA_HandleTypeDef hdma_dcmi = { 0 };

void BSP_CAMERA_IRQHandler ();
void BSP_CAMERA_DMA_IRQHandler ();
void CAMERA_IO_Init ();

static int dma1HandlerCnt = 0;
extern "C" void DMA1_Stream0_IRQHandler ()
{
        BSP_CAMERA_DMA_IRQHandler ();
        ++dma1HandlerCnt;
}

/**
 * @brief  DCMI interrupt handler.
 * @param  None
 * @retval None
 */
static int dcmiHandlerCnt = 0;
extern "C" void DCMI_IRQHandler ()
{
        BSP_CAMERA_IRQHandler ();
        ++dcmiHandlerCnt;
}

void BSP_CAMERA_IRQHandler (void) { HAL_DCMI_IRQHandler (&hdcmi); }

/**
 * @brief  Handles DMA interrupt request.
 */
void BSP_CAMERA_DMA_IRQHandler (void) { HAL_DMA_IRQHandler (hdcmi.DMA_Handle); }

/*****************************************************************************/

void myCamera (uint8_t *buf, size_t size)
{
        // Clock generation
        const int PWM_PERIOD = 200;

        // TIM3 -> APB1 (100MHz) (Ponieważ prescaler jest 1, to CK_INT = 100MHz. Gdyby nie to, to CK_INT = 200Hz).
        Pwm pwmLeft (TIM3, 1, 10 - 1);
        pwmLeft.enableChannels (Pwm::CHANNEL2);
        Gpio pwmLeftPin (GPIOB, GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, GPIO_AF2_TIM3);
        pwmLeft.setDuty (Pwm::CHANNEL2, 5 - 1);

        /*---------------------------------------------------------------------------*/

        HAL_StatusTypeDef status;

        hdcmi.Instance = DCMI;
        hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
        hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
        hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
        hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
        hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
        hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
        hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
        hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
        hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
        hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
        hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;

        //        if (HAL_DCMI_Init (&hdcmi) != HAL_OK) {
        //                Error_Handler ();
        //        }

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
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOE, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOD, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
        HAL_GPIO_Init (GPIOG, &GPIO_InitStruct);

        /* DCMI DMA Init */
        /* DCMI Init */
        hdma_dcmi.Instance = DMA1_Stream0;
        hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_dcmi.Init.Request = DMA_REQUEST_DCMI;
        hdma_dcmi.Init.MemInc = DMA_MINC_ENABLE;
        hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        hdma_dcmi.Init.Mode = DMA_CIRCULAR;
        hdma_dcmi.Init.Priority = DMA_PRIORITY_MEDIUM;
        hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        //        hdma_dcmi.Init.FIFOThreshold =

        __HAL_LINKDMA (&hdcmi, DMA_Handle, hdma_dcmi);

        /*** Configure the NVIC for DCMI and DMA ***/
        /* NVIC configuration for DCMI transfer complete interrupt */
        HAL_NVIC_SetPriority (DCMI_IRQn, 0x0F, 0);
        HAL_NVIC_EnableIRQ (DCMI_IRQn);

        /* NVIC configuration for DMA2D transfer complete interrupt */
        HAL_NVIC_SetPriority (DMA1_Stream0_IRQn, 0x0F, 0);
        HAL_NVIC_EnableIRQ (DMA1_Stream0_IRQn);

        /* Configure the DMA stream */
        status = HAL_DMA_Init (hdcmi.DMA_Handle);

        if (status != HAL_OK) {
                Error_Handler ();
        }

        /*---------------------------------------------------------------------------*/

        status = HAL_DCMI_Init (&hdcmi);

        if (status != HAL_OK) {
                Error_Handler ();
        }

        CAMERA_IO_Init ();

        /*---------------------------------------------------------------------------*/

        static Ov3640 camera (Ov3640::QVGA);

        // Continuous frames.
        // HAL_DCMI_Start_DMA (&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)buff, GetSize (current_resolution));

        // Single frame
        //        status = HAL_DCMI_Start_DMA (&hdcmi, DCMI_MODE_CONTINUOUS, reinterpret_cast<uint32_t> (buf), size / 4);
        status = HAL_DCMI_Start_DMA (&hdcmi, DCMI_MODE_SNAPSHOT, reinterpret_cast<uint32_t> (buf), size / 4);

        if (status != HAL_OK) {
                Error_Handler ();
        }
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
        GPIO_InitStruct.Pull = GPIO_PULLUP;
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

/**
 * @brief  Camera delay..
 * @param  Delay: Delay in ms
 */
void CAMERA_Delay (uint32_t Delay) { HAL_Delay (Delay); }

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

        Gpio led (GPIOB, GPIO_PIN_14);
        Blinker<4> blink (led, 1000, 50, 100, 50);

        Gpio debugGpio (GPIOD, GPIO_PIN_8 | GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART3);
        Usart debugUart (USART3, 115200);

        Debug debug (&debugUart);
        Debug::singleton () = &debug;
        ::debug = Debug::singleton ();
        ::debug->println ("camera test");

        Gpio espGpio1 (GPIOA, GPIO_PIN_3, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART2);
        Gpio espGpio2 (GPIOD, GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART2);
        HAL_NVIC_SetPriority (USART2_IRQn, 0x0F, 0);
        HAL_NVIC_EnableIRQ (USART2_IRQn);
        Usart espUart (USART2, 115200);

        // TODO może inna struktura danych
        //        StringQueue gsmQueue (32);
        //        MySink modemResponseSink (gsmQueue);
        //        espUart.setSink (&modemResponseSink);
        //        Esp8266 esp8266 (espUart, gsmQueue);
        //        espUart.startReceive ();

        /* -1- Initialize LEDs mounted on STM32H743ZI-NUCLEO board */
        //        BSP_LED_Init (LED1);

        initialise_monitor_handles ();
        printf ("Semihosting on camera-h7 project. Hello.");
        static constexpr size_t BUF_SIZE = 128000;
        // uint8_t buffer[BUF_SIZE];

        uint8_t *imageData = new (reinterpret_cast<void *> (0x24000000)) uint8_t[BUF_SIZE];
        memset (imageData, 0, BUF_SIZE);

        myCamera (imageData, BUF_SIZE);

        // uint8_t encodedCbor[1024];
        // 0x24000000 + 128000
        uint8_t *encodedCbor = new (reinterpret_cast<void *> (0x2401F400)) uint8_t[BUF_SIZE + 1024];

        // uint8_t encodedMqtt[2048];
        // 0x2401F400 + 128000 + 1024 -> ends at 0x2405E800 - 1
        uint8_t *encodedMqtt = new (reinterpret_cast<void *> (0x2403EC00)) uint8_t[BUF_SIZE + 2048];
        int encodedMqttLen = BUF_SIZE + 2048;

        ssize_t len = 0;
        ssize_t offset = 0;

        HAL_Delay (500);

        FILE *f = fopen ("data.dat", "w");

        if (!f) {
                Error_Handler ();
        }

        fwrite (imageData, BUF_SIZE, 1, f);

        fclose (f);
        Timer t;

        while (true) {
                blink.run ();
#if 0
                esp8266.run ();

                if (offset == 0) {

                        /*---------------------------------------------------------------------------*/

                        cn_cbor_errback err;
                        const char *dataStr = "abc";
                        cn_cbor *cb_map = cn_cbor_map_create (&err);
                        cn_cbor_map_put (cb_map, cn_cbor_string_create ("systick", &err), cn_cbor_int_create (HAL_GetTick (), &err), &err);
                        cn_cbor_map_put (cb_map, cn_cbor_string_create ("image", &err), cn_cbor_data_create (imageData, BUF_SIZE, &err), &err);

                        ssize_t encodedCborLen = cn_cbor_encoder_write (encodedCbor, 0, BUF_SIZE + 1024, cb_map);

                        //        ASSERT_DATA (enc_sz);

                        cn_cbor_free (cb_map);

                        /*---------------------------------------------------------------------------*/

                        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
                        data.clientID.cstring = "ium-lite";
                        data.keepAliveInterval = 20;
                        data.cleansession = 1;
                        //        data.username.cstring = "testuser";
                        //        data.password.cstring = "testpassword";
                        data.username.cstring = nullptr;
                        data.password.cstring = nullptr;
                        data.MQTTVersion = 4;

                        len = MQTTSerialize_connect (encodedMqtt, encodedMqttLen, &data);

                        MQTTString topicString = MQTTString_initializer;
                        topicString.cstring = "iwasz";
                        len += MQTTSerialize_publish ((encodedMqtt + len), encodedMqttLen - len, 0, 0, 0, 0, topicString, encodedCbor,
                                                      encodedCborLen);

                        len += MQTTSerialize_disconnect ((encodedMqtt + len), encodedMqttLen - len);

                        //        if (rc == len) {
                        //                // printf ("Successfully published\n");
                        //        }
                        //        else {
                        //                // printf ("Publish failed\n");
                        //        }

                        t.start (5000);
                }

                ssize_t bytesSend = 0;
                while ((bytesSend = esp8266.send (0, encodedMqtt + offset, size_t (len))) > 0) {
                        offset += bytesSend;
                }

                if (offset >= len) {
                        offset = 0;
                }
#endif
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

// char MySink::allData[256];

/*****************************************************************************/

void MySink::onData (char c)
{
        if (!std::isprint (c) && c != '\r' && c != '\n') {
                return;
        }

        //        allData[allDataCnt++] = c;

        if (rxBufferGsmPos > 0 && (c == '\r' || c == '\n')) {

                rxBufferGsm[rxBufferGsmPos] = '\0';

                // Nie wrzucamy na kolejkę odpowiedzi, które zawierają tylko \r\n
                if (rxBufferGsmPos > 1) {
#if 0
                        debug->print ("rx : ");
                        debug->println (rxBufferGsm);
#endif

                        //  __disable_irq ();

                        if (!gsmQueue.push_back ()) {
                                Error_Handler ();
                        }

                        char *queueBuffer = gsmQueue.back ();
                        strcpy (queueBuffer, rxBufferGsm);
                        // __enable_irq ();
                }

                rxBufferGsmPos = 0;
        }
        else if (std::isprint (c)) {
                rxBufferGsm[rxBufferGsmPos++] = c;
        }
}

/*****************************************************************************/

int onData (uint8_t *data) { ::debug->printArray (data, 1); }

/*****************************************************************************/

namespace __gnu_cxx {
void __verbose_terminate_handler ()
{
        while (1)
                ;
}
} // namespace __gnu_cxx

// Potrzebne do cn-cbor, nieprzenosne, zadziała tylko z GCC!
extern "C" uint32_t htonl (uint32_t a) { return __builtin_bswap32 (a); }
extern "C" uint16_t htons (uint16_t a) { return __builtin_bswap16 (a); }
