#include "main.h"


HAL_StatusTypeDef hx8882_byte_write(uint8_t dev_addr, uint8_t addr, uint8_t data)
{
  HAL_StatusTypeDef Status = HAL_OK;
  uint8_t buf[2] = {addr, data};
  
  TCA9546APWR_Channel_Sel(eI2C_TCON);
  
  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, buf, 2, 100);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return Status;
}

HAL_StatusTypeDef hx8882_burst_write(uint8_t dev_addr, uint8_t *buf, uint16_t len)
{
  HAL_StatusTypeDef Status = HAL_OK;
  
  TCA9546APWR_Channel_Sel(eI2C_TCON);
  
  Status = HAL_I2C_Master_Transmit(&hi2c2, dev_addr, buf, len, 1000);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return Status;
}

HAL_StatusTypeDef hx8882_byte_read(uint8_t dev_addr, uint8_t addr, uint8_t *buf)
{
  HAL_StatusTypeDef Status = HAL_OK;

  TCA9546APWR_Channel_Sel(eI2C_TCON);
  
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

HAL_StatusTypeDef hx8882_burst_read(uint8_t dev_addr, uint8_t addr, uint8_t *buf, uint8_t len)
{
  HAL_StatusTypeDef Status = HAL_OK;

  TCA9546APWR_Channel_Sel(eI2C_TCON);
    
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
