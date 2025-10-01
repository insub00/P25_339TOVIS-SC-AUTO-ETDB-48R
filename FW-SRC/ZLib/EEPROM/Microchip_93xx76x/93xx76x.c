#include "main.h"

/* 8Kbit => 1KB SPI EEPROM */

uint8_t eep_buff[LCD_EEP_8BIT_MAX_SIZE] = {0};

static inline void _dly(void){ for(volatile int i=0;i<E93_DELAY_LOOP;i++) __NOP(); }

static inline void CS_H(void){ E93_GPIO_WRITE(E93_CS_PORT, E93_CS_PIN, 1); }
static inline void CS_L(void){ E93_GPIO_WRITE(E93_CS_PORT, E93_CS_PIN, 0); }
static inline void SK_H(void){ E93_GPIO_WRITE(E93_SK_PORT, E93_SK_PIN, 1); }
static inline void SK_L(void){ E93_GPIO_WRITE(E93_SK_PORT, E93_SK_PIN, 0); }
static inline void DI_W(int v){ E93_GPIO_WRITE(E93_DI_PORT, E93_DI_PIN, v); } //eeprom ic 기준 DI Pin
static inline int  DO_R(void){ return E93_GPIO_READ(E93_DO_PORT, E93_DO_PIN); } //eeprom ic 기존 DO pin

static inline void clk_pulse(void){ SK_H(); _dly(); SK_L(); _dly(); }

static void     send_bit(uint8_t b) { DI_W(b ? 1 : 0); clk_pulse(); }
static uint8_t  read_bit(void)      { SK_H(); _dly(); uint8_t v=DO_R(); SK_L(); _dly(); return v; }

static void send_bits(uint32_t val, uint8_t bits)
{
    for(int8_t i=bits-1;i>=0;--i) send_bit( (val>>i) & 1U );
}

/* Start(1) + Opcode(2) + Address(9) ? 93LC76B 고정 */
static void send_cmd_addr(uint8_t opc2, uint16_t addr)
{
    CS_H(); _dly();
    send_bit(1);                       /* Start */
    send_bits(opc2 & 0x3U, 2);         /* Opcode */
    send_bit(0);                       /* don't care */
    send_bits(addr, E93_ADDR_BITS);    /* Address (A8..A0) */
}

/* RDY/BSY 폴링: CS High 유지, SK 토글하며 DO==1이 될 때까지 대기 */
static bool wait_ready(uint32_t max_toggles)
{
    while(max_toggles--){
        SK_H(); _dly();
        int ready = DO_R();
        SK_L(); _dly();
        if(ready) return true;
    }
    return false;
}

/* ====== 초기화/핀설정 헬퍼 ====== */
void e93_init(void)
{
    /* 핀 방향은 보드 레벨에서:
       CS/SK/DI = 출력 Push-Pull, DO = 입력 Pull-up 권장
    */
    SK_L(); DI_W(0); CS_L(); _dly();
}

/* ====== 제어 명령 ====== */
void e93_ewen(void){             /* Write Enable */
    CS_H(); _dly();
    send_bit(1);                 /* start bit */
    send_bits(0b00,2);           /* EWxx prefix */
    send_bits(0b11,2);           /* '11' = EWEN */
    send_bits((1U<<E93_ADDR_BITS)-1U, E93_ADDR_BITS-1); /* don't care fill */
    CS_L(); _dly();
}

void e93_ewds(void){             /* Write Disable */
    CS_H(); _dly();
    send_bit(1);
    send_bits(0b00,2);
    send_bits(0b00,2);           /* '00' = EWDS */
    send_bits((1U<<E93_ADDR_BITS)-1U, E93_ADDR_BITS);
    CS_L(); _dly();
}

/* ====== 단발 READ/WRITE/ERASE ====== */
/* READ 한 워드 ? 93LC76B는 데이터 앞에 dummy '0' 비트가 선행 */
bool e93_read(uint16_t addr, uint16_t *out_word)
{
    if(!out_word) return false;
    
    e93_init();

    SK_L(); DI_W(0); CS_L(); _dly();
    CS_H(); _dly();
    send_bit(1);                // SB
    send_bits(0b10, 2);         // WRITE
    send_bit(0);                // Don't care
    send_bits(addr, 9);
    

//    (void)read_bit();                 /* dummy 0 drop (B-version x16) */

    uint16_t v=0;
    for(int i=0;i<E93_DATA_BITS;i++)
        v = (uint16_t)((v<<1) | read_bit());

    CS_L(); _dly();
    *out_word = v;
    return true;
}

static bool e93_check_status_after_op(void)
{
    // 1) TCSL: CS를 아주 잠깐 Low로 (최소 250ns 수준)
    CS_L(); _dly();                 // _dly()는 수백 ns 정도가 되도록 조정
    CS_H(); _dly();                 // 다시 High

    // 2) TSV: 상태 유효까지 짧게 대기
    _dly();

    // 3) CS=High 유지 + SCK 토글하면서 DO 폴링 (DO=1 → Ready)
    DI_W(0);                        // DI는 don't-care지만 Low로 유지
    uint32_t to = 5000;            // 보드에 맞춰 조정 (수 ms 커버)
    while (to--) {
        SK_H(); _dly();
        int ready = DO_R();
        SK_L(); _dly();
        if (ready) return true;
    }
    return false; // timeout
}

/* WRITE 한 워드 */
bool e93_write(uint16_t addr, uint16_t data)
{
  e93_init();
  e93_ewen();                       /* 반드시 한 번 Enable */
  
  SK_L(); DI_W(0); CS_L(); _dly();
  CS_H(); _dly();
  send_bit(1);                // SB
  send_bits(0b01, 2);         // WRITE
  send_bit(0);                // Don't care
  send_bits(addr, 9);
  send_bits(data, 16);
  
  bool ok = e93_check_status_after_op();
  
  CS_L(); _dly();
  e93_ewds();
  return ok;
}

/* ERASE 한 워드 */
bool e93_erase(uint16_t addr)
{
    e93_ewen();

    SK_L(); DI_W(0); CS_L(); _dly();
    CS_H(); _dly();
    send_bit(1);                // SB
    send_bits(0b11, 2);         // WRITE
    send_bit(0);                // Don't care
    send_bits(addr, 9);

    bool ok = e93_check_status_after_op();
    
    CS_L(); _dly();
    e93_ewds();
    return ok;
}

/* ====== 연속(버스트) READ ======
 * READ 명령 1회 후 CS를 유지하면 내부 주소가 자동 증가하며 계속 출력.
 * 각 워드 앞에 dummy '0' 비트가 1개 선행(데이터시트 Read 설명).
 */
bool e93_read_seq(uint16_t start_addr, uint8_t *buf, uint32_t count)
{
    if(!buf || !count) return false;
    
    e93_init();

    printf("%s : addr = %X, datas = ", __func__, start_addr);
    
    SK_L(); DI_W(0); CS_L(); _dly();

    /* READ + 시작 주소 */
    SK_L(); DI_W(0); CS_L(); _dly();
    CS_H(); _dly();
    send_bit(1);                // SB
    send_bits(0b10, 2);         // WRITE
    send_bit(0);                // Don't care
    send_bits(start_addr, 9);
    
    uint16_t index = 0;
    for(uint32_t i=0;i<count;i++)
    {
        uint16_t v=0;
        for(int b=0;b<E93_DATA_BITS;b++)
            v = (uint16_t)((v<<1) | read_bit());
        
        buf[index] = (v>>8)&0xFF;
        buf[index+1] = v&0xFF;
        
        printf("%02X ", buf[index]);
        printf("%02X ", buf[index+1]);

        index+=2;
    }
    printf("\r\n"); 

    CS_L(); _dly();
    return true;
}

/* ====== 연속(버스트) WRITE ======
 * Microwire는 READ만 "한 번에" 연속. WRITE는 워드마다 명령 반복.
 * 본 함수는 EWEN 1회 → 워드 쓰기 반복(각각 RDY/BSY 폴링) → 마지막 EWDS 수행.
 */
bool e93_write_seq(uint16_t start_addr, const uint8_t *buf, uint32_t count)
{
    if(!buf || !count) return false;
    
    e93_init();

    e93_ewen();

    uint16_t addr = start_addr;
    uint16_t index;
    
    printf("%s : addr = %X, datas = ", __func__, addr); 
    
    index = 0;
    for(uint32_t i=0;i<count;i++, addr++)
    {
      printf("%02X ", buf[index]);
      printf("%02X ", buf[index+1]);
        SK_L(); DI_W(0); CS_L(); _dly();
        CS_H(); _dly();
        send_bit(1);                // SB
        send_bits(0b01, 2);         // WRITE
        send_bit(0);                // Don't care
        send_bits(addr, 9);
        send_bits(buf[index], 8);
        send_bits(buf[index+1], 8);
          
        if (e93_check_status_after_op() == false) return false;

        CS_L(); _dly();                 /* 다음 명령 전 TCSL 보장 */
        
        index+=2;
    }
    printf("\r\n"); 

    e93_ewds();
    return true;
}

void e93_test(void)
{
  uint16_t w=0xABCD, r=0;
  uint8_t wbuff[255*2] = {0}; 
  uint8_t rbuff[255*2] = {0};  
    
    
    
    hx8295_eep_ctrl_by_host();
      
    e93_init();
   
    // 16-bit 조직(ORG=1) 예시
//    (void)e93_erase(0x0010);
    (void)e93_write(0x0010, w);
    (void)e93_read(0x0010, &r);
    // r == 0xABCD 기대
    printf("LCD EEP test : write datas = 0xABCD, read datas = %X\r\n", r);
    
    
//    for(int i=0; i<0xFF; i++) wbuff[i] = i;
    for(int i=0; i<0xFF; i++) printf("%04X ", wbuff[i]);
    printf("\r\n");
    e93_write_seq(0, wbuff, 255*2);
    
    memset(rbuff,0,sizeof(rbuff));
    e93_read_seq(0, rbuff, 255*2);
    for(int i=0; i<0xFF; i++) printf("%04X ", rbuff[i]);
    printf("\r\n");
    
}

void e93_eep_read(void)
{
  hx8295_eep_ctrl_by_host();
  e93_read_seq(0, &pTxBuf[0], 303);
  hx8295_eep_ctrl_by_ddi();
  
  hx8295_eep_fail_flag_check();
}

void tovis_eeprom_packet(uint8_t *pInBuf)
{
  uint8_t rwf = 0;
  uint16_t w_Cnt = 0;
  uint16_t in_length = 0;
  uint16_t w_Len = 0;
  uint32_t start_page_addr=0;
  
//  uint8_t ic_type = 0; // himax
//  uint8_t ic_type = 1; // novatek
  uint8_t ic_type = 2; // Microchip
  

  if (ic_type == 0) // himax
  {
//    rwf = pInBuf[5]; /* novatek ic 는  pInBuf[5]에 Read, Write flag가 있다. */
    w_Cnt = (uint16_t)(pInBuf[5] << 8 | pInBuf[6]);
    w_Len = (uint16_t)(pInBuf[7] << 8 | pInBuf[8]);    //64씩 PC 에서 날아옴.
    start_page_addr = w_Cnt * w_Len;
  }
  else if (ic_type == 1) //novatek , 12.9"
  {
    rwf = pInBuf[5]; /* novatek ic 는  pInBuf[5]에 Read, Write flag가 있다. */
    w_Cnt = (uint16_t)(pInBuf[6] << 8 | pInBuf[7]);
    w_Len = (uint16_t)(pInBuf[8] << 8 | pInBuf[9]);    //64씩 PC 에서 날아옴.
    start_page_addr = w_Cnt * w_Len;
  }
  else if (ic_type == 2) //microchip
  {
    rwf = pInBuf[5]; /* microchip ic 는  pInBuf[5]에 Read, Write flag가 있다. */
    w_Cnt = (uint16_t)(pInBuf[6] << 8 | pInBuf[7]);
    in_length = (uint16_t)(pInBuf[8] << 8 | pInBuf[9]);    //606날아옴
    w_Len = (in_length/2); //중요 : 16bit 처리라서 나누기2함.
    start_page_addr = w_Cnt * w_Len;
    
    printf("%s : RW = %d, Cnt = %d, Len = %d\r\n", __func__, rwf, w_Cnt, w_Len);
    
    
    if (rwf == 1) //WRITE
    {
      hx8295_eep_ctrl_by_host(); //eep host 
      
      e93_write_seq(start_page_addr, &pInBuf[10], w_Len);
      e93_read_seq(start_page_addr, &pTxBuf[1], w_Len);
      
      hx8295_eep_ctrl_by_ddi(); //eep ddi
      
      pTxBuf[0] = w_Cnt;
      UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], pTxBuf, in_length+1); //1번째 바이트는 cnt값 넣어서 송신
    }
    else if (rwf == 0) //READ
    {
      hx8295_eep_ctrl_by_host(); //eep host 
      
      e93_read_seq(start_page_addr, &pTxBuf[1], w_Len); //기존 write 후 read까지 진행을 해서 아래 rwf == 0에서 처리 안하고 위에 둠
      
      hx8295_eep_ctrl_by_ddi(); //eep ddi
      
      //토비스 자료상 마지막 read후 아래 flag까지 판정하게 되어 있음
      if (hx8295_eep_fail_flag_check() == 1) pTxBuf[0] = 0; // EEPROM FAIL FLAG OK
      else pTxBuf[0] = 1; // EEPROM FAIL FLAG NG
 
      UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], pTxBuf, in_length+1); //1번째 바이트는 eeprom flag값 넣어서 송신
      
    }
  }
}

/*
* 아래 packet 사용X 
* 93LC76B IC address 9bit, data가 16bit여서 아래 packet 사용 못함.
*/
void eeprom_save_packet(uint8_t *pInBuf)
{
//  uint8_t wBuf[2];
//  uint8_t rBuf[256] = {0};
//  uint8_t ret=0;
//  
//  uint8_t len = pInBuf[5];
//  uint8_t dev_addr = pInBuf[6]; //i2c용으로 사용X
//  uint8_t addr = pInBuf[7];
//  uint8_t rdFlag = pInBuf[8];
//  uint8_t *data = &pInBuf[9];
//  
//  
//  //write
//  if(rdFlag == 0)
//  {
//    hx8295_eep_ctrl_by_host();
//    e93_write_seq(addr, data, len);
//  }
//  
//  //read
//  LCM_EEP_i2c_sequential_read(pInBuf[6], &wBuf[0], 1, &rBuf[0], pInBuf[5]);
//  
//  UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], &rBuf[0], pInBuf[5]);
}


uint8_t GetMod255Checksum(uint8_t *data, uint16_t length)
{
    uint32_t sum = 0;

    for (uint16_t i = 0; i < length; i++)
    {
        sum += data[i];
    }
    printf("sum = %d\r\n", sum);

    return (uint8_t)(sum % 255);
}

void set_vcom_data_to_eep(uint16_t vcom_data)
{
  //Power ON
  //RESETB =H
  //20MS
  //STBYB = H
  //64MS
  
  printf("EEP VCOM save data = 0x%04X\r\n", vcom_data);
  
  //1. eep host 제어설정
  hx8295_eep_ctrl_by_host();
  
  //2. 기존 eep data read
  memset(eep_buff,0,LCD_EEP_8BIT_MAX_SIZE);
  e93_read_seq(0, &eep_buff[0], LCD_EEP_16BIT_MAX_SIZE); //303 : 전체 체크썸까지 읽음
  uint8_t checksum = GetMod255Checksum(eep_buff, LCD_EEP_8BIT_MAX_SIZE-1); //605
  printf("EEP read checksum = 0x%02X, calc checksum = 0x%02X\r\n", eep_buff[LCD_EEP_CHECKSUM_ADDR], checksum);
  printf("EEP VCOM MSB data = 0x%02X, LSB data = 0x%02X\r\n", eep_buff[LCD_EEP_VCOM_H_ADDR], eep_buff[LCD_EEP_VCOM_L_ADDR]);
  
  //VCOM DATA 변경
  eep_buff[LCD_EEP_VCOM_H_ADDR] = (vcom_data>>8)&0xFF;
  eep_buff[LCD_EEP_VCOM_L_ADDR] = vcom_data&0xFF;
  
  checksum = GetMod255Checksum(eep_buff, LCD_EEP_8BIT_MAX_SIZE-1); //605
  printf("EEP datas checksum = 0x%02X\r\n", checksum);
  eep_buff[LCD_EEP_CHECKSUM_ADDR] = checksum;
  
  e93_write_seq(0, &eep_buff[0], LCD_EEP_16BIT_MAX_SIZE); //303
  
  
  
  //TEST용
//  memset(eep_buff,0,LCD_EEP_8BIT_MAX_SIZE);
//  e93_read_seq(0, &eep_buff[0], LCD_EEP_16BIT_MAX_SIZE); //303 : 전체 체크썸까지 읽음
//  checksum = GetMod255Checksum(eep_buff, LCD_EEP_16BIT_MAX_SIZE-1); //302
//  printf("EEP datas checksum = 0x%02X\r\n", checksum);
//
//  printf("EEP VCOM MSB data = 0x%02X\r\n", eep_buff[99]);
//  printf("EEP VCOM LSB data = 0x%02X\r\n", eep_buff[100]);
  
  
  
  //?. eep ddi 제어설정
  hx8295_eep_ctrl_by_ddi();
}

void get_vcom_data_to_eep(uint16_t *vcom_data)
{
  //Power ON
  //RESETB =H
  //20MS
  //STBYB = H
  //64MS

  
  //1. eep host 제어설정
  hx8295_eep_ctrl_by_host();
  
  //2. 기존 eep data read
  memset(eep_buff,0,LCD_EEP_8BIT_MAX_SIZE);
  e93_read_seq(0x31, &eep_buff[0], 2);
  printf("EEP VCOM MSB data = 0x%02X, LSB data = 0x%02X\r\n", eep_buff[1], eep_buff[2]);
  
 
  //?. eep ddi 제어설정
  hx8295_eep_ctrl_by_ddi();
  
  hx8295_eep_fail_flag_check();
  
  *vcom_data = (uint16_t)eep_buff[1]<<8 | eep_buff[2];
  
  printf("EEP VCOM read data = 0x%04X\r\n", *vcom_data);
}
