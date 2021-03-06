CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET (CMAKE_VERBOSE_MAKEFILE OFF)

PROJECT (camera-h7)
ENABLE_LANGUAGE (ASM-ATT)

INCLUDE_DIRECTORIES("src/")
INCLUDE_DIRECTORIES("../libmicro/src/")
INCLUDE_DIRECTORIES("../etl/include/")

SET (CN_CBOR_ROOT "../cn-cbor")
INCLUDE_DIRECTORIES("${CN_CBOR_ROOT}/include")

SET (MQTT_PACKET_SRC "../paho.mqtt.embedded-c/MQTTPacket/src")
INCLUDE_DIRECTORIES("mqtt-x86" ${MQTT_PACKET_SRC})

SET (LIB_STATE_MACHINE_ROOT "../libstatemachine")
INCLUDE_DIRECTORIES ("${LIB_STATE_MACHINE_ROOT}/src")

# +-----------------+
# | My code         |
# +-----------------+
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf
    "src/Credentials.h"
    "src/main.cc"
    "src/Ov3640.h"
    "src/Ov3640.cc"
#    "src/Mt9t112.cc"
#    "src/Mt9t112.h"
#    "src/communicationInterface/AtCommandAction.cc"
#    "src/communicationInterface/AtCommandAction.h"
#    "src/communicationInterface/esp8266/Esp8266.cc"
#    "src/communicationInterface/esp8266/Esp8266.h"
#    "src/communicationInterface/esp8266/SendNetworkAction.cc"
#    "src/communicationInterface/esp8266/SendNetworkAction.h"

    "../libmicro/src/Blinker.h"
    "../libmicro/src/ErrorHandler.h"
    "../libmicro/src/ErrorHandler.c"
    "../libmicro/src/Hal.h"
    "../libmicro/src/HalStm32H7.h"
    "../libmicro/src/TimerF.cc"
    "../libmicro/src/Timer.h"
    "../libmicro/src/Debug.cc"
    "../libmicro/src/Debug.h"
    "../libmicro/src/debug.h"
    "../libmicro/src/Gpio.h"
    "../libmicro/src/GpioF.h"
    "../libmicro/src/GpioH.cc"
    "../libmicro/src/GpioH7.cc"
    "../libmicro/src/Usart.h"
    "../libmicro/src/UsartF.cc"
    "../libmicro/src/UsartF.h"
    "../libmicro/src/UsartF4.cc"
    "../libmicro/src/Pwm.cc"
    "../libmicro/src/Pwm.h"
    "../libmicro/src/collection/Queue.h"
    "../libmicro/src/collection/CircularBuffer.h"
    "../libmicro/src/collection/CircularBuffer.cc"

#    "${LIB_STATE_MACHINE_ROOT}/src/Action.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/DelayAction.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/State.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/StateMachine.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/TimeCounter.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/Transition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/Condition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/StringCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/TimePassedCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/BeginsWithCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/LikeCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/NotCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/BoolCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/BoolAction.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/IntegerAction.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/IntegerCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/OrCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/AndCondition.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/StateChangeAction.h"
#    "${LIB_STATE_MACHINE_ROOT}/src/StateCondition.h"

#    "${MQTT_PACKET_SRC}/MQTTConnectClient.c"
#    "${MQTT_PACKET_SRC}/MQTTConnect.h"
#    "${MQTT_PACKET_SRC}/MQTTConnectServer.c"
#    "${MQTT_PACKET_SRC}/MQTTDeserializePublish.c"
#    "${MQTT_PACKET_SRC}/MQTTFormat.c"
#    "${MQTT_PACKET_SRC}/MQTTFormat.h"
#    "${MQTT_PACKET_SRC}/MQTTPacket.c"
#    "${MQTT_PACKET_SRC}/MQTTPacket.h"
#    "${MQTT_PACKET_SRC}/MQTTPublish.h"
#    "${MQTT_PACKET_SRC}/MQTTSerializePublish.c"
#    "${MQTT_PACKET_SRC}/MQTTSubscribeClient.c"
#    "${MQTT_PACKET_SRC}/MQTTSubscribe.h"
#    "${MQTT_PACKET_SRC}/MQTTSubscribeServer.c"
#    "${MQTT_PACKET_SRC}/MQTTUnsubscribeClient.c"
#    "${MQTT_PACKET_SRC}/MQTTUnsubscribe.h"
#    "${MQTT_PACKET_SRC}/MQTTUnsubscribeServer.c"
#    "${MQTT_PACKET_SRC}/StackTrace.h"

#    "${CN_CBOR_ROOT}/include/cn-cbor/cn-cbor.h"
#    "${CN_CBOR_ROOT}/src/cbor.h"
#    "${CN_CBOR_ROOT}/src/cn-cbor.c"
#    "${CN_CBOR_ROOT}/src/cn-create.c"
#    "${CN_CBOR_ROOT}/src/cn-encoder.c"
#    "${CN_CBOR_ROOT}/src/cn-error.c"
#    "${CN_CBOR_ROOT}/src/cn-get.c"
)

# +--------------+
# | STM Cube     |
# +--------------+
# Workaround : rozdzielenie kodu C++ i C powoduje, że QTCreator dobrze parsuje nagłówki C++. Bez tego podziału QTCreator się gubi.
add_library ("stm" STATIC
#        "src/startup.c"
#        "src/syscalls.c"
        "${STARTUP_CODE}"
        "src/stm32h7xx_it.c"
        "src/stm32h7xx_hal_conf.h"
        "src/system_stm32h7xx.c"

        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_adc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_adc.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_cec.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_comp.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_conf_template.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_cortex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_crc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_crc.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_cryp_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_cryp.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dac_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dac.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dcmi.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_def.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dfsdm.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dma2d.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dma_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_dma.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_eth_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_eth.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_fdcan.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_flash_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_flash.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_gpio_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_gpio.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_hash_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_hash.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_hcd.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_hrtim.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_hsem.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_i2c_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_i2c.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_i2s_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_i2s.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_irda_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_irda.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_iwdg.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_jpeg.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_lptim.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_ltdc.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_mdios.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_mdma.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_mmc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_mmc.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_nand.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_nor.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_opamp_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_opamp.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_pcd_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_pcd.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_pwr_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_pwr.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_qspi.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_rcc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_rcc.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_rng.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_rtc_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_rtc.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_sai_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_sai.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_sd_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_sd.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_sdram.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_smartcard_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_smartcard.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_smbus.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_spdifrx.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_spi_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_spi.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_sram.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_swpmi.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_tim_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_tim.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_uart_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_uart.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_usart_ex.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_usart.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_wwdg.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_delayblock.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_fmc.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_sdmmc.h"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_usb.h"

        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_adc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cec.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_comp.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_crc.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_crc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cryp.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cryp_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dac.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dac_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dcmi.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dfsdm.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma2d.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_eth.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_eth_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_fdcan.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hash.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hash_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hcd.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hrtim.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2s.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2s_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_irda.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_iwdg.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_jpeg.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_lptim.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_ltdc.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdios.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mmc.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mmc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_nand.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_nor.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_opamp.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_opamp_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pcd_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_qspi.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rng.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rtc.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rtc_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sai.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sai_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sd.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sd_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sdram.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_smartcard.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_smartcard_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_smbus.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spdifrx.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sram.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_swpmi.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_usart.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_wwdg.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_delayblock.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_fmc.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_sdmmc.c"
        "${CUBE_ROOT}/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_usb.c"
)


TARGET_LINK_LIBRARIES (${CMAKE_PROJECT_NAME}.elf -Wl,--whole-archive stm -Wl,--no-whole-archive)

FIND_PROGRAM (OPENOCD openocd)
ADD_CUSTOM_TARGET("upload" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32h7x.cfg -c 'program ${CMAKE_PROJECT_NAME}.elf verify reset exit')
ADD_CUSTOM_TARGET("debug" DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${OPENOCD} -f /home/iwasz/local/share/openocd/scripts/interface/stlink.cfg -f /home/iwasz/local/share/openocd/scripts/target/stm32h7x.cfg)

ADD_CUSTOM_TARGET(${CMAKE_PROJECT_NAME}.bin ALL DEPENDS ${CMAKE_PROJECT_NAME}.elf COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin)
