#include "main.h"




HAL_StatusTypeDef hx8295_byte_write(uint8_t dev_addr, uint8_t addr, uint8_t data)
{
  HAL_StatusTypeDef Status = HAL_OK;
  uint8_t buf[2] = {addr, data};
  
  TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
  
  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, buf, 2, 100);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return Status;
}

HAL_StatusTypeDef hx8295_burst_write(uint8_t dev_addr, uint8_t *buf, uint16_t len)
{
  HAL_StatusTypeDef Status = HAL_OK;
  
  TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
  
  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, buf, len, 1000);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return Status;
}

HAL_StatusTypeDef hx8295_byte_read(uint8_t dev_addr, uint8_t addr, uint8_t *buf)
{
  HAL_StatusTypeDef Status = HAL_OK;

  TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
  
  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, &addr, 1, 100);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);
    return Status;
  }
  
  Status = HAL_I2C_Master_Receive(&hi2c2, dev_addr | 0x01, buf, 1, 100);
  if(Status != HAL_OK)
  {
    printf("%s i2c readd Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return Status;
}

HAL_StatusTypeDef hx8295_burst_read(uint8_t dev_addr, uint8_t addr, uint8_t *buf, uint8_t len)
{
  HAL_StatusTypeDef Status = HAL_OK;

  TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
    
  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, &addr, 1, 100);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);  
    return Status;
  }
  
  Status = HAL_I2C_Master_Receive(&hi2c2, dev_addr | 0x01, buf, len, 1000);
  if(Status != HAL_OK)
  {
    printf("%s i2c read Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return Status;
}

void hx8295_eep_ctrl_by_host(void)
{
  //RESETB =H
  //20MS
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x15); //select Page15h
  hx8295_byte_write(HX8295_DEV_ID, 0x02, 0x66); //disble ATREN //pass-word 
  hx8295_byte_write(HX8295_DEV_ID, 0x0C, 0x50); //switch EEPROM controled by Host
}

void hx8295_eep_ctrl_by_ddi(void)
{
  //RESETB =H
  //20MS
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x15); //select Page15h
  hx8295_byte_write(HX8295_DEV_ID, 0x02, 0x99); //disble ATREN //pass-word 
  hx8295_byte_write(HX8295_DEV_ID, 0x0C, 0x40); //switch EEPROM controled by Host
}

/* hx8295_eep_ctrl_by_host 내용은 받았으나, off를 못받아서 디폴트 상태 확인해서 ddi control 할수있게 할려고 read함 */
void hx8295_eep_ctrl_by_host_default_value_read_test(void)
{
  //RESETB =H
  //20MS
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x15); //select Page15h
//  hx8295_byte_write(HX8295_DEV_ID, 0x02, 0x66); //disble ATREN //pass-word 
//  hx8295_byte_write(HX8295_DEV_ID, 0x0C, 0x50); //switch EEPROM controled by Host
  
  uint8_t read_datas[5] = {0};
  hx8295_byte_read(HX8295_DEV_ID, 0x02, &read_datas[0]);
  hx8295_byte_read(HX8295_DEV_ID, 0x0C, &read_datas[1]);
  
  printf("reg 0x02 read = [%02X]\r\n", read_datas[0]);
  printf("reg 0x0C read = [%02X]\r\n", read_datas[1]);
}

void hx8295_power_set_test(void)
{
  //RESETB =H
  //20MS

  //select Page03h
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x03);
  hx8295_byte_write(HX8295_DEV_ID, 0x04, 0x10); 
  
  uint8_t read_datas[5] = {0};
  hx8295_byte_read(HX8295_DEV_ID, 0x04, &read_datas[0]);
}

uint8_t hx8295_eep_fail_flag_check(void)
{
  uint8_t fail_flag = 0;
  uint8_t ret = 0; //ok
  //RESETB =H
  //20MS

  //select Page15h
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x15);
  //read eeprom fail flag
  hx8295_byte_read(HX8295_DEV_ID, 0x18, &fail_flag);
  ret = (fail_flag >> 5) & 0x01;
  printf("reg 0x18 = 0x%02X, eep fail flag(0:ng, 1:ok) = %d\r\n", fail_flag, ret);
  return ret;
}

void hx8295_ATREN_ctrl(uint8_t onoff)
{
  uint8_t data = 0;
  if (onoff) data = 0x05;
  else data = 0x0B;

  //RESETB =H
  //20MS
 
  //select Page15h
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x15);
  hx8295_byte_write(HX8295_DEV_ID, 0x02, 0x66); //pass-word
  hx8295_byte_write(HX8295_DEV_ID, 0x0B, data);
  
  uint8_t read_datas[5] = {0};
  hx8295_byte_read(HX8295_DEV_ID, 0x00, &read_datas[0]);
  hx8295_byte_read(HX8295_DEV_ID, 0x0B, &read_datas[1]);

}

void hx8295_test_code(void)
{
  //RESETB =H
  //20MS
  
  //select Page15h
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x00);
  hx8295_byte_write(HX8295_DEV_ID, 0x01, 0x10);

  uint8_t read_datas[5] = {0};
  hx8295_byte_read(HX8295_DEV_ID, 0x01, &read_datas[0]);
}

void set_vcom_data(uint16_t vcom_data)
{
  //Power ON
  //RESETB =H
  //20MS
  //STBYB = H
  //64MS
  
  printf("VCOM Tuning data = 0x%04X\r\n", vcom_data);
  
  //select Page03h
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x03);
  //Adjust VCOM
  hx8295_byte_write(HX8295_DEV_ID, 0x1A, (vcom_data>>8)&0xFF); //MSB
  hx8295_byte_write(HX8295_DEV_ID, 0x1B, vcom_data&0xFF); //LSB
}
  
uint16_t get_vcom_data(void)
{
  uint8_t rdBuff[2] = {0};
  uint16_t vcom_data = 0;

  //select Page03h
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x03);
  //read VCOM
  hx8295_byte_read(HX8295_DEV_ID, 0x1A, &rdBuff[0]);
  hx8295_byte_read(HX8295_DEV_ID, 0x1B, &rdBuff[1]);
  
  vcom_data = (rdBuff[0]<<8) | rdBuff[1];
  
  printf("VCOM Read data = 0x%04X\r\n", vcom_data);
  return vcom_data;
}





void ddi_init_code_write_packet(uint8_t *pInBuff)
{
  uint16_t w_len = (uint16_t)(pInBuff[5] << 8 | pInBuff[6]);
  
  printf("%s : w_len = %d \r\n", __func__, w_len);
  
  //1. eeprom load off
  hx8295_byte_write(HX8295_DEV_ID, 0x00, 0x15); //select Page15h
  hx8295_byte_write(HX8295_DEV_ID, 0x02, 0x66); 
  
  HAL_Delay(100);
  
  
  //2. initial code write
  printf("ddi init code write : \r\n");
  for (int i=0; i<w_len; i+=2)
  {
    printf("I2CW %02X %02X \r\n", pInBuff[7+i], pInBuff[7+1+i]);
    hx8295_byte_write(HX8295_DEV_ID, pInBuff[7+i], pInBuff[7+1+i]); 
  }
  
  uint8_t TxBuf = 0;
  UARTxSendData(pInBuff[PROTOCOL_MAIN_CMD], pInBuff[PROTOCOL_SUB_CMD], &TxBuf, 1);
}



    










