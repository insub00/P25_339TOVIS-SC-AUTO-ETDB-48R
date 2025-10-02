#include "main.h"


void pvcom_tune(uint8_t *pInBuf)
{
  uint8_t TxData[2] = {0};
  uint8_t rwf = pInBuf[5]; 
  uint16_t setVcom = ((uint16_t)pInBuf[6]<<8 | pInBuf[7]) & 0x1FF;
  uint16_t getVcom = 0;
  
  printf("%s : rwf = %02X\r\n", __func__, rwf);
  
  if (rwf & 0x80)               /* ºñÈÖ¹ß ¿µ¿ª */
  {
    if ((rwf & 0x01) == 0)      /* WRITE */
    {
//      set_vcom_data_to_eep(setVcom);
//      get_vcom_data_to_eep(&getVcom);
//      TxData[0] = setVcom >> 8;
//      TxData[1] = setVcom;
    }
    else                        /* READ */
    {
      get_vcom_data_to_eep(&getVcom);
      TxData[0] = getVcom >> 8;
      TxData[1] = getVcom;
    }
  }
  else                          /* ÈÖ¹ß ¿µ¿ª */ 
  {
    hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x15); //select Page15h
    hx8295_byte_write(HX8295_DEV_ID, 0x02, 0x66); //disable ATREN
    
    if ((rwf & 0x01) == 0)      /* WRITE */
    {    
      set_vcom_data(setVcom);
    }
    getVcom = get_vcom_data();
    TxData[0] = getVcom >> 8;
    TxData[1] = getVcom;
  }
  
  UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], TxData, 2);
}