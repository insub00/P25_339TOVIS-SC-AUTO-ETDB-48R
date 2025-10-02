#ifndef	__HX8295_H__
#define __HX8295_H__


#define HX8295_DEV_ID   (0x52<<1) //0xA4[8bit]
#define HX8295_SID      0x00


extern HAL_StatusTypeDef hx8295_byte_write(uint8_t dev_addr, uint8_t addr, uint8_t data);
extern HAL_StatusTypeDef hx8295_burst_write(uint8_t dev_addr, uint8_t *buf, uint16_t len);
extern HAL_StatusTypeDef hx8295_byte_read(uint8_t dev_addr, uint8_t addr, uint8_t *buf);
extern HAL_StatusTypeDef hx8295_burst_read(uint8_t dev_addr, uint8_t addr, uint8_t *buf, uint8_t len);

extern void hx8295_eep_ctrl_by_host(void);
extern void hx8295_eep_ctrl_by_ddi(void);
extern void hx8295_eep_ctrl_by_host_default_value_read_test(void);
extern void hx8295_power_set_test(void);
extern uint8_t hx8295_eep_fail_flag_check(void);
extern void hx8295_ATREN_ctrl(uint8_t onoff);
extern void hx8295_test_code(void);
extern void set_vcom_data(uint16_t vcom_data);
extern uint16_t get_vcom_data(void);

#endif