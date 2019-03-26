/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_DMA_H
#define LIB_MICRO_DMA_H

#include "ErrorHandler.h"
#include "Timer.h"
#include <cstdlib>
#include <stm32h7xx.h>

/*
  Basics
  ======
  * Requests are like intewrrupts : they come from a peripheral to DMA, and there is a lot of them (like hundred).
  * Every DMA (DMA1 or DMA2) has 2 ports : the memory port and the peripheral port.
  * It can take control over the AHB bus (200MHz on H7).

Transaction : there is no special meaning of this I think. Transaction is simply an unit of work the DMA has to perform.

EDIT : not true. In 14.3.11 Double-buffer mode they explicitly say about swaping pointers at the end of transaction which
means, the start and end of a trasaction must be defined somewhere.

+ Transfers. Transaction performs a number of transfers.
 +

Arbiter : deals with stream priorities (4 programable, and then lower stream number wins).

8 Streams. Each stream works in one direction only.

 */

class Dma {
public:
        struct Mburst {
                static constexpr uint32_t SINGLE = 0x00000000U;
                static constexpr uint32_t INC4 = DMA_SxCR_MBURST_0;
                static constexpr uint32_t INC8 = DMA_SxCR_MBURST_1;
                static constexpr uint32_t INC16 = DMA_SxCR_MBURST;
        };

        struct Pinc {
                static constexpr uint32_t ENABLE = DMA_SxCR_PINC;
                static constexpr uint32_t DISABLE = 0x00000000U;
        };

        struct Minc {
                static constexpr uint32_t ENABLE = DMA_SxCR_MINC;
                static constexpr uint32_t DISABLE = 0x00000000U;
        };

        struct Dir {
                static constexpr uint32_t PERIPH_TO_MEMORY = 0x00000000U;
                static constexpr uint32_t MEMORY_TO_PERIPH = DMA_SxCR_DIR_0;
                static constexpr uint32_t MEMORY_TO_MEMORY = DMA_SxCR_DIR_1;
        };

        struct PAlign {
                static constexpr uint32_t BYTE = 0x00000000U;
                static constexpr uint32_t HALFWORD = DMA_SxCR_PSIZE_0;
                static constexpr uint32_t WORD = DMA_SxCR_PSIZE_1;
        };

        struct MAlign {
                static constexpr uint32_t BYTE = 0x00000000U;
                static constexpr uint32_t HALFWORD = DMA_SxCR_MSIZE_0;
                static constexpr uint32_t WORD = DMA_SxCR_MSIZE_1;
        };

        struct Mode {
                static constexpr uint32_t NORMAL = 0x00000000U;
                static constexpr uint32_t CIRCULAR = DMA_SxCR_CIRC;
                static constexpr uint32_t PFCTRL = DMA_SxCR_PFCTRL;
        };

        struct Priority {
                static constexpr uint32_t LOW = 0x00000000U;
                static constexpr uint32_t MEDIUM = DMA_SxCR_PL_0;
                static constexpr uint32_t HIGH = DMA_SxCR_PL_1;
                static constexpr uint32_t VERY_HIGH = DMA_SxCR_PL;
        };
};

void initDma1Stream0 (uint8_t *source, uint8_t *destination, size_t numberOfElements)
{
        // Potential parameters:
        constexpr size_t DMA_NUMBER = 1;
        constexpr size_t STREAM_NUMBER = 0;
        constexpr size_t CONFIG_TIMEOUT_MS = 1000;

        //__HAL_RCC_DMA2_CLK_ENABLE ();

        //        {
        //                __IO uint32_t tmpreg;
        //                SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN); /* Delay after an RCC peripheral clock enabling */
        //                tmpreg = READ_BIT (RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);
        //                (void)(tmpreg);
        //        }

        // 1. If the stream is enabled, disable it by resetting the EN bit in the DMA_SxCR register,
        // then read this bit in order to confirm that there is no ongoing stream operation. Writing
        // this bit to 0 is not immediately effective since it is actually written to 0 once all the
        // current transfers are finished. When the EN bit is read as 0, this means that the stream
        // is ready to be configured. It is therefore necessary to wait for the EN bit to be cleared
        // before starting any stream configuration. All the stream dedicated bits set in the status
        // register (DMA_LISR and DMA_HISR) from the previous data block DMA transfer must
        // be cleared before the stream can be re-enabled.

        // Turn off the stream. Only then can it be configured.
        DMA1_Stream0->CR &= ~DMA_SxCR_EN;

        // Wait for EN to become 0
        for (Timer timeout (CONFIG_TIMEOUT_MS); DMA1_Stream0->CR & DMA_SxCR_EN;) {
                if (timeout.isExpired ()) {
                        Error_Handler ();
                }
        }

        // 2. Set the peripheral port register address in the DMA_SxPAR register. The data is moved
        // from/ to this address to/ from the peripheral port after the peripheral event.
        DMA1_Stream0->PAR = reinterpret_cast<uint32_t> (source);

        // 3. Set the memory address in the DMA_SxMA0R register (and in the DMA_SxMA1R
        // register in the case of a double-buffer mode). The data is written to or read from this
        // memory after the peripheral event.
        DMA1_Stream0->M0AR = reinterpret_cast<uint32_t> (destination);

        // 4. Configure the total number of data items to be transferred in the DMA_SxNDTR
        // register. After each peripheral event or each beat of the burst, this value is
        // decremented.
        DMA1_Stream0->NDTR = numberOfElements;

        // 5. Use DMAMUX1 to route a DMA request line to the DMA channel.
        // 6. If the peripheral is intended to be the flow controller and if it supports this feature, set
        // the PFCTRL bit in the DMA_SxCR register.
        // 7. Configure the stream priority using the PL[1:0] bits in the DMA_SxCR register.
        // 8. Configure the FIFO usage (enable or disable, threshold in transmission and reception)

        // 9. Configure the data transfer direction, peripheral and memory incremented/fixed mode,
        // single or burst transactions, peripheral and memory data widths, circular mode,
        // double-buffer mode and interrupts after half and/or full transfer, and/or errors in the
        // DMA_SxCR register.

        //        uint32_t registerValue = DMA1_Stream0->CR;

        /* Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC, DIR, CT and DBM bits */
        //        registerValue &= ((uint32_t) ~(DMA_SxCR_MBURST | DMA_SxCR_PBURST | DMA_SxCR_PL | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE |
        //        DMA_SxCR_MINC
        //                                       | DMA_SxCR_PINC | DMA_SxCR_CIRC | DMA_SxCR_DIR | DMA_SxCR_CT | DMA_SxCR_DBM));

        /* Prepare the DMA Stream configuration */
        //        registerValue |= hdma->Init.Direction | hdma->Init.PeriphInc | hdma->Init.MemInc | hdma->Init.PeriphDataAlignment
        //                | hdma->Init.MemDataAlignment | hdma->Init.Mode | hdma->Init.Priority;

        //        uint32_t registerValue = Dma::Minc::ENABLE | Dma::Pinc::ENABLE | Dma::Dir::MEMORY_TO_MEMORY | Dma::PAlign::WORD |
        //        Dma::MAlign::WORD
        //                | Dma::Mode::NORMAL | Dma::Priority::VERY_HIGH;

        //        DMA1_Stream0->CR = Dma::Minc::ENABLE | Dma::Pinc::ENABLE | Dma::Dir::MEMORY_TO_MEMORY | Dma::PAlign::BYTE | Dma::MAlign::BYTE;
        DMA1_Stream0->CR = Dma::Minc::ENABLE | Dma::Pinc::ENABLE | Dma::Dir::MEMORY_TO_MEMORY | Dma::PAlign::WORD | Dma::MAlign::WORD
                | Dma::Mode::NORMAL | Dma::Priority::HIGH;

        uint32_t registerValue = DMA1_Stream0->FCR;
        /* Clear Direct mode and FIFO threshold bits */
        registerValue &= ~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);
        DMA1_Stream0->FCR = registerValue;

        /*---------------------------------------------------------------------------*/
        // (DMAMUX)
        /*
        Request Input (107) : signal FROM a peripheral (107 possibilities in total )

        Request output channel (16) : gignal from a peripheral that went through the DMAMUX and is heading to a DMA
         DMAMUX1 Output channels 0-7 are connected to DMA1, channels 8-15 to DMA2.

        Request trigger inputs (8) : ???

        Request synchronisation inputs (8) : ???
        */

        DMAMUX1_Channel0->CCR = 0x0;
        //        DMAMUX1_RequestGenerator0->RGCR;
        DMAMUX1_ChannelStatus->CFR = 0;
        //        DMAMUX1_ChannelStatus->CSR;
        //        DMAMUX1_RequestGenStatus->RGSR;
        //        DMAMUX1_RequestGenStatus->RGCFR;

        /*---------------------------------------------------------------------------*/

        // All the stream dedicated bits set in the status
        // register (DMA_LISR and DMA_HISR) from the previous data block DMA transfer must
        // be cleared before the stream can be re-enabled.
        for (Timer timeout (CONFIG_TIMEOUT_MS); DMA1->HISR != 0 && DMA1->LISR != 0;) {
                if (timeout.isExpired ()) {
                        Error_Handler ();
                }
        }

        // 10. Activate the stream by setting the EN bit in the DMA_SxCR register.
        DMA1_Stream0->CR |= DMA_SxCR_EN;
}

#endif // DMA_H
