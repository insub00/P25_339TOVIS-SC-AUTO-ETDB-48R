#include "main.h"


uint8_t gPowerOff = 0;
uint8_t gPower_en = 0;
uint8_t gMini_ptg_use = 0;

/* TCA9546APWR(TSSOP-16) I2C Switch */
Gp3Sel_Def I2cMuxInfo = {
  ADD_SET0_GPIO_Port, ADD_SET0_Pin, 
  ADD_SET1_GPIO_Port, ADD_SET1_Pin,
  ADD_SET2_GPIO_Port, ADD_SET2_Pin
};

/* GPO 설정 */
void GPO_WritePin(GPO_INFO name, uint8_t lowhigh)
{
  switch (name)
  {
  case I2C_RESET: lowhigh?HAL_GPIO_WritePin(I2C_RESET_GPIO_Port, I2C_RESET_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(I2C_RESET_GPIO_Port, I2C_RESET_Pin, GPIO_PIN_RESET); break;
  case TP_VDD_EN: lowhigh?HAL_GPIO_WritePin(TP_VDD_EN_GPIO_Port, TP_VDD_EN_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(TP_VDD_EN_GPIO_Port, TP_VDD_EN_Pin, GPIO_PIN_RESET); break;
  case ENVSP: lowhigh?HAL_GPIO_WritePin(ENVSP_GPIO_Port, ENVSP_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(ENVSP_GPIO_Port, ENVSP_Pin, GPIO_PIN_RESET); break;
  case ENVSN: lowhigh?HAL_GPIO_WritePin(ENVSN_GPIO_Port, ENVSN_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(ENVSN_GPIO_Port, ENVSN_Pin, GPIO_PIN_RESET); break;
  case ENOTP: lowhigh?HAL_GPIO_WritePin(ENOTP_GPIO_Port, ENOTP_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(ENOTP_GPIO_Port, ENOTP_Pin, GPIO_PIN_RESET); break;
  case LED1: lowhigh?HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); break;
  case LED2: lowhigh?HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); break;
  case LED3: lowhigh?HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); break;
  case T_RST: lowhigh?HAL_GPIO_WritePin(T_RST_GPIO_Port, T_RST_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(T_RST_GPIO_Port, T_RST_Pin, GPIO_PIN_RESET); break;
  case E_WP: lowhigh?HAL_GPIO_WritePin(E_WP_GPIO_Port, E_WP_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(E_WP_GPIO_Port, E_WP_Pin, GPIO_PIN_RESET); break;
  case TCON_RST: lowhigh?HAL_GPIO_WritePin(TCON_RST_GPIO_Port, TCON_RST_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(TCON_RST_GPIO_Port, TCON_RST_Pin, GPIO_PIN_RESET); break;
  case PON: lowhigh?HAL_GPIO_WritePin(PON_GPIO_Port, PON_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(PON_GPIO_Port, PON_Pin, GPIO_PIN_RESET); break;
//  case LCD_RST: lowhigh?HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET); break;
  
  }
  
}

/* GPI READ */
GPIO_PinState GPI_ReadPin(GPI_INFO name)
{
  GPIO_PinState pin_state;
  
  switch (name)
  {
  case LCD_FAIL: pin_state = HAL_GPIO_ReadPin(LCD_FAIL_GPIO_Port, LCD_FAIL_Pin); break;
  case TP_FAIL: pin_state = HAL_GPIO_ReadPin(TP_FAIL_GPIO_Port, TP_FAIL_Pin); break;
  case TCON_ASIL: pin_state = HAL_GPIO_ReadPin(TCON_ASIL_GPIO_Port, TCON_ASIL_Pin); break;
  case TCH_EXT: pin_state = HAL_GPIO_ReadPin(TCH_EXT_GPIO_Port, TCH_EXT_Pin); break;
  case TEST_KEY: pin_state = HAL_GPIO_ReadPin(TEST_KEY_GPIO_Port, TEST_KEY_Pin); break;
  case LED_FAIL: pin_state = HAL_GPIO_ReadPin(LED_FAIL_GPIO_Port, LED_FAIL_Pin); break;
  case VLCD_ON: pin_state = HAL_GPIO_ReadPin(VLCD_ON_GPIO_Port, VLCD_ON_Pin); break;
  case BL_EN: pin_state = HAL_GPIO_ReadPin(BL_EN_GPIO_Port, BL_EN_Pin); break;
  case SIGNAL_ON: pin_state = HAL_GPIO_ReadPin(SIGNAL_ON_GPIO_Port, SIGNAL_ON_Pin); break;
  case LR: pin_state = HAL_GPIO_ReadPin(LR_GPIO_Port, LR_Pin); break;
//  case PON: pin_state = HAL_GPIO_ReadPin(PON_GPIO_Port, PON_Pin); break;
  case TCH_INT: pin_state = HAL_GPIO_ReadPin(TCH_INT_GPIO_Port, TCH_INT_Pin); break;
  }
  return pin_state;
}

/* 윙보드 초기 설정 */
void board_init()
{
  printf(ANSI_GREEN"Main Application Start => [%s][%s]\r\n"ANSI_RESET, __DATE__,__TIME__);
  printf("SystemCoreClock = %d\r\n", SystemCoreClock);
  
  M95Pxx_Init(); //내부 eeprom
  TCA9546APWR_Init(); //I2C switch
}

#if 1
/*
* PTG CTRL : RESET, 
* WING CTRL : MUTE 신호 받아서 POWER SEQ 진행.
*/
void power_seq(uint8_t onoff)
{
  uint8_t test = 9;
  
  if (onoff == ON)
  { 
//    HAL_Delay(4);
    /* 내부 파워 */ 
    GPO_WritePin(TP_VDD_EN, LOW); //low active
    GPO_WritePin(ENVSP, HIGH);
    GPO_WritePin(ENVSN, HIGH);
    
    GPO_WritePin(E_WP, HIGH);
    GPO_WritePin(T_RST, HIGH);
    
    HAL_Delay(20);
    
    /******************** I2C or SPI *****************************/
//    test_func(eDIC_I2C_CHECK);
//    hx8295_ATREN_ctrl(1); //ATREN H -> register 0x0B:0x05 write 후 read시 0x01 읽어짐.
//    hx8295_test_code();
    /*************************************************************/
   
    GPO_WritePin(PON, HIGH);
   
    HAL_Delay(100);
    led_driver_ctrl();

#ifdef WING_BOARD_TEST    
    GPO_WritePin(ENOTP, HIGH);
#endif    
  }
  else
  {
    GPO_WritePin(PON, LOW);
    HAL_Delay(10);
    GPO_WritePin(T_RST, LOW);
    GPO_WritePin(E_WP, LOW);
    HAL_Delay(10);
    GPO_WritePin(ENVSN, LOW);
    GPO_WritePin(ENVSP, LOW);
    GPO_WritePin(TP_VDD_EN, HIGH);
    
#ifdef WING_BOARD_TEST    
    GPO_WritePin(ENOTP, LOW);
#endif 
  }
}
#else
void power_seq(uint8_t onoff)
{
  uint8_t test = 9;
  
  if (onoff == ON)
  { 

//    GPO_WritePin(LCD_RST, HIGH);
    GPO_WritePin(PON, HIGH);
    GPO_WritePin(E_WP, HIGH);
    GPO_WritePin(T_RST, HIGH);
    
   
    /* 내부 파워 */ 
    GPO_WritePin(TP_VDD_EN, LOW); //low active
    GPO_WritePin(ENVSP, HIGH);
    GPO_WritePin(ENVSN, HIGH);
   

    HAL_Delay(500);
    led_driver_ctrl();
  }
  else
  {
 
  }
}
#endif

void sw_func(void)
{
  if (GPI_ReadPin(TEST_KEY) == GPIO_PIN_RESET)
  {
    HAL_Delay(100);
    if (GPI_ReadPin(TEST_KEY) == GPIO_PIN_RESET)
    {
      printf("test key \r\n");
      
      /* 전체 I2C2 라인 통신 체크 */
//      test_func(eTEST_I2C_SLAVE);
      
      /* D-IC I2C 통신 체크 */
//      test_func(eDIC_I2C_CHECK);
      
      /* 내부 eep test <- 완료 */
//      test_func(eINTER_EEP);
      
      /* LCD eep test */
//      test_func(eLCD_EEP_TEST);
      
      /* LCD 내부 전원 test
         디폴트 12V에서 가변되는지 확인용
      */
//      hx8295_power_set_test();
      
      /* DDI Register read */
//      test_func(HX8295_EEP_CTRL_BY_HOST_DEFAULT_VALUE_READ);
////      test_func(HX82595_DEFAULT_TEST);
      
      /* VCOM TEST */
//      test_func(eVCOM);
      
      /* TOUCH TEST */
      test_func(TOUCH_TEST);
      
#ifdef WING_BOARD_TEST        
      /* 생기 test용 */
      wing_board_test();
#endif      
    }
  }
}


uint8_t OnStatus  = 0;
void input_func(void)
{
  if ( (GPI_ReadPin(VLCD_ON) == GPIO_PIN_SET) && (OnStatus == OFF) )
  {
    HAL_Delay(5);
    if (GPI_ReadPin(VLCD_ON) == GPIO_PIN_SET)
    {
      power_seq(ON);
      OnStatus = ON;
    }
  }
  if ( (GPI_ReadPin(VLCD_ON) == GPIO_PIN_RESET) && (OnStatus == ON) )
  {
    HAL_Delay(5);
    if (GPI_ReadPin(VLCD_ON) == GPIO_PIN_RESET)
    {
      power_seq(OFF);
      OnStatus = OFF;
    }
  }
  

 
  // Touch Drawing 검사
//  if(nvt_infor.coordinate_sense)
//  {
//    request_cooridnate_interrupt();
//    nvt_infor.coordinate_sense = 0;
//  }
  
  sw_func(); //배포 버젼으로 컴파일시 주석 처리
}

void output_func(void)
{
  if (Utick.output >= 500)
  {
    Utick.output = 0;
    
    if (g_wing_test_finish) 
    {
      if ((g_wing_test_err & 0x01) == 0) HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      else HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
      
      if ((g_wing_test_err & 0x02) == 0) HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
      else HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
      
      if ((g_wing_test_err & 0x0C) == 0) HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
      else HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
    }
    else
    {
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin); //동작 상태 LED
    }

  }
}


/*
* 25.09.11 토비스 곽상윤 책임 내방시 테스트한 내용
*/
void power_seq_test(uint8_t onoff)
{
  uint8_t test = 9;
  
  if (onoff == ON)
  { 
    
    HAL_Delay(4);
    /* 내부 파워 */ 
    GPO_WritePin(TP_VDD_EN, LOW); //low active
    GPO_WritePin(ENVSP, HIGH);
    GPO_WritePin(ENVSN, HIGH);
    
//    HAL_Delay(1);
//    GPO_WritePin(LCD_RST, HIGH);
    
    
    // "*****" 시료 오면 확인해라.
    HAL_Delay(20);
//    test_func(eDIC_I2C_CHECK);
//    hx8295_ATREN_ctrl(1); //ATREN H -> register 0x0B:0x05 write 후 read시 0x01 읽어짐.
//    hx8295_test_code();
    
    
    /* TEST 
    1. ATREN=L 제품
    1) register 로 강제 ATREN H 만든 후 통신 시도.
    2) EEPEN L/H 일때, DDI 통신 가능 여부
    3) EEPEN L/H 일때, EEPROM write 가능 여부
    4) PON L/H 일때, DDI 통신 가능 여부
    */
    switch (test)
    {

    case 0:
//1) register 로 강제 ATREN H 만든 후 통신 시도. -> OK   
      hx8295_ATREN_ctrl(1); //ATREN H -> register 0x0B:0x05 write 후 read시 0x01 읽어짐. 
      HAL_Delay(5);
      test_func(eDIC_I2C_CHECK);
      GPO_WritePin(PON, HIGH);
      break;
      
    case 1:
//2) EEPEN L/H 일때, DDI 통신 가능 여부 - LOW  -> OK
      GPO_WritePin(E_WP, LOW);
      HAL_Delay(5);
      test_func(eDIC_I2C_CHECK);
      GPO_WritePin(PON, HIGH);
      break;
      
    case 2:
//2) EEPEN L/H 일때, DDI 통신 가능 여부 - HIGH -> OK
      GPO_WritePin(E_WP, HIGH);
      HAL_Delay(10);
      test_func(eDIC_I2C_CHECK);
      GPO_WritePin(PON, HIGH);
      break;
      
    case 3:
//E_WP 핀은 우리 기준 low active, ddi 기준 high active
//3) EEPEN L/H 일때, EEPROM write 가능 여부 - LOW
      GPO_WritePin(E_WP, LOW);
      printf("EEPEN == LOW\r\n");
      HAL_Delay(5);
      test_func(eLCD_EEP_TEST); //파워시퀀스 탈때 정상 WRITE -> READ, PON HIGH 이후도 정상
      GPO_WritePin(PON, HIGH);
      break;
      
    case 4:
//3) EEPEN L/H 일때, EEPROM write 가능 여부 - HIGH
      GPO_WritePin(E_WP, HIGH);
      printf("EEPEN == HIGH\r\n");
      HAL_Delay(5);
      test_func(eLCD_EEP_TEST); //파워시퀀스 탈때 정상 WRITE -> READ, PON HIGH 이후도 NG
      GPO_WritePin(PON, HIGH);
      break;
      
    case 5:
//4) PON L/H 일때, DDI 통신 가능 여부 - LOW -> 통신 완료
      GPO_WritePin(PON, LOW);
      HAL_Delay(5);
      test_func(eDIC_I2C_CHECK);
      break;
      
    case 6:
//4) PON L/H 일때, DDI 통신 가능 여부 - HIGH -> 통신 완료
      GPO_WritePin(PON, HIGH);
      HAL_Delay(5);
      test_func(eDIC_I2C_CHECK);
      break;
    }
    
    
    /*
    2. ATREN=H 제품
    1) EEPROM write 가능 여부
    2) EEPROM write 불가 시, RESET L 후 write 시도
    3) EEPEN L/H 일때, DDI 통신 가능 여부
    4) PON L/H 일때, DDI 통신 가능 여부
    */
    
    
    
    

    
    GPO_WritePin(PON, HIGH);
    HAL_Delay(5);
      
/* 제어 신호 */  
    if (test == 3 || test == 4) {

    }
    else {
      GPO_WritePin(E_WP, HIGH);
      printf("EEPEN == HIGH\r\n");
    }
    
    GPO_WritePin(T_RST, HIGH);
    

   
      
      
      

    HAL_Delay(500);
    led_driver_ctrl();
  }
  else
  {
 
  }
}

void ver_revision(struct SYS_VER* Ver)
{
  char temp [] = __DATE__;
  char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
                    "Sep", "Oct", "Nov", "Dec"};
  unsigned char month, day;
  int year;
  
  //ex:"Aug 19 2023"
  year = atoi(temp + 9); //년 뒤에2자리(20xx)
  
  *(temp + 6) = 0;
  day  = atoi(temp + 4); //일 
  
  *(temp + 3) = 0;
  for (int k = 0; k < 12; k++){ //월  
    if (!strcmp(temp, months[k])){ month = k + 1; break; }
  }
  
//  printf("year = %d, month = %d, day = %d\r\n", year, month, day);  
  
//  Ver->Year 	= (year/10)*16 + (year%10); //10진수를 16진수로 변환. 
//  Ver->Month	= (month/10)*16 + (month%10);
//  Ver->Day	= (day/10)*16 + (day%10);
  
  Ver->Year 	= year;
  Ver->Month	= month;
  Ver->Day	= day;
}

// 16비트 숫자의 바이트를 스왑하는 함수
uint16_t swap16(uint16_t num) 
{
  return (num >> 8) | (num << 8);
}

// bytes to short 변환 함수 (big endian)
uint16_t b2s(uint8_t *byte) 
{
  return (uint16_t)((byte[0] << 8) | byte[1]);
}

// bytes to long 변환 함수 (big endian)
uint32_t b2l(uint8_t *byte) 
{
  return (uint32_t)( (byte[0] << 24) | (byte[1] << 16) | (byte[2] << 8) | byte[3] );
}

// short to bytes 변환 함수 (big endian)
void s2b(uint16_t short_data, uint8_t *byte) 
{
  byte[0] = short_data >> 8;
  byte[1] = short_data & 0xFF;
}

// long to bytes 변환 함수 (big endian)
void l2b(uint32_t long_data, uint8_t *byte) 
{
  byte[0] = long_data >> 24;
  byte[1] = long_data >> 16;
  byte[2] = long_data >> 8;
  byte[3] = long_data & 0xFF;
}

// long to 3bytes 변환 함수 (big endian)
void l23b(uint32_t long_data, uint8_t *byte) 
{
  byte[0] = long_data >> 16;
  byte[1] = long_data >> 8;
  byte[2] = long_data & 0xFF;
}

// 오차율을 계산하는 함수
float calculate_error_rate(float actual, float predicted) 
{
  if (actual == 0) return 0;
  
  float error_rate = (fabsf(actual - predicted) / actual) * 100;
  return error_rate;
}

void gp_switch_2pin(Gp2Sel_Def* PinInfo, uint8_t No, uint32_t Delay)
{
  GPIO_PinState PinStare0 = GPIO_PIN_RESET;
  GPIO_PinState PinStare1 = GPIO_PIN_RESET;
  
  switch (No)
  {
  case 0:
    PinStare0 = GPIO_PIN_RESET;
    PinStare1 = GPIO_PIN_RESET;
    break;
  case 1:
    PinStare0 = GPIO_PIN_SET;
    PinStare1 = GPIO_PIN_RESET;
    break;
  case 2:
    PinStare0 = GPIO_PIN_RESET;
    PinStare1 = GPIO_PIN_SET;
    break;
  case 3:
    PinStare0 = GPIO_PIN_SET;
    PinStare1 = GPIO_PIN_SET;
    break;
  }
  HAL_GPIO_WritePin(PinInfo->s0_port, PinInfo->s0_pin, PinStare0);
  HAL_GPIO_WritePin(PinInfo->s1_port, PinInfo->s1_pin, PinStare1);
  
  HAL_Delay(Delay);
}

void gpSwitch3Pin(Gp3Sel_Def* PinInfo, uint8_t No, uint32_t Delay)
{
  printf("mux ctrl : no[%d], delay[%d] \r\n", No, Delay);
  
  GPIO_PinState PinStare0 = GPIO_PIN_RESET;
  GPIO_PinState PinStare1 = GPIO_PIN_RESET;
  GPIO_PinState PinStare2 = GPIO_PIN_RESET;
  
  switch (No)
  {
  case 0:
    PinStare0 = GPIO_PIN_RESET;
    PinStare1 = GPIO_PIN_RESET;
    PinStare2 = GPIO_PIN_RESET;
    break;
  case 1:
    PinStare0 = GPIO_PIN_SET;
    PinStare1 = GPIO_PIN_RESET;
    PinStare2 = GPIO_PIN_RESET;
    break;
  case 2:
    PinStare0 = GPIO_PIN_RESET;
    PinStare1 = GPIO_PIN_SET;
    PinStare2 = GPIO_PIN_RESET;
    break;
  case 3:
    PinStare0 = GPIO_PIN_SET;
    PinStare1 = GPIO_PIN_SET;
    PinStare2 = GPIO_PIN_RESET;
    break;
  case 4:
    PinStare0 = GPIO_PIN_RESET;
    PinStare1 = GPIO_PIN_RESET;
    PinStare2 = GPIO_PIN_SET;
    break;
  case 5:
    PinStare0 = GPIO_PIN_SET;
    PinStare1 = GPIO_PIN_RESET;
    PinStare2 = GPIO_PIN_SET;
    break;
  case 6:
    PinStare0 = GPIO_PIN_RESET;
    PinStare1 = GPIO_PIN_SET;
    PinStare2 = GPIO_PIN_SET;
    break;
  case 7:
    PinStare0 = GPIO_PIN_SET;
    PinStare1 = GPIO_PIN_SET;
    PinStare2 = GPIO_PIN_SET;
    break;
  }
  HAL_GPIO_WritePin(PinInfo->s0_port, PinInfo->s0_pin, PinStare0);
  HAL_GPIO_WritePin(PinInfo->s1_port, PinInfo->s1_pin, PinStare1);
  HAL_GPIO_WritePin(PinInfo->s2_port, PinInfo->s2_pin, PinStare2);
  
  HAL_Delay(Delay);
}

/*
* TCA9546APWR(TSSOP-16) IC 
* Slave Address 설정 핀으로 사용X (GPIO 설정시 LOW로 적용되어 있음)
*/
void I2C_mux_ctrl(uint8_t i2c)
{
  gpSwitch3Pin(&I2cMuxInfo, i2c, TIMER_mSec(5));
}

// 퍼센트를 계산하는 함수
uint8_t calculatePercentage(uint16_t part, uint16_t total) 
{
  if (total == 0) {
    // 전체 값이 0이면, 0으로 나누는 것을 방지
    return 0;
  }
  return (uint8_t)(((float)part / (float)total) * (float)100.0);
}

void led_driver_ctrl(void)
{
  uint8_t dev_id = 0x22<<1;
  uint8_t buff[5] = {0};

#if 1
  //10.25 기준 설정 값
  LED_i2c_write(dev_id, 0x02, 0x02); // ENA=1, FAST_SS=1
//  LED_i2c_write(dev_id, 0x03, 0x0C);  
  LED_i2c_write(dev_id, 0x0F, 0x08); // CH4 Disable
#else
//  LED_i2c_write(dev_id, 0x02, 0x0E); // ENA=1, FAST_SS=
//  LED_i2c_write(dev_id, 0x0F, 0x08); // CH4 Disable
  
  /* 무부하 강제 ON, 전압값 : 31.xxV로 부스팅으로 흔들림 */
  LED_i2c_write(dev_id, 0x02, 0x0E); // ENA=1, FAST_SS=1
  LED_i2c_write(dev_id, 0x03, 0x0C); 
#endif
  buff[0] = LED_i2c_read(dev_id, 0x02);
  buff[1] = LED_i2c_read(dev_id, 0x03);
  
  printf("LED_D read reg[0x%02X] = 0x%02X\r\n", 0x02, buff[0]);
  printf("LED_D read reg[0x%02X] = 0x%02X\r\n", 0x03, buff[1]);
}

void led_driver_tune(uint8_t *pInBuf)
{
  uint8_t rdData;
  uint8_t ucRw = 0;
  uint8_t w_addr = 0;
  uint8_t w_data = 0;

  ucRw = pInBuf[5]; //Rw
  w_addr = pInBuf[6]; //Addr
  
  if (ucRw == 0) //write
  {
    w_data = pInBuf[7];
    printf("led driver tune Rw[%d] Addr[0x%02X] Data[0x%02X]\r\n", ucRw, w_addr, w_data);
    
    LED_i2c_write(0x22<<1, w_addr, w_data);
  }

  // read
  rdData = LED_i2c_read(0x22<<1, w_addr); //slave address : 0x44[8bit]
  printf("read led driver[0x%02X]\r\n", rdData);
 
  uint8_t TxBuf = rdData;
  UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], &TxBuf, 1);
}

void led_fail_detection_check(uint8_t *pInBuf)
{
  uint8_t txBuff[10] = {0};
  uint8_t index = 0;
  uint8_t pin = 0;
  
//  TCA9546APWR_Channel_Sel(eI2C_LED);
//  
////  pin = HAL_GPIO_ReadPin(FAIL_DET2_GPIO_Port, FAIL_DET2_Pin); //LED
////  if (pin == 1) //HIGH == OK
////  {
////    txBuff[index++] = 0;
////    printf("LED Fail Det Pin = HIGH\r\n");
////  }
////  else //lOW ==NG
////  {
////    txBuff[index++] = 1;
////    printf("LED Fail Det Pin = LOW\r\n");
////  }
//  
//  txBuff[index++] = HAL_GPIO_ReadPin(FAIL_DET2_GPIO_Port, FAIL_DET2_Pin); //LED
//  txBuff[index++] = LED_i2c_read(0x22<<1, 0x11); //slave address : 0x44[8bit]
//  txBuff[index++] = LED_i2c_read(0x22<<1, 0x12); //slave address : 0x44[8bit]
//  txBuff[index++] = LED_i2c_read(0x22<<1, 0x13); //slave address : 0x44[8bit]
//  txBuff[index++] = LED_i2c_read(0x22<<1, 0x15); //slave address : 0x44[8bit]
//  
//  for (uint8_t i=1; i<index; i++)
//  {
//    printf("LED Fail Det Register = 0x%02X\r\n", txBuff[i]);
//  }

  UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], txBuff, index);
}

//void tovis_eeprom_packet(uint8_t *pInBuf)
//{
//  uint8_t rdFlag = 0;
//  uint16_t w_Cnt = 0;
//  uint16_t w_Len = 0;
//  uint32_t start_page_addr=0;
//  uint8_t w_max_cnt=0, i =0;
////  uint8_t ic_type = 0; // himax
//  uint8_t ic_type = 1; // novatek
//  uint8_t w_Buf[5] = {0};
//  uint8_t r_Buf[100] = {0};
//  
//
//  printf("tovis eeprom wr packet \r\n");
//
//  
//  if (ic_type == 0)
//  {
////    rdFlag = pInBuf[5]; /* novatek ic 는  pInBuf[5]에 Read, Write flag가 있다. */
//    w_Cnt = (uint16_t)(pInBuf[5] << 8 | pInBuf[6]);
//    w_Len = (uint16_t)(pInBuf[7] << 8 | pInBuf[8]);    //64씩 PC 에서 날아옴.
//    start_page_addr = w_Cnt * w_Len;
//    w_max_cnt = w_Len / 16;
//    
//    printf("w_Cnt = %d, w_Len = %d\r\n", w_Cnt, w_Len);
//    
//    GPO_WritePin(E_WP, LOW);
//    HAL_Delay(5);
//    
//    // i2c switch 변경
//    TCA9546APWR_Channel_Sel(eI2C_EEPROM);
//
//    for(i=0; i<w_Len; i++)
//    {
//      LCM_EEP_i2c_write(LCM_EEP_DEV_ID, start_page_addr + i, &pInBuf[9 + i], 1);
//      //        HAL_Delay(2); //25.05.14 제품 마진에 걸리는 ic가 있어 DELAY MAX 5ms까지 적용해야 할것 같음
//        HAL_Delay(10);
//    }
//    GPO_WritePin(E_WP, HIGH);
//  }
//  else
//  {
//    rdFlag = pInBuf[5]; /* novatek ic 는  pInBuf[5]에 Read, Write flag가 있다. */
//    w_Cnt = (uint16_t)(pInBuf[6] << 8 | pInBuf[7]);
//    w_Len = (uint16_t)(pInBuf[8] << 8 | pInBuf[9]);    //64씩 PC 에서 날아옴.
//    start_page_addr = w_Cnt * w_Len;
//    w_max_cnt = w_Len / 16;
//    
//    printf("rdFlag = %d, w_Cnt = %d, w_Len = %d, w_max_cnt = %d\r\n", rdFlag, w_Cnt, w_Len, w_max_cnt);
//    
//    
//    // i2c switch 변경
//    TCA9546APWR_Channel_Sel(eI2C_EEPROM);
//    
//    if (rdFlag == 1) //wirte
//    {
//      GPO_WritePin(E_WP, LOW);
//      HAL_Delay(5);
//      
//      for(i=0; i<w_Len; i++)
//      {
//        LCM_EEP_i2c_write(LCM_EEP_DEV_ID, start_page_addr + i, &pInBuf[10 + i], 1);
////        HAL_Delay(2); //25.05.14 제품 마진에 걸리는 ic가 있어 DELAY MAX 5ms까지 적용해야 할것 같음
//        HAL_Delay(10);
//      }
//      GPO_WritePin(E_WP, HIGH);
//    }
//    
//    w_Buf[0] = start_page_addr;
//    LCM_EEP_i2c_sequential_read(LCM_EEP_DEV_ID, &w_Buf[0], 1, &r_Buf[0], w_Len);
//  }
//  
//  
//  uint8_t TxBuf[100] = {0};
//  TxBuf[0] = w_Cnt;
//  memcpy(&TxBuf[1], r_Buf, w_Len);
//  UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], TxBuf, w_Len+1);
//  
//}

//void eeprom_save_packet(uint8_t *pInBuf)
//{
//
//}

void eeprom_write_packet(uint8_t *pInBuf)
{
    uint16_t w_Cnt = 0;
    uint16_t w_Len = 0;
    uint32_t start_page_addr=0;
    
    
    w_Cnt = (uint16_t)(pInBuf[5] << 8 | pInBuf[6]);
    w_Len = (uint16_t)(pInBuf[7] << 8 | pInBuf[8]);    //64

#ifdef EEPROM_SIZE256
    if(w_Cnt < 1024)
    {
        start_page_addr = w_Cnt * w_Len;
        eeprom_write_reg(EEPROM_DEV_ADDR_0, start_page_addr, &pInBuf[9], w_Len);
    }
    else
    {
        start_page_addr = ( (uint32_t)((uint32_t)w_Cnt * (uint32_t)w_Len) - (uint32_t)FW_SIZE_64k ) & 0xFFFF;
        eeprom_write_reg(EEPROM_DEV_ADDR_1, start_page_addr, &pInBuf[9], w_Len);
    }
#else    
    /* 24.12.12 기존 eeprom(256KB)에서 M95P08E IC로 변경.  (8 Mbit = 1 Mbyte = 1,048,576 bytes)  */
    start_page_addr = (uint32_t)w_Cnt * (uint32_t)w_Len;
    M95Pxx_WriteData(start_page_addr, &pInBuf[9], w_Len); 
    
//    M95Pxx_ReadData(start_page_addr, );
#endif    
    
    
#if(DEBUG == USE)
//    printf("eeprom write Cnt[%d]Len[%d]start addr[%ld]\r\n",w_Cnt,w_Len,(uint32_t)start_page_addr);
//    uint16_t j;
//    for(j=1; j<=w_Len; j++)
//    {
//            if(!(j%16))    printf("0x%02X,\r\n",pInBuf[9+(j-1)]);
//            else        printf("0x%02X, ",pInBuf[9+(j-1)]);
//    }
#endif
    
    uint8_t TxBuf = w_Cnt;
    UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], &TxBuf, 1);
}

void asil_fb_check(uint8_t *pInBuf)
{
  uint8_t txBuff[25] = {0};
  uint8_t len;
  
//  txBuff[0] = HAL_GPIO_ReadPin(FAIL_DET1_GPIO_Port, FAIL_DET1_Pin); //TDDI
//  printf("TDDI Fail Det Pin[%X]\r\n", txBuff[0]);
//  
//  len = asil_fail_detection_check(&txBuff[1]);
//  len += asil_touch_error_event_check(&txBuff[1+len]);

  UARTxSendData(pInBuf[PROTOCOL_MAIN_CMD], pInBuf[PROTOCOL_SUB_CMD], txBuff, 1+len);
}











