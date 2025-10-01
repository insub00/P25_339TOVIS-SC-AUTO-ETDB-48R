#include "main.h"

bool SPI_Write(uint8_t *data, uint32_t len)
{
  HAL_StatusTypeDef status;
  // -------------------------------------------------------------------------
  // An SPI transfer consists of two steps
  // 1. Wait until SPTEF=1 and then write data to SPIx->D (even if there is
  //    nothing to write, do a dummy write to generate the SCK)
  // 2. Wait until SPRF=1 and then read data from SPIx->D (even if there is
  //    nothing to read, do a dummy read so data will not get lost)

  status = HAL_SPI_Transmit(&hspi1, data, len, 500);
  if (status != HAL_OK)
  {
    printf("spi1 Transmit err (%X)\r\n", status);
  }
  
  return true;
}

bool SPI_Read(uint8_t *data, uint32_t len)
{
  HAL_StatusTypeDef status;
  // -------------------------------------------------------------------------
  // An SPI transfer consists of two steps
  // 1. Wait until SPTEF=1 and then write data to SPIx->D (even if there is
  //    nothing to write, do a dummy write to generate the SCK)
  // 2. Wait until SPRF=1 and then read data from SPIx->D (even if there is
  //    nothing to read, do a dummy read so data will not get lost)

  status = HAL_SPI_Receive(&hspi1, data, len, 500);
  if (status != HAL_OK)
  {
    printf("spi1 Receive err (%X)\r\n", status);
  }
  
  return true;
}

void M95Pxx_WP_pin(uint8_t LowHigh)
{
  HAL_GPIO_WritePin(M95M04_WP_GPIO_Port, M95M04_WP_Pin, (GPIO_PinState)LowHigh);
}

void M95Pxx_HOLD_pin(uint8_t LowHigh)
{
  HAL_GPIO_WritePin(M95M04_HOLD_GPIO_Port, M95M04_HOLD_Pin, (GPIO_PinState)LowHigh);
}

void M95Pxx_Init(void)
{
  M95Pxx_WP_pin(HIGH);
  M95Pxx_HOLD_pin(HIGH);
  spi1_cs(HIGH);
}


// 쓰기 허용 명령
void M95Pxx_WriteEnable(void) 
{
  uint8_t cmd = M95Pxx_WRITE_ENABLE;
  
  spi1_cs(LOW);
  HAL_Delay(10);
  
  SPI_Write(&cmd, 1);
  
  HAL_Delay(10);
  spi1_cs(HIGH);
}

// EEPROM이 준비될 때까지 대기
void M95Pxx_WaitUntilReady(void) 
{
  uint8_t cmd = M95Pxx_READ_STATUS;
  uint8_t status = 0;
  
  spi1_cs(LOW);
  HAL_Delay(10);
  
  do {
    SPI_Write(&cmd, 1); // 상태 레지스터 읽기 명령 전송
    SPI_Read(&status, 1); // 상태 레지스터 값 읽기
  } while (status & M95Pxx_WIP_BIT); // WIP 비트가 클리어될 때까지 대기
  
  HAL_Delay(10);
  spi1_cs(HIGH);
}

// 데이터 쓰기 함수
void M95Pxx_WriteData(uint32_t address, uint8_t *data, uint16_t length) 
{
  if (address > M95Pxx_MAX_ADDRESS || (address + length - 1) > M95Pxx_MAX_ADDRESS) {
    return; // 주소 범위를 벗어나는 경우
  }
  
  while (length > 0) {
    M95Pxx_WriteEnable();
    
    uint16_t page_offset = address % M95Pxx_PAGE_SIZE;
    uint16_t bytes_to_write = M95Pxx_PAGE_SIZE - page_offset;
    if (bytes_to_write > length) {
      bytes_to_write = length;
    }
#if(DEBUG == USE)    
    printf("%s : addr = 0x%05X, len  = 0x%02X\r\n", __func__, address, length);
#endif    
    uint8_t cmd[4];
    cmd[0] = M95Pxx_WRITE_DATA;
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;
    
    spi1_cs(LOW);
    HAL_Delay(10);
    
    SPI_Write(cmd, 4); // 명령과 주소 전송
    SPI_Write(data, bytes_to_write); // 데이터 전송
    
    HAL_Delay(10);
    spi1_cs(HIGH);
    
    M95Pxx_WaitUntilReady();
    
    address += bytes_to_write;
    data += bytes_to_write;
    length -= bytes_to_write;
  }
}

// 데이터 읽기 함수
void M95Pxx_ReadData(uint32_t address, uint8_t *data, uint16_t length) 
{
  if (address > M95Pxx_MAX_ADDRESS || (address + length - 1) > M95Pxx_MAX_ADDRESS) {
    return; // 주소 범위를 벗어나는 경우
  }
//  printf("%s : address =  0x%06X, length = %d\r\n", __func__, address, length);
  
  uint8_t cmd[4];
  cmd[0] = M95Pxx_READ_DATA;
  cmd[1] = (address >> 16) & 0xFF;
  cmd[2] = (address >> 8) & 0xFF;
  cmd[3] = address & 0xFF;
  
  spi1_cs(LOW);
  HAL_Delay(10);
  
  SPI_Write(cmd, 4); // 명령과 주소 전송
  SPI_Read(data, length); // 데이터 읽기
  
  HAL_Delay(10);
  spi1_cs(HIGH);
}

// 구성 레지스터 읽기 함수
void M95Pxx_ReadConfig(uint8_t *config) 
{
  uint8_t cmd = M95Pxx_READ_CONFIG;
  
  spi1_cs(LOW);
  HAL_Delay(10);
  
  SPI_Write(&cmd, 1); // 구성 레지스터 읽기 명령 전송
  SPI_Read(config, 1); // 구성 레지스터 값 읽기
  
  HAL_Delay(10);
  spi1_cs(HIGH);
}

// 구성 레지스터 쓰기 함수
void M95Pxx_WriteConfig(uint8_t config) 
{
  M95Pxx_WriteEnable();
  
  uint8_t cmd[2];
  cmd[0] = M95Pxx_WRITE_CONFIG;
  cmd[1] = config;
  
  spi1_cs(LOW);
  HAL_Delay(10);
  
  SPI_Write(cmd, 2); // 구성 레지스터 쓰기 명령과 데이터 전송
  
  HAL_Delay(10);
  spi1_cs(HIGH);
  
  M95Pxx_WaitUntilReady();
}

void internal_eep_all_read(uint16_t cnt)
{
  uint32_t start_addr = 0x00000;
  uint32_t end_addr = 0x10000;
  uint32_t curr_addr = 0x00000;
  
  start_addr = 0x00000 + (cnt * 0x10000);
  end_addr = start_addr + 0x10000;
    
  printf("read addr : 0x%05X ~ 0x%05X\r\n", start_addr, end_addr); 
  
  for(curr_addr = start_addr; curr_addr < end_addr; curr_addr+=256)
  {
    M95Pxx_ReadData(curr_addr, &gRdBuf[0], 256);
    for (uint16_t i = 0; i < 256; i++)
    {
      printf("%02X ", gRdBuf[i]);
    }
  }
  
  printf("\r\nM95Pxx Read end\r\n");
}

/*
* \param[in] block		--> Block 0 = 0x0 	--> Block 15 = 0xF
* \param[in] sector	        --> Sector 0 = 0x0	--> Sector 15 = 0xF
* \param[in] page		--> Page 0 = 0x0 	--> Page 7 = E00	
* \param[in] data		--> data of max 512 bytes
* \param[in] len		--> data length. 
*/
void internal_eeprom_test(void) // 검증 완료
{
  uint8_t block = 0;
  uint8_t sector = 0;
  uint8_t page = 0;
//  uint8_t rbuf[512] = {0};
  uint32_t i,j;
  
//  uint8_t wrBuff[512] = {0};
//  uint8_t rdBuff[512] = {0};
  
  memset(gWrBuf, 0, 512);
  M95Pxx_WriteData(0x00, gWrBuf, 512); 
  
  printf("M95Pxx Read :\r\n");
  M95Pxx_ReadData(0, gRdBuf, 512);
  for (j=0; j<32; j++)
  {
    for (i=0; i<16; i++) printf("%02X ", gRdBuf[j*16+i]);
    printf("\r\n");
  }
  
  
  
#if 1
//  M95Pxx_Init();
  
  uint8_t minor = 0xFF;
  for (i=0; i<0x100; i++) gWrBuf[i] = i;
  for (i=0x100; i<0x200; i++) gWrBuf[i] = minor--;
  M95Pxx_WriteData(0, gWrBuf, 512);
  
  HAL_Delay(200);

  printf("M95Pxx Read :\r\n");
  M95Pxx_ReadData(0, gRdBuf, 512);
  for (j=0; j<32; j++)
  {
    for (i=0; i<16; i++) printf("%02X ", gRdBuf[j*16+i]);
    printf("\r\n");
  }
     
  
  for (i=0; i<0x0F; i++)
  {
    if(gWrBuf[i] != gRdBuf[i])
    {
//      gTestFalgOk = 0;
      
      printf("gWrBuf[%02X] != gRdBuf[%02X] \r\n", gWrBuf[i], gRdBuf[i]);
    }
  }
  uint32_t err = 0;
  for (i=0; i<512; i++)
  {
    if (gWrBuf[i] != gRdBuf[i]) err++;
  }
  if(err) g_wing_test_err |= 0x04;
#endif
  
//  for (i=0; i<0x3FC00/2; i+=256)  
//  {
//    M95Pxx_ReadData(i, rdBuff, 256);
//    for (j=0; j<256; j++) printf("%02X ", rdBuff[j]);
//  }
//  printf("\r\n");
//  for (i=0x1FE00; i<0x3FC00; i+=256)  
//  {
//    M95Pxx_ReadData(i, rdBuff, 256);
//    for (j=0; j<256; j++) printf("%02X ", rdBuff[j]);
//  }
//  printf("\r\n");
}
#if 0
void internal_eeprom_tddi_flash_test(void)
{
  uint8_t block = 0;
  uint8_t sector = 0;
  uint8_t page = 0;
//  uint8_t rbuf[512] = {0};
  uint32_t i,j;
  
  uint8_t *wrBuff = &Uart0.m_TX_payload[0];
  uint8_t *rdBuff = &Uart0.m_RX_payload[0];
  
  for (i=0; i<0x100; i++) wrBuff[i] = i;
  
  // 1. eeprom gamma area save
  M95Pxx_WriteData(M95Pxx_ANALOG_GMA_ADDR, wrBuff, 256);
  M95Pxx_WriteData(M95Pxx_ANALOG_GMA_ADDR+0x200, wrBuff, 256);
  
  // 2. eeprom gamma area read
//  M95Pxx_ReadData(M95Pxx_ANALOG_GMA_ADDR, rdBuff, 256);
//  for (j=0; j<16; j++)
//  {
//    for (i=0; i<16; i++) printf("%02X ", rdBuff[j*16+i]);
//    printf("\r\n");
//  }
  
//  hx83195_fw_update_on = 1;
//  if (hx83195_chip_detect() == TRUE)
//  {
  
  // 3. tddi flash write
  himax_burn_GMA_to_flash('A');
//  himax_mcu_fts_ctpm_gma_flash_write_with_sys_fs_4k(7, 0);  // None Volitile Write
  
  
  // 4. tddi flash read
//  himax_mcu_fts_ctpm_gma_flash_write_with_sys_fs_4k(6,0); // vcom
  himax_mcu_fts_ctpm_gma_flash_write_with_sys_fs_4k(8,0); // analog gma
//  }
  
  
  hx83195_fw_update_on = 0;
}
#endif



