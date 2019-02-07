/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Ov3640.h"
#include <cstdint>
//#include <etl/array_view.h>
#include <iterator>

#define OV3640_CHIPID_HIGH 0x300a
#define OV3640_CHIPID_LOW 0x300b

const Ov3640::SensorReg OV3640_VGA[] = {
        { 0x3012, 0x80 }, { 0x3012, 0x80 }, { 0x304d, 0x45 }, { 0x3087, 0x16 }, { 0x30aa, 0x45 }, { 0x30b0, 0xff }, { 0x30b1, 0xff },
        { 0x30b2, 0x10 }, { 0x30d7, 0x10 }, { 0x309e, 0x00 }, { 0x3602, 0x26 }, { 0x3603, 0x4D }, { 0x364c, 0x04 }, { 0x360c, 0x12 },
        { 0x361e, 0x00 }, { 0x361f, 0x11 }, { 0x3633, 0x32 }, { 0x3629, 0x3c }, { 0x300e, 0x32 }, { 0x300f, 0x21 }, { 0x3010, 0x21 },
        { 0x3011, 0x00 }, { 0x304c, 0x81 }, { 0x3018, 0x58 }, { 0x3019, 0x59 }, { 0x301a, 0x61 }, { 0x307d, 0x00 }, { 0x3087, 0x02 },
        { 0x3082, 0x20 }, { 0x303c, 0x08 }, { 0x303d, 0x18 }, { 0x303e, 0x06 }, { 0x303F, 0x0c }, { 0x3030, 0x62 }, { 0x3031, 0x26 },
        { 0x3032, 0xe6 }, { 0x3033, 0x6e }, { 0x3034, 0xea }, { 0x3035, 0xae }, { 0x3036, 0xa6 }, { 0x3037, 0x6a }, { 0x3015, 0x12 },
        { 0x3014, 0x84 }, { 0x3013, 0xf7 }, { 0x3104, 0x02 }, { 0x3105, 0xfd }, { 0x3106, 0x00 }, { 0x3107, 0xff }, { 0x3308, 0xa5 },
        { 0x3316, 0xff }, { 0x3317, 0x00 }, { 0x3087, 0x02 }, { 0x3082, 0x20 }, { 0x3300, 0x13 }, { 0x3301, 0xd6 }, { 0x3302, 0xef },
        { 0x30b8, 0x20 }, { 0x30b9, 0x17 }, { 0x30ba, 0x04 }, { 0x30bb, 0x08 }, { 0x3100, 0x02 }, { 0x3304, 0xfc }, { 0x3400, 0x00 },
        { 0x3404, 0x00 }, { 0x3020, 0x01 }, { 0x3021, 0x1d }, { 0x3022, 0x00 }, { 0x3023, 0x0a }, { 0x3024, 0x08 }, { 0x3025, 0x18 },
        { 0x3026, 0x06 }, { 0x3027, 0x0c }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x68 }, { 0x3363, 0x08 },
        { 0x3364, 0x04 }, { 0x3403, 0x42 }, { 0x3088, 0x08 }, { 0x3089, 0x00 }, { 0x308a, 0x06 }, { 0x308b, 0x00 }, { 0x3507, 0x06 },
        { 0x350a, 0x4f }, { 0x3600, 0xc4 }, { 0x3011, 0x01 }, /*{ 0xffff, 0xff },*/

};

const Ov3640::SensorReg OV3640_QVGA[] = {
        { 0x3012, 0x80 }, { 0x304d, 0x45 }, { 0x30a7, 0x5e }, { 0x3087, 0x16 }, { 0x309c, 0x1a }, { 0x30a2, 0xe4 }, { 0x30aa, 0x42 },
        { 0x30b0, 0xff }, { 0x30b1, 0xff }, { 0x30b2, 0x10 }, { 0x300e, 0x32 }, { 0x300f, 0x21 }, { 0x3010, 0x20 }, { 0x3011, 0x04 },
        { 0x304c, 0x81 }, { 0x30d7, 0x10 }, { 0x30d9, 0x0d }, { 0x30db, 0x08 }, { 0x3016, 0x82 }, { 0x3018, 0x58 }, { 0x3019, 0x59 },
        { 0x301a, 0x61 }, { 0x307d, 0x00 }, { 0x3087, 0x02 }, { 0x3082, 0x20 }, { 0x3015, 0x12 }, { 0x3014, 0x84 }, { 0x3013, 0xf7 },
        { 0x303c, 0x08 }, { 0x303d, 0x18 }, { 0x303e, 0x06 }, { 0x303f, 0x0c }, { 0x3030, 0x62 }, { 0x3031, 0x26 }, { 0x3032, 0xe6 },
        { 0x3033, 0x6e }, { 0x3034, 0xea }, { 0x3035, 0xae }, { 0x3036, 0xa6 }, { 0x3037, 0x6a }, { 0x3104, 0x02 }, { 0x3105, 0xfd },
        { 0x3106, 0x00 }, { 0x3107, 0xff }, { 0x3300, 0x12 }, { 0x3301, 0xde }, { 0x3302, 0xef }, { 0x3316, 0xff }, { 0x3317, 0x00 },
        { 0x3312, 0x26 }, { 0x3314, 0x42 }, { 0x3313, 0x2b }, { 0x3315, 0x42 }, { 0x3310, 0xd0 }, { 0x3311, 0xbd }, { 0x330c, 0x18 },
        { 0x330d, 0x18 }, { 0x330e, 0x56 }, { 0x330f, 0x5c }, { 0x330b, 0x1c }, { 0x3306, 0x5c }, { 0x3307, 0x11 }, { 0x336a, 0x52 },
        { 0x3370, 0x46 }, { 0x3376, 0x38 }, { 0x3300, 0x13 }, { 0x30b8, 0x20 }, { 0x30b9, 0x17 }, { 0x30ba, 0x4 },  { 0x30bb, 0x8 },
        { 0x3507, 0x06 }, { 0x350a, 0x4f }, { 0x3100, 0x02 }, { 0x3301, 0xde }, { 0x3304, 0x00 }, { 0x3400, 0x01 }, { 0x3404, 0x11 },
        { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0xc },  { 0x3362, 0x12 }, { 0x3363, 0x88 }, { 0x3364, 0xe4 }, { 0x3403, 0x42 },
        { 0x3088, 0x2 },  { 0x3089, 0x80 }, { 0x308a, 0x1 },  { 0x308b, 0xe0 }, { 0x308d, 0x4 },  { 0x3086, 0x3 },  { 0x3086, 0x0 },
        { 0x3011, 0x0 },  { 0x304c, 0x85 }, { 0x3600, 0xd0 }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0xc },  { 0x3362, 0x1 },
        { 0x3363, 0x48 }, { 0x3364, 0xf4 }, { 0x3403, 0x42 }, { 0x3088, 0x1 },  { 0x3089, 0x40 }, { 0x308a, 0x0 },  { 0x308b, 0xf0 },
        { 0x307c, 0x12 }, { 0x3090, 0xc8 }, { 0x3600, 0xc4 }, /*{ 0xffff, 0xff },*/
};

const Ov3640::SensorReg OV3640_176x144_JPEG[] = {
        { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x00 }, { 0x3363, 0xb8 }, { 0x3364, 0x94 },
        { 0x3403, 0x42 }, { 0x3088, 0x00 }, { 0x3089, 0xb0 }, { 0x308a, 0x00 }, { 0x308b, 0x90 }, { 0x304c, 0x84 },

        { 0xffff, 0xff },
};

const Ov3640::SensorReg OV3640_320x240_JPEG[] = {
        { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x01 }, { 0x3363, 0x48 }, { 0x3364, 0xf4 },
        { 0x3403, 0x42 }, { 0x3088, 0x01 }, { 0x3089, 0x40 }, { 0x308a, 0x00 }, { 0x308b, 0xf0 },

        { 0xffff, 0xff },
};

const Ov3640::SensorReg OV3640_352x288_JPEG[] = {
        { 0x3302, 0xef }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x11 }, { 0x3363, 0x68 },
        { 0x3364, 0x24 }, { 0x3403, 0x42 }, { 0x3088, 0x01 }, { 0x3089, 0x60 }, { 0x308a, 0x01 }, { 0x308b, 0x20 },

        { 0xffff, 0xff },
};

const Ov3640::SensorReg OV3640_640x480_JPEG[] = {
        { 0x3302, 0xef }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x12 }, { 0x3363, 0x88 }, { 0x3364, 0xe4 },
        { 0x3403, 0x42 }, { 0x3088, 0x02 }, { 0x3089, 0x80 }, { 0x308a, 0x01 }, { 0x308b, 0xe0 }, { 0x304c, 0x84 },

        { 0xffff, 0xff },
};
const Ov3640::SensorReg OV3640_800x600_JPEG[] = {
        { 0x3302, 0xef }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x23 }, { 0x3363, 0x28 }, { 0x3364, 0x5c },
        { 0x3403, 0x42 }, { 0x3088, 0x03 }, { 0x3089, 0x20 }, { 0x308a, 0x02 }, { 0x308b, 0x58 }, { 0x304c, 0x82 },

        { 0xffff, 0xff },
};

const Ov3640::SensorReg OV3640_1024x768_JPEG[] = {
        { 0x3302, 0xef }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x34 }, { 0x3363, 0x08 }, { 0x3364, 0x06 },
        { 0x3403, 0x42 }, { 0x3088, 0x04 }, { 0x3089, 0x00 }, { 0x308a, 0x03 }, { 0x308b, 0x00 }, { 0x304c, 0x82 },

        { 0xffff, 0xff },
};

const Ov3640::SensorReg OV3640_1600x1200_JPEG[] = {
        { 0x3302, 0xef }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0C }, { 0x3362, 0x46 }, { 0x3363, 0x48 }, { 0x3364, 0xb4 },
        { 0x3403, 0x42 }, { 0x3088, 0x06 }, { 0x3089, 0x40 }, { 0x308a, 0x04 }, { 0x308b, 0xb0 }, { 0x304c, 0x85 },

        { 0xffff, 0xff },

};
const Ov3640::SensorReg OV3640_1280x960_JPEG[] = {
        { 0x3302, 0xef }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x35 }, { 0x3363, 0x08 }, { 0x3364, 0xc4 },
        { 0x3403, 0x42 }, { 0x3088, 0x05 }, { 0x3089, 0x00 }, { 0x308a, 0x03 }, { 0x308b, 0xc0 }, { 0x304c, 0x81 },

        { 0xffff, 0xff },
};

const Ov3640::SensorReg OV3640_2048x1536_JPEG[] = {
        { 0x3302, 0xef }, { 0x335f, 0x68 }, { 0x3360, 0x18 }, { 0x3361, 0x0c }, { 0x3362, 0x68 }, { 0x3363, 0x08 },
        { 0x3364, 0x04 }, { 0x3403, 0x42 }, { 0x3088, 0x08 }, { 0x3089, 0x00 }, { 0x308a, 0x06 }, { 0x308b, 0x00 },

        { 0xffff, 0xff },
};

Ov3640::Ov3640 (SensorResolution resolution)
{
        if (resolution == SensorResolution::QVGA) {
                wrSensorRegs16_8 (OV3640_QVGA, std::size (OV3640_QVGA));
        }
        else if (resolution == SensorResolution::VGA) {
                wrSensorRegs16_8 (OV3640_VGA, std::size (OV3640_VGA));
        }
}

void Ov3640::setJpegSize (JpegResolution size)
{
        switch (size) {
        case OV3640_176x144:
                wrSensorRegs16_8 (OV3640_176x144_JPEG, std::size (OV3640_176x144_JPEG));
                break;
        case OV3640_320x240:
                wrSensorRegs16_8 (OV3640_320x240_JPEG, std::size (OV3640_320x240_JPEG));
                break;
        case OV3640_352x288:
                wrSensorRegs16_8 (OV3640_352x288_JPEG, std::size (OV3640_352x288_JPEG));
                break;
        case OV3640_640x480:
                wrSensorRegs16_8 (OV3640_640x480_JPEG, std::size (OV3640_640x480_JPEG));
                break;
        case OV3640_800x600:
                wrSensorRegs16_8 (OV3640_800x600_JPEG, std::size (OV3640_800x600_JPEG));
                break;
        case OV3640_1024x768:
                wrSensorRegs16_8 (OV3640_1024x768_JPEG, std::size (OV3640_1024x768_JPEG));
                break;
        case OV3640_1280x960:
                wrSensorRegs16_8 (OV3640_1280x960_JPEG, std::size (OV3640_1280x960_JPEG));
                break;
        case OV3640_1600x1200:
                wrSensorRegs16_8 (OV3640_1600x1200_JPEG, std::size (OV3640_1600x1200_JPEG));
                break;
        case OV3640_2048x1536:
                wrSensorRegs16_8 (OV3640_2048x1536_JPEG, std::size (OV3640_2048x1536_JPEG));
                break;
        default:
                wrSensorRegs16_8 (OV3640_320x240_JPEG, std::size (OV3640_320x240_JPEG));
                break;
        }
}

void Ov3640::setLightMode (LightMode lightMode)
{
        switch (lightMode) {

        case Auto:
                wrSensorReg16_8 (0x332b, 0x00); // AWB auto, bit[3]:0,auto
                break;
        case Sunny:
                wrSensorReg16_8 (0x332b, 0x08); // AWB off
                wrSensorReg16_8 (0x33a7, 0x5e);
                wrSensorReg16_8 (0x33a8, 0x40);
                wrSensorReg16_8 (0x33a9, 0x46);
                break;
        case Cloudy:
                wrSensorReg16_8 (0x332b, 0x08);
                wrSensorReg16_8 (0x33a7, 0x68);
                wrSensorReg16_8 (0x33a8, 0x40);
                wrSensorReg16_8 (0x33a9, 0x4e);
                break;
        case Office:
                wrSensorReg16_8 (0x332b, 0x08);
                wrSensorReg16_8 (0x33a7, 0x52);
                wrSensorReg16_8 (0x33a8, 0x40);
                wrSensorReg16_8 (0x33a9, 0x58);
                break;
        case Home:
                wrSensorReg16_8 (0x332b, 0x08);
                wrSensorReg16_8 (0x33a7, 0x44);
                wrSensorReg16_8 (0x33a8, 0x40);
                wrSensorReg16_8 (0x33a9, 0x70);
                break;

        default:
                break;
        }
}

void Ov3640::setColorSaturation (ColorSaturation colorSaturation)
{
        switch (colorSaturation) {
        case Saturation2:
                wrSensorReg16_8 (0x3302, 0xef); // bit[7]:1, enable SDE
                wrSensorReg16_8 (0x3355, 0x02); // enable color saturation
                wrSensorReg16_8 (0x3358, 0x70);
                wrSensorReg16_8 (0x3359, 0x70);
                break;
        case Saturation1:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x02);
                wrSensorReg16_8 (0x3358, 0x50);
                wrSensorReg16_8 (0x3359, 0x50);
                break;
        case Saturation0:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x02);
                wrSensorReg16_8 (0x3358, 0x40);
                wrSensorReg16_8 (0x3359, 0x40);
                break;
        case Saturation_1:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x02);
                wrSensorReg16_8 (0x3358, 0x30);
                wrSensorReg16_8 (0x3359, 0x30);
                break;
        case Saturation_2:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x02);
                wrSensorReg16_8 (0x3358, 0x20);
                wrSensorReg16_8 (0x3359, 0x20);
                break;
        default:
                break;
        }
}

void Ov3640::setBrightness (Brightness brightness)
{
        switch (brightness) {
        case Brightness3:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04); // bit[2] enable
                wrSensorReg16_8 (0x3354, 0x01); // bit[3] sign of brightness
                wrSensorReg16_8 (0x335e, 0x30);
                break;

        case Brightness2:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335e, 0x20);
                break;

        case Brightness1:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335e, 0x10);
                break;

        case Brightness0:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335e, 0x00);
                break;

        case Brightness_1:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x09);
                wrSensorReg16_8 (0x335e, 0x10);
                break;

        case Brightness_2:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x09);
                wrSensorReg16_8 (0x335e, 0x20);
                break;

        case Brightness_3:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x09);
                wrSensorReg16_8 (0x335e, 0x30);
                break;

        default:
                break;
        }
}

void Ov3640::setContrast (Contrast contrast)
{
        switch (contrast) {
        case Contrast3:

                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04); // bit[2] enable contrast/brightness
                wrSensorReg16_8 (0x3354, 0x01); // bit[2] Yoffset sign
                wrSensorReg16_8 (0x335c, 0x2c);
                wrSensorReg16_8 (0x335d, 0x2c);
                break;

        case Contrast2:

                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335c, 0x28);
                wrSensorReg16_8 (0x335d, 0x28);
                break;

        case Contrast1:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335c, 0x24);
                wrSensorReg16_8 (0x335d, 0x24);
                break;

        case Contrast0:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335c, 0x20);
                wrSensorReg16_8 (0x335d, 0x20);
                break;

        case Contrast_1:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335c, 0x1c);
                wrSensorReg16_8 (0x335d, 0x1c);
                break;

        case Contrast_2:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335c, 0x18);
                wrSensorReg16_8 (0x335d, 0x18);
                break;

        case Contrast_3:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x04);
                wrSensorReg16_8 (0x3354, 0x01);
                wrSensorReg16_8 (0x335c, 0x14);
                wrSensorReg16_8 (0x335d, 0x14);
                break;

        default:
                break;
        }
}

void Ov3640::setSpecialEffects (SpecialEffect specialEffect)
{
        switch (specialEffect) {
        case Antique:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x18);
                wrSensorReg16_8 (0x335a, 0x40);
                wrSensorReg16_8 (0x335b, 0xa6);
                break;
        case Bluish:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x18);
                wrSensorReg16_8 (0x335a, 0xa0);
                wrSensorReg16_8 (0x335b, 0x40);
                break;
        case Greenish:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x18);
                wrSensorReg16_8 (0x335a, 0x60);
                wrSensorReg16_8 (0x335b, 0x60);
                break;
        case Reddish:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x18);
                wrSensorReg16_8 (0x335a, 0x80);
                wrSensorReg16_8 (0x335b, 0xc0);
                break;
        case Yellowish:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x18);
                wrSensorReg16_8 (0x335a, 0x30);
                wrSensorReg16_8 (0x335b, 0x90);
                break;
        case BW:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x18); // bit[4]fix u enable, bit[3]fix v enable
                wrSensorReg16_8 (0x335a, 0x80);
                wrSensorReg16_8 (0x335b, 0x80);
                break;
        case Negative:
                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x40); // bit[6] negative
                break;
        case BWnegative:
                // ?
                wrSensorReg16_8 (0x00ff, 0x00);
                wrSensorReg16_8 (0x007c, 0x00);
                wrSensorReg16_8 (0x007d, 0x58);
                wrSensorReg16_8 (0x007c, 0x05);
                wrSensorReg16_8 (0x007d, 0x80);
                wrSensorReg16_8 (0x007d, 0x80);

                break;
        case Normal:

                wrSensorReg16_8 (0x3302, 0xef);
                wrSensorReg16_8 (0x3355, 0x00);
                break;

        default:
                break;
        }
}

void Ov3640::setExposureLevel (ExposureLevel level)
{
        switch (level) {
        case Exposure_17_EV:
                wrSensorReg16_8 (0x3018, 0x10);
                wrSensorReg16_8 (0x3019, 0x08);
                wrSensorReg16_8 (0x301a, 0x21);
                break;
        case Exposure_13_EV:
                wrSensorReg16_8 (0x3018, 0x18);
                wrSensorReg16_8 (0x3019, 0x10);
                wrSensorReg16_8 (0x301a, 0x31);
                break;
        case Exposure_10_EV:
                wrSensorReg16_8 (0x3018, 0x20);
                wrSensorReg16_8 (0x3019, 0x18);
                wrSensorReg16_8 (0x301a, 0x41);
                break;
        case Exposure_07_EV:
                wrSensorReg16_8 (0x3018, 0x28);
                wrSensorReg16_8 (0x3019, 0x20);
                wrSensorReg16_8 (0x301a, 0x51);
                break;
        case Exposure_03_EV:
                wrSensorReg16_8 (0x3018, 0x30);
                wrSensorReg16_8 (0x3019, 0x28);
                wrSensorReg16_8 (0x301a, 0x61);
                break;
        case Exposure_default:
                wrSensorReg16_8 (0x3018, 0x38);
                wrSensorReg16_8 (0x3019, 0x30);
                wrSensorReg16_8 (0x301a, 0x61);
                break;
        case Exposure03_EV:
                wrSensorReg16_8 (0x3018, 0x40);
                wrSensorReg16_8 (0x3019, 0x38);
                wrSensorReg16_8 (0x301a, 0x71);
                break;
        case Exposure07_EV:
                wrSensorReg16_8 (0x3018, 0x48);
                wrSensorReg16_8 (0x3019, 0x40);
                wrSensorReg16_8 (0x301a, 0x81);
                break;
        case Exposure10_EV:
                wrSensorReg16_8 (0x3018, 0x50);
                wrSensorReg16_8 (0x3019, 0x48);
                wrSensorReg16_8 (0x301a, 0x91);
                break;
        case Exposure13_EV:
                wrSensorReg16_8 (0x3018, 0x58);
                wrSensorReg16_8 (0x3019, 0x50);
                wrSensorReg16_8 (0x301a, 0x91);
                break;
        case Exposure17_EV:
                wrSensorReg16_8 (0x3018, 0x60);
                wrSensorReg16_8 (0x3019, 0x58);
                wrSensorReg16_8 (0x301a, 0xa1);
                break;
        default:
                break;
        }
}

void Ov3640::setSharpness (Sharpness sharpness)
{
        switch (sharpness) {
        case Sharpness1:
                wrSensorReg16_8 (0x332d, 0x41);
                break;
        case Sharpness2:
                wrSensorReg16_8 (0x332d, 0x42);
                break;
        case Sharpness3:
                wrSensorReg16_8 (0x332d, 0x43);
                break;
        case Sharpness4:
                wrSensorReg16_8 (0x332d, 0x44);
                break;
        case Sharpness5:
                wrSensorReg16_8 (0x332d, 0x45);
                break;
        case Sharpness6:
                wrSensorReg16_8 (0x332d, 0x46);
                break;
        case Sharpness7:
                wrSensorReg16_8 (0x332d, 0x47);
                break;
        case Sharpness8:
                wrSensorReg16_8 (0x332d, 0x48);
                break;
        case Sharpness_auto:
                wrSensorReg16_8 (0x332d, 0x60);
                wrSensorReg16_8 (0x332f, 0x03);
                break;
        default:
                break;
        }
}

void Ov3640::setMirrorFlip (MirrorFlip mirrorFlip)
{
        switch (mirrorFlip) {
        case MIRROR:
                wrSensorReg16_8 (0x307c, 0x12); // mirror
                wrSensorReg16_8 (0x3090, 0xc8);
                wrSensorReg16_8 (0x3023, 0x0a);

                break;
        case FLIP:
                wrSensorReg16_8 (0x307c, 0x11); // flip
                wrSensorReg16_8 (0x3023, 0x09);
                wrSensorReg16_8 (0x3090, 0xc0);
                break;
        case MIRROR_FLIP:
                wrSensorReg16_8 (0x307c, 0x13); // flip/mirror
                wrSensorReg16_8 (0x3023, 0x09);
                wrSensorReg16_8 (0x3090, 0xc8);
                break;
        case FLIP_NORMAL:
                wrSensorReg16_8 (0x307c, 0x10); // no mirror/flip
                wrSensorReg16_8 (0x3090, 0xc0);
                wrSensorReg16_8 (0x3023, 0x0a);
                break;

        default:
                break;
        }
}

// TODO delete
#include "stm32h7xx_hal.h"
extern I2C_HandleTypeDef hi2c1;
extern "C" void Error_Handler ();

// Write 8 bit values to 16 bit register address
void Ov3640::wrSensorRegs16_8 (SensorReg const reglist[], size_t len)
{
        //        int err = 0;
        //        unsigned int reg_addr;
        //        unsigned char reg_val;
        //        const struct SensorReg *next = reglist;

        //        while ((reg_addr != 0xffff) | (reg_val != 0xff)) {

        //#if defined(RASPBERRY_PI)
        //                reg_addr = next->reg;
        //                reg_val = next->val;
        //#else
        //                reg_addr = pgm_read_word (&next->reg);
        //                reg_val = pgm_read_word (&next->val);
        //#endif
        //                err = wrSensorReg16_8 (reg_addr, reg_val);
        //                // if (!err)
        //                // return err;
        //                next++;

        //        return 1;

        for (size_t i = 0; i < len; ++i) {
                wrSensorReg16_8 (reglist[i].address, reglist[i].value);
        }
}

// Read/write 8 bit value to/from 16 bit register address
void Ov3640::wrSensorReg16_8 (uint16_t regId, uint8_t value)
{
        //#if defined(RASPBERRY_PI)
        //        Ov3640_i2c_word_write (regID, regDat);
        //        Ov3640_delay_ms (1);
        //#else
        //        Wire.beginTransmission (sensor_addr >> 1);
        //        Wire.write (regID >> 8); // sends instruction byte, MSB first
        //        Wire.write (regID & 0x00FF);
        //        Wire.write (regDat & 0x00FF);
        //        if (Wire.endTransmission ()) {
        //                return 0;
        //        }
        //        delay (1);
        //#endif
        //        return 1;

        HAL_StatusTypeDef status = HAL_OK;

        status = HAL_I2C_Mem_Write (&hi2c1, SENSOR_I2C_ADDRESS, regId, I2C_MEMADD_SIZE_16BIT, &value, 1, 1000);

        /* Check the communication status */
        if (status != HAL_OK) {
                /* Execute user timeout callback */
                Error_Handler ();
        }
}

uint8_t Ov3640::rdSensorReg16_8 (uint16_t regId)
{
        //#if defined(RASPBERRY_PI)
        //        Ov3640_i2c_word_read (regID, regDat);
        //#else
        //        Wire.beginTransmission (sensor_addr >> 1);
        //        Wire.write (regID >> 8);
        //        Wire.write (regID & 0x00FF);
        //        Wire.endTransmission ();
        //        Wire.requestFrom ((sensor_addr >> 1), 1);
        //        if (Wire.available ()) {
        //                *regDat = Wire.read ();
        //        }
        //        delay (1);
        //#endif
        //        return 1;

        HAL_StatusTypeDef status = HAL_OK;
        uint8_t value = 0;

        status = HAL_I2C_Mem_Read (&hi2c1, SENSOR_I2C_ADDRESS, regId, I2C_MEMADD_SIZE_16BIT, &value, 1, 1000);

        /* Check the communication status */
        if (status != HAL_OK) {
                /* Execute user timeout callback */
                Error_Handler ();
        }

        return value;
}
