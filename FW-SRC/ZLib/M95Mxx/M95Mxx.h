#ifndef	__M95PXX_H__
#define __M95PXX_H__


/* Z-TEC*/
#define M95Pxx_VCOM_ADDR               0x40000
#define M95Pxx_ANALOG_GMA_ADDR         0x41000
#define M95Pxx_DIGITAL_GMA_ADDR        0x42000



#define M95Pxx_WRITE_ENABLE   0x06
#define M95Pxx_WRITE_DISABLE  0x04
#define M95Pxx_READ_STATUS    0x05
#define M95Pxx_WRITE_STATUS   0x01
#define M95Pxx_READ_DATA      0x03
#define M95Pxx_WRITE_DATA     0x02
#define M95Pxx_READ_CONFIG    0x35
#define M95Pxx_WRITE_CONFIG   0x01

// EEPROM의 페이지 크기와 최대 주소를 정의합니다.
#define M95Pxx_PAGE_SIZE      512
#define M95Pxx_MAX_ADDRESS    0xFFFFF // 8 Mbit = 1 Mbyte = 1,048,576 bytes
#define M95Pxx_ID_PAGE_SIZE   512 // 식별 페이지 크기

// 상태 레지스터의 Write In Progress 비트
#define M95Pxx_WIP_BIT        0x01

// 함수 선언
extern void M95Pxx_Init(void);
void M95Pxx_WriteEnable(void);
void M95Pxx_WaitUntilReady(void);
void M95Pxx_WriteData(uint32_t address, uint8_t *data, uint16_t length);
void M95Pxx_ReadData(uint32_t address, uint8_t *data, uint16_t length);
void M95Pxx_ReadConfig(uint8_t *config);
void M95Pxx_WriteConfig(uint8_t config);

extern void internal_eep_all_read(uint16_t cnt);
extern void internal_eeprom_test(void);
void internal_eeprom_tddi_flash_test(void);
#endif