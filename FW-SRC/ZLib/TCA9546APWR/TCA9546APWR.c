#include "main.h"



/* TCA9546A i2c switch */

/*
데이터시트 기준 채널 스위칭 타임 :

t<sub>switch</sub> (Switching Time): 최대 500 ns (0.5 μs)
  -> 즉, 마스터가 제어 레지스터에 채널을 선택한 뒤 실제 신호 경로가 열리거나 닫히는 데 걸리는 시간이 이 정도입니다.

I²C 명령 전송(제어 바이트 쓰기) 시간은 I²C 속도(100 kHz, 400 kHz, 1 MHz 등)에 따라 달라지며, 실제 체감 지연은 I²C 전송 시간 + ~0.5 μs 수준입니다.
*/
uint8_t g_pre_i2c_sel = 100;

void TCA9546APWR_Init()
{
  GPO_WritePin(I2C_RESET, HIGH); //i2c switch on
}

void TCA9546APWR_Channel_Sel(uint8_t ch)
{
  if (ch == g_pre_i2c_sel) return;
  
  HAL_StatusTypeDef Status;
  uint8_t buf[1] = {0};
 
  g_pre_i2c_sel = ch;
  buf[0] = (0x01 << ch) & 0x0F;
  Status = HAL_I2C_Master_Transmit(&hi2c2, TCA9545_DEV_ID, buf, 1, 100);
  if(Status != HAL_OK)
  {
    printf("%s Error!!%d\r\n",__FUNCTION__,Status);  
  }
}

