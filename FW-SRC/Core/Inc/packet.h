#ifndef __PACKET_H__
#define __PACKET_H__



#define	STX                     0x02
#define	ETX                     0x03
#define	MAIN_CMD                0x01
#define	SUB_CMD                 0x02

#define WING_MAJOR_VER  0
#define WING_MINOR_VER  1

typedef enum {
    PROTOCOL_STX = 0
  , PROTOCOL_MAIN_CMD                   //1
  , PROTOCOL_SUB_CMD                    //2
  , PROTOCOL_LENGTH                     //3
  , PROTOCOL_DATA = PROTOCOL_LENGTH + 2 //5
  , PROTOCOL_CHKSUM                     //6
  , PROTOCOL_ETX                        //7
  , PROTOCOL_NUM                        //8
  , PROTOCOL_COMPLET                    //9
}eProtocol;

// MAIN COMMAND
#define	M_FUSING_PACKET                         0x10
#define M_GSP_PACKET                            0x01
#define	M_FUNC_PACKET		                0x02

// MAIN - M_FUNC_PACKET :
#define S_TOUCH_FW_VERSION_PACEKT               0xD0
#define S_TOUCH_PANNEL_INFOR                    0x02    // 
#define S_TOUCH_INSPECTION_START                0x03    // touch item inspection
#define S_TOUCH_COORDINATE_START                0x04    // drawing start
#define S_TOUCH_COORDINATE_END                  0x05    // drawing end
#define S_TOUCH_PROJECT_INFOR                   0x06    // project id
#define S_TOUCH_RST_CHECK                       0x07    // Check Reset pin
#define S_TOUCH_CHECK_INT                       0x08    // Check INT pin
#define	S_VCOM_PACKET		                0x09
#define S_WING_FW_VERSION_CHECK                 0x84
#define	S_EEPROM_WRITE_PACKET                   0x0A
#define	S_GAMMA_PACKET		                0xE0
#define S_DRIVER_PASSWORD_DATA                  0xE2
#define S_READ_FLASH_DATA                       0xE3
#define S_TOUCH_FW_UPDATE                       0xDA
#define S_GMA_FLASH_WRITE                       0xDB
#define	S_ASIL_FB_PIN_CHECK	                0x90
#define	S_VCOM_EN_OFF_PACKET	                0x91
#define	S_VCOM_EN_ON_PACKET	                0x92
#define	S_ASIL_FB_TEST_PACKET	                0x93
#define	S_ASIL_FB_REPEAT_PACKET	                0x95
#define	S_TCON_ABD_REAL_TIME_TEST_START_PACKET	0x96
#define	S_TCON_ABD_REAL_TIME_TEST_READ_PACKET	0x97
#define	S_LED_DRAVIER_CTRL_PACKET               0xA6	// Led driver tuen packet
#define S_EDID_PACKET                           0xB1
#define S_NT51927TT_FW_ERASE                    0xBA
#define S_NT51927TT_FW_WRITE_START              0xBB
#define S_NT51927TT_FW_WRITE                    0xBC
#define S_NT51927TT_FW_WRITE_END                0xBD
#define S_NT51927TT_FW_READ                     0xBE   
#define S_NT51927TT_FW_READ_END                 0xBF
#define S_FLASH_ERASE_PACKET                    0xC0
#define S_FLASH_WRITE_PACKET                    0xC1
#define S_FLASH_END_PACKET                      0xC2
#define S_FLASH_READ_PACKET                     0xC3
#define S_FLASH_SAVE_DATA_PACKET                0xC4
#define S_EEPROM_TO_FLASH_WRITE_PACKET          0xC5
#define S_EEPROM_SAVE_PACKET                    0xF0
#define S_TOVIS_EEPROM_PACKET	                0xF3
#define S_LED_DRIVER_FAIL_DETECT_PACKET		0xF5
#define S_VLCD_CTRL_PACKET	                0xFA
#define S_WING_INFO                             0xFF
#define S_REG_READ                              0xC5
// OTP ฐüทร
#define S_OTP_WRITE_PACKET                      0xC6
#define S_OTP_READ_PACKET                       0xC7
#define S_OTP_COUNT_CHECK_PACKET                0xC8

#define S_DDI_WRITE_PACKET                      0xCA




typedef	struct
{
  uint8_t start_flag;
//  uint8_t edge_count;
  uint16_t edge_count;
  uint16_t hertz;
  uint8_t result;
} Gsp_TypeDef;


extern uint8_t *pTxBuf;


extern void UARTxSendData(uint8_t MainCmd, uint8_t SubCmd, uint8_t *Buf, uint16_t TxLength);
extern void PtgCommCheck(void);
extern void analyze_packet(uint8_t *buf);

#endif