#ifndef	__TCA9546APWE_H__
#define __HX8295_H__

#define TCA9545_DEV_ID  (0x70<<1) //0xE0

enum
{
  eI2C_TOUCH = 0,
  eI2C_TCON,
  eI2C_DISPLAY,
  eI2C_LED,
};

extern void TCA9546APWR_Init();
extern void TCA9546APWR_Channel_Sel(uint8_t ch);

#endif