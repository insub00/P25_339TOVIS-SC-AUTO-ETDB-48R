#ifndef	__93XX76X_H__
#define __93XX76X_H__


#define E93_CS_PORT   GPIOB
#define E93_CS_PIN    GPIO_PIN_12
#define E93_SK_PORT   GPIOB
#define E93_SK_PIN    GPIO_PIN_13
#define E93_DI_PORT   GPIOB  /* MCU -> EEPROM */
#define E93_DI_PIN    GPIO_PIN_14
#define E93_DO_PORT   GPIOB  /* EEPROM -> MCU */
#define E93_DO_PIN    GPIO_PIN_15

// Organization (set according to ORG pin wiring)
// ORG=1 -> 16-bit words (512x16): addr 9 bits, data 16 bits
// ORG=0 -> 8-bit bytes (1024x8): addr 10 bits, data 8 bits
#define E93_ADDR_BITS  9
#define E93_DATA_BITS 16

#define E93_DELAY_LOOP 80   /* 0.5~1us 정도. 보드/최적화에 맞춰 50~200 사이로 조절 */

#define E93_GPIO_WRITE(PORT,PIN,LV) HAL_GPIO_WritePin((PORT),(PIN),(LV)?GPIO_PIN_SET:GPIO_PIN_RESET)
#define E93_GPIO_READ(PORT,PIN)     (HAL_GPIO_ReadPin((PORT),(PIN))==GPIO_PIN_SET)


#define LCD_EEP_16BIT_MAX_SIZE        (303u)
#define LCD_EEP_8BIT_MAX_SIZE        (LCD_EEP_16BIT_MAX_SIZE*2)
#define LCD_EEP_VCOM_H_ADDR               99u
#define LCD_EEP_VCOM_L_ADDR               100u
#define LCD_EEP_CHECKSUM_ADDR             (LCD_EEP_8BIT_MAX_SIZE-1)

extern bool e93_read_seq(uint16_t start_addr, uint8_t *buf, uint32_t count);
extern bool e93_write_seq(uint16_t start_addr, const uint8_t *buf, uint32_t count);

extern void e93_test(void);
extern void e93_eep_read(void);
extern void set_vcom_data_to_eep(uint16_t vcom_data);
extern void get_vcom_data_to_eep(uint16_t *vcom_data);


#endif