#ifndef __MYTEST_H__
#define __MYTEST_H__

//#define WING_BOARD_TEST //윙보드 하드웨어 테스트용

enum{
  eTEST_I2C_SLAVE, //0
  eDIC_I2C_CHECK,
  eVCOM,
  eTEST_LED, 
  eINTER_EEP,
  ePOWER_TEST,
  eLCD_EEP_TEST,
  HX8295_EEP_CTRL_BY_HOST_DEFAULT_VALUE_READ,
  HX82595_DEFAULT_TEST,
  TOUCH_TEST,
  
  
  eTOUCH,
  eASIL_FB,
  eFLASH,
  eFLASH_READ,
  eFAIL_DET,
  eALL_TEST,
  eLED_DET,
  eOTP_VCOM,
};

extern uint8_t gWrBuf[512];
extern uint8_t gRdBuf[512];
extern uint8_t g_wing_test_err;
extern uint8_t g_wing_test_finish;

extern void test_func(uint8_t test_no);
extern void wing_board_test(void);

#endif