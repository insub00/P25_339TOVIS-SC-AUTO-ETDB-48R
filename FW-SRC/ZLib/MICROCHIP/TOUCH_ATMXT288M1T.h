#ifndef	__ATMXT288M1T_H__
#define __ATMXT288M1T_H__


#define MXT288_DEV_ID   (0x4A<<1) //0x94[8bit]

#define PC_DATA_PLUS            1   // PC에서는 REQUEST_TX_COUNT에서 + 1로 계산
#define REQUEST_TX_LINE_COUNT   11   // ex) 56 x 5(REQUEST_TX_LINE_COUNT = 5줄) x 2(byte) = 1번에 560byte 씩 
#define REQUEST_TX_COUNT        1   // Total(12*11*2 = 264)를 264Byte 씩 총 1번 응답

#define OPTION_PRINT_MESSAGES

#ifndef OPTION_PRINT_MESSAGES
#define MXT_printf  null_printf
#define DBG_printf  null_printf
#else
#define MXT_printf  printf
#define DBG_printf  printf
#endif

#define delay_ms        HAL_Delay

enum status_code {
    STATUS_OK              = 0,  //!< Success
    STATUS_ERR_BUSY        = 1,  //!< Busy
    STATUS_ERR_DENIED      = 2,  //!< Denied
    STATUS_ERR_ALREADY_INITIALIZED = 3,  //!< Already initialized
    STATUS_ERR_NOT_INITIALIZED     = 4,  //!< Not initialized
    STATUS_ERR_INVALID_ARG = 5,  //!< Invalid argument
    STATUS_ERR_BAD_ADDRESS = 6,  //!< Bad address
    STATUS_ERR_TIMEOUT     = 7,  //!< Operation timed out
    STATUS_ERR_OVERFLOW    = 8,  //!< Overflow
    STATUS_ERR_UNDERFLOW   = 9,  //!< Underflow
    STATUS_ERR_IO          = 10, //!< I/O error
    STATUS_ABORTED         = 11,  //!< Operation aborted
    STATUS_ERR_NO_MEMORY   = 12,
    // … (ASF 버전에 따라 항목 추가 가능)
};

//#define FIRM_UP_ENABLE

#define RETRY_COUNT		3u

//#define MXT_SPI
#define MXT_MAX_BUFF_SIZE		64u


/*! Address where object table starts at touch IC memory map. */
#define ID_INFO_ELEMENT_SIZE			0x07
#define OBJECT_TABLE_START_ADDRESS		0x07
#define OBJECT_TABLE_ELEMENT_SIZE       0x06


/* Define for MXT_GEN_COMMAND_T6 */
#define MXT_BOOT_VALUE		0xA5
#define MXT_RESET_VALUE		0x01
#define MXT_BACKUP_VALUE	0x55

/* MXT_GEN_COMMAND_T6 field */
#define MXT_COMMAND_RESET		0
#define MXT_COMMAND_BACKUPNV	1
#define MXT_COMMAND_CALIBRATE	2
#define MXT_COMMAND_REPORTALL	3
#define MXT_COMMAND_DIAGNOSTIC	5

/*! Offset to RESET register from the beginning of command processor. */
#define RESET_OFFSET        0u

/*! Offset to BACKUP register from the beginning of command processor. */
#define BACKUP_OFFSET       1u

/*! Offset to CALIBRATE register from the beginning of command processor. */
#define CALIBRATE_OFFSET    2u

/*! Offset to REPORTALL register from the beginning of command processor. */
#define REPORTATLL_OFFSET   3u

/*! Offset to DEBUG_CTRL register from the beginning of command processor. */
#define DEBUG_CTRL_OFFSET   4u

/*! Offset to DIAGNOSTIC_CTRL register from the beginning of command processor. */
#define DIAGNOSTIC_OFFSET   5u


/* Touchscreen configuration infomation */
#define MXT_FW_MAGIC		0x4D3C2B1A

/* Command to unlock bootloader */
#define MXT_UNLOCK_CMD_MSB	0xAA
#define MXT_UNLOCK_CMD_LSB	0xDC

/* Bootloader mode status */
#define MXT_WAITING_BOOTLOAD_CMD		0xC0	/* valid 7 6 bit only */
#define MXT_WAITING_FRAME_DATA			0x80	/* valid 7 6 bit only */
#define MXT_FRAME_CRC_CHECK				0x02
#define MXT_FRAME_CRC_FAIL				0x03
#define MXT_FRAME_CRC_PASS				0x04
#define MXT_APP_CRC_FAIL				0x40	/* valid 7 8 bit only */
#define MXT_BOOT_STATUS_MASK			0x3F
#define MXT_BOOT_EXTENDED_ID			0x20
#define MXT_BOOT_ID_MASK				0x1F

/* Define for T6 status byte */
#define MXT_T6_STATUS_RESET			0x80
#define MXT_T6_STATUS_OFL			0x40
#define MXT_T6_STATUS_SIGERR		0x20
#define MXT_T6_STATUS_CAL	    	0x10
#define MXT_T6_STATUS_CFGERR		0x08
#define MXT_T6_STATUS_COMSERR		0x04

/* MXT_TOUCH_MULTI_T9 status */
#define MXT_T9_UNGRIP			(1<<0)
#define MXT_T9_SUPPRESS			(1<<1)
#define MXT_T9_AMP				(1<<2)
#define MXT_T9_VECTOR			(1<<3)
#define MXT_T9_MOVE				(1<<4)
#define MXT_T9_RELEASE			(1<<5)
#define MXT_T9_PRESS			(1<<6)
#define MXT_T9_DETECT			(1<<7)

#define MXT_TOUCH_XRANGE_LSB		18
#define MXT_TOUCH_XRANGE_MSB		19
#define MXT_TOUCH_YRANGE_LSB		20
#define MXT_TOUCH_YRANGE_MSB		21


/* Message type of T100 object */

#define MXT_T100_SCREEN_MSG_FIRST_RPT_ID 			0
#define MXT_T100_SCREEN_MSG_SECOND_RPT_ID 			1
#define MXT_T100_SCREEN_MESSAGE_NUM_RPT_ID 			2

//#define MXT_T100_TCHAUX_VECT		0x01
//#define MXT_T100_TCHAUX_AMPL		0x02
//#define MXT_T100_TCHAUX_AREA		0x04
#define MXT_T100_TCHAUX_HW		0x08
#define MXT_T100_TCHAUX_PEAK 		0x10
#define MXT_T100_TCHAUX_AREAHW  	0x20


#define MXT_T100_SCRAUX_NUMRPTTCH	0x01
#define MXT_T100_SCRAUX_TCHAREA		0x02
#define MXT_T100_SCRAUX_ATCHAREA		0x04
#define MXT_T100_SCRAUX_INTHRAREA		0x08

#define MXT_T100_XSIZE      	9
#define MXT_T100_YSIZE	     	20
#define MXT_T100_SCRAUX      	2
#define MXT_T100_TCHAUX	     	3

#define MXT_T100_CTRL		0
#define MXT_T100_CFG1		1
#define MXT_T100_TCHAUX		3
#define MXT_T100_XRANGE		13
#define MXT_T100_YRANGE		24
#define MXT_T100_CFG_SWITCHXY	(1 << 5)
#define MXT_T100_TCHAUX_VECT	(1 << 0)
#define MXT_T100_TCHAUX_AMPL	(1 << 1)
#define MXT_T100_TCHAUX_AREA	(1 << 2)
#define MXT_T100_DETECT		(1 << 7)
#define MXT_T100_TYPE_MASK	0x70
enum t100_type  {
	MXT_T100_TYPE_FINGER		= 1,	
	MXT_T100_TYPE_PASSIVE_STYLUS	= 2,	
	MXT_T100_TYPE_ACTIVE_STYLUS	= 3,	
	MXT_T100_TYPE_HOVERING_FINGER	= 4,	
	MXT_T100_TYPE_GLOVE		= 5,	
	MXT_T100_TYPE_LARGE_TOUCH	= 6,
};


enum {
	MXT_RESERVED_T0 = 0,
	MXT_RESERVED_T1,
	MXT_DEBUG_DELTAS_T2,
	MXT_DEBUG_REFERENCES_T3,
	MXT_DEBUG_SIGNALS_T4,
	MXT_GEN_MESSAGEPROCESSOR_T5,
	MXT_GEN_COMMANDPROCESSOR_T6,
	MXT_GEN_POWERCONFIG_T7,
	MXT_GEN_ACQUISITIONCONFIG_T8,
	MXT_TOUCH_MULTITOUCHSCREEN_T9,
	MXT_TOUCH_SINGLETOUCHSCREEN_T10,
	MXT_TOUCH_XSLIDER_T11,
	MXT_TOUCH_YSLIDER_T12,
	MXT_TOUCH_XWHEEL_T13,
	MXT_TOUCH_YWHEEL_T14,
	MXT_TOUCH_KEYARRAY_T15,
	MXT_PROCG_SIGNALFILTER_T16,
	MXT_PROCI_LINEARIZATIONTABLE_T17,
	MXT_SPT_COMCONFIG_T18,
	MXT_SPT_GPIOPWM_T19,
	MXT_PROCI_GRIPFACESUPPRESSION_T20,
	MXT_RESERVED_T21,
	MXT_PROCG_NOISESUPPRESSION_T22,
	MXT_TOUCH_PROXIMITY_T23,
	MXT_PROCI_ONETOUCHGESTUREPROCESSOR_T24,
	MXT_SPT_SELFTEST_T25,
	MXT_DEBUG_CTERANGE_T26,
	MXT_PROCI_TWOTOUCHGESTUREPROCESSOR_T27,
	MXT_SPT_CTECONFIG_T28,
	MXT_SPT_GPI_T29,
	MXT_SPT_GATE_T30,
	MXT_TOUCH_KEYSET_T31,
	MXT_TOUCH_XSLIDERSET_T32,
	MXT_RESERVED_T33,
	MXT_GEN_MESSAGEBLOCK_T34,
	MXT_SPT_GENERICDATA_T35,
	MXT_RESERVED_T36,
	MXT_DEBUG_DIAGNOSTIC_T37,
	MXT_SPT_USERDATA_T38,
	MXT_SPARE_T39,
	MXT_PROCI_GRIPSUPPRESSION_T40,
	MXT_SPARE_T41,
	MXT_PROCI_TOUCHSUPPRESSION_T42,
	MXT_SPT_DIGITIZER_T43,
	MXT_SPARE_T44,
	MXT_SPARE_T45,
	MXT_SPT_CTECONFIG_T46,
	MXT_PROCI_STYLUS_T47,
	MXT_PROCG_NOISESUPPRESSION_T48,
	MXT_SPARE_T49,
	MXT_SPARE_T50,
	MXT_SPARE_T51,
	MXT_TOUCH_PROXIMITY_KEY_T52,
	MXT_GEN_DATASOURCE_T53,
	MXT_SPARE_T54,
	MXT_ADAPTIVE_T55,
	MXT_PROCI_SHIELDLESS_T56,
	MXT_PROCI_EXTRATOUCHSCREENDATA_T57,
	MXT_SPARE_T58,
	MXT_SPARE_T59,
	MXT_SPARE_T60,
	MXT_SPT_TIMER_T61,
	MXT_PROCG_NOISESUPPRESSION_T62,
	MXT_PROCI_ACTIVESTYLUS_T63,
	MXT_SPARE_T64,
	MXT_PROCI_LENSBENDING_T65,
	MXT_SPT_GOLDENREFERENCES_T66,
	MXT_SPARE_T67,
	MXT_SPARE_T68,
	MXT_PROCI_PALMGESTUREPROCESSOR_T69,
	MXT_SPT_DYNAMICCONFIGURATIONCONTROLLER_T70,
	MXT_SPT_DYNAMICCONFIGURATIONCONTAINER_T71,
	MXT_PROCG_NOISESUPPRESSION_T72,
	MXT_SPT_CTESCANCONFIG_T77	= 77,
	MXT_PROCI_RETRANSMISSIONCOMPENSATION_T80 = 80,  /* 0320 */
	MXT_TOUCH_MULTITOUCHSCREEN_T100 = 100,
	MXT_SPT_TOUCHSCREENHOVER_T101,
	MXT_SPT_SELFCAPHOVERCTECONFIG_T102,
	MXT_SPT_SELFCAPCONFIG_T111 = 111,  /* 0318 */
	MXT_SPT_DATACONTAINER_T117 = 117,
	MXT_SPT_DATACONTAINERCTRL_T118 = 118,
	
	MXT_RESERVED_T255 = 255,
};

/* MXT_GEN_COMMAND_T6 Field */
#define MXT_COMMAND_RESET		0
#define MXT_COMMAND_BACKUPNV	1
#define MXT_COMMAND_CALIBRATE	2
#define MXT_COMMAND_REPORTALL	3
#define MXT_COMMAND_DEBUGCTL	4
#define MXT_COMMAND_DIAGNOSTIC	5


/* Delay times */
#define MXT_BACKUP_TIME		50		/* msec */
#define MXT_RESET_TIME		200		/* msec */
#define MXT_RESET_TIMEOUT	3000	/* msec */
#define MXT_CRC_TIMEOUT		1000	/* msec */
#define MXT_FW_RESET_TIME	3000	/* msec */
#define MXT_FW_CHG_TIMEOUT	300		/* msec */
#define MXT_WAKEUP_TIME		25		/* msec */
#define MXT_REGULATOR_DELAY	150		/* msec */
#define MXT_CHG_DELAY	    100		/* msec */
#define MXT_POWERON_DELAY	150		/* msec */
#define MXT_BOOTLOADER_WAIT	36E5	/* 1 minute */


/*! \brief Info ID struct. */
struct mxt_info
{
	uint8_t family_id;            /* address 0 */
	uint8_t variant_id;           /* address 1 */

	uint8_t version;              /* address 2 */
	uint8_t build;                /* address 3 */

	uint8_t matrix_x_size;        /* address 4 */
	uint8_t matrix_y_size;        /* address 5 */

	/*! Number of entries in the object table. The actual number of objects
	* can be different if any object has more than one instance. */
	uint8_t num_declared_objects; /* address 6 */
};

/*! \brief Object table element struct. */
__packed struct mxt_object
{
	uint8_t object_type;     /*!< Object type ID. */
	uint16_t i2c_address;    /*!< Start address of the obj config structure. */
	uint8_t size_minus_one;            /*!< Byte length of the obj config structure -1.*/
	uint8_t instances_minus_one;       /*!< Number of objects of this obj. type -1. */
	uint8_t num_report_ids;  /*!< The max number of touches in a screen,
							 *   max number of sliders in a slider array, etc.*/
};

struct report_id_map
{
	uint8_t object_type;     /*!< Object type. */
	uint8_t instance;        /*!< Instance number. */
};


/**
* struct mxt_fw_image - Represents a firmware file.
* @ magic_code: Identifier of file type.
* @ hdr_len: Size of file header (struct mxt_fw_image).
* @ cfg_len: Size of configuration data.
* @ fw_len: Size of firmware data.
* @ cfg_crc: CRC of configuration settings.
* @ fw_ver: Version of firmware.
* @ build_ver: Build version of firmware.
* @ data: Configuration data followed by firmware image.
*/

struct mxt_fw_image{
	uint32_t magic_code;
	uint32_t hdr_len;
	uint32_t cfg_len;
	uint32_t fw_len;
	uint32_t cfg_crc;
	uint8_t fw_ver;
	uint8_t build_ver;
	uint8_t data[0];
};


/**
* struct mxt_cfg_data - Represents a configuration data item.
* @ type: T
nce: Instance number of object.
* @ size: Size of object.
* @ register_val: Series of register values for object.
*/
struct mxt_cfg_data{
	uint8_t type;
	uint8_t instance;
	uint8_t size;
	uint8_t register_val[0];
};

/* Write Operation and Responses */
#define SPI_WRITE_REQ 	0x01 	//Write operation request
#define SPI_WRITE_OK 	0x81 	//Write operation succeeded (response)
#define SPI_WRITE_FAIL 	0x41 	//Write operation failed (response)

/* Read Operation and Responses */
#define SPI_READ_REQ 	0x02 	//Read operation request
#define SPI_READ_OK 	0x82 	//Read operation succeeded (response)
#define SPI_READ_FAIL 	0x42 	//Read operation failed (response)

/* General Responses */
#define SPI_INVALID_REQ 0x04 	//Invalid operation (response)
#define SPI_INVALID_CRC 0x08 	//Invalid CRC (response)

struct mxt_spi_header{
	uint8_t opcode;
	uint16_t address;
	uint16_t length;
	uint8_t crc8;
};


struct mxt_fw_info{
	uint8_t fw_ver;
	uint8_t build_ver;
	uint32_t hdr_len;
	uint32_t cfg_len;
	uint32_t fw_len;
	uint32_t cfg_crc;
	const uint8_t *cfg_raw_data;	/* start address of configuration data */
	const uint8_t *fw_raw_data;	/* start address of firmware data */
};

typedef struct 
{
	struct mxt_info info;
	struct mxt_object *object_table;
	struct report_id_map *rid_map;
	struct mxt_fw_info fw_info;
	struct mxt_fw_image *fw_img;
	struct mxt_cfg_data *cfg_data;

	uint8_t i2c_address;
	uint8_t i2c_boot_address;
	uint8_t max_report_id ;
	uint8_t max_message_length;
	uint8_t *message;
	uint16_t T5_address;
	uint16_t T6_address;
	uint32_t config_crc;
	uint32_t info_crc;
	uint8_t t6_status;
	bool in_bootloader;
	uint16_t max_x;
	uint16_t max_y;
	uint8_t scraux;
	uint8_t tchaux;
	uint8_t t100_aux_ampl;	
	uint8_t t100_aux_area;	
	uint8_t t100_aux_vect;
	uint8_t t100_max_id;
	uint8_t t100_min_id;
    uint16_t T117_address;
    uint16_t T118_address;
    
    uint8_t t100_screen_xsize;
    uint8_t t100_screen_ysize;
      
}mxt_st;

extern mxt_st mxt;

typedef enum 
{
  QT_PAGE_UP                 = 0x01,
  QT_PAGE_DOWN               = 0x02,
  
  QT_DELTA_MODE           = 0x10,
  QT_REFERENCE_MODE       = 0x11,
  
  QT_CTE_MODE         = 0x30,   // CTE Diagnostic Mode
  
  DC_DATA_MODE            = 0x38,
  DEVICE_INFO_MODE        = 0x80,
  
  SCT_SIGNAL_MODE         = 0xF5,
  SCT_DELTA_MODE          = 0xF7,
  SCT_REFERENCE_MODE      = 0xF8,
  
}diagnostic_debug_command;

#define mxt_bootloader_read	mxt_bootloader_read_i2c
#define mxt_bootloader_write	mxt_bootloader_write_i2c

//#define MAX_T117_BUFF_SIZE		5832
//#define MAX_T117_INSTACE_SIZE	        24
//#define MAX_T117_OBJ_SIZE		243
//#define MAX_NODE  (20*142)

#define MAX_T117_BUFF_SIZE		5832
#define MAX_T117_INSTACE_SIZE	        13
#define MAX_T117_OBJ_SIZE		200
#define MAX_NODE  (20*142)


__packed struct t37_diagnostic_data {
    uint8_t mode;
    uint8_t page;
    uint8_t data[128];   // or [256], 칩에 따라 다름
};


extern uint8_t mxt_bootloader_read_i2c(uint8_t *val, uint16_t count);
extern uint8_t mxt_bootloader_write_i2c(uint8_t *val, uint16_t count);


extern uint8_t  mxt_read_object(uint8_t type, uint16_t offset, uint8_t*val);
extern uint8_t  mxt_write_object( uint8_t type, uint16_t offset, uint8_t val);
extern struct mxt_object * mxt_get_object(uint8_t type);


extern uint8_t load_fw_file(struct mxt_fw_info *fw_info, const unsigned char *fw_file);
extern uint8_t get_T37_data(uint8_t mode, uint8_t tx_send_flag);
extern uint8_t mxt_driver_init(void);
extern void mxt_config_mem_clear(void);
extern void gen_commandprocessor_t6_process(uint8_t *t6_msg);
extern uint8_t mxt_read_t9_resolution(void);
extern void touch_multitouchscreen_t9_process(uint8_t *t9_msg);
extern uint8_t mxt_read_t100_config(void);


extern void touch_test();


enum touch_request_type{
    REQUEST_INFOR,
    REQUEST_DATA,
};
#define TOUCH_REQUEST_ADDR      0xA2

#define T_REF_DATA    0x02  //D
#define T_NN_RAW_DATA 0x03
#define T_SHORT_DATA  0x04
#define T_OPEN_DATA   0x05
#define T_NOISE_DATA  0x06  // = Jitter

extern void touch_request_packet(uint8_t type, uint8_t s_cmd, uint32_t len, uint8_t* data);
extern void touch_ver_check_packet(uint8_t *pInBuff);
extern void touch_inspection_packet(uint8_t *pInBuff);


#endif