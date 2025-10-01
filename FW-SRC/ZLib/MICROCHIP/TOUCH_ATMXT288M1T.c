#include "main.h"

/*******************************************************************************
Maxtouch I2c Address Table
*******************************************************************************/
uint8_t mxt_i2c_address_table[] =
{
  0x24,
  0x25,
  0x26,
  0x27,
  0x34,
  0x35,
  0x4A,
  0x4B,
  0x4C,
  0x4D,
  0x5A,
  0x5B,
};

struct t118_header_t {
    uint8_t crc;
    uint16_t timestamp;
    uint16_t cyccnt;
}t118_header;

mxt_st mxt;

uint8_t cfg_err_is_occurred;
uint8_t backup_is_on_going;
uint8_t reset_is_ongoing;
uint8_t debug_is_ongoing;

uint8_t uSyncEnable;
uint32_t prev_msg_time;
uint32_t curr_msg_time;

uint8_t previous;

//uint8_t t117_data[MAX_T117_INSTACE_SIZE * MAX_T117_OBJ_SIZE];
uint8_t dataready;

uint8_t touch_buff[1024] = {0};
uint32_t g_fw_raw_data_start_addr = 0;
uint32_t g_cfg_raw_data_start_addr = 0;
uint8_t *dbg_data;

uint8_t mxt_bootloader_write_i2c(uint8_t *val, uint16_t count)
{  
  HAL_StatusTypeDef Status = HAL_OK;
  
  TCA9546APWR_Channel_Sel(eI2C_TOUCH);
  
  Status = HAL_I2C_Master_Transmit(&hi2c2, mxt.i2c_boot_address<<1, val, count, 1000);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return Status;
}

uint8_t mxt_bootloader_read_i2c(uint8_t *val, uint16_t count)
{
  HAL_StatusTypeDef Status = HAL_OK;
  
  TCA9546APWR_Channel_Sel(eI2C_TOUCH);
  
  Status = HAL_I2C_Master_Receive(&hi2c2, (mxt.i2c_boot_address<<1) | 0x01, val, count, 1000);
  if(Status != HAL_OK)
  {
    printf("%s i2c readd Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  return Status;
}

uint8_t write_mem_i2c( uint16_t reg, uint16_t len, uint8_t *val )
{
  uint8_t *tmp;
//  uint16_t timeout;
  uint16_t buff_len;
  
  buff_len = 2 + len;
  tmp =(uint8_t *) malloc(buff_len);
  if (tmp == NULL)
  {
    return(-1);
  }
  
  TCA9546APWR_Channel_Sel(eI2C_TOUCH);
  
  
  /* Swap start address nibbles since MSB is first but touch IC wants
  * LSB first. */
  *tmp = (uint8_t) (reg & 0xFF);
  *(tmp + 1) = (uint8_t) (reg >> 8);
  
  memcpy((tmp + 2), val, len);
  
  /* Write buffer to slave until success. */
  HAL_StatusTypeDef Status = HAL_OK;
  Status = HAL_I2C_Master_Transmit(&hi2c2, mxt.i2c_address<<1, tmp, buff_len, 1000);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  free(tmp);
  return(Status);
}


uint8_t write_mem( uint16_t reg, uint16_t len, uint8_t *val )
{
  uint8_t status;
  
  if( len > MXT_MAX_BUFF_SIZE)
  {
    do {
#ifdef MXT_SPI
      status = write_mem_spi(reg, MXT_MAX_BUFF_SIZE, val);
#else
      status = write_mem_i2c(reg, MXT_MAX_BUFF_SIZE, val);
#endif
      
      if(status != STATUS_OK)
      {
        return(status);
      }
      
      val += MXT_MAX_BUFF_SIZE;
      reg += MXT_MAX_BUFF_SIZE;
      len -= MXT_MAX_BUFF_SIZE;
    }while(len > MXT_MAX_BUFF_SIZE);
  }
  
#ifdef MXT_SPI
  status = write_mem_spi(reg, MXT_MAX_BUFF_SIZE, val);
#else
  status = write_mem_i2c(reg, MXT_MAX_BUFF_SIZE, val);
#endif
  
  return(status);
}

uint8_t read_mem_i2c( uint16_t reg, uint16_t len, uint8_t *val )
{
  uint8_t tmp[2];
  
  TCA9546APWR_Channel_Sel(eI2C_TOUCH);
  
  /* Swap start address nibbles since MSB is first but touch IC wants
  * LSB first. */
  tmp[0] = (uint8_t) (reg & 0xFF);
  tmp[1] = (uint8_t) (reg >> 8);
  
  /* Write register address */
  HAL_StatusTypeDef Status = HAL_OK;
  Status = HAL_I2C_Master_Transmit(&hi2c2, mxt.i2c_address<<1, tmp, 2, 100);
  if(Status != HAL_OK)
  {
    printf("%s i2c write Error!!%d\r\n",__FUNCTION__,Status);  
  }
  
  /* Read buffer to slave until success. */
  Status = HAL_I2C_Master_Receive(&hi2c2, (mxt.i2c_address<<1) | 0x01, val, len, 1000);
  if(Status != HAL_OK)
  {
    printf("%s i2c readd Error!!%d\r\n",__FUNCTION__,Status);  
  }
  return (Status);
}


uint8_t read_mem( uint16_t reg, uint16_t len, uint8_t *val )
{
  uint8_t status;
#ifdef MXT_SPI
  status = read_mem_spi(reg, len, val);
#else
  status = read_mem_i2c(reg, len, val);
#endif
  
  return(status);	
}

uint8_t  mxt_read_object(uint8_t type, uint16_t offset, uint8_t*val)
{	
  struct mxt_object *object;	
  uint8_t status ;	
  object = mxt_get_object(type);	
  if (!object)		
    return -1;	
  
  status = read_mem(object->i2c_address + offset, 1, val);	
  if (status)	
  {
    DBG_printf( "Error to read T[%d] offset[%d] val[%d]\n", type, offset, *val);
    return status;
  }
  return status;
}

uint8_t  mxt_write_object( uint8_t type, uint16_t offset, uint8_t val) 
{	
  struct mxt_object *object;	
  uint8_t status ;
  
  object = mxt_get_object(type);	
  if (!object)		
    return -1;	
  
  if (offset >= ((object->size_minus_one +1) * (object->instances_minus_one +1)) )
  {		
    DBG_printf("Tried to write outside object T%d offset:%d, size:%d\n", type, offset, object->size_minus_one+1);
    return -1;	
  }	
  
  status = write_mem(object->i2c_address + offset, 1, &val);	
  if (status)	
  {	
    DBG_printf( "Error to write T[%d] offset[%d] val[%d]\n",	type, offset, val);	
    return status;
  }
  return status;
}

uint8_t crc8(unsigned char crc, unsigned char data)
{
  static const uint8_t crcpoly = 0x8C;
  uint8_t index;
  uint8_t fb;
  index = 8;
  do
  {
    fb = (crc ^ data) & 0x01;
    data >>= 1;
    crc >>= 1;
    if (fb)
      crc ^= crcpoly;
  } while (--index);
  return crc;
}

uint8_t calc_crc8(uint8_t *buff, uint16_t len)
{
  uint8_t crc = 0;
  uint16_t i;
  
  for(i = 0; i < len ; i++)
  {
    crc = crc8(crc,buff[i]);
  }
  return(crc);
}


uint16_t crc16(uint16_t crc, uint8_t data)
{
  static const uint16_t crcPoly = 0x8005;
  
  uint8_t n;
  uint32_t m;
  
  m = ((uint32_t)crc << 8) | data;
  for (n = 0; n < 8; n++)
  {
    if ((m <<= 1) & 0x1000000)
    {
      m ^= ((uint32_t)crcPoly << 8);
    }
  }
  return (uint16_t)(m >> 8);
}

uint32_t crc24(uint32_t crc, uint8_t firstbyte, uint8_t secondbyte)
{
  static const uint32_t crcpoly = 0x80001B;
  uint32_t result;
  uint16_t data_word;
  data_word = (uint16_t)firstbyte | (uint16_t)((uint16_t)secondbyte << 8u);
  result = (uint32_t)data_word ^ (uint32_t)(crc << 1u);
  if(result & 0x1000000) // If bit 25 is set
  {
    result ^= crcpoly;// XOR result with crcpoly
  }
  /* Mask to 24-bit */
  result &= 0x00FFFFFF;
  return result;
}

uint8_t maxtouch_check_address(void)
{
  uint16_t timeout;
  //  uint8_t tmp[6];
  uint8_t i;
  
  //  rd.data = tmp;
  //  rd.data_length = 6;
  //  rd.hs_master_code = false;
  //  rd.ten_bit_address = false;
  //  rd.hs_master_code = 0;
  
  TCA9546APWR_Channel_Sel(eI2C_TOUCH); //i2c switch
  
  /* Find maxtouch I2C Slave Address */
  for (i = 0; i < sizeof(mxt_i2c_address_table); i++)
  {
    //    memset(tmp,0xFF,3);
    //    rd.address = mxt_i2c_address_table[i];
    
    for(timeout = 0; timeout < RETRY_COUNT; timeout++)
    {
      if (HAL_I2C_IsDeviceReady(&hi2c2, mxt_i2c_address_table[i] << 1, 1, 100) == HAL_OK) 
      {
        mxt.i2c_address = mxt_i2c_address_table[i];
        printf("Found i2c address = 0x%02X\r\n",mxt.i2c_address<<1);
        return HAL_OK;
      }
    }
  }
  return HAL_ERROR;
}

uint8_t mxt_check_bootloader(void)
{
  uint8_t status;
  uint8_t buf[3];
  uint8_t boot_status;
  
  status = mxt_bootloader_read(buf, sizeof(buf));
  if(status != STATUS_OK)
  {
    DBG_printf("Bootloader read status failed\r\n");
    return(status);
  }
  
  boot_status = buf[0];
  
  if((boot_status & MXT_WAITING_BOOTLOAD_CMD) == MXT_APP_CRC_FAIL)
  {
    DBG_printf("APP_CRC_FAIL occurred\r\n");
  }
  
  if(boot_status & MXT_BOOT_EXTENDED_ID)
  {
    DBG_printf("Bootloader ID = %02x\r\n",buf[1]);
    DBG_printf("Bootloader Version = %02x\r\n",buf[2]);
  }
  else
  {
    DBG_printf("Bootloader ID = %02x\r\n",buf[0]&MXT_BOOT_ID_MASK);
  }
  
  return(status);
}

uint16_t get_object_address(uint8_t object_type, uint8_t instance)
{
  uint8_t object_table_index = 0;
  uint8_t address_found = 0;
  uint16_t address = NULL;
  
  struct mxt_object *object_table;
  struct mxt_object obj;
  
  object_table = mxt.object_table;
  
  while ((object_table_index < mxt.info.num_declared_objects) && (address_found == 0))
  {
    obj = object_table[object_table_index];
    /* Does object type match? */
    if (obj.object_type == object_type)
    {
      
      address_found = 1;
      
      /* Are there enough instances defined in the FW? */
      if (obj.instances_minus_one >= instance)
      {
        address = obj.i2c_address + ((obj.size_minus_one + 1) * instance);
      }
    }
    object_table_index++;
  }
  
  return(address);
}

uint8_t get_object_size(uint8_t object_type)
{
  uint8_t object_table_index = 0;
  uint8_t object_found = 0;
  uint16_t size = NULL;
  
  struct mxt_object *object_table;
  struct mxt_object obj;
  
  object_table = mxt.object_table;
  while ((object_table_index < mxt.info.num_declared_objects) && (object_found == 0))
  {
    obj = object_table[object_table_index];
    /* Does object type match? */
    if (obj.object_type == object_type)
    {
      object_found = 1;
      size = obj.size_minus_one + 1;
    }
    object_table_index++;
  }
  return(size);
}

uint8_t get_object_instance(uint8_t object_type)
{
  uint8_t object_table_index = 0;
  uint8_t object_found = 0;
  uint8_t instance = 0;
  
  struct mxt_object *object_table;
  struct mxt_object obj;
  object_table = mxt.object_table;
  while ((object_table_index < mxt.info.num_declared_objects) && (object_found == 0))
  {
    obj = object_table[object_table_index];
    /* Does object type match? */
    if (obj.object_type == object_type)
    {
      object_found = 1;
      instance = obj.instances_minus_one + 1;
    }
    object_table_index++;
  }
  
  return(instance);
}

uint8_t mxt_read_message_reportid(uint8_t reportid)
{
  uint8_t status;
  uint16_t try;
  uint16_t fail_count;
  
  fail_count = mxt.max_report_id * 2;
  
  printf("%s : fail_count = %d\r\n", __func__, fail_count);
  
  for(try = 0; try < fail_count; try++)
  {
    status = read_mem(mxt.T5_address, mxt.max_message_length, mxt.message);
    
//    printf("\t mxt.T5_address = %d(0x%X), mxt.max_message_length = %d(0x%X)\r\n", 
//                        mxt.T5_address, mxt.T5_address,
//                        mxt.max_message_length, mxt.max_message_length);
//    for (int i =0; i<mxt.max_message_length; i++) printf("0x%02X ", mxt.message[i]);
//      printf("\r\n");
    
    if (status == STATUS_OK)
    {
      if (mxt.message[0] == reportid)
      {
        return(status);
      }
    }
    else
    {
      return(status);
    }
  }
  return (-1);
}

uint8_t type_to_rid(uint8_t object_type, uint8_t * rid, uint8_t instance)
{
  
  uint8_t report_id = 1;
  int8_t report_id_found = 0;
  
  struct report_id_map *report_id_map;
  report_id_map =  mxt.rid_map;
  
  while((report_id <= mxt.max_report_id) && (report_id_found == 0))
  {
    if((report_id_map[report_id].object_type == object_type) &&
       (report_id_map[report_id].instance == instance))
    {
      report_id_found = 1;
    }
    else
    {
      report_id++;
    }
  }
  if (report_id_found)
  {
    * rid = report_id;
    return(STATUS_OK);
  }
  else
  {
    return(-1);
  }
}

uint8_t rid_to_type(uint8_t *object_type, uint8_t rid, uint8_t *instance)
{
  struct report_id_map *report_id_map;
  report_id_map =  mxt.rid_map;
  *object_type = 0xFF;
  
  if (rid <= mxt.max_report_id)
  {
    *object_type = report_id_map[rid].object_type;
    *instance = report_id_map[rid].instance;
    return(STATUS_OK);
  }
  else
  {
    return(-1);
  }
}

uint8_t mxt_read_config_crc(uint32_t *crc)
{
  uint8_t status;
  uint8_t t6_rid;
  uint16_t chg_dly;
  
  uint8_t data;
  
  
  data = 0x55;
  
  // disable the interrupt in order to read the message manually
  // __interrupt_disable();
  
  mxt_read_message_reportid(MXT_RESERVED_T255);
  status = type_to_rid(MXT_GEN_COMMANDPROCESSOR_T6, &t6_rid, 0);
  status = write_mem(mxt.T6_address + BACKUP_OFFSET, 1, &data);
  chg_dly = 0;
  backup_is_on_going = true;
  do{
    delay_ms(1);
    chg_dly++;
    
    if(GPI_ReadPin(TCH_INT) == GPIO_PIN_RESET) 
    {
      status = mxt_read_message_reportid(t6_rid);
      if(status == STATUS_OK)
      {
        *crc =  (uint32_t)mxt.message[2] | ((uint32_t)mxt.message[3] << 8) | ((uint32_t)mxt.message[4] << 16);
        return STATUS_OK;
      }
    }	
  }while((backup_is_on_going == true) && (chg_dly < 3000));
  
  
  
  return(status);
}

struct mxt_object * mxt_get_object(uint8_t type)
{
  struct mxt_object *object;
  int i;
  
  for (i = 0; i < mxt.info.num_declared_objects; i++)
  {
    object = mxt.object_table + i;
    if (object->object_type == type){
      MXT_printf("Valid object type T%u\n", type);
      return object;
    }
  }
  
  MXT_printf("Invalid object type T%u\n", type);
  return NULL;
}

void mxt_fw_info_data_set()
{
  mxt.fw_img->magic_code = 0x4D3C2B1A;
  mxt.fw_img->hdr_len = 0x16;
  mxt.fw_img->cfg_len = 0x102E;
  mxt.fw_img->fw_len = 0x2E61C;
  mxt.fw_img->cfg_crc = 0xA604CA;
  mxt.fw_img->fw_ver = 0x10;
  mxt.fw_img->build_ver = 0xAA;
//  mxt.fw_img->data[0]; //0x44;
  
  
  mxt.fw_info.fw_ver =   mxt.fw_img->fw_ver;
  mxt.fw_info.build_ver =   mxt.fw_img->build_ver;
  mxt.fw_info.hdr_len =   mxt.fw_img->hdr_len;
  mxt.fw_info.cfg_len =   mxt.fw_img->cfg_len;
  mxt.fw_info.fw_len =   mxt.fw_img->fw_len;
  mxt.fw_info.cfg_crc =   mxt.fw_img->cfg_crc;
}

uint8_t write_configuration(void)
{
  
  //uint32_t config_crc_r;
  //static id_info_t cfg_info_r;
  uint8_t i;
  uint16_t object_address;
  uint8_t object_size;
  uint8_t instance;
  uint8_t status;
  uint8_t zero_val = 0;
//  uint8_t *cfg_ptr;
//  uint16_t reg;
  uint32_t dev_cfg_crc1,dev_cfg_crc2;
  uint8_t err_cnt;
  
//  struct mxt_object *object;
  struct mxt_cfg_data *cfg_data;
  
  uint16_t index;
  
//  mxt_fw_info_data_set(); // *중요 : 변수들 강제 적용, 추후 enc파일 참조해서 변수 설정해라.
  load_fw_file(&mxt.fw_info, NULL);
  
  if(mxt.fw_img->cfg_len == 0) {
    DBG_printf("No cfg data in file\r\n");
    return (!STATUS_OK);
  }
  
  /* Check Firmware Version information */
  if (mxt.fw_info.fw_ver != mxt.info.version) {
    DBG_printf("Warning: version mismatch!\r\n");
    DBG_printf("fw file ver[%d], Dev ver[%d]\r\n",mxt.fw_info.fw_ver, mxt.info.version);
//    return (!STATUS_OK);
  }
  
  /* Check Firmware Build information */
  if (mxt.fw_info.build_ver != mxt.info.build) {
    DBG_printf("Warning: Build mismatch!\r\n");
    DBG_printf("fw file Build[%d], Dev Build[%d]\r\n",mxt.fw_info.build_ver, mxt.info.build);
//    return (!STATUS_OK);
  }
  
  if(mxt.fw_info.cfg_crc == 0x00)
  {
    DBG_printf("Configuration file corrupted\n");
    return (!STATUS_OK);
  }
  
  err_cnt = 0;
  mxt.config_crc = 0x00;
  do{
    dev_cfg_crc1 = 0;
    dev_cfg_crc2 = 0;
    status = mxt_read_config_crc(&dev_cfg_crc1);
    if(status == STATUS_OK)
    {
      status = mxt_read_config_crc(&dev_cfg_crc2);
      {
        if(status == STATUS_OK)
        {
          
          if(dev_cfg_crc2 == dev_cfg_crc1)
          {
            mxt.config_crc = dev_cfg_crc1;
            break;
          }
        }
      }
    }
    err_cnt++;
  }while(err_cnt < 10);
  
  if(err_cnt == 10)
  {
    DBG_printf("Configuration crc read failed\r\n");
    return (!STATUS_OK);
  }
  
  DBG_printf("Read config CRC = 0x%06X\r\n", mxt.config_crc);
  DBG_printf("Target config CRC = 0x%06X\r\n", mxt.fw_info.cfg_crc);
  
  
  if((mxt.config_crc  == mxt.fw_info.cfg_crc) && (mxt.config_crc != 0x00))
  {
    DBG_printf("Config is same as file, skip write config\r\n");
    return (STATUS_OK);
  }
  
  DBG_printf("Config update\r\n");
  for (index= 0; index < mxt.fw_info.cfg_len;)
  {
    
      if (index + sizeof(struct mxt_cfg_data) >= mxt.fw_info.cfg_len)
      {
        DBG_printf("index(%d) of cfg_data exceeded total size(%d)!!\r\n",
                   (index + sizeof(struct mxt_cfg_data)), mxt.fw_info.cfg_len);
        return (!STATUS_OK);
      }
      
      /* Get the info about each object */
//      cfg_data = (struct mxt_cfg_data *)(&mxt.fw_info.cfg_raw_data[index]);  //예제 코드 내용.
      memset(touch_buff, 0, sizeof(touch_buff));
      M95Pxx_ReadData(g_cfg_raw_data_start_addr + index, &touch_buff[0], 1024);
      cfg_data = (struct mxt_cfg_data *)touch_buff;
      
      
      
      index += sizeof(struct mxt_cfg_data) + cfg_data->size;
      
      DBG_printf("cfg_data->type = %d, cfg_data->instance = %d, cfg_data->size = %d\r\n", cfg_data->type, cfg_data->instance, cfg_data->size);
      
      if (index > mxt.fw_info.cfg_len) {
        DBG_printf("index(%d) of cfg_data exceeded total size(%d) in T%d object!!\r\n",
                   index, mxt.fw_info.cfg_len, cfg_data->type);
        return (!STATUS_OK);
      }
      
      
      object_address = get_object_address(cfg_data->type, cfg_data->instance);
      object_size = get_object_size(cfg_data->type);
      instance = get_object_instance(cfg_data->type);
      
      if (object_address == 0)
      {
        DBG_printf("T%d is Invalid object type\r\n", cfg_data->type);
        continue;
      }
      
      /* Check and compare the size, instance of each object */
      if (cfg_data->size > object_size)
      {
        DBG_printf("T%d Object length exceeded!\r\n", cfg_data->type);
        
        cfg_data->size = object_size;
      }
      
      
      if (cfg_data->instance >= instance)
      {
        DBG_printf("T%d Object instances exceeded!\r\n",cfg_data->type);
        continue;
      }
      
      //DBG_printf("Writing config for obj %d len %d instance %d (%d/%d)\n", cfg_data->type, cfg_data->size,
      //cfg_data->instance, index, mxt.fw_info.cfg_len);
      
      
      status = write_mem(object_address, cfg_data->size, cfg_data->register_val);
      if (status != STATUS_OK)
      {
        DBG_printf("Write mem failed %s\n",__FUNCTION__);
        return (!STATUS_OK);
      }
      
      /*
      * If firmware is upgraded, new bytes may be added to end of
      * objects. It is generally forward compatible to zero these
      * bytes - previous behaviour will be retained. However
      * this does invalidate the CRC and will force a config
      * download every time until the configuration is updated.
      */
      if (cfg_data->size < object_size)
      {
        DBG_printf("Warning: zeroing %d byte(s) in T%d\n",
                   object_size - cfg_data->size, cfg_data->type);
        
        for (i = cfg_data->size + 1; i < object_size; i++)
        {
          status = write_mem(object_address + i, 1, &zero_val);
          if (status != STATUS_OK)
          {
            return (STATUS_ABORTED);
          }
        }
      }
  }
  return (STATUS_OK);
}

void mxt_config_mem_clear(void)
{
	uint16_t object_address;
	uint8_t tmp = 0;
	uint8_t i,j,k;
	uint8_t object_size;
	uint8_t instance;

	MXT_printf("%s \n",__FUNCTION__);
	for(i = 7; i < 255; i++)
	{
		/* Get object address of instance 0 */
		object_address = get_object_address(i, 0);
		if(object_address != 0)
		{
			object_size = get_object_size(i);
			instance = get_object_instance(i);
			
			for(j = 0; j < instance ; j++)
			{
				for(k = 0; k < object_size; k++)
				{
					write_mem(object_address+k, 1,&tmp);
				}
				object_address += object_size;
			}
		}
	}
}


void gen_commandprocessor_t6_process(uint8_t *t6_msg)
{
  uint8_t status;
  uint32_t crc;
  
  status = t6_msg[1];
  
  crc = t6_msg[2] | (t6_msg[3] << 8) | (t6_msg[4] << 16);
  /* Output debug if status has changed */
  if (status != mxt.t6_status)
    MXT_printf("T6 Status=0x%02X %s%s%s%s%s%s%s\n",
               status,
               status == 0 ? " OK" : "",
               status & MXT_T6_STATUS_RESET ? " RESET" : "",
               status & MXT_T6_STATUS_OFL ? " OFL" : "",
               status & MXT_T6_STATUS_SIGERR ? " SIGERR" : "",
               status & MXT_T6_STATUS_CAL ? " CAL" : "",
               status & MXT_T6_STATUS_CFGERR ? " CFGERR" : "",
               status & MXT_T6_STATUS_COMSERR ? " COMSERR" : "");
  
  if(status == 0x00) 
  {
    MXT_printf("T6 Config Checksum: 0x%06X\n", crc);
    cfg_err_is_occurred = false;
    backup_is_on_going = false;
    reset_is_ongoing = false;
//    port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
  }
  
  
  if(status & MXT_T6_STATUS_CFGERR)
  {
    
    MXT_printf("CFGERR occurred !!! \n");
    if(cfg_err_is_occurred == false)
    {
      
      if(cfg_err_is_occurred == false) {
        MXT_printf("Device config CRC =  0x%06X\n", crc);
        MXT_printf("Config file CRC =  0x%06X\n", mxt.fw_info.cfg_crc);
        cfg_err_is_occurred = true;
      }
    }		
    return;
    //mxt_config_mem_clear();
    //write_configuration();
    
  }
  /* Save current status */
  mxt.t6_status = status;
  
}



uint8_t mxt_read_t9_resolution(void)
{
  struct mxt_object *object;
  uint8_t buf[4];
  uint8_t status;
  
  object = mxt_get_object(MXT_TOUCH_MULTITOUCHSCREEN_T9);
  if (!object)
    return -1;
  
  status = read_mem(object->i2c_address + MXT_TOUCH_XRANGE_LSB,
                    4, buf);
  if(status != STATUS_OK)
  {
    return status;
  }		   
  
  mxt.max_x = buf[0] + (buf[1] << 8);		   
  mxt.max_y = buf[2] + (buf[3] << 8);
  
  return STATUS_OK;
}

void touch_multitouchscreen_t9_process(uint8_t *t9_msg)
{
  uint8_t id;
  uint8_t status;
  uint16_t xPos;
  uint16_t yPos;
  uint8_t tcharea;
  uint8_t tchamp;
  uint8_t tchvec;
  
  id = t9_msg[0];
  status = t9_msg[1];
  
  if(mxt.max_x == 0) mxt.max_x = 1023;
  if(mxt.max_y == 0) mxt.max_y = 1023;
  
  xPos = (t9_msg[2] << 4) | ((t9_msg[4] >> 4) & 0xf);
  yPos = (t9_msg[3] << 4) | ((t9_msg[4] & 0xf));
  /* Handle 10/12 bit switching */
  if (mxt.max_x < 1024)
    xPos >>= 2;
  if (mxt.max_y < 1024)
    yPos >>= 2;
  
  tcharea = t9_msg[5];
  tchamp = t9_msg[6];
  tchvec = t9_msg[7];
  
  DBG_printf("[%u] %c%c%c%c%c%c%c%c x: %5u y: %5u area: %3u amp: %3u vector: %02X\n",
             id,
             (status & MXT_T9_DETECT) ? 'D' : '.',
             (status & MXT_T9_PRESS) ? 'P' : '.',
             (status & MXT_T9_RELEASE) ? 'R' : '.',
             (status & MXT_T9_MOVE) ? 'M' : '.',
             (status & MXT_T9_VECTOR) ? 'V' : '.',
             (status & MXT_T9_AMP) ? 'A' : '.',
             (status & MXT_T9_SUPPRESS) ? 'S' : '.',
             (status & MXT_T9_UNGRIP) ? 'U' : '.',
             xPos, yPos, tcharea, tchamp, tchvec);
}


//#define MXT_T100_XSIZE      	9
//#define MXT_T100_YSIZE	     	20
uint8_t mxt_read_t100_config(void)
{
//  struct mxt_object *object;
  uint8_t status;
  uint8_t aux;
  
  
//  status = mxt_read_object(MXT_TOUCH_MULTITOUCHSCREEN_T100, MXT_T100_SCRAUX, &mxt.scraux);
  status = mxt_read_object(MXT_TOUCH_MULTITOUCHSCREEN_T100, MXT_T100_XSIZE, &mxt.t100_screen_xsize);
  if (status)
  {
    //DBG_printf("Screen aux info read failed\n");
    return status;
  };
  
//  status = mxt_read_object(MXT_TOUCH_MULTITOUCHSCREEN_T100, MXT_T100_TCHAUX, &mxt.tchaux);
  status = mxt_read_object(MXT_TOUCH_MULTITOUCHSCREEN_T100, MXT_T100_YSIZE, &mxt.t100_screen_ysize);
  if (status)
  {
    //DBG_printf("Screen aux info read failed\n");
    return status;
  }
  
  printf("%s : t100_screen_xsize = %d, t100_screen_ysize = %d\r\n", __func__, mxt.t100_screen_xsize, mxt.t100_screen_ysize);
  /* allocate aux bytes */	
//  aux = 6;	
//  if (mxt.tchaux & MXT_T100_TCHAUX_VECT)		mxt.t100_aux_vect = aux++;	
//  if (mxt.tchaux & MXT_T100_TCHAUX_AMPL)		mxt.t100_aux_ampl = aux++;	
//  if (mxt.tchaux & MXT_T100_TCHAUX_AREA)		mxt.t100_aux_area = aux++;
  
  return STATUS_OK;
}

/** Used to specify that touch-processing will not be blocked when the noise is excessive.
  * @see procg_noisesuppression_t72_config_t.cfg2
  */
#define T72_CONFIG_CFG2_NOBLKTCH          (0x01u)

/** Used to enable monitoring of the CHRG_IN pin as an indication of the presence of a noise-source.
  * @see procg_noisesuppression_t72_config_t.cfg2
  */
#define T72_CONFIG_CFG2_NOISEIN           (0x02u)

/** Used to enable monitoring of the CHRG_IN pin as an indication of the presence of a high noise-source.
  * @see procg_noisesuppression_t72_config_t.cfg2
  */
#define T72_CONFIG_CFG2_VNOISEIN          (0x04u)

struct t72_message {
	uint8_t id;
	uint8_t status1;
	uint8_t status2;
	uint8_t mintchthr;
	uint8_t pknoiselvl;
	uint8_t noiselvl;
	uint8_t nlthr;
};

#define DUALXCHG_MSK	0x40u
#define NLTHRCHG_MSK	0x20u 
#define NOISELVLCHG_MSK	0x10u 
#define STATECHG_MSK	0x08u 
#define ACQCHG_MSK		0x04u 
#define TCH_MSK			0x02u 
#define BLKTCH_MSK		0x01u
#define ACQINDEX_MASK   0xE0u
#define DUALX_MASK		0x08u
#define STATE_MASK		0x03u
	
void procg_noisesuppression_t72_process(struct t72_message *t72_msg)
{
	uint8_t freq_index;
	uint8_t dualx;
	uint8_t current_state;
	
	freq_index = (t72_msg->status2 & ACQINDEX_MASK) >> 5;
	dualx =  (t72_msg->status2 & DUALX_MASK) >> 3;
	current_state =  (t72_msg->status2 & STATE_MASK);

	DBG_printf("[%u] freq:%d dualx:%d state:%d  mintchthr:%d pknoiselvl:%d noiselvl:%d  nlthr:%d    \n",
		t72_msg->id, freq_index, dualx, current_state, t72_msg->mintchthr, t72_msg->pknoiselvl, t72_msg->noiselvl, t72_msg->nlthr);
}

void touch_multitouchscreen_t100_process(uint8_t *t100_msg)
{
	uint8_t id;
	uint8_t status;

	/* for first id report */
	uint8_t ScrStatus;
	uint8_t NumRptTch;
	uint8_t TchArea;
	uint8_t AtchArea;
	uint8_t IntTchArea;
	uint8_t scrauxindex;

	/* for thrid id report */
	uint8_t type;
	uint8_t event;
	uint16_t xPos;
	uint16_t yPos;

	uint8_t tchaux;
	uint8_t thcauxindex ;

	uint8_t aux_vect = 0;
	uint8_t aux_ampl = 0;
	uint8_t aux_area = 0;
	uint16_t peak_delta = 0;
	uint8_t height = 0;
	uint8_t width = 0;
	uint8_t hw_area = 0;
	uint8_t hw_exp = 0;
	uint8_t hw_height = 0;
	uint8_t hw_weight = 0;

	uint8_t major = 0;	
	uint8_t pressure = 0;	
	uint8_t orientation = 0;
	bool active = false;	
	bool hover = false;


	
	//Message Data for TOUCH_MULTITOUCHSCREEN_T100 == First Report ID
	//SCRSTATUS		[1]
	//NUMRPTTCH		[n]
	//TCHAREA		[n][n+1]
	//ATCHAREA		[n][n+1]
	//INTTHRAREA	[n][n+1]

	id = t100_msg[0] - mxt.t100_min_id;
		
	if (id < MXT_T100_SCREEN_MESSAGE_NUM_RPT_ID) 
	{
		if (id == MXT_T100_SCREEN_MSG_FIRST_RPT_ID)
		{
			/* TODO: Need to be implemeted after fixed protocol
			* This messages will indicate TCHAREA, ATCHAREA
			*/
			scrauxindex = 1;
			ScrStatus = 0;
			if(t100_msg[scrauxindex] & 0x80) {
				ScrStatus |= 0x80;  /* any touch detected */
			}
			if(t100_msg[scrauxindex] & 0x40) {
				ScrStatus |= 0x40;  /* full screen suppression occurred */
			}
			scrauxindex = 2;
			if(mxt.scraux & MXT_T100_SCRAUX_NUMRPTTCH) {
				NumRptTch = t100_msg[scrauxindex++];
			}
			if(mxt.scraux & MXT_T100_SCRAUX_TCHAREA) {
				TchArea = t100_msg[scrauxindex++];
				TchArea +=(t100_msg[scrauxindex++] << 8);
			}
			if(mxt.scraux & MXT_T100_SCRAUX_ATCHAREA) {
				AtchArea = t100_msg[scrauxindex++];
				AtchArea +=(t100_msg[scrauxindex++] << 8);
			}
			if(mxt.scraux & MXT_T100_SCRAUX_INTHRAREA) {
				IntTchArea = t100_msg[scrauxindex++];
				IntTchArea +=(t100_msg[scrauxindex++] << 8); 
			}
		}		
		//Message Data for TOUCH_MULTITOUCHSCREEN_T100 == Second Report ID
		//Reserved
		//DBG_printf("NumRptTch = %d,  TchArea:%d  AtchArea:%d IntTchArea:%d \n",	 NumRptTch, TchArea, AtchArea, IntTchArea);
		return;	
	}
	
	id -= MXT_T100_SCREEN_MESSAGE_NUM_RPT_ID;
	//Message Data for TOUCH_MULTITOUCHSCREEN_T100 ? Subsequent Touch Report IDs
	//	TCHSTATUS  [1]
	//	XPOS LSB   [2] 
	//	XPOS MSB   [3]
	//	YPOS LSB   [4]
	//	YPOs MSB   [5]
	//	VECT       		[ ]
	//	AMPL       	[ ]
	//	AREA       	[ ]
	//	HW[Height] 	[ ]
	//	HW[Width]  	[ ]
	//	PEAK		[ ]
	//	AREAHW[0]	[ ]
	//	AREAHW[1]	[ ]

	/* Treate touch status messages */
	
	status = t100_msg[1];
	xPos=     (t100_msg[3]<<8) + (t100_msg[2]);
	yPos=     (t100_msg[5]<<8) + (t100_msg[4]);

	type =   (t100_msg[1]&0x70)>>4;
	event =   t100_msg[1] & 0x0F;

	if (status & MXT_T100_DETECT)  //Detect
	{		
#if 1
		/* detail report for touch aux data */
		tchaux= mxt.tchaux;
		thcauxindex = 6;
		for(thcauxindex = 6; thcauxindex < mxt.max_message_length; thcauxindex++)
		{
			if(tchaux & MXT_T100_TCHAUX_VECT) {
				aux_vect = t100_msg[thcauxindex];
				tchaux &=  ~MXT_T100_TCHAUX_VECT;
				continue;
			}

			if(tchaux & MXT_T100_TCHAUX_AMPL) {
				aux_ampl = t100_msg[thcauxindex];
				tchaux &=  ~MXT_T100_TCHAUX_AMPL;
				continue;
			}

			if(tchaux & MXT_T100_TCHAUX_AREA) {
				aux_area = t100_msg[thcauxindex];
				tchaux &=  ~MXT_T100_TCHAUX_AREA;
				continue;
			}

			if(tchaux & MXT_T100_TCHAUX_HW) {
				if((thcauxindex + 1) < mxt.max_message_length) 
				{
					height = t100_msg[thcauxindex];
					width = t100_msg[thcauxindex+1];
					thcauxindex += 1;
				}
				tchaux &=  ~MXT_T100_TCHAUX_HW;
				continue;
			}

			if(tchaux & MXT_T100_TCHAUX_PEAK) {
				peak_delta = t100_msg[thcauxindex];
				peak_delta *=8;
				tchaux &=  ~MXT_T100_TCHAUX_PEAK;
				continue;
			}

			if(tchaux & MXT_T100_TCHAUX_AREAHW) {
				hw_exp = t100_msg[thcauxindex] & 0x60 ;
				hw_exp >>=5;
				hw_area = (t100_msg[thcauxindex]& 0x1F);
				hw_area <<= hw_exp;
				hw_height = t100_msg[thcauxindex+1]&0x0F;
				hw_weight = t100_msg[thcauxindex+1]&0xF0;
				hw_weight >>=4;
				tchaux &=  ~MXT_T100_TCHAUX_AREAHW;
			}
		}

#endif
		type = (status & MXT_T100_TYPE_MASK) >> 4;

		switch(type) {
			case MXT_T100_TYPE_HOVERING_FINGER:
				hover = true;
			case MXT_T100_TYPE_FINGER:		
			case MXT_T100_TYPE_GLOVE:
				active = true;
				if (mxt.t100_aux_area)	 major = t100_msg[mxt.t100_aux_area];			
				if (mxt.t100_aux_ampl)	  pressure = t100_msg[mxt.t100_aux_ampl];			
				if (mxt.t100_aux_vect)	  orientation = t100_msg[mxt.t100_aux_vect];
				break;
			case MXT_T100_TYPE_PASSIVE_STYLUS:
				active = true;
				if (mxt.t100_aux_ampl)	 pressure = t100_msg[mxt.t100_aux_ampl];
				break;
			case MXT_T100_TYPE_LARGE_TOUCH:			
				/* Ignore suppressed touch */			
				break;
			default:			
				DBG_printf("Unexpected T100 type \n");			
				return;
		}
		
		if (active)  
		{	
			DBG_printf("Touch ID = %d,  type:%d x:%u y:%u a:%d p:%d v:%02X \n",	 id, type, xPos, yPos, major, pressure, orientation);
				
			//DBG_printf("[%u] type:%u x:%u y:%u a:%d p:%d v:%02X \n",	 id, type, xPos, yPos, major, pressure, orientation);	
			//DBG_printf("[%u] height:%d width:%d peak_delta:%d hw_area:%d hw_height:%d hw_weight:%d \n",	id, height, width, peak_delta, hw_area, hw_height, hw_weight);
		} 
	}
	else 
	{		
		DBG_printf("[%u] release  \n", id); 
	}

}



void data_container_controller_t118_process(uint8_t *t108_msg)
{
    dataready = t108_msg[1];
}


void mxt_message_process_callback(void)
{
	uint8_t status;
	uint8_t object_type;
	uint8_t instance;
	uint8_t t5_msg[10];
	uint8_t i;

    if(debug_is_ongoing == true) return;

	do{
		status = read_mem(mxt.T5_address, mxt.max_message_length, t5_msg);
//                printf("\t %s : mxt.T5_address = %d, mxt.max_message_length = %d, t5_msg[0] = 0x%02X\r\n", __func__, mxt.T5_address, mxt.max_message_length, t5_msg[0]);
		if(status != STATUS_OK)
		{
			DBG_printf("T5 message read fail \n");
			return;
		}

	
		/* Read Object type */
		status = rid_to_type(&object_type, t5_msg[0] , &instance);

#if 1
		DBG_printf("T%d     ",object_type);

		for(i=0; i< mxt.max_message_length; i++)
		{
			DBG_printf("0x%02x ",t5_msg[i]);
		}
		DBG_printf("\n");


		//DBG_printf("Object = %d, msg[0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x] \n",
		//object_type,
		//t5_msg[0], t5_msg[1],
		//t5_msg[2], t5_msg[3],
		//t5_msg[4], t5_msg[5],
		//t5_msg[6], t5_msg[7],
		//t5_msg[8]);
#endif

		if(status == STATUS_OK)
		{
			switch(object_type)
			{
				case MXT_GEN_COMMANDPROCESSOR_T6:
				gen_commandprocessor_t6_process(&t5_msg[0]);
				break;
				case MXT_TOUCH_MULTITOUCHSCREEN_T9:
				touch_multitouchscreen_t9_process(&t5_msg[0]);
				break;
				case MXT_TOUCH_KEYARRAY_T15:
				//touch_keyarray_t15_process();
				break;
				case MXT_PROCG_NOISESUPPRESSION_T72:
					//procg_noisesuppression_t72_process(&t5_msg[0]);
					break;
				case MXT_PROCI_TOUCHSUPPRESSION_T42:
				//proci_touchsuppression_t42_process();
				break;
				case MXT_PROCG_NOISESUPPRESSION_T48:
				//procg_noisesuppression_t48_process();
				break;
				case MXT_SPT_GPIOPWM_T19:
				//spt_gpiopwm_t19_process();
				break;
				case MXT_TOUCH_MULTITOUCHSCREEN_T100:
					touch_multitouchscreen_t100_process(&t5_msg[0]);
					break;

				case MXT_SPT_DATACONTAINERCTRL_T118:
					data_container_controller_t118_process(&t5_msg[0]);
					break;
					
				default:
					//write_message_to_usart(tmsg,max_message_length);
					break;
			};		
		}
	}while(GPI_ReadPin(TCH_INT) == GPIO_PIN_RESET);
}

uint8_t backup_is_on_going;
uint8_t backup_config(void)
{
	uint32_t config_crc_r;

	/* Write 0x55 to BACKUPNV register to initiate the backup. */
	uint8_t data = 0x55u;
	uint8_t status;
	uint8_t t6_rid;
	uint16_t backup_dly;
	/* read pending message */
	backup_is_on_going = true;
	backup_dly = 0;
	mxt_read_message_reportid(MXT_RESERVED_T255);
	status = type_to_rid(MXT_GEN_COMMANDPROCESSOR_T6, &t6_rid, 0);
	status = write_mem(mxt.T6_address + BACKUP_OFFSET, 1, &data);
	do{
		delay_ms(1);
		backup_dly++;
		if(GPI_ReadPin(TCH_INT) == GPIO_PIN_RESET) 
		{
			mxt_message_process_callback();
		}	
	}while((backup_is_on_going == true) && (backup_dly < 3000));
	printf("backup_dly = %d \n", backup_dly);
	return(status);
}

/* 중요 : T37로 아래 검사 가능하기 때문에 사용 안함. */
//uint8_t get_T117_data(uint8_t mode)  //'0' is refenrece, '1' is delta
//{
//    uint8_t status;
//    uint16_t address;
//    uint8_t instance;
//    uint8_t x,y;
//    uint16_t nodecount;
//    uint16_t i = 0;
//    uint8_t *ptr;
//    uint16_t cyccnt;
//    uint16_t print_data;
//    uint16_t dly;
//
//    uint8_t cmd[4];
//    
//    printf("%s : mode = 0x%X\r\n", __func__, mode);
//    
//    cmd[0] = 0x03;
//    cmd[1] = mode;  //0x11 reference, 0x10 delta
//    cmd[2] = 0;     //header type
//    cmd[3] = 0;     //time-out (ms)
//
//    // Set command
//    address = get_object_address(MXT_SPT_DATACONTAINERCTRL_T118, 0);
//    printf("address = %d\r\n", address);
//    status = write_mem(address, 4, &cmd[0]);
//    if(status) return -1;
//
//    dataready = 0;
//    
//    //Read T5 message to get T118 message
//    mxt_message_process_callback();
//
//    dly = 0;
//    do{
//        dly++;
//        delay_ms(1);
//        if(dataready != 0x00) break;
//    }while(dly < 3000);
//
//    if(dly == 3000) return -1;
//
//    //if data is ready
//    if(dataready == mode)
//    {
//        DBG_printf("DATAREADY = 0x%x\n", dataready);
//
//        nodecount = 0;
//        memset(t117_data, 0, sizeof(t117_data));
////        ptr = t117_data;
//        debug_is_ongoing = true;
//
//        for(instance = 0; instance < MAX_T117_INSTACE_SIZE; instance++)
//        {
//            address = get_object_address(MXT_SPT_DATACONTAINER_T117,instance);
//            read_mem(address, MAX_T117_OBJ_SIZE, &t117_data[nodecount]);
//            nodecount += MAX_T117_OBJ_SIZE;
//        }
//
//        
//        //Print out read data
//        i = 0;
////        for(x = 0; x < 20; x++)
//        for(x = 0; x < 12; x++)
//        {
////            for(y = 0; y < 142; y++)
//          for(y = 0; y < 11; y++)
//            {
//                if(dataready == 0x11)
//                {
//                    DBG_printf("%d ", t117_data[i] | (uint16_t)t117_data[i+1]<<8);
//                    i+=2;
//                }
//                else if(dataready == 0x10)
//                {
//                    DBG_printf("%d ", (uint16_t)t117_data[i]<<8 | t117_data[i+1]);
//                    i+=2;
//                }
//            }
//            DBG_printf("\n");
//        }
//        
//        //Clear command
//        cmd[0] = 0x00;
//        cmd[1] = 0x00;
//        write_mem(mxt.T118_address, 4, &cmd[0]);
//        debug_is_ongoing = false;
//        
//        //Read t5 message
//        mxt_message_process_callback();
//
//        return 0;
//    }
//}









































uint8_t load_fw_file(struct mxt_fw_info *fw_info, const unsigned char *fw_file)
{
  struct mxt_fw_image *fw_img;
  
  //================================================================//
  // 터치 펌웨어 파일(.bin) read 
  memset(touch_buff, 0, sizeof(touch_buff));
  M95Pxx_ReadData(0, &touch_buff[0], 1024);
  //================================================================//
  
  fw_img = (struct mxt_fw_image *)touch_buff;
  
  mxt.fw_info.fw_ver =   0;
  mxt.fw_info.build_ver =   0;
  mxt.fw_info.hdr_len =   0;
  mxt.fw_info.cfg_len =   0;
  mxt.fw_info.fw_len =   0;
  mxt.fw_info.cfg_crc =   0;
  mxt.fw_info.cfg_raw_data = 0;
  mxt.fw_info.fw_raw_data = 0;
  
  if(sizeof(fw_img) == NULL)
  {
    MXT_printf("Cannot found firmware file\n");
    return(STATUS_ABORTED);
  }
  
  if (fw_img->magic_code != MXT_FW_MAGIC)
  {
    /* In case, firmware file only consist of firmware */
    DBG_printf("Firmware file type is .enc\n");
    mxt.fw_info.fw_len =   sizeof(fw_file);
    mxt.fw_info.fw_raw_data = fw_file;
  }
  else
  {
    DBG_printf("Firmware file type is .fw\n");
    mxt.fw_info.fw_ver =   fw_img->fw_ver;
    mxt.fw_info.build_ver =   fw_img->build_ver;
    mxt.fw_info.hdr_len =   fw_img->hdr_len;
    mxt.fw_info.cfg_len =   fw_img->cfg_len;
    mxt.fw_info.fw_len =   fw_img->fw_len;
    mxt.fw_info.cfg_crc =   fw_img->cfg_crc;
    
    if (mxt.fw_info.cfg_len != 0)
    {
      /* Get the address of configuration data */
//      mxt.fw_info.cfg_raw_data = fw_img->data;
      g_cfg_raw_data_start_addr = 22;
    }
    else
    {
      DBG_printf(".fw file dose not include configuration data\n");
    }
    
    if (mxt.fw_info.fw_len != 0)
    {
      /* Get the address of firmware data */
//      mxt.fw_info.fw_raw_data = fw_img->data + mxt.fw_info.cfg_len;
      g_fw_raw_data_start_addr = g_cfg_raw_data_start_addr + mxt.fw_info.cfg_len;
    }
    else
    {
      DBG_printf(".fw file dose not include raw firmware data\n");
    }
  }
  return(STATUS_OK);
}

#if 0
uint8_t get_T37_data(uint8_t mode)
{
  struct mxt_object * t37_object;
  struct t37_diagnostic_data t37_buf;
  
  uint8_t error;
  uint8_t *ptr;
  
  uint16_t nodecount;
  uint16_t data_values;
  uint8_t page_size;
  uint16_t pages_per_pass;
  uint16_t page;
  
//  uint8_t *dbg_data;
  
  uint16_t diagnostic_addr;
  uint8_t diagnostic_size;
  uint16_t failures;
  uint16_t total_ref_read_time;	
  
  uint8_t x,y;
  uint16_t print_data;
  
  //==============================================================//
//  mxt.t100_screen_xsize = 14;
//  mxt.t100_screen_ysize = 11;
  //==============================================================//  
  
  uint8_t dbg_cmd;
  
  
  printf(ANSI_GREEN"%s : mode = 0x%02X \r\n"ANSI_RESET, __func__, mode);
  
  /* get the address of the diagnostic object so we can get the data we need */
  t37_object = mxt_get_object(MXT_DEBUG_DIAGNOSTIC_T37);
  if(t37_object == NULL) return (-1);
  
  diagnostic_addr = t37_object->i2c_address;
  diagnostic_size = t37_object->size_minus_one+1;
  
  printf("diagnostic_addr = 0x%X, diagnostic_size = %d \r\n", diagnostic_addr, diagnostic_size);
  
  //init debug buffer & count
  
  if((mode == QT_REFERENCE_MODE) || (mode == QT_DELTA_MODE))
  {
//    data_values = mxt.info.matrix_x_size * mxt.info.matrix_y_size;
    data_values = mxt.t100_screen_xsize * mxt.t100_screen_ysize;
  }
  else if((mode == SCT_DELTA_MODE) || (mode == SCT_REFERENCE_MODE) || ( mode == SCT_SIGNAL_MODE))
  {
    data_values = mxt.info.matrix_x_size + mxt.info.matrix_y_size;
  }
  printf("data_values = %d \r\n", data_values);
  
  
  page_size = (t37_object->size_minus_one +1) - 2;
  pages_per_pass = (data_values * 2 + (page_size - 1)) / page_size;
  
  printf("page_size = %d, pages_per_pass = %d \r\n", page_size, pages_per_pass);
  
  nodecount = data_values*2;
  printf("nodecount = %d \r\n", nodecount);
  
//  dbg_data = (uint8_t *) malloc(nodecount);
//  if(dbg_data == NULL) return (-1);
  
  memset(touch_buff, 0, nodecount);
  
//  ptr = touch_buff;
  
  for(page = 0; page < pages_per_pass; page++)
  {
    /* Clear memory for new reading */
    memset(&t37_buf, 0, sizeof(struct t37_diagnostic_data));
    
    error = read_mem_i2c(diagnostic_addr,2, &t37_buf.mode);
    if(error)
    {
//      free(&dbg_data);
      return (-1);
    }
    
    if((t37_buf.mode != mode) && (page == 0x00))
    {
      dbg_cmd = mode;
    }
    else
    {
      dbg_cmd = QT_PAGE_UP;
    }
    
    printf("dbg_cmd = %d \r\n", dbg_cmd);
    
    //error = write_mem_i2c(mxt.T6_address + MXT_COMMAND_DIAGNOSTIC, 1, &dbg_cmd);
    error = mxt_write_object( MXT_GEN_COMMANDPROCESSOR_T6,MXT_COMMAND_DIAGNOSTIC, dbg_cmd);
    if(error) 
    {
//      free(&dbg_data);
      return (-1);			
    }
    
    /* Read back diagnostic register in T6 command processor until it has been
    * cleared. This means that the chip has actioned the command */
    failures = 0;
#if 1
    do 
    {
      error = read_mem_i2c(mxt.T6_address + MXT_COMMAND_DIAGNOSTIC, 1, &dbg_cmd);
      if(error) 
      {
//        free(dbg_data);
        return -1;
      }
      
      if(dbg_cmd == 0) 
      {
        break;
      }
      
      delay_ms(1);	
      failures++;			
      
      if (failures > 500)
      {
        printf("Timeout waiting for command to be actioned\r\n");
//        free(dbg_data);
        return -1;
      }
    } while (dbg_cmd != 0);
    
#endif			
    error = read_mem_i2c(diagnostic_addr,diagnostic_size, &t37_buf.mode);
    if(error) 
    {
//      free(dbg_data);
      return -1;
    }
    
    printf("t37_buf.mode = %d \r\n", t37_buf.mode);
    printf("t37_buf.page = %d \r\n", t37_buf.page);
    
    if (t37_buf.mode != mode)
    {
      printf("Bad mode in diagnostic data read");
//      free(dbg_data);
      return -1;
    }
    
    if (t37_buf.page != page)
    {
      printf("Bad page in diagnostic data read");
//      free(dbg_data);
      return -1;
    }
    
    if(nodecount > page_size)
    {
      memcpy(&touch_buff[page*page_size], &t37_buf.data, page_size);
//      dbg_data += page_size;
      nodecount -= page_size;
    }
    else
    {
      memcpy(&touch_buff[page*page_size], &t37_buf.data, nodecount);	
    }
    
//    mxt_log_dbg("debug read complete take %d ms \n", total_ref_read_time);
    
  }
  
//  ptr = dbg_data;
  
  //Print out read data
#if 1
  
  uint16_t i = 0;
  
  if((mode == QT_REFERENCE_MODE) || (mode == QT_DELTA_MODE))
  {
    //for(x = 0; x < mxt.info.matrix_x_size; x++)  // 
    for(x=0; x <  mxt.t100_screen_xsize; x++)   //X는 Screen size
    {
      for(y = 0; y < mxt.t100_screen_ysize ; y++)  //Y는 matrix Y size 까지
      {
//        print_data = *ptr++;
//        print_data|= (*ptr++)<<8;
        
        DBG_printf("%d ", touch_buff[i] | (uint16_t)touch_buff[i+1]<<8);
        i+=2;
      }
      printf("\n");
    }
    
  }
//  else if((mode == SCT_DELTA_MODE) || (mode == SCT_REFERENCE_MODE) || ( mode == SCT_SIGNAL_MODE))
//  {
//    printf("\n");
//    printf("Self-cap Y : \n");
//    
//    
//    for(y = 0; y < mxt.info.matrix_y_size ; y++)
//    {
//      print_data = *ptr++;
//      print_data|= (*ptr++)<<8;
//      
//      
//      if(y <  mxt.t100_screen_ysize)
//      {
//        if(mode == SCT_DELTA_MODE)		printf("%d, ", (signed short)print_data);
//        else							printf("%d, ", (unsigned short)print_data);
//      }
//    }
//    
//    
//    printf("\n");
//    printf("Self-cap X : \n");
//    
//    
//    
//    for(x = 0; x < mxt.info.matrix_x_size ; x++)
//    {
//      print_data = *ptr++;
//      print_data|= (*ptr++)<<8;
//      
//      
//      if(x <  mxt.t100_screen_xsize)
//      {
//        if(mode == SCT_DELTA_MODE)		printf("%d, ", (signed short)print_data);
//        else							printf("%d, ", (unsigned short)print_data);
//      }
//    }
//    
//  }
  
  
#endif
  
  
//  mode = QT_CTE_MODE;
//  error = mxt_write_object( MXT_GEN_COMMANDPROCESSOR_T6,MXT_COMMAND_DIAGNOSTIC, mode);
  
//  if(dbg_data) 
//  {
//    free(dbg_data);
//  }
  
  return (error);
}
#else
uint8_t get_T37_data(uint8_t mode, uint8_t tx_send_flag)
{
  struct mxt_object * t37_object;
  struct t37_diagnostic_data t37_buf;
  
  uint8_t error;
  uint8_t *ptr;
  
  uint16_t nodecount;
  uint16_t data_values;
  uint8_t page_size;
  uint16_t pages_per_pass;
  uint16_t page;
  
//  uint8_t *dbg_data;
  
  uint16_t diagnostic_addr;
  uint8_t diagnostic_size;
  uint16_t failures;
  uint16_t total_ref_read_time;	
  
  uint8_t x,y;
  uint16_t print_data;

  uint8_t dbg_cmd;
  
  printf(ANSI_GREEN"%s : mode = 0x%02X \r\n"ANSI_RESET, __func__, mode);
  
  /* get the address of the diagnostic object so we can get the data we need */
  t37_object = mxt_get_object(MXT_DEBUG_DIAGNOSTIC_T37);
  if(t37_object == NULL) return (-1);
  
  diagnostic_addr = t37_object->i2c_address;
  diagnostic_size = t37_object->size_minus_one+1;
  
  printf("diagnostic_addr = 0x%X, diagnostic_size = %d \r\n", diagnostic_addr, diagnostic_size);
  
  //init debug buffer & count
  
  if((mode == QT_REFERENCE_MODE) || (mode == QT_DELTA_MODE))
  {
//    data_values = mxt.info.matrix_x_size * mxt.info.matrix_y_size;
    data_values = mxt.t100_screen_xsize * mxt.t100_screen_ysize;
  }
  else if((mode == SCT_DELTA_MODE) || (mode == SCT_REFERENCE_MODE) || ( mode == SCT_SIGNAL_MODE))
  {
    data_values = mxt.info.matrix_x_size + mxt.info.matrix_y_size;
  }
  printf("data_values = %d \r\n", data_values);
  
  
  page_size = (t37_object->size_minus_one +1) - 2;
  pages_per_pass = (data_values * 2 + (page_size - 1)) / page_size;
  
  printf("page_size = %d, pages_per_pass = %d \r\n", page_size, pages_per_pass);
  
  nodecount = data_values*2;
  printf("nodecount = %d \r\n", nodecount);
  
//  dbg_data = (uint8_t *) malloc(nodecount);
//  if(dbg_data == NULL) return (-1);
  
  memset(touch_buff, 0, nodecount);
  
  ptr = touch_buff;
  
  for(page = 0; page < pages_per_pass; page++)
  {
    /* Clear memory for new reading */
    memset(&t37_buf, 0, sizeof(struct t37_diagnostic_data));
    
    error = read_mem_i2c(diagnostic_addr,2, &t37_buf.mode);
    if(error)
    {
//      free(&dbg_data);
      return (-1);
    }
    
    if((t37_buf.mode != mode) && (page == 0x00))
    {
      dbg_cmd = mode;
    }
    else
    {
      dbg_cmd = QT_PAGE_UP;
    }
    
    printf("dbg_cmd = %d \r\n", dbg_cmd);
    
    //error = write_mem_i2c(mxt.T6_address + MXT_COMMAND_DIAGNOSTIC, 1, &dbg_cmd);
    error = mxt_write_object( MXT_GEN_COMMANDPROCESSOR_T6,MXT_COMMAND_DIAGNOSTIC, dbg_cmd);
    if(error) 
    {
//      free(&dbg_data);
      return (-1);			
    }
    
    /* Read back diagnostic register in T6 command processor until it has been
    * cleared. This means that the chip has actioned the command */
    failures = 0;
#if 1
    do 
    {
      error = read_mem_i2c(mxt.T6_address + MXT_COMMAND_DIAGNOSTIC, 1, &dbg_cmd);
      if(error) 
      {
//        free(dbg_data);
        return -1;
      }
      
      if(dbg_cmd == 0) 
      {
        break;
      }
      
      delay_ms(1);	
      failures++;			
      
      if (failures > 500)
      {
        printf("Timeout waiting for command to be actioned\r\n");
//        free(dbg_data);
        return -1;
      }
    } while (dbg_cmd != 0);
    
#endif			
    error = read_mem_i2c(diagnostic_addr,diagnostic_size, &t37_buf.mode);
    if(error) 
    {
//      free(dbg_data);
      return -1;
    }
    
    printf("t37_buf.mode = %d \r\n", t37_buf.mode);
    printf("t37_buf.page = %d \r\n", t37_buf.page);
    
    if (t37_buf.mode != mode)
    {
      printf("Bad mode in diagnostic data read");
//      free(dbg_data);
      return -1;
    }
    
    if (t37_buf.page != page)
    {
      printf("Bad page in diagnostic data read");
//      free(dbg_data);
      return -1;
    }
    
    if(nodecount > page_size)
    {
      memcpy(ptr, &t37_buf.data, page_size);
      ptr += page_size;
      nodecount -= page_size;
    }
    else
    {
      memcpy(ptr, &t37_buf.data, nodecount);		
    }
    
//    mxt_log_dbg("debug read complete take %d ms \n", total_ref_read_time);
    
  }
  
  ptr = touch_buff;
  
  //Print out read data
#if 1
  
  uint16_t i = 0;
  
  if((mode == QT_REFERENCE_MODE) || (mode == QT_DELTA_MODE))
  {
    //for(x = 0; x < mxt.info.matrix_x_size; x++)  // 
    for(x=0; x <  mxt.t100_screen_xsize; x++)   //X는 Screen size
    {
      for(y = 0; y < mxt.t100_screen_ysize ; y++)  //Y는 matrix Y size 까지
      {
        print_data = *ptr++;
        print_data|= (*ptr++)<<8;
        
        if(mode == QT_DELTA_MODE)	printf("%d ", (signed short)print_data);
        else				printf("%d ", (unsigned short)print_data);
      }
      printf("\n");
    }
    
  }
  else if((mode == SCT_DELTA_MODE) || (mode == SCT_REFERENCE_MODE) || ( mode == SCT_SIGNAL_MODE))
  {
    printf("\n");
    printf("Self-cap Y : \n");
    
    
    for(y = 0; y < mxt.info.matrix_y_size ; y++)
    {
      print_data = *ptr++;
      print_data|= (*ptr++)<<8;
      
      
      if(y <  mxt.t100_screen_ysize)
      {
        if(mode == SCT_DELTA_MODE)		printf("%d, ", (signed short)print_data);
        else							printf("%d, ", (unsigned short)print_data);
      }
    }
    
    
    printf("\n");
    printf("Self-cap X : \n");
    
    
    
    for(x = 0; x < mxt.info.matrix_x_size ; x++)
    {
      print_data = *ptr++;
      print_data|= (*ptr++)<<8;
      
      
      if(x <  mxt.t100_screen_xsize)
      {
        if(mode == SCT_DELTA_MODE)		printf("%d, ", (signed short)print_data);
        else							printf("%d, ", (unsigned short)print_data);
      }
    }
    
    printf("\n");
    
  }
  
  
#endif
  
  
  mode = QT_CTE_MODE;
  error = mxt_write_object( MXT_GEN_COMMANDPROCESSOR_T6,MXT_COMMAND_DIAGNOSTIC, mode);
  
//  if(dbg_data) 
//  {
//    free(dbg_data);
//  }
  
  
  if (tx_send_flag){
    /*
    touch_request_packet 함수 인자 설명 : 
    - REQUEST_DATA : 실 터치 데이터
    - 0 : 횟수 카운터로 48R은 데이터가 12*11*2 = 264여서 한번만 전송하므로 0만 사용
    - (data_values*2) : 264
    - ptr : 실 데이터 배열
    */
    touch_request_packet(REQUEST_DATA, 0, (data_values*2), touch_buff);
  }
  return (error);
}
#endif

uint8_t mxt_driver_init(void)
{
  uint16_t i;
  uint8_t status;
  uint16_t current_address;
  uint16_t info_block_size;
  uint8_t num_of_object;
  uint32_t calculated_crc;
  uint8_t buf[ID_INFO_ELEMENT_SIZE];
  uint8_t *info_block_data;
  uint8_t min_id;
  uint8_t max_id;
  
#ifdef FIRM_UP_ENABLE
  status = load_fw_file(&mxt.fw_info, &firmware);
  if(status != STATUS_OK )
  {
    MXT_printf("Cannot found firmware file\n");
  }
  else
  {
    //status = mxt_boot_crc();
    //if(status == STATUS_OK)
    //{
    //	MXT_printf("Firmware crc check is ok\n");
    //}
  }
  
  status = maxtouch_request_firmware_update();
  
#endif
  
#ifndef MXT_SPI
  
  /*Check i2c address for maxtouch  */
  status = maxtouch_check_address(); //0x94 found
  if(status != STATUS_OK )
  {
    MXT_printf("I2C address check failed\n");
    goto maxtouch_driver_init_err;
  }
  
  /* The maxtouch device should be in the application mode. */                                                          //maxtouch 장치는 애플리케이션 모드에 있어야 합니다.
  /* If the device still in the boot mode then device has failed firmware update or has application CRC fail */         //기기가 여전히 부팅 모드에 있는 경우, 기기는 펌웨어 업데이트에 실패했거나 애플리케이션 CRC에 실패합니다.
  
  if((mxt.i2c_address >= 0x24 ) && (mxt.i2c_address <= 0x35 )) //진행안함.
  {
    MXT_printf("Maxtouch device in the boot mode\n");
    /* printf out boot info */
    mxt_check_bootloader();
    
    goto maxtouch_driver_init_err;
  }
#endif
  
  /* Get declared object size @0x0006 */        //선언된 객체 크기 가져오기
  status = read_mem(6, 1, &num_of_object);
  if((status != STATUS_OK) || (num_of_object == 0))
  {
    DBG_printf("declared object size read failed\r\n");
    goto maxtouch_driver_init_err;
  }
  
  info_block_size = ID_INFO_ELEMENT_SIZE + (num_of_object * OBJECT_TABLE_ELEMENT_SIZE);
  DBG_printf("info_block_size = %d\r\n", info_block_size);      //info_block_size = 277
  info_block_data = (uint8_t *) malloc(info_block_size);
  if (info_block_data == NULL)
  {
    DBG_printf("info block memory allocation failed \r\n");
    goto maxtouch_driver_init_err;
  }
  
  /* Read ID Information */
  current_address = 0;
  status = read_mem(0, ID_INFO_ELEMENT_SIZE , info_block_data);
  if(status != STATUS_OK)
  {
    DBG_printf("device info read faild\r\n");
    goto maxtouch_driver_init_err;
  }
  
  /* Read object Table */
  for(current_address = OBJECT_TABLE_START_ADDRESS; current_address < info_block_size; current_address += OBJECT_TABLE_ELEMENT_SIZE)
  {
    // 0x0007 부터 6byte씩 read
    status = read_mem(current_address, OBJECT_TABLE_ELEMENT_SIZE ,info_block_data+current_address); 
    if(status != STATUS_OK)
    {
      DBG_printf("object table read faild\r\n");
      goto maxtouch_driver_init_err;
    }
  }
  
  /* Read info block crc */
  status = read_mem( info_block_size, 3, buf); //info_block_size 이후 crc값 불러옴.
  if (status != STATUS_OK)
  {
    DBG_printf("Read info block CRC failed, skipping check!\r\n");
    goto maxtouch_driver_init_err;
  }
  mxt.info_crc = (uint32_t)(buf[0] | (buf[1] << 8) | (buf[2] << 16));
  
  
  /* Calculate Info block crc */
  calculated_crc = 0;
  for( i = 0; i < (info_block_size - 1) ; i+=2 )
  {
    calculated_crc = crc24(calculated_crc, *(info_block_data + i), *(info_block_data + i + 1));
  }
  calculated_crc = crc24(calculated_crc, *(info_block_data + i), 0);
  /* use  only 24 bit CRC */
  calculated_crc = (calculated_crc & 0x00FFFFFF);
  
  
  /* Verify info block crc */
  /*
  * CRC mismatch can be caused by data corruption due to I2C comms
  * issue or else device is not using Object Based Protocol (eg i2c-hid)
  */
  DBG_printf("Info Block CRC = 0x%06X,  Calculated CRC = 0x%06X \r\n",mxt.info_crc, calculated_crc); //Info Block CRC = 0xFC2071,  Calculated CRC = 0xFC2071
  
  if ((mxt.info_crc == 0) || (mxt.info_crc != calculated_crc)) {
    DBG_printf("Info block checksum mismatched\r\n");
    goto maxtouch_driver_init_err;
  }
  
  
  /* Parsing ID info */
  mxt.info.family_id   = info_block_data[0];
  mxt.info.variant_id  = info_block_data[1];
  mxt.info.version     = info_block_data[2];
  mxt.info.build       = info_block_data[3];
  mxt.info.matrix_x_size = info_block_data[4];
  mxt.info.matrix_y_size = info_block_data[5];
  mxt.info.num_declared_objects = info_block_data[6];
  
  /*
  Falily ID = 0xa7
  Variant ID = 0x12
  Version = 1.0
  Build = 0xaa
  Matrix X Size = 12
  Matrix Y Size = 24
  Num of Objects = 45
  */
  DBG_printf("Falily ID = 0x%02X\r\n", mxt.info.family_id);
  DBG_printf("Variant ID = 0x%02X\r\n", mxt.info.variant_id);
  DBG_printf("Version = %d.%d\r\n", (mxt.info.version>>4),(mxt.info.version&0x0F));
  DBG_printf("Build = 0x%02X\r\n", mxt.info.build);
  DBG_printf("Matrix X Size = %d\r\n", mxt.info.matrix_x_size);
  DBG_printf("Matrix Y Size = %d\r\n", mxt.info.matrix_y_size);
  DBG_printf("Num of Objects = %d\r\n", mxt.info.num_declared_objects);
  DBG_printf("\r\n");
  
  
  if(mxt.info.num_declared_objects == 0)
  {
    DBG_printf("Num of declared obj size is Zero\r\n");
    goto maxtouch_driver_init_err;
  }
  
  //        DBG_printf("num_declared_objects=%d, size=%zu\r\n", mxt.info.num_declared_objects,
  //           mxt.info.num_declared_objects * sizeof(struct mxt_object));
  
  /* Parsing object Table */
  printf("mxt.info.num_declared_objects = %d, sizeof(struct mxt_object) = %d \r\n", mxt.info.num_declared_objects, sizeof(struct mxt_object));
  printf("mxt.info.num_declared_objects * sizeof(struct mxt_object) = %d \r\n", mxt.info.num_declared_objects * sizeof(struct mxt_object));
  mxt.object_table = (struct mxt_object *) malloc(mxt.info.num_declared_objects * sizeof(struct mxt_object));
  if (mxt.object_table == NULL)
  {
    DBG_printf("Object table mem allocation failed\r\n");
    goto maxtouch_driver_init_err;
  }
  
  mxt.max_report_id = 0;
  mxt.t100_min_id = 0;
  mxt.t100_max_id = 0;
  current_address = OBJECT_TABLE_START_ADDRESS;
  for (i = 0; i < mxt.info.num_declared_objects; i++) //num_declared_objects = 45(0x2D)
  {
    memcpy(buf, info_block_data+current_address, OBJECT_TABLE_ELEMENT_SIZE);
    mxt.object_table[i].object_type = buf[0];
    mxt.object_table[i].i2c_address = buf[1]+ (buf[2] << 8);
    mxt.object_table[i].size_minus_one = buf[3];
    mxt.object_table[i].instances_minus_one = buf[4];
    mxt.object_table[i].num_report_ids = buf[5];
    
    DBG_printf("object_type = %d\r\n", mxt.object_table[i].object_type );
    DBG_printf("i2c_address = %d\r\n", mxt.object_table[i].i2c_address);
    DBG_printf("size_minus_one = %d\r\n", mxt.object_table[i].size_minus_one);
    DBG_printf("instances_minus_one = %d\r\n", mxt.object_table[i].instances_minus_one);
    DBG_printf("num_report_ids = %d\r\n", mxt.object_table[i].num_report_ids);
    DBG_printf("\r\n");
    
    current_address += OBJECT_TABLE_ELEMENT_SIZE;
    
    if(mxt.object_table[i].num_report_ids != 0)
    {
      min_id = mxt.max_report_id +1;
      mxt.max_report_id += mxt.object_table[i].num_report_ids * (mxt.object_table[i].instances_minus_one+1);
      max_id = mxt.max_report_id;
    }
    
    if(mxt.object_table[i].object_type == MXT_TOUCH_MULTITOUCHSCREEN_T100)
    {
      mxt.t100_min_id = min_id;
      mxt.t100_max_id = max_id;
    }
  }
  
  
  DBG_printf("max_report_id = %d\r\n", mxt.max_report_id);
  
  
  mxt.T6_address = get_object_address(MXT_GEN_COMMANDPROCESSOR_T6, 0);
  mxt.T5_address = get_object_address(MXT_GEN_MESSAGEPROCESSOR_T5, 0);
  mxt.max_message_length = get_object_size(MXT_GEN_MESSAGEPROCESSOR_T5) - 1;
  
  mxt.T117_address = get_object_address(MXT_SPT_DATACONTAINER_T117, 0);
  mxt.T118_address = get_object_address(MXT_SPT_DATACONTAINERCTRL_T118, 0);
  
  DBG_printf("mxt.T6_address = %X\r\n", mxt.T6_address);
  DBG_printf("mxt.T5_address = %X, mxt.max_message_length = %d\r\n", mxt.T5_address, mxt.max_message_length);
  DBG_printf("mxt.T117_address = %X\r\n", mxt.T117_address);
  DBG_printf("mxt.T118_address = %X\r\n", mxt.T118_address);
  
  
  if ((mxt.info.family_id == 0x80) && ( mxt.info.version < 0x20))
  {
    /*
    * On mXT224 firmware versions prior to V2.0
    * read and discard unused CRC byte otherwise
    * DMA reads are misaligned.
    */
    
    mxt.max_message_length += 1;
  }
  
  mxt.message = (uint8_t *) malloc(mxt.max_message_length);
  if (mxt.message == NULL)
  {
    DBG_printf("msg mem allocation failed\r\n");
    goto maxtouch_driver_init_err;
  }
  
  DBG_printf("mxt.max_message_length = %d\r\n", mxt.max_message_length);
  
  /* Allocate memory for report id map now that the number of report id's  is known. */
  
  uint8_t instance;
  uint16_t current_report_id;
  uint16_t start_report_id;
  
  mxt.rid_map = malloc(sizeof(struct report_id_map) * mxt.max_report_id + 1);
  if (mxt.rid_map == NULL)
  {
    DBG_printf("rid mem allocation failed\n");
    free(mxt.rid_map);
    return(STATUS_ERR_NO_MEMORY);
  }
  
  /* Report ID 0 is reserved, so start from 1. */
  
  mxt.rid_map[0].instance = 0;
  mxt.rid_map[0].object_type = 0;
  current_report_id = 1;
  
  for (i = 0; i < mxt.info.num_declared_objects; i++)
  {
    if (mxt.object_table[i].num_report_ids != 0)
    {
      
      for (instance = 0; instance <= mxt.object_table[i].instances_minus_one; instance++)
      {
        start_report_id = current_report_id;
        for (; current_report_id < (start_report_id + mxt.object_table[i].num_report_ids); current_report_id++)
        {
          mxt.rid_map[current_report_id].instance = instance;
          mxt.rid_map[current_report_id].object_type = mxt.object_table[i].object_type;
          
          //DBG_printf("ID = %d , Type = %d, Instance = %d \n", current_report_id,mxt.rid_map[current_report_id].object_type,mxt.rid_map[current_report_id].instance);
        }
      }
    }
  }
  
  
  //status = mxt_build_rid();
  //if(status != STATUS_OK)
  //{
  //	DBG_printf("report id map build failed\n");
  //	goto maxtouch_driver_init_err;
  //}
  
  
  //status = write_mem(MXT_CFG_MEM_ADDR, MXT_CFG_MEM_SIZE, mxt_config);
  //if(status != STATUS_OK)
  //{
  //	DBG_printf("Config update failed\n");
  //	//goto maxtouch_driver_init_err;
  //}
  //status = backup_config();
  //if (status != STATUS_OK)
  //{
  //	DBG_printf("Backup Failed  %s\n",__FUNCTION__);
  //	return (STATUS_ABORTED);
  //}
  
#if 1
  status = write_configuration();
  if(status != STATUS_OK)
  {
    DBG_printf("Configuraiton writing failed\n");
    //goto maxtouch_driver_init_err;
  }
  
  status = backup_config();
  if (status != STATUS_OK)
  {
    DBG_printf("Backup Failed  %s\n",__FUNCTION__);
    return (STATUS_ABORTED);
  }
#endif	
  
  if(get_object_address(MXT_TOUCH_MULTITOUCHSCREEN_T9,0) != 0)
  {
    status = mxt_read_t9_resolution();
    if(status != STATUS_OK)
    {
      DBG_printf("Failed to initialize T9 resolution\n");
      mxt.max_x = 0;
      mxt.max_y = 0;
    }		
  }
  else if(get_object_address(MXT_TOUCH_MULTITOUCHSCREEN_T100,0) != 0)
  {
    status = mxt_read_t100_config();
    if(status != STATUS_OK)
    {
      DBG_printf("Failed to read T100 config\n");
    }
  }
  
  
  status = STATUS_OK;
  DBG_printf("Maxtouch initialization is finished\n");
  goto maxtouch_driver_init_end;
  
maxtouch_driver_init_err:
  status = STATUS_ABORTED;
  
maxtouch_driver_init_end:
  
  if(info_block_data != NULL)
  {
    free(info_block_data);
  }
  
  if(mxt.message != NULL)
  {
    free(mxt.message);
  }
  
//  if(mxt.object_table != NULL)
//  {
//    free(mxt.object_table);
//  }
  
  if(mxt.rid_map != NULL)
  {
    
    free(mxt.rid_map);
  }
  
  printf("%s : finish~~~~(%d)\r\n", __func__, status);
  printf("\r\n");
  return (status);
  
}

void touch_test()
{
  uint8_t status;
  
  status = mxt_driver_init();
  if(status == STATUS_OK) 
  {
//    get_T117_data(0x11);    //Read reference 
    //get_T117_data(0x10);    //Read Touch delta
    

    get_T37_data(QT_REFERENCE_MODE, 0); //Read reference 
    get_T37_data(QT_DELTA_MODE, 0); //Read Touch delta
  }
}












void touch_request_packet(uint8_t type, uint8_t s_cmd, uint32_t len, uint8_t* data)
{
  uint8_t Index=0;
  
#if(DEBUG == USE)
  //        printf("send touch request type[%d]Cnt[%d]len[%ld]\r\n",type,Cnt,len);
#endif
  switch(type)
  {
  case    REQUEST_INFOR    : 
    data[0] = mxt.t100_screen_xsize;            Index++;        // 가로 COUNT
    data[1] = mxt.t100_screen_ysize;            Index++;        // 세로 COUNT 
    data[2] = REQUEST_TX_COUNT - PC_DATA_PLUS;  Index++;        // Total Packet Count
    data[3] = REQUEST_TX_LINE_COUNT;            Index++;        // 1 Packet 당 TX Line number 
    data[4] = 0;                                Index++;   
    data[5] = 2;                                Index++;        //DATA 2바이트 
    
    UARTxSendData(M_FUNC_PACKET, s_cmd, data, Index);
    break;
    
  case    REQUEST_DATA    :
    UARTxSendData(TOUCH_REQUEST_ADDR, s_cmd, data, len);
    break;
  }
}

void touch_inspection_packet(uint8_t *pInBuff)
{
  uint8_t txdata[10],rxdata[10];
  uint8_t touch_test_type = 0;
  uint8_t retry_cnt = 0;
  
  touch_test_type = pInBuff[PROTOCOL_DATA+1];
  
  printf("touch_inspection type[0x%x]\r\n", touch_test_type);
  
    // request data : touch tx,rx etc... data -- 초기 총 data를 몇바이트씩 보낼것인지 미리 정보를 전송한다.
  touch_request_packet(REQUEST_INFOR, pInBuff[PROTOCOL_SUB_CMD], 0, pTxBuf);
  
  

  // 2. Read touch data
  switch(touch_test_type)
  {
    case        T_REF_DATA      :       get_T37_data(QT_REFERENCE_MODE, 1);        break;  //Read reference
//    case        T_SHORT_DATA  :    NT23_get_short_rawdata(buf);   break;
//    case        T_OPEN_DATA   :    NT23_get_open_rawdata(buf);   break;
//    case        T_NOISE_DATA  :    NT23_get_noise_rawdata(buf);   break;// = Jitter
    default : break;
  }
  
  
  printf("touch test end\r\n");
}

void touch_ver_check_packet(uint8_t *pInBuff)
{
  uint8_t TxBuf[5] = {0};
  uint8_t i = 0;
  
  uint8_t status = mxt_driver_init();
  if(status == STATUS_OK) 
  {
    TxBuf[i++] = mxt.info.family_id; //TOVIS 추후 적용할수도 있다고 함.
    TxBuf[i++] = mxt.info.variant_id;
    TxBuf[i++] = mxt.info.version; //TOVIS 요청.
    TxBuf[i++] = mxt.info.build;
  }
  else
  {
    TxBuf[i++] = 0xFF;
    TxBuf[i++] = 0xFF;
    TxBuf[i++] = 0xFF;
    TxBuf[i++] = 0xFF;
  }
  
  UARTxSendData(pInBuff[PROTOCOL_MAIN_CMD], pInBuff[PROTOCOL_SUB_CMD], TxBuf, i);
}







