#ifndef __UART_H__
#define __UART_H__


#define RING_BUFFER_SIZE                (512*2)
#define UART1_TX_BUFFER_SIZE            (512*2)
#define PACKET_BUFFER_SIZE              (512*2)

typedef enum {
   COM_UART_1 = 0
 , COM_UART_NUM
}enum_com_uart;

typedef struct{
  uint16_t Head;
  uint16_t Tail;
  uint8_t RxBuf[RING_BUFFER_SIZE];
} Ring_TypeDef;

typedef struct{
  
  uint8_t TxBuf[UART1_TX_BUFFER_SIZE];
  
  uint8_t MainCmd;
  uint8_t SubCmd;
  uint16_t Length;
  
  uint8_t PacketBuf[PACKET_BUFFER_SIZE];
  uint16_t PacketCnt;
  
} UART_1_TypeDef;


extern Ring_TypeDef Ring[COM_UART_NUM];
extern UART_1_TypeDef Uart1;


extern uint8_t RingCheck(Ring_TypeDef *pRing);
extern void SetRing(Ring_TypeDef *pRing, uint8_t Data);
extern uint8_t GetRing(Ring_TypeDef *pRing);
extern void SetPacket(UART_1_TypeDef *pUart, uint8_t Data);


#endif