/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Mt9t112.h"
#include <cstdint>
//#include <etl/array_view.h>
#include <iterator>

// TODO delete - szhould use a driver (I2C class)
#include "Debug.h"
#include "stm32h7xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
extern "C" void Error_Handler ();

#define OV3640_CHIPID_HIGH 0x300a
#define OV3640_CHIPID_LOW 0x300b

const Ov7670::SensorReg OV7670_QVGA[] = {
        { 0x3a, 0x04 }, { 0x40, 0xd0 }, { 0x12, 0x14 }, { 0x32, 0x80 }, { 0x17, 0x16 }, { 0x18, 0x04 }, { 0x19, 0x02 }, { 0x1a, 0x7b },
        { 0x03, 0x06 }, { 0x0c, 0x00 }, { 0x3e, 0x00 }, { 0x70, 0x00 }, { 0x71, 0x00 }, { 0x72, 0x11 }, { 0x73, 0x00 }, { 0xa2, 0x02 },
        { 0x11, 0x81 }, { 0x7a, 0x20 }, { 0x7b, 0x1c }, { 0x7c, 0x28 }, { 0x7d, 0x3c }, { 0x7e, 0x55 }, { 0x7f, 0x68 }, { 0x80, 0x76 },
        { 0x81, 0x80 }, { 0x82, 0x88 }, { 0x83, 0x8f }, { 0x84, 0x96 }, { 0x85, 0xa3 }, { 0x86, 0xaf }, { 0x87, 0xc4 }, { 0x88, 0xd7 },
        { 0x89, 0xe8 }, { 0x13, 0xe0 }, { 0x00, 0x00 }, { 0x10, 0x00 }, { 0x0d, 0x00 }, { 0x14, 0x28 }, { 0xa5, 0x05 }, { 0xab, 0x07 },
        { 0x24, 0x75 }, { 0x25, 0x63 }, { 0x26, 0xA5 }, { 0x9f, 0x78 }, { 0xa0, 0x68 }, { 0xa1, 0x03 }, { 0xa6, 0xdf }, { 0xa7, 0xdf },
        { 0xa8, 0xf0 }, { 0xa9, 0x90 }, { 0xaa, 0x94 }, { 0x13, 0xe5 }, { 0x0e, 0x61 }, { 0x0f, 0x4b }, { 0x16, 0x02 }, { 0x1e, 0x17 },
        { 0x21, 0x02 }, { 0x22, 0x91 }, { 0x29, 0x07 }, { 0x33, 0x0b }, { 0x35, 0x0b }, { 0x37, 0x1d }, { 0x38, 0x71 }, { 0x39, 0x2a },
        { 0x3c, 0x78 }, { 0x4d, 0x40 }, { 0x4e, 0x20 }, { 0x69, 0x00 }, { 0x6b, 0x00 }, { 0x74, 0x19 }, { 0x8d, 0x4f }, { 0x8e, 0x00 },
        { 0x8f, 0x00 }, { 0x90, 0x00 }, { 0x91, 0x00 }, { 0x92, 0x00 }, { 0x96, 0x00 }, { 0x9a, 0x80 }, { 0xb0, 0x84 }, { 0xb1, 0x0c },
        { 0xb2, 0x0e }, { 0xb3, 0x82 }, { 0xb8, 0x0a }, { 0x43, 0x14 }, { 0x44, 0xf0 }, { 0x45, 0x34 }, { 0x46, 0x58 }, { 0x47, 0x28 },
        { 0x48, 0x3a }, { 0x59, 0x88 }, { 0x5a, 0x88 }, { 0x5b, 0x44 }, { 0x5c, 0x67 }, { 0x5d, 0x49 }, { 0x5e, 0x0e }, { 0x64, 0x04 },
        { 0x65, 0x20 }, { 0x66, 0x05 }, { 0x94, 0x04 }, { 0x95, 0x08 }, { 0x6c, 0x0a }, { 0x6d, 0x55 }, { 0x6e, 0x11 }, { 0x6f, 0x9f },
        { 0x6a, 0x40 }, { 0x01, 0x40 }, { 0x02, 0x40 }, { 0x13, 0xe7 }, { 0x15, 0x02 }, { 0x4f, 0x80 }, { 0x50, 0x80 }, { 0x51, 0x00 },
        { 0x52, 0x22 }, { 0x53, 0x5e }, { 0x54, 0x80 }, { 0x58, 0x9e }, { 0x41, 0x08 }, { 0x3f, 0x00 }, { 0x75, 0x05 }, { 0x76, 0xe1 },
        { 0x4c, 0x00 }, { 0x77, 0x01 }, { 0x3d, 0xc2 }, { 0x4b, 0x09 }, { 0xc9, 0x60 }, { 0x41, 0x38 }, { 0x56, 0x40 }, { 0x34, 0x11 },
        { 0x3b, 0x02 }, { 0xa4, 0x89 }, { 0x96, 0x00 }, { 0x97, 0x30 }, { 0x98, 0x20 }, { 0x99, 0x30 }, { 0x9a, 0x84 }, { 0x9b, 0x29 },
        { 0x9c, 0x03 }, { 0x9d, 0x4c }, { 0x9e, 0x3f }, { 0x78, 0x04 }, { 0x79, 0x01 }, { 0xc8, 0xf0 }, { 0x79, 0x0f }, { 0xc8, 0x00 },
        { 0x79, 0x10 }, { 0xc8, 0x7e }, { 0x79, 0x0a }, { 0xc8, 0x80 }, { 0x79, 0x0b }, { 0xc8, 0x01 }, { 0x79, 0x0c }, { 0xc8, 0x0f },
        { 0x79, 0x0d }, { 0xc8, 0x20 }, { 0x79, 0x09 }, { 0xc8, 0x80 }, { 0x79, 0x02 }, { 0xc8, 0xc0 }, { 0x79, 0x03 }, { 0xc8, 0x40 },
        { 0x79, 0x05 }, { 0xc8, 0x30 }, { 0x79, 0x26 }, { 0x09, 0x03 }, { 0x3b, 0x42 },
};

static const struct Ov7670::SensorReg vga_ov7670[] = {
        { REG_HREF, 0xF6 }, // was B6
        { 0x17, 0x13 },     // HSTART
        { 0x18, 0x01 },     // HSTOP
        { 0x19, 0x02 },     // VSTART
        { 0x1a, 0x7a },     // VSTOP
        { REG_VREF, 0x0a }, // VREF
        { 0xff, 0xff },     /* END MARKER */
};
static const struct Ov7670::SensorReg qvga_ov7670[] = {
        { REG_COM14, 0x19 }, { 0x72, 0x11 },       { 0x73, 0xf1 },      { REG_HSTART, 0x16 }, { REG_HSTOP, 0x04 },
        { REG_HREF, 0x24 },  { REG_VSTART, 0x02 }, { REG_VSTOP, 0x7a }, { REG_VREF, 0x0a },   { 0xff, 0xff }, /* END MARKER */
};
static const struct Ov7670::SensorReg qqvga_ov7670[] = {
        { REG_COM14, 0x1a }, // divide by 4
        { 0x72, 0x22 },      // downsample by 4
        { 0x73, 0xf2 },      // divide by 4
        { REG_HSTART, 0x16 }, { REG_HSTOP, 0x04 }, { REG_HREF, 0xa4 }, { REG_VSTART, 0x02 },
        { REG_VSTOP, 0x7a },  { REG_VREF, 0x0a },  { 0xff, 0xff }, /* END MARKER */
};
static const struct Ov7670::SensorReg yuv422_ov7670[] = {
        { REG_COM7, 0x0 }, /* Selects YUV mode */
        { REG_RGB444, 0 }, /* No RGB444 please */
        { REG_COM1, 0 },
        { REG_COM15, COM15_R00FF },
        { REG_COM9, 0x6A }, /* 128x gain ceiling; 0x8 is reserved bit */
        { 0x4f, 0x80 },     /* "matrix coefficient 1" */
        { 0x50, 0x80 },     /* "matrix coefficient 2" */
        { 0x51, 0 },        /* vb */
        { 0x52, 0x22 },     /* "matrix coefficient 4" */
        { 0x53, 0x5e },     /* "matrix coefficient 5" */
        { 0x54, 0x80 },     /* "matrix coefficient 6" */
        { REG_COM13, /*COM13_GAMMA|*/ COM13_UVSAT },
        { 0xff, 0xff }, /* END MARKER */
};
static const struct Ov7670::SensorReg rgb565_ov7670[] = {
        { REG_COM7, COM7_RGB }, /* Selects RGB mode */
        { REG_RGB444, 0 },      /* No RGB444 please */
        { REG_COM1, 0x0 },
        { REG_COM15, COM15_RGB565 | COM15_R00FF },
        { REG_COM9, 0x6A }, /* 128x gain ceiling; 0x8 is reserved bit */
        { 0x4f, 0xb3 },     /* "matrix coefficient 1" */
        { 0x50, 0xb3 },     /* "matrix coefficient 2" */
        { 0x51, 0 },        /* vb */
        { 0x52, 0x3d },     /* "matrix coefficient 4" */
        { 0x53, 0xa7 },     /* "matrix coefficient 5" */
        { 0x54, 0xe4 },     /* "matrix coefficient 6" */
        { REG_COM13, /*COM13_GAMMA|*/ COM13_UVSAT },
        { 0xff, 0xff }, /* END MARKER */
};
static const struct Ov7670::SensorReg bayerRGB_ov7670[] = {
        { REG_COM7, COM7_BAYER }, { REG_COM13, 0x08 }, /* No gamma, magic rsvd bit */
        { REG_COM16, 0x3d },                           /* Edge enhancement, denoise */
        { REG_REG76, 0xe1 },                           /* Pix correction, magic rsvd */
        { 0xff, 0xff },                                /* END MARKER */
};
static const struct Ov7670::SensorReg ov7670_default_regs[] = {
        // from the linux driver
        //        { REG_COM7, COM7_RESET },
        { REG_TSLB, 0x04 }, /* OV */
        { REG_COM7, 0 },    /* VGA */
        /*
         * Set the hardware window.  These values from OV don't entirely
         * make sense - hstop is less than hstart.  But they work...
         */
        { REG_HSTART, 0x13 },
        { REG_HSTOP, 0x01 },
        { REG_HREF, 0xb6 },
        { REG_VSTART, 0x02 },
        { REG_VSTOP, 0x7a },
        { REG_VREF, 0x0a },

        { REG_COM3, 0 },
        { REG_COM14, 0 },
        /* Mystery scaling numbers */
        { 0x70, 0x3a },
        { 0x71, 0x35 },
        { 0x72, 0x11 },
        { 0x73, 0xf0 },
        { 0xa2, /* 0x02 changed to 1*/ 1 },
        { REG_COM10, COM10_VS_NEG },
        /* Gamma curve values */
        { 0x7a, 0x20 },
        { 0x7b, 0x10 },
        { 0x7c, 0x1e },
        { 0x7d, 0x35 },
        { 0x7e, 0x5a },
        { 0x7f, 0x69 },
        { 0x80, 0x76 },
        { 0x81, 0x80 },
        { 0x82, 0x88 },
        { 0x83, 0x8f },
        { 0x84, 0x96 },
        { 0x85, 0xa3 },
        { 0x86, 0xaf },
        { 0x87, 0xc4 },
        { 0x88, 0xd7 },
        { 0x89, 0xe8 },
        /* AGC and AEC parameters.  Note we start by disabling those features,
           then turn them only after tweaking the values. */
        { REG_COM8, COM8_FASTAEC | COM8_AECSTEP },
        { REG_GAIN, 0 },
        { REG_AECH, 0 },
        { REG_COM4, 0x40 }, /* magic reserved bit */
        { REG_COM9, 0x18 }, /* 4x gain + magic rsvd bit */
        { REG_BD50MAX, 0x05 },
        { REG_BD60MAX, 0x07 },
        { REG_AEW, 0x95 },
        { REG_AEB, 0x33 },
        { REG_VPT, 0xe3 },
        { REG_HAECC1, 0x78 },
        { REG_HAECC2, 0x68 },
        { 0xa1, 0x03 }, /* magic */
        { REG_HAECC3, 0xd8 },
        { REG_HAECC4, 0xd8 },
        { REG_HAECC5, 0xf0 },
        { REG_HAECC6, 0x90 },
        { REG_HAECC7, 0x94 },
        { REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_AGC | COM8_AEC },
        { 0x30, 0 },
        { 0x31, 0 }, // disable some delays
        /* Almost all of these are magic "reserved" values.  */
        { REG_COM5, 0x61 },
        { REG_COM6, 0x4b },
        { 0x16, 0x02 },
        { REG_MVFP, 0x07 },
        { 0x21, 0x02 },
        { 0x22, 0x91 },
        { 0x29, 0x07 },
        { 0x33, 0x0b },
        { 0x35, 0x0b },
        { 0x37, 0x1d },
        { 0x38, 0x71 },
        { 0x39, 0x2a },
        { REG_COM12, 0x78 },
        { 0x4d, 0x40 },
        { 0x4e, 0x20 },
        { REG_GFIX, 0 },
        /*{0x6b, 0x4a},*/ { 0x74, 0x10 },
        { 0x8d, 0x4f },
        { 0x8e, 0 },
        { 0x8f, 0 },
        { 0x90, 0 },
        { 0x91, 0 },
        { 0x96, 0 },
        { 0x9a, 0 },
        { 0xb0, 0x84 },
        { 0xb1, 0x0c },
        { 0xb2, 0x0e },
        { 0xb3, 0x82 },
        { 0xb8, 0x0a },

        /* More reserved magic, some of which tweaks white balance */
        { 0x43, 0x0a },
        { 0x44, 0xf0 },
        { 0x45, 0x34 },
        { 0x46, 0x58 },
        { 0x47, 0x28 },
        { 0x48, 0x3a },
        { 0x59, 0x88 },
        { 0x5a, 0x88 },
        { 0x5b, 0x44 },
        { 0x5c, 0x67 },
        { 0x5d, 0x49 },
        { 0x5e, 0x0e },
        { 0x6c, 0x0a },
        { 0x6d, 0x55 },
        { 0x6e, 0x11 },
        { 0x6f, 0x9e }, /* it was 0x9F "9e for advance AWB" */
        { 0x6a, 0x40 },
        { REG_BLUE, 0x40 },
        { REG_RED, 0x60 },
        { REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_AGC | COM8_AEC | COM8_AWB },

        /* Matrix coefficients */
        { 0x4f, 0x80 },
        { 0x50, 0x80 },
        { 0x51, 0 },
        { 0x52, 0x22 },
        { 0x53, 0x5e },
        { 0x54, 0x80 },
        { 0x58, 0x9e },

        { REG_COM16, COM16_AWBGAIN },
        { REG_EDGE, 0 },
        { 0x75, 0x05 },
        { REG_REG76, 0xe1 },
        { 0x4c, 0 },
        { 0x77, 0x01 },
        { REG_COM13, /*0xc3*/ 0x48 },
        { 0x4b, 0x09 },
        { 0xc9, 0x60 }, /*{REG_COM16, 0x38},*/
        { 0x56, 0x40 },

        { 0x34, 0x11 },
        { REG_COM11, COM11_EXP | COM11_HZAUTO },
        { 0xa4, 0x82 /*Was 0x88*/ },
        { 0x96, 0 },
        { 0x97, 0x30 },
        { 0x98, 0x20 },
        { 0x99, 0x30 },
        { 0x9a, 0x84 },
        { 0x9b, 0x29 },
        { 0x9c, 0x03 },
        { 0x9d, 0x4c },
        { 0x9e, 0x3f },
        { 0x78, 0x04 },

        /* Extra-weird stuff.  Some sort of multiplexor register */
        { 0x79, 0x01 },
        { 0xc8, 0xf0 },
        { 0x79, 0x0f },
        { 0xc8, 0x00 },
        { 0x79, 0x10 },
        { 0xc8, 0x7e },
        { 0x79, 0x0a },
        { 0xc8, 0x80 },
        { 0x79, 0x0b },
        { 0xc8, 0x01 },
        { 0x79, 0x0c },
        { 0xc8, 0x0f },
        { 0x79, 0x0d },
        { 0xc8, 0x20 },
        { 0x79, 0x09 },
        { 0xc8, 0x80 },
        { 0x79, 0x02 },
        { 0xc8, 0xc0 },
        { 0x79, 0x03 },
        { 0xc8, 0x40 },
        { 0x79, 0x05 },
        { 0xc8, 0x30 },
        { 0x79, 0x26 },

        { 0xff, 0xff }, /* END MARKER */
};

Ov7670::Ov7670 (SensorResolution resolution)
{
        //        for (int i = 0; i < 256; ++i) {
        //                HAL_StatusTypeDef status = HAL_OK;

        //                uint8_t value = i;
        //                status = HAL_I2C_Mem_Write (&hi2c1, i, i, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000);
        //        }

        HAL_Delay (500);

        uint16_t productId;
        if ((productId = getProductId ()) != 0x7673) {
                debug->println (productId);

                Error_Handler ();
        }

        wrSensorReg8_8 (REG_COM7, COM7_RESET);
        HAL_Delay (100);
        wrSensorRegs8_8 (OV7670_QVGA, std::size (OV7670_QVGA));
#if 0
        // Reset
        wrSensorReg8_8 (REG_COM7, COM7_RESET);
        HAL_Delay (100);
        wrSensorRegs8_8 (ov7670_default_regs, std::size (ov7670_default_regs));

        // case VGA:
        //        wrReg (REG_COM3, 0); // REG_COM3
        //        wrSensorRegs8_8 (vga_ov7670);

        // case QVGA:
        wrSensorReg8_8 (REG_COM3, 4); // REG_COM3 enable scaling
        wrSensorRegs8_8 (qvga_ov7670, std::size (qvga_ov7670));

        // case QQVGA:
        //        wrReg (REG_COM3, 4); // REG_COM3 enable scaling
        //        wrSensorRegs8_8 (qqvga_ov7670);

        /*---------------------------------------------------------------------------*/

        // case YUV422:
        // wrSensorRegs8_8 (yuv422_ov7670, std::size (yuv422_ov7670));

        // case RGB565:
        {
                wrSensorRegs8_8 (rgb565_ov7670, std::size (rgb565_ov7670));

                // according to the Linux kernel driver rgb565 PCLK needs rewriting
                uint8_t temp = rdSensorReg8_8 (0x11);
                HAL_Delay (1);
                wrSensorReg8_8 (0x11, temp);
                wrSensorReg8_8 (0x11, 25);
        }


        // Color pattern
//        wrSensorReg8_8 (0x70, 0b11111111);
//        wrSensorReg8_8 (0x71, 0b01111111);

        // case BAYER_RGB:
        //        wrSensorRegs8_8 (bayerRGB_ov7670, std::size (bayerRGB_ov7670));
//        HAL_Delay (100);
#endif
}

void Ov7670::setJpegSize (JpegResolution size)
{
        //        switch (size) {
        //        case OV3640_176x144:
        //                wrSensorRegs16_8 (OV3640_176x144_JPEG, std::size (OV3640_176x144_JPEG));
        //                break;
        //        case OV3640_320x240:
        //                wrSensorRegs16_8 (OV3640_320x240_JPEG, std::size (OV3640_320x240_JPEG));
        //                break;
        //        case OV3640_352x288:
        //                wrSensorRegs16_8 (OV3640_352x288_JPEG, std::size (OV3640_352x288_JPEG));
        //                break;
        //        case OV3640_640x480:
        //                wrSensorRegs16_8 (OV3640_640x480_JPEG, std::size (OV3640_640x480_JPEG));
        //                break;
        //        case OV3640_800x600:
        //                wrSensorRegs16_8 (OV3640_800x600_JPEG, std::size (OV3640_800x600_JPEG));
        //                break;
        //        case OV3640_1024x768:
        //                wrSensorRegs16_8 (OV3640_1024x768_JPEG, std::size (OV3640_1024x768_JPEG));
        //                break;
        //        case OV3640_1280x960:
        //                wrSensorRegs16_8 (OV3640_1280x960_JPEG, std::size (OV3640_1280x960_JPEG));
        //                break;
        //        case OV3640_1600x1200:
        //                wrSensorRegs16_8 (OV3640_1600x1200_JPEG, std::size (OV3640_1600x1200_JPEG));
        //                break;
        //        case OV3640_2048x1536:
        //                wrSensorRegs16_8 (OV3640_2048x1536_JPEG, std::size (OV3640_2048x1536_JPEG));
        //                break;
        //        default:
        //                wrSensorRegs16_8 (OV3640_320x240_JPEG, std::size (OV3640_320x240_JPEG));
        //                break;
        //        }
}

void Ov7670::setLightMode (LightMode lightMode)
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

void Ov7670::setColorSaturation (ColorSaturation colorSaturation)
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

void Ov7670::setBrightness (Brightness brightness)
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

void Ov7670::setContrast (Contrast contrast)
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

void Ov7670::setSpecialEffects (SpecialEffect specialEffect)
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

void Ov7670::setExposureLevel (ExposureLevel level)
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

void Ov7670::setSharpness (Sharpness sharpness)
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

void Ov7670::setMirrorFlip (MirrorFlip mirrorFlip)
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

// Write 8 bit values to 16 bit register address
void Ov7670::wrSensorRegs16_8 (SensorReg const reglist[], size_t len)
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
void Ov7670::wrSensorReg16_8 (uint16_t regId, uint8_t value)
{
        HAL_StatusTypeDef status = HAL_OK;

        status = HAL_I2C_Mem_Write (&hi2c1, SENSOR_I2C_ADDRESS, regId, I2C_MEMADD_SIZE_16BIT, &value, 1, 1000);

        /* Check the communication status */
        if (status != HAL_OK) {
                /* Execute user timeout callback */
                Error_Handler ();
        }
}

void Ov7670::wrSensorReg8_8 (uint8_t regId, uint8_t value)
{
        HAL_StatusTypeDef status = HAL_OK;

        status = HAL_I2C_Mem_Write (&hi2c1, SENSOR_I2C_ADDRESS, regId, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000);

        /* Check the communication status */
        if (status != HAL_OK) {
                /* Execute user timeout callback */
                Error_Handler ();
        }
}

uint8_t Ov7670::rdSensorReg16_8 (uint16_t regId)
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

        status = HAL_I2C_Master_Transmit (&hi2c1, SENSOR_I2C_ADDRESS, reinterpret_cast<uint8_t *> (&regId), 2, 1000);

        if (status != HAL_OK) {
                Error_Handler ();
        }

        uint8_t value = 0;
        status = HAL_I2C_Master_Receive (&hi2c1, SENSOR_I2C_ADDRESS, &value, 1, 1000);

        if (status != HAL_OK) {
                Error_Handler ();
        }

        return value;

        //        status = HAL_I2C_Mem_Read (&hi2c1, SENSOR_I2C_ADDRESS, regId, I2C_MEMADD_SIZE_16BIT, &value, 1, 1000);

        //        /* Check the communication status */
        //        if (status != HAL_OK) {
        //                /* Execute user timeout callback */
        //                Error_Handler ();
        //        }

        //        return value;
}

uint8_t Ov7670::rdSensorReg8_8 (uint8_t regId)
{
        HAL_StatusTypeDef status = HAL_OK;

        status = HAL_I2C_Master_Transmit (&hi2c1, SENSOR_I2C_ADDRESS, &regId, 1, 1000);

        if (status != HAL_OK) {
                Error_Handler ();
        }

        uint8_t value = 0;
        status = HAL_I2C_Master_Receive (&hi2c1, SENSOR_I2C_ADDRESS, &value, 1, 1000);

        if (status != HAL_OK) {
                Error_Handler ();
        }

        return value;
}

void Ov7670::wrSensorRegs8_8 (SensorReg const reglist[], size_t len)
{
        for (size_t i = 0; i < len; ++i) {
                wrSensorReg8_8 (reglist[i].address, reglist[i].value);
        }
}

uint16_t Ov7670::getProductId () { return uint16_t (rdSensorReg8_8 (0x0a)) << 8 | rdSensorReg8_8 (0x0b); }
