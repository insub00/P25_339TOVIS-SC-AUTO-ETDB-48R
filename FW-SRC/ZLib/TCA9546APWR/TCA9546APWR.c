#include "main.h"



/* TCA9546A i2c switch */

/*
�����ͽ�Ʈ ���� ä�� ����Ī Ÿ�� :

t<sub>switch</sub> (Switching Time): �ִ� 500 ns (0.5 ��s)
  -> ��, �����Ͱ� ���� �������Ϳ� ä���� ������ �� ���� ��ȣ ��ΰ� �����ų� ������ �� �ɸ��� �ð��� �� �����Դϴ�.

I��C ��� ����(���� ����Ʈ ����) �ð��� I��C �ӵ�(100 kHz, 400 kHz, 1 MHz ��)�� ���� �޶�����, ���� ü�� ������ I��C ���� �ð� + ~0.5 ��s �����Դϴ�.
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

