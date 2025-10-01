#include "main.h"

Ring_TypeDef Ring[COM_UART_NUM];
UART_1_TypeDef Uart1;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  //BaseType_t  status;
  
  if(huart->Instance == USART1)
  {  
//    app_uart_protocol(app_uart_p);
    SetRing(&Ring[COM_UART_1], uart_rxdata);
    HAL_UART_Receive_IT(huart,&uart_rxdata,1);
  }
}

uint8_t RingCheck(Ring_TypeDef *pRing)
{
  if(pRing->Head != pRing->Tail) return TRUE;	
  return FALSE;
}

void SetRing(Ring_TypeDef *pRing, uint8_t Data)
{
  pRing->RxBuf[ pRing->Tail ] = Data;
  if (++pRing->Tail >= RING_BUFFER_SIZE) pRing->Tail = 0;
}

uint8_t GetRing(Ring_TypeDef *pRing)
{
  uint8_t Data = pRing->RxBuf[ pRing->Head ];
  
  if(++pRing->Head >= RING_BUFFER_SIZE) pRing->Head = 0;
  
  return Data;
}

void SetPacket(UART_1_TypeDef *pUart, uint8_t Data)
{
  pUart->PacketBuf[ pUart->PacketCnt ] = Data;
  if (++pUart->PacketCnt >= PACKET_BUFFER_SIZE) pUart->PacketCnt = 0;
}