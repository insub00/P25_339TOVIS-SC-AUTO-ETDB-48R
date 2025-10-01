#ifndef	__HX8882_H__
#define __HX8882_H__


#define HX8882_DEV_ID   (0x52<<1) //0xA4[8bit]


extern HAL_StatusTypeDef hx8882_byte_write(uint8_t dev_addr, uint8_t addr, uint8_t data);
extern HAL_StatusTypeDef hx8882_burst_write(uint8_t dev_addr, uint8_t *buf, uint16_t len);
extern HAL_StatusTypeDef hx8882_byte_read(uint8_t dev_addr, uint8_t addr, uint8_t *buf);
extern HAL_StatusTypeDef hx8882_burst_read(uint8_t dev_addr, uint8_t addr, uint8_t *buf, uint8_t len);



#endif