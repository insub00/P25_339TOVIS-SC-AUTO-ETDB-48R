#include "main.h"


uint8_t *pTxBuf = &Uart1.TxBuf[ PROTOCOL_DATA ]; //send buffer

Gsp_TypeDef vsync_infor;
Gsp_TypeDef hsync_infor;

uint8_t GetCheckSum(uint8_t *Buf, uint16_t Length)
{
  uint8_t CheckSum = 0;
  
  for (uint16_t Cnt = 1; Cnt < Length - 2; Cnt++) CheckSum += Buf[Cnt];
  
  return CheckSum;
}

void SetCheckSum(uint8_t *Buf, uint16_t Length)
{
  uint8_t CheckSum = 0;
  
  for (uint16_t Cnt = 1; Cnt < Length - 2; Cnt++) CheckSum += Buf[Cnt];
  
  Buf[Length - 2] = CheckSum;
  Buf[Length - 1] = ETX;
}

uint8_t CheckCheckSum(uint8_t *Buf, uint16_t Length)
{
  if (Buf[Length - 2] == GetCheckSum(Buf, Length)) return TRUE;
  return FALSE;	
}

void MakeProtocol(uint8_t MainCmd, uint8_t SubCmd, uint8_t *Buf, uint16_t TxLength)
{ 
  Uart1.TxBuf[0] = STX;
  Uart1.TxBuf[1] = MainCmd;
  Uart1.TxBuf[2] = SubCmd;
  Uart1.TxBuf[3] = TxLength >> 8;
  Uart1.TxBuf[4] = TxLength & 0xFF;
  memcpy(&Uart1.TxBuf[5], Buf, TxLength);
  SetCheckSum(Uart1.TxBuf, 5 + TxLength + 2);
}

void UARTxSendData(uint8_t MainCmd, uint8_t SubCmd, uint8_t *Buf, uint16_t TxLength)
{
  MakeProtocol(MainCmd, SubCmd, Buf, TxLength);
  for(uint16_t Cnt = 0; Cnt < 7 + TxLength; Cnt++) //stx(1) + main_cmd(1) + sub_cmd(1) + length(2) + checksum(1) + etx(1) = 7
  {
    //    UartTxChar(Channel, Buf[Cnt]);
    HAL_UART_Transmit(&huart1, &Uart1.TxBuf[Cnt], 1, 0xFFFF);
  }
}

void GetPacketInfo(uint8_t UartNo, UART_1_TypeDef *Uart)
{
  Uart->MainCmd = Uart->PacketBuf[PROTOCOL_MAIN_CMD];
  Uart->SubCmd = Uart->PacketBuf[PROTOCOL_SUB_CMD];
  Uart->Length = Uart->PacketBuf[PROTOCOL_LENGTH]<<8 | Uart->PacketBuf[PROTOCOL_LENGTH+1];
}

void PtgCommCheck(void)
{
  Ring_TypeDef *pRing = &Ring[ COM_UART_1 ];
  uint16_t PacketLength = 0;
  uint8_t TxData;
  
  if ( RingCheck(pRing) ) // 수신 확인. 
  {
    /* packet data save */
    Uart1.PacketBuf[ Uart1.PacketCnt ] = GetRing(pRing);
    if (++Uart1.PacketCnt >= PACKET_BUFFER_SIZE) Uart1.PacketCnt = 0;

    if (Uart1.PacketBuf[0] != STX) Uart1.PacketCnt = 0; // stx 확인. 
    
    if (Uart1.PacketCnt >= PROTOCOL_DATA) // 수신 data 가 length 이상 인지 확인. 
    {
      PacketLength = Uart1.PacketBuf[PROTOCOL_LENGTH]<<8 | Uart1.PacketBuf[PROTOCOL_LENGTH+1]; // length save
      
      if (Uart1.PacketCnt == (7 + PacketLength)) // header(7)
      {
        if (CheckCheckSum(Uart1.PacketBuf, Uart1.PacketCnt) == TRUE) //checksum ok
        {
          GetPacketInfo(COM_UART_1, &Uart1);
          analyze_packet(&Uart1.PacketBuf[0]);
        }
        else //checksum error
        {
          TxData = GetCheckSum(Uart1.PacketBuf, Uart1.PacketCnt);
          UARTxSendData(0x60, 0x0A, &TxData, 1);
          
//          printf("M[%x], S[%x], Pcnt[%d]\r\n", Uart1.PacketBuf[1], Uart1.PacketBuf[2], Uart1.PacketCnt);
//          for (uint16_t cnt = 0; cnt < Uart1.PacketCnt; cnt++)
//            printf("[%x]", Uart1.PacketBuf[cnt]);
        }
        
        Uart1.PacketCnt = 0; // packet 처리 완료.
      }
    }
  }
}

/* MINI PG 사용시 12V 사용해야 함.  
   파원 시퀀스 동작시 gMini_ptg_use 값 참고해서 동작함. 
*/
void vlcd_switch_ctrl(uint8_t *pInBuf)
{
  uint8_t mini_ptg_on = pInBuf[5];
  
  if (mini_ptg_on) gMini_ptg_use = 1; //MINI PTG
  else gMini_ptg_use = 0; //Existing PTG
  
  printf("gMini_ptg_use = (%d)\r\n", gMini_ptg_use);
  
  uint8_t TxBuf = 1;
  UARTxSendData(pInBuf[MAIN_CMD], pInBuf[SUB_CMD], &TxBuf, 1);
}

void fw_version(uint8_t *pInBuf)
{
  uint8_t TxBuf[4] = {24, 7, 17, 1};
  struct SYS_VER ver;
  
  ver_revision(&ver);
  
  TxBuf[0] = ver.Year;
  TxBuf[1] = ver.Month;
  TxBuf[2] = ver.Day;
  TxBuf[3] = 1;
  UARTxSendData(pInBuf[MAIN_CMD], pInBuf[SUB_CMD], TxBuf, 4);
}








void analyze_packet(uint8_t *buf)
{
  uint8_t TxBuf[100] = {0};
  printf(ANSI_GREEN"MC[0x%02X]SC[0x%02X]\r\n"ANSI_RESET, buf[1], buf[2]);
  
  switch(buf[PROTOCOL_MAIN_CMD])
  {
//====================================================================//    
// MAIN CMD : 0x10
//====================================================================//    
  case M_FUSING_PACKET: // 0x10
    break;
  case M_GSP_PACKET:    // 0x01
//    gspm_packet(buf);
    break;
//====================================================================//    
 
    
//====================================================================//    
// MAIN CMD : 0x02
//====================================================================//   
  case M_FUNC_PACKET:   // 0x02
    switch(buf[PROTOCOL_SUB_CMD])
    {
#if 0      
/* NOVATEK TOUCH */      
    case S_TOUCH_FW_VERSION_PACEKT : 
      MX_I2C2_Init_speed_ctrl(400);
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      Initializtion_For_MP_Test(); 
      touch_infor_check();    
      break;
      
    case S_TOUCH_INSPECTION_START :
      MX_I2C2_Init_speed_ctrl(400);
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      touch_inspection(buf);        
      break;
    
    case S_TOUCH_COORDINATE_START :   
      coordinate_sense_control (1);             
      break;
    case S_TOUCH_COORDINATE_END :      
      coordinate_sense_control(0);             
      break;
      
    case S_TOUCH_RST_CHECK :                 
      touch_rst_check();               
      break;
      
    case S_TOUCH_CHECK_INT :
      check_int_pin();         
      break;
      
    case S_TOUCH_FW_UPDATE :  
      MX_I2C2_Init_speed_ctrl(400);
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      touch_fw_update(buf);
      break;

/* NOVATEK FLASH */  
    case S_NT51927TT_FW_ERASE :     
      MX_I2C2_Init_speed_ctrl(100);       // I2C1 Enable 
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      nt51923tt_gma_erase(buf);
      break;
    case S_NT51927TT_FW_WRITE_START : 
      MX_I2C2_Init_speed_ctrl(100);       // I2C1 Enable 
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      nt51923tt_gma_write_start(buf);
      break;
    case S_NT51927TT_FW_WRITE :     
      MX_I2C2_Init_speed_ctrl(100);       // I2C1 Enable 
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      nt51923tt_gma_write(buf);
      break;
    case S_NT51927TT_FW_WRITE_END :   
      MX_I2C2_Init_speed_ctrl(100);       // I2C1 Enable 
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      nt51923tt_gma_write_end(buf);
      break;
    case S_NT51927TT_FW_READ :
      MX_I2C2_Init_speed_ctrl(100);       // I2C1 Enable 
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      nt51923tt_gma_read(buf);
      break;
    case S_NT51927TT_FW_READ_END :
      MX_I2C2_Init_speed_ctrl(100);       // I2C1 Enable 
      TCA9546APWR_Channel_Sel(eI2C_TOUCH);
      nt51923tt_gma_read_end(buf);
      break;      
    

/* LED DRIVER CONTROL */
    case S_LED_DRAVIER_CTRL_PACKET :	  
      led_driver_tune(buf); //MT25 packet인 0xA5에서 0xA6으로 변경	
      break;


      
/* ASIL FB */       
    case S_ASIL_FB_PIN_CHECK :                 
      asil_fb_check(buf);
      break;
      
/* LED FAIL DETECT */      
    case S_LED_DRIVER_FAIL_DETECT_PACKET:
      led_fail_detection_check(buf);
      break;
   

    
    
      
      
//    case S_GAMMA_PACKET :  
//      gamma_tune(buf);
//      break;

//      
//    case S_VCOM_EN_OFF_PACKET :
//      pvcom_enable(buf, DISABLE);
//      break;  
//    case S_VCOM_EN_ON_PACKET : 
//      pvcom_enable(buf, ENABLE);
//      break;

 
//    case S_EEPROM_TO_FLASH_WRITE_PACKET	:
//      nt_eeprom_to_flash_write(buf, 1);
//      break;
//      
//    case S_EDID_PACKET :
//      edid_packet(buf);
//      break;
//      




  

/* VLCD switch control */      
    case S_VLCD_CTRL_PACKET :
      vlcd_switch_ctrl(buf);
      break;
#endif
      
      
/* MICRO CHIP TOUCH */      
    case S_TOUCH_FW_VERSION_PACEKT : 
      touch_ver_check_packet(buf);   
      break;
      
    case S_TOUCH_INSPECTION_START :
      touch_inspection_packet(buf);        
      break;
    
    case S_TOUCH_COORDINATE_START :              
      break;
    case S_TOUCH_COORDINATE_END :               
      break;
      
    case S_TOUCH_RST_CHECK :                              
      break;
      
    case S_TOUCH_CHECK_INT :        
      break;
      
    case S_TOUCH_FW_UPDATE :  
      break;

      
/* DDI */      
    case S_DDI_WRITE_PACKET :   /* 0xCA */
      ddi_init_code_write_packet(buf);
      break;      
      
/* VCOM */    
    case S_VCOM_PACKET:         /* 0x09 */
      //VCOM WRITE & READ 
      pvcom_tune(buf);         
      break;   
      
/* LCM EEPROM (PGM TEST창 오른쪽 검사박스로도 동작함"48R R/W")*/      
    case S_TOVIS_EEPROM_PACKET: /* 0xF3 */
      tovis_eeprom_packet(buf);		
      break;
//    case S_EEPROM_SAVE_PACKET : /* 0xF0 */
//      eeprom_save_packet(buf);    
//      break;      
      
      
/* WING EEPROM */      
    case S_EEPROM_WRITE_PACKET :        
      eeprom_write_packet(buf);
      break;   
      

/* HIMAX FLASH */      
//himax touch 설정으로 packet time out 방지용으로 적용.      
    case S_DRIVER_PASSWORD_DATA: /* 0xE2 */
      TxBuf[0] = buf[PROTOCOL_DATA];
      TxBuf[1] = buf[PROTOCOL_DATA+1];
      TxBuf[2] = buf[PROTOCOL_DATA+2];
      UARTxSendData(buf[PROTOCOL_MAIN_CMD], buf[PROTOCOL_SUB_CMD], TxBuf, 3);
      break;
    case S_READ_FLASH_DATA:     /* 0xE3 */
      TxBuf[0] = 1;
      UARTxSendData(buf[PROTOCOL_MAIN_CMD], buf[PROTOCOL_SUB_CMD], TxBuf, 1);
      break;

      
/* WING VER */       
    case S_WING_INFO:           /* 0xFF */
      fw_version(buf);       
      break; 
    }
    break;
  }
}














