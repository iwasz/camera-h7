/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/
#ifndef OV3640_H
#define OV3640_H

#include <cstdint>
#include <cstdlib>

/**
 * @brief Ov3640 drivber based on ArduCam.
 */
class Ov3640 {
public:
        struct SensorReg {
                uint16_t address;
                uint8_t value;
        };

        enum SensorResolution : uint8_t { VGA, QVGA };

        Ov3640 (SensorResolution resolution = QVGA);

        // OV7670 = 0x42 (0x21);
        // Pozostałe 0x78 (0x3c)

        static constexpr uint8_t SENSOR_I2C_ADDRESS = 0x42;

        enum JpegResolution : uint8_t {
                OV3640_176x144 = 0,   // 176x144
                OV3640_320x240 = 1,   // 320x240
                OV3640_352x288 = 2,   // 352x288
                OV3640_640x480 = 3,   // 640x480
                OV3640_800x600 = 4,   // 800x600
                OV3640_1024x768 = 5,  // 1024x768
                OV3640_1280x960 = 6,  // 1280x960
                OV3640_1600x1200 = 7, // 1600x1200
                OV3640_2048x1536 = 8  // 2048x1536
        };

        void setJpegSize (JpegResolution size);

        enum LightMode : uint8_t { Auto = 0, Sunny = 1, Cloudy = 2, Office = 3, Home = 4 };

        void setLightMode (LightMode lightMode);

        enum ColorSaturation : uint8_t {
                Saturation4 = 0,
                Saturation3 = 1,
                Saturation2 = 2,
                Saturation1 = 3,
                Saturation0 = 4,
                Saturation_1 = 5,
                Saturation_2 = 6,
                Saturation_3 = 7,
                Saturation_4 = 8
        };

        void setColorSaturation (ColorSaturation colorSaturation);

        enum Brightness : uint8_t {
                Brightness4 = 0,
                Brightness3 = 1,
                Brightness2 = 2,
                Brightness1 = 3,
                Brightness0 = 4,
                Brightness_1 = 5,
                Brightness_2 = 6,
                Brightness_3 = 7,
                Brightness_4 = 8
        };

        void setBrightness (Brightness brightness);

        enum Contrast : uint8_t {
                Contrast4 = 0,
                Contrast3 = 1,
                Contrast2 = 2,
                Contrast1 = 3,
                Contrast0 = 4,
                Contrast_1 = 5,
                Contrast_2 = 6,
                Contrast_3 = 7,
                Contrast_4 = 8
        };

        void setContrast (Contrast contrast);

        enum SpecialEffect : uint8_t {
                Antique = 0,
                Bluish = 1,
                Greenish = 2,
                Reddish = 3,
                BW = 4,
                Negative = 5,
                BWnegative = 6,
                Normal = 7,
                Sepia = 8,
                Overexposure = 9,
                Solarize = 10,
                Blueish = 11,
                Yellowish = 12
        };

        void setSpecialEffects (SpecialEffect specialEffect);

        enum ExposureLevel : uint8_t {
                Exposure_17_EV = 0,
                Exposure_13_EV = 1,
                Exposure_10_EV = 2,
                Exposure_07_EV = 3,
                Exposure_03_EV = 4,
                Exposure_default = 5,
                Exposure07_EV = 6,
                Exposure10_EV = 7,
                Exposure13_EV = 8,
                Exposure17_EV = 9,
                Exposure03_EV = 10
        };

        void setExposureLevel (ExposureLevel level);

        enum Sharpness : uint8_t {
                Sharpness1 = 0,
                Sharpness2 = 1,
                Sharpness3 = 2,
                Sharpness4 = 3,
                Sharpness5 = 4,
                Sharpness6 = 5,
                Sharpness7 = 6,
                Sharpness8 = 7,
                Sharpness_auto = 8
        };

        void setSharpness (Sharpness sharpness);

        enum MirrorFlip : uint8_t { MIRROR = 0, FLIP = 1, MIRROR_FLIP = 2, FLIP_NORMAL };

        void setMirrorFlip (MirrorFlip mirrorFlip);

private:
        void wrSensorRegs16_8 (SensorReg const reglist[], size_t len);
        void wrSensorReg16_8 (uint16_t regID, uint8_t regDat);
        uint8_t rdSensorReg16_8 (uint16_t regId);
};

#endif // OV3640_H
