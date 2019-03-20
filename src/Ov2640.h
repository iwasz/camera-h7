/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef OV2640_H
#define OV2640_H

#include <cstdint>
#include <cstdlib>

/**
 * @brief Ov3640 drivber based on ArduCam.
 */
class Ov2640 {
public:
        struct SensorReg {
                uint16_t address;
                uint8_t value;
        };

        enum SensorResolution : uint8_t { VGA, QVGA };

        Ov2640 (SensorResolution resolution = QVGA);

        // OV7670 = 0x42 (0x21);
        // Pozostałe 0x78 (0x3c)

        // Ten adres jest OK dla czarnej z długim obiektywem.
        // static constexpr uint8_t SENSOR_I2C_ADDRESS = 0x42;

        // To jest OK dla czerwonej.
        static constexpr uint8_t SENSOR_I2C_ADDRESS = 0x60;

        enum JpegResolution : uint8_t {
                OV2640_160x120,
                OV2640_176x144,
                OV2640_320x240,
                OV2640_352x288,
                OV2640_640x480,
                OV2640_800x600,
                OV2640_1024x768,
                OV2640_1280x960,
                OV2640_1280x1024,
                OV2640_1600x1200,
                OV2640_2048x1536
        };

        void setJpegSize (JpegResolution size);

        enum LightMode : uint8_t { Auto = 0, Sunny = 1, Cloudy = 2, Office = 3, Home = 4 };

        void setLightMode (LightMode lightMode);

        enum ColorSaturation : uint8_t {
                Saturation4,
                Saturation3,
                Saturation2,
                Saturation1,
                Saturation0,
                Saturation_1,
                Saturation_2,
                Saturation_3,
                Saturation_4
        };

        void setColorSaturation (ColorSaturation colorSaturation);

        enum Brightness : uint8_t {
                Brightness4,
                Brightness3,
                Brightness2,
                Brightness1,
                Brightness0,
                Brightness_1,
                Brightness_2,
                Brightness_3,
                Brightness_4
        };

        void setBrightness (Brightness brightness);

        enum Contrast : uint8_t { Contrast4, Contrast3, Contrast2, Contrast1, Contrast0, Contrast_1, Contrast_2, Contrast_3, Contrast_4 };

        void setContrast (Contrast contrast);

        enum SpecialEffect : uint8_t {
                Antique,
                Bluish,
                Greenish,
                Reddish,
                BW,
                Negative,
                BWnegative,
                Normal,
                Sepia,
                Overexposure,
                Solarize,
                Blueish,
                Yellowish
        };

        void setSpecialEffects (SpecialEffect specialEffect);

        uint16_t getProductId ();

private:
        void wrSensorRegs16_8 (SensorReg const reglist[], size_t len);
        void wrSensorReg16_8 (uint16_t regID, uint8_t regDat);
        uint8_t rdSensorReg16_8 (uint16_t regId);

        void wrSensorRegs8_8 (SensorReg const reglist[], size_t len);
        void wrSensorReg8_8 (uint8_t regID, uint8_t regDat);
        uint8_t rdSensorReg8_8 (uint8_t regId);
};

#endif // OV3640_H
