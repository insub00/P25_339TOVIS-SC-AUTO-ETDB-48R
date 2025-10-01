#ifndef __USER_H__
#define __USER_H__


#define TIMER_Min(x)            (x*60*1000)
#define TIMER_Sec(x)            (x*1000)
#define TIMER_mSec(x)		(x)

enum{
  OFF = 0, ON, 
  LOW = 0, HIGH,
  IC_1ST = 0, IC_2ND,
  PMIC_ABD = 0, TOUCH_ABD, TCON_ABD,
  FALSE = 0, TRUE,
};

struct SYS_VER{       
  uint8_t	Year;
  uint8_t	Month;
  uint8_t	Day;
};

 
typedef enum
{
  I2C_RESET = 0u,
  TP_VDD_EN,
  ENVSP,
  ENVSN,
  ENOTP,
  LED1,
  LED2,
  LED3,
  T_RST,
  E_WP,
  TCON_RST,
  PON,
  LCD_RST,
  
} GPO_INFO;
 
typedef enum
{
  LCD_FAIL = 0u,
  TP_FAIL,
  TCON_ASIL,
  TCH_EXT,
  TEST_KEY,
  LED_FAIL,
  VLCD_ON,
  BL_EN,
  SIGNAL_ON,
  LR,
  TCH_INT,
  
} GPI_INFO;

typedef struct {
  GPIO_TypeDef* s0_port;        uint16_t s0_pin;
  GPIO_TypeDef* s1_port;        uint16_t s1_pin;
}Gp2Sel_Def;

typedef struct {
  GPIO_TypeDef* s0_port;        uint16_t s0_pin;
  GPIO_TypeDef* s1_port;        uint16_t s1_pin;
  GPIO_TypeDef* s2_port;        uint16_t s2_pin;
}Gp3Sel_Def;

extern uint8_t gMini_ptg_use;

extern void GPO_WritePin(GPO_INFO name, uint8_t lowhigh);
extern GPIO_PinState GPI_ReadPin(GPI_INFO name);
extern void board_init();
extern void power_seq(uint8_t onoff);
extern void input_func(void);
extern void output_func(void);
extern void ver_revision(struct SYS_VER* Ver);
extern void I2C_mux_ctrl(uint8_t i2c);
extern void GPO_WritePin(GPO_INFO name, uint8_t lowhigh);
extern void led_driver_ctrl(void);
extern void led_driver_tune(uint8_t *pInBuf);
extern void led_fail_detection_check(uint8_t *pInBuf);
extern void tovis_eeprom_packet(uint8_t *pInBuf);
extern void eeprom_save_packet(uint8_t *pInBuf);
extern void eeprom_write_packet(uint8_t *pInBuf);
extern void asil_fb_check(uint8_t *pInBuf);
#endif