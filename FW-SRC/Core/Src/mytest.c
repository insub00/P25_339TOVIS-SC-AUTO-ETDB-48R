#include "main.h"


uint8_t gWrBuf[512] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x03, 0x01, 0x00, 0x00, 0x20, 0x02, 0x00};
uint8_t gRdBuf[512] = {0};
uint8_t g_wing_test_err = 0;
uint8_t g_wing_test_finish = 0;

void all_test()
{
  uint8_t ret = 0; //false
  
  Slave_Addr_Check_All_I2cLine();
    
//  TCA9546APWR_Channel_Sel(eI2C_TOUCH);
//  ret = Initializtion_For_MP_Test(); 
////  if (ret == 0) goto Func_exit;
  
  internal_eeprom_test();
  
//  asil_fb_check(NULL);
  
  
//  TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
//  asil_fail_detection_check();
    
  
//Func_exit:
//  if (ret == 0)
//  {
//    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
//  }
}

void test_func(uint8_t test_no)
{
  uint8_t rdData[20] = {0};
  static uint16_t cnt = 0;
  
  switch (test_no)
  {
  case eTEST_I2C_SLAVE:
/* 전체 I2c2 Line 통신 체크 */
    Slave_Addr_Check_All_I2cLine(); //전체 i2c라인 확인용
    break;
  case eDIC_I2C_CHECK:
    D_IC_I2C_check(); //ddi i2c 확인용
    break;
    
  case eVCOM:
/* 전체 I2c2 Line 통신 체크 */
//    set_vcom_data(0x1A5); //강제 0x1A5 write
    get_vcom_data(); //eeprom에서 load한 vcom 값 확인용
    break;

/* 윙보드 eep 테스트 */    
  case eINTER_EEP:
    internal_eeprom_test(); //내부 eeprom 확인용
    break;
    
/* 윙보드 출력 파워 테스트 */    
  case ePOWER_TEST:
    /* 내부 파워 */    
    GPO_WritePin(ENOTP, HIGH);
    GPO_WritePin(TP_VDD_EN, LOW); //low active
    GPO_WritePin(ENVSP, HIGH);
    GPO_WritePin(ENVSN, HIGH);
    break;
    
/* LCD EEP SPI 테스트 */    
  case eLCD_EEP_TEST:
//    e93_test();
    e93_eep_read();
    break;
    
  case HX8295_EEP_CTRL_BY_HOST_DEFAULT_VALUE_READ:
    hx8295_eep_ctrl_by_host_default_value_read_test();
    break;
    
  case HX82595_DEFAULT_TEST:
    hx8295_eep_fail_flag_check();
    break;
    
  case TOUCH_TEST:
    touch_test();
    break;
    
    
//  case eTEST_LED:
//    /* LED driver */
//    
//    TCA9546APWR_Channel_Sel(eI2C_LED);
//    HAL_Delay(5);
//    
//    LED_i2c_write(0x22<<1, 0x02, 0x02);
//    LED_i2c_write(0x22<<1, 0xFF, 0xFF);
//    
////    LED_i2c_write(0x22<<1, 0x02, 0x03);
////    LED_i2c_write(0x22<<1, 0x04, 0xA2);
//    
//    rdData[0] = LED_i2c_read(0x22<<1, 0x02); //slave address : 0x44[8bit]
//    rdData[1] = LED_i2c_read(0x22<<1, 0x04); //slave address : 0x44[8bit]
//    printf("LED Driver 0x02 reg read = %X\r\n", rdData[0]);
//    printf("LED Driver 0x04 reg read = %X\r\n", rdData[1]);
//    break;
//    
//  case eINTER_EEP:
////    internal_eeprom_test();
//    internal_eep_all_read(cnt++);
//    if (cnt >= 4) cnt = 0;
//    break;
//    
//  case eTOUCH:
//    TCA9546APWR_Channel_Sel(eI2C_TOUCH);
//    Initializtion_For_MP_Test(); 
//    touch_infor_check();  
//    break;
//    
//  case eASIL_FB:
//    asil_fb_check(NULL);
//    break;
//    
//  case eFLASH:
//    TCA9546APWR_Channel_Sel(eI2C_TOUCH);
//    rdData[5] = 3;
//    touch_fw_update(rdData);
//    break;
//    
//  case eFLASH_READ:
//    TCA9546APWR_Channel_Sel(eI2C_TOUCH);
//    nt51923tt_read(0x00000);
//    nt51923tt_read(0x00200);
//    nt51923tt_read(0x00400);
//    nt51923tt_read(0x3F000);
//    break;
//    
//  case eFAIL_DET:
//    TCA9546APWR_Channel_Sel(eI2C_DISPLAY);
//    asil_fail_detection_check(rdData);
//    break;
//    
//  case eALL_TEST:
//    all_test();
//    break;
//
//  case eVCOM:
//    pvcom_tune_debug();
//    break;
//    
//  case eLED_DET:
//    led_fail_detection_check(NULL);
//    break;
//
//  case eOTP_VCOM:
//    nvt_otp_data_write(0x0002);
//    break;
  }
  
}

void wing_board_test(void)
{
  g_wing_test_err=0;
  wing_board_i2c_check();
  internal_eeprom_test();
  
  uint8_t check_buff[5] = {0x95, 0x50};
  uint8_t r_buff[5] = {0};
  hx8295_eep_ctrl_by_host(); //eep host 
  e93_read_seq(0, &r_buff[0], 1);
  hx8295_eep_ctrl_by_ddi(); //eep ddi
  
  if (check_buff[0] != r_buff[0] || check_buff[1] != r_buff[1]) g_wing_test_err |= 0x08;
      
  printf("g_wing_test_err = 0x%02X\r\n", g_wing_test_err);
  g_wing_test_finish = 1;
}




