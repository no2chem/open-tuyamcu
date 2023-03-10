/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2020, 涂鸦科技
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    system.c
 * @author  涂鸦综合协议开发组
 * @version v2.5.6
 * @date    2020.12.16
 * @brief   串口数据处理，用户无需关心该文件实现内容
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#define SYSTEM_GLOBAL

#include "wifi.h"

extern const DOWNLOAD_CMD_S download_cmd[];



/**
 * @brief  写wifi_uart字节
 * @param[in] {dest} 缓存区其实地址
 * @param[in] {byte} 写入字节值
 * @return 写入完成后的总长度
 */
unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte)
{
    unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;
    
    *obj = byte;
    dest += 1;
    
    return dest;
}

/**
 * @brief  写wifi_uart_buffer
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {len} 数据长度
 * @return 写入结束的缓存地址
 */
unsigned short set_wifi_uart_buffer(unsigned short dest, const unsigned char *src, unsigned short len)
{
    unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;
    
    my_memcpy(obj,src,len);
    
    dest += len;
    return dest;
}

/**
 * @brief  计算校验和
 * @param[in] {pack} 数据源指针
 * @param[in] {pack_len} 计算校验和长度
 * @return 校验和
 */
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
    unsigned short i;
    unsigned char check_sum = 0;
    
    for(i = 0; i < pack_len; i ++) {
        check_sum += *pack ++;
    }
    
    return check_sum;
}

/**
 * @brief  串口发送一段数据
 * @param[in] {in} 发送缓存指针
 * @param[in] {len} 数据发送长度
 * @return Null
 */
static void wifi_uart_write_data(unsigned char *in, unsigned short len)
{
    if((NULL == in) || (0 == len)) {
        return;
    }

    while(len --) {
        uart_transmit_output(*in);
        in ++;
    }
}

/**
 * @brief  向wifi串口发送一帧数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {fr_ver} 帧版本
 * @param[in] {len} 数据长度
 * @return Null
 */
void wifi_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len)
{
    unsigned char check_sum = 0;
    
    wifi_uart_tx_buf[HEAD_FIRST] = 0x55;
    wifi_uart_tx_buf[HEAD_SECOND] = 0xaa;
    wifi_uart_tx_buf[PROTOCOL_VERSION] = fr_ver;
    wifi_uart_tx_buf[FRAME_TYPE] = fr_type;
    wifi_uart_tx_buf[LENGTH_HIGH] = len >> 8;
    wifi_uart_tx_buf[LENGTH_LOW] = len & 0xff;
    
    len += PROTOCOL_HEAD;
    check_sum = get_check_sum((unsigned char *)wifi_uart_tx_buf, len - 1);
    wifi_uart_tx_buf[len - 1] = check_sum;
    
    wifi_uart_write_data((unsigned char *)wifi_uart_tx_buf, len);
}

/**
 * @brief  心跳包检测
 * @param  Null
 * @return Null
 */
static void heat_beat_check(void)
{
    unsigned char length = 0;
    static unsigned char mcu_reset_state = FALSE;
    
    if(FALSE == mcu_reset_state) {
        length = set_wifi_uart_byte(length, FALSE);
        mcu_reset_state = TRUE;
    }else {
        length = set_wifi_uart_byte(length, TRUE);
    }
    
    wifi_uart_write_frame(HEAT_BEAT_CMD, MCU_TX_VER, length);
}

/**
 * @brief  产品信息上传
 * @param  Null
 * @return Null
 */
static void product_info_update(void)
{
    unsigned char length = 0;
    unsigned char str[10] = {0};
    
    length = set_wifi_uart_buffer(length, "{\"p\":\"", my_strlen("{\"p\":\""));
    length = set_wifi_uart_buffer(length,(unsigned char *)PRODUCT_KEY,my_strlen((unsigned char *)PRODUCT_KEY));
    length = set_wifi_uart_buffer(length, "\",\"v\":\"", my_strlen("\",\"v\":\""));
    length = set_wifi_uart_buffer(length,(unsigned char *)MCU_VER,my_strlen((unsigned char *)MCU_VER));
    length = set_wifi_uart_buffer(length, "\",\"m\":", my_strlen("\",\"m\":"));
    length = set_wifi_uart_buffer(length, (unsigned char *)CONFIG_MODE, my_strlen((unsigned char *)CONFIG_MODE));
#ifdef CONFIG_MODE_DELAY_TIME
    sprintf((char *)str,",\"mt\":%d",CONFIG_MODE_DELAY_TIME);
    length = set_wifi_uart_buffer(length, str, my_strlen(str));
#endif
#ifdef CONFIG_MODE_CHOOSE
    sprintf((char *)str,",\"n\":%d",CONFIG_MODE_CHOOSE);
    length = set_wifi_uart_buffer(length, str, my_strlen(str));
#endif
#ifdef ENABLE_MODULE_IR_FUN
    sprintf((char *)str,",\"ir\":\"%d.%d\"",MODULE_IR_PIN_TX,MODULE_IR_PIN_RX);
    length = set_wifi_uart_buffer(length, str, my_strlen(str));
#endif
#ifdef LONG_CONN_LOWPOWER
    sprintf((char *)str,",\"low\":%d",LONG_CONN_LOWPOWER);
    length = set_wifi_uart_buffer(length, str, my_strlen(str));
#endif
  
    length = set_wifi_uart_buffer(length, "}", my_strlen("}"));
    
    wifi_uart_write_frame(PRODUCT_INFO_CMD, MCU_TX_VER, length);
}

/**
 * @brief  mcu查询mcu和wifi的工作模式
 * @param  Null
 * @return Null
 */
static void get_mcu_wifi_mode(void)
{
    unsigned char length = 0;
    
#ifdef WIFI_CONTROL_SELF_MODE                                   //模块自处理
    length = set_wifi_uart_byte(length, WF_STATE_KEY);
    length = set_wifi_uart_byte(length, WF_RESERT_KEY);
#else                                                           
    //No need to process data
#endif
    
    wifi_uart_write_frame(WORK_MODE_CMD, MCU_TX_VER, length);
}

/**
 * @brief  获取制定DPID在数组中的序号
 * @param[in] {dpid} dpid
 * @return dp序号
 */
static unsigned char get_dowmload_dpid_index(unsigned char dpid)
{
    unsigned char index;
    unsigned char total = get_download_cmd_total();
    
    for(index = 0; index < total; index ++) {
        if(download_cmd[index].dp_id == dpid) {
            break;
        }
    }
    
    return index;
}

/**
 * @brief  下发数据处理
 * @param[in] {value} 下发数据源指针
 * @return 返回数据处理结果
 */
static unsigned char data_point_handle(const unsigned char value[])
{
    unsigned char dp_id,index;
    unsigned char dp_type;
    unsigned char ret;
    unsigned short dp_len;
    
    dp_id = value[0];
    dp_type = value[1];
    dp_len = value[2] * 0x100;
    dp_len += value[3];
    
    index = get_dowmload_dpid_index(dp_id);

    if(dp_type != download_cmd[index].dp_type) {
        //错误提示
        return FALSE;
    }else {
        ret = dp_download_handle(dp_id,value + 4,dp_len);
    }
    
    return ret;
}

#ifdef WEATHER_ENABLE
/**
 * @brief  天气数据解析
 * @param[in] {p_data} 接收数据指针
 * @param[in] {data_len} 接收数据长度
 * @return Null
 */
static void weather_data_raw_handle(const unsigned char p_data[], unsigned short data_len)
{
    int i = 1;
    int can_len = 0; 
    char can[15] = {0};
    char day = 0;
    int type1 = 0;
    unsigned char value_string[100] = {0};
    int val_cnt = 0;
    int val_len = 0;
    
    if(p_data[0] != 1 || data_len < 1) {
        //接收失败
    }else {
        if(data_len < 4) {
            //数据为空
        }
        
        while (i < data_len) {
            can_len = p_data[i];
            
            my_memset(can, '\0', 15);
            my_memcpy(can, p_data + i + 1, can_len - 2);

            day = p_data[i + can_len] - '0';

            type1 = p_data[i + 1 + can_len];
            if(type1 != 0 && type1 != 1) {
                return;
            }

            my_memset(value_string, '\0', 100);
            val_cnt = i + 1 + can_len + 1;
            val_len = p_data[val_cnt];
            if (type1 == 0) { //int32
                weather_data_user_handle(can+2, type1, p_data+val_cnt+1, day);
            }
            else if(type1 == 1) { //string
                my_memcpy(value_string, p_data + val_cnt + 1, val_len);
                weather_data_user_handle(can+2, type1, value_string, day);
            }

            i += 1 + can_len + 1 + 1 + val_len;
        }
        
        wifi_uart_write_frame(WEATHER_DATA_CMD, 0, 0);
    }
}
#endif

#ifdef WIFI_STREAM_ENABLE
/**
 * @brief  流数据传输
 * @param[in] {id} 流服务标识
 * @param[in] {offset} 偏移量
 * @param[in] {buffer} 数据地址
 * @param[in] {buf_len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char stream_trans(unsigned short id, unsigned int offset, unsigned char *buffer, unsigned short buf_len)
{
    unsigned short send_len = 0;

    stream_status = 0xff;

    if(stop_update_flag == ENABLE)
        return ERROR;

    //ID
    send_len = set_wifi_uart_byte(send_len,id / 0x100);
    send_len = set_wifi_uart_byte(send_len,id % 0x100);
    //Offset
    send_len = set_wifi_uart_byte(send_len,offset >> 24);
    send_len = set_wifi_uart_byte(send_len,offset >> 16);
    send_len = set_wifi_uart_byte(send_len,offset >> 8);
    send_len = set_wifi_uart_byte(send_len,offset % 256);
    //data
    send_len = set_wifi_uart_buffer(send_len, buffer, buf_len);
    wifi_uart_write_frame(STREAM_TRANS_CMD, MCU_TX_VER, send_len);
    return SUCCESS;
}

/**
 * @brief  多地图流数据传输
 * @param[in] {pro_ver} 地图服务协议版本
 * @param[in] {id} 地图流服务会话ID
 * @param[in] {sub_id} 子地图ID
 * @param[in] {sub_id_pro_mode} 子地图ID数据处理方式
 * @ref           0x00:继续累加
 * @ref           0x00:清除上传的数据
 * @param[in] {offset} 偏移量
 * @param[in] {buffer} 数据地址
 * @param[in] {buf_len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char maps_stream_trans(unsigned char pro_ver, unsigned short id, unsigned char sub_id, unsigned char sub_id_pro_mode, 
                                unsigned int offset, unsigned char *buffer, unsigned short buf_len)
{
    unsigned short send_len = 0;

    maps_stream_status = 0xff;

    if(stop_update_flag == ENABLE)
        return ERROR;

    //地图服务协议版本
    send_len = set_wifi_uart_byte(send_len, pro_ver);
    
    //地图流服务会话ID
    send_len = set_wifi_uart_byte(send_len,id / 0x100);
    send_len = set_wifi_uart_byte(send_len,id % 0x100);
    
    //子地图ID
    send_len = set_wifi_uart_byte(send_len, sub_id);
    
    //子地图ID数据处理方式
    send_len = set_wifi_uart_byte(send_len, sub_id_pro_mode);
    
    //偏移量
    send_len = set_wifi_uart_byte(send_len,offset >> 24);
    send_len = set_wifi_uart_byte(send_len,offset >> 16);
    send_len = set_wifi_uart_byte(send_len,offset >> 8);
    send_len = set_wifi_uart_byte(send_len,offset % 256);
    //Data
    send_len = set_wifi_uart_buffer(send_len, buffer, buf_len);
    wifi_uart_write_frame(MAPS_STREAM_TRANS_CMD, MCU_TX_VER, send_len);
    return SUCCESS;
}
#endif

/**
 * @brief  数据帧处理
 * @param[in] {offset} 数据起始位
 * @return Null
 */
void data_handle(unsigned short offset)
{
#ifdef SUPPORT_MCU_FIRM_UPDATE
    unsigned char *firmware_addr = NULL;
    static unsigned short firm_size;                                            //升级包一包的大小
    static unsigned long firm_length;                                           //MCU升级文件长度
    static unsigned char firm_update_flag = 0;                                  //MCU升级标志
    unsigned long dp_len;
    unsigned char firm_flag;                                                    //升级包大小标志
#else
    unsigned short dp_len;
#endif
  
    unsigned char ret;
    unsigned short i,total_len;
    unsigned char cmd_type = wifi_data_process_buf[offset + FRAME_TYPE];
    unsigned char result;

#ifdef WEATHER_ENABLE
    static unsigned char isWoSend = 0;                                          //是否已经打开过天气数据, 0:否  1:是
#endif

#ifdef WIFI_TEST_ENABLE
    unsigned char rssi;
#endif

#ifdef FILE_DOWNLOAD_ENABLE
    unsigned char *file_data_addr = NULL;
    static unsigned short file_package_size = 0;                                //文件数据包一包的大小
    static unsigned char file_download_flag = 0;                                //文件下载标志
    unsigned int file_download_size = 0;
#endif

    switch(cmd_type)
    {
        case HEAT_BEAT_CMD:                                     //心跳包
            heat_beat_check();
        break;
    
        case PRODUCT_INFO_CMD:                                  //产品信息
            product_info_update();
        break;
    
        case WORK_MODE_CMD:                                     //查询MCU设定的模块工作模式
            get_mcu_wifi_mode();
        break;
    
#ifndef WIFI_CONTROL_SELF_MODE
        case WIFI_STATE_CMD:                                    //wifi工作状态	
            wifi_work_state = wifi_data_process_buf[offset + DATA_START];
            wifi_uart_write_frame(WIFI_STATE_CMD, MCU_TX_VER, 0);
#ifdef WEATHER_ENABLE
            if(wifi_work_state == WIFI_CONNECTED && isWoSend == 0) { //当WIFI连接成功，打开天气数据且仅一次
                mcu_open_weather();
                isWoSend = 1;
            }
#endif
        break;

        case WIFI_RESET_CMD:                                    //重置wifi(wifi返回成功)
            reset_wifi_flag = RESET_WIFI_SUCCESS;
        break;
    
        case WIFI_MODE_CMD:                                     //选择smartconfig/AP模式(wifi返回成功)	
            set_wifimode_flag = SET_WIFICONFIG_SUCCESS;
        break;
#endif
    
        case DATA_QUERT_CMD:                                    //命令下发
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
    
            for(i = 0;i < total_len; ) {
                dp_len = wifi_data_process_buf[offset + DATA_START + i + 2] * 0x100;
                dp_len += wifi_data_process_buf[offset + DATA_START + i + 3];
                //
                ret = data_point_handle((unsigned char *)wifi_data_process_buf + offset + DATA_START + i);
      
                if(SUCCESS == ret) {
                    //成功提示
                }else {
                    //错误提示
                }
      
                i += (dp_len + 4);
            }
        break;
    
        case STATE_QUERY_CMD:                                   //状态查询
            all_data_update();                               
        break;
    
#ifdef SUPPORT_MCU_FIRM_UPDATE
        case UPDATE_START_CMD:                                  //升级开始
            //获取升级包大小全局变量
            firm_flag = PACKAGE_SIZE;
            if(firm_flag == 0) {
                firm_size = 256;
            }else if(firm_flag == 1) {
                firm_size = 512;
            }else if(firm_flag == 2) { 
                firm_size = 1024;
            }

            firm_length = wifi_data_process_buf[offset + DATA_START];
            firm_length <<= 8;
            firm_length |= wifi_data_process_buf[offset + DATA_START + 1];
            firm_length <<= 8;
            firm_length |= wifi_data_process_buf[offset + DATA_START + 2];
            firm_length <<= 8;
            firm_length |= wifi_data_process_buf[offset + DATA_START + 3];
            
            upgrade_package_choose(PACKAGE_SIZE);
            firm_update_flag = UPDATE_START_CMD;
        break;
    
        case UPDATE_TRANS_CMD:                                  //升级传输
            if(firm_update_flag == UPDATE_START_CMD) {
                //停止一切数据上报
                stop_update_flag = ENABLE;
      
                total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
      
                dp_len = wifi_data_process_buf[offset + DATA_START];
                dp_len <<= 8;
                dp_len |= wifi_data_process_buf[offset + DATA_START + 1];
                dp_len <<= 8;
                dp_len |= wifi_data_process_buf[offset + DATA_START + 2];
                dp_len <<= 8;
                dp_len |= wifi_data_process_buf[offset + DATA_START + 3];
      
                firmware_addr = (unsigned char *)wifi_data_process_buf;
                firmware_addr += (offset + DATA_START + 4);
      
                if((total_len == 4) && (dp_len == firm_length)) {
                    //最后一包
                    ret = mcu_firm_update_handle(firmware_addr,dp_len,0);
                    firm_update_flag = 0;
                }else if((total_len - 4) <= firm_size) {
                    ret = mcu_firm_update_handle(firmware_addr,dp_len,total_len - 4);
                }else {
                    firm_update_flag = 0;
                    ret = ERROR;
                }
      
                if(ret == SUCCESS) {
                    wifi_uart_write_frame(UPDATE_TRANS_CMD, MCU_TX_VER, 0);
                }
                //恢复一切数据上报
                stop_update_flag = DISABLE;
            }
        break;
#endif      

#ifdef SUPPORT_GREEN_TIME
        case GET_ONLINE_TIME_CMD:                               //获取格林时间
            mcu_get_greentime((unsigned char *)(wifi_data_process_buf + offset + DATA_START));
        break;
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
        case GET_LOCAL_TIME_CMD:                               //获取本地时间
            mcu_write_rtctime((unsigned char *)(wifi_data_process_buf + offset + DATA_START));
        break;
#endif
 
#ifdef WIFI_TEST_ENABLE
        case WIFI_TEST_CMD:                                     //wifi功能测试（扫描指定路由）
            result = wifi_data_process_buf[offset + DATA_START];
            rssi = wifi_data_process_buf[offset + DATA_START + 1];
            wifi_test_result(result, rssi);
        break;
#endif

#ifdef WEATHER_ENABLE
        case WEATHER_OPEN_CMD:                                  //打开天气服务返回
            weather_open_return_handle(wifi_data_process_buf[offset + DATA_START], wifi_data_process_buf[offset + DATA_START + 1]);
        break;
    
        case WEATHER_DATA_CMD:                                  //天气数据下发
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            weather_data_raw_handle((unsigned char *)wifi_data_process_buf + offset + DATA_START, total_len);
        break;
#endif

#ifdef WIFI_STREAM_ENABLE
        case STREAM_TRANS_CMD:                                  //流服务
            stream_status = wifi_data_process_buf[offset + DATA_START];//流服务传输返回接收
            stream_trans_send_result(stream_status);
        break;
        
        case MAPS_STREAM_TRANS_CMD:                             //流数据传输(支持多张地图)
            maps_stream_status = wifi_data_process_buf[offset + DATA_START];//流服务传输返回接收
            maps_stream_trans_send_result(maps_stream_status);
        break;
#endif

#ifdef WIFI_CONNECT_TEST_ENABLE
        case WIFI_CONNECT_TEST_CMD:                             //wifi功能测试（连接指定路由）
            result = wifi_data_process_buf[offset + DATA_START];
            wifi_connect_test_result(result);
        break;
#endif

#ifdef GET_MODULE_MAC_ENABLE
        case GET_MAC_CMD:                                       //获取模块mac
            mcu_get_mac((unsigned char *)(wifi_data_process_buf + offset + DATA_START));
        break;
#endif

#ifdef GET_WIFI_STATUS_ENABLE
        case GET_WIFI_STATUS_CMD:                               //获取当前wifi联网状态
            result = wifi_data_process_buf[offset + DATA_START];
            get_wifi_status(result);
        break;
#endif

#ifdef MCU_DP_UPLOAD_SYN
        case STATE_UPLOAD_SYN_RECV_CMD:                         //状态上报（同步）
            result = wifi_data_process_buf[offset + DATA_START];
            get_upload_syn_result(result);
        break;
#endif

#ifdef GET_IR_STATUS_ENABLE
        case GET_IR_STATUS_CMD:                                 //红外状态通知
            result = wifi_data_process_buf[offset + DATA_START];
            get_ir_status(result);
        break;
#endif
      
#ifdef IR_TX_RX_TEST_ENABLE
        case IR_TX_RX_TEST_CMD:                                 //红外进入收发产测
            result = wifi_data_process_buf[offset + DATA_START];
            ir_tx_rx_test_result(result);
        break;
#endif
        
#ifdef FILE_DOWNLOAD_ENABLE
        case FILE_DOWNLOAD_START_CMD:                           //文件下载启动
            //获取文件包大小选择
            if(FILE_DOWNLOAD_PACKAGE_SIZE == 0) {
                file_package_size = 256;
            }else if(FILE_DOWNLOAD_PACKAGE_SIZE == 1) {
                file_package_size = 512;
            }else if(FILE_DOWNLOAD_PACKAGE_SIZE == 2) { 
                file_package_size = 1024;
            }
            
            file_download_size = wifi_data_process_buf[offset + DATA_START];
            file_download_size = (file_download_size << 8) |  wifi_data_process_buf[offset + DATA_START + 1];
            file_download_size = (file_download_size << 8) |  wifi_data_process_buf[offset + DATA_START + 2];
            file_download_size = (file_download_size << 8) |  wifi_data_process_buf[offset + DATA_START + 3];
        
            file_download_package_choose(FILE_DOWNLOAD_PACKAGE_SIZE);
            file_download_flag = FILE_DOWNLOAD_START_CMD;
        break;
        
        case FILE_DOWNLOAD_TRANS_CMD:                           //文件下载数据传输
            if(file_download_flag == FILE_DOWNLOAD_START_CMD) {
                total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
      
                dp_len = wifi_data_process_buf[offset + DATA_START];
                dp_len <<= 8;
                dp_len |= wifi_data_process_buf[offset + DATA_START + 1];
                dp_len <<= 8;
                dp_len |= wifi_data_process_buf[offset + DATA_START + 2];
                dp_len <<= 8;
                dp_len |= wifi_data_process_buf[offset + DATA_START + 3];
      
                file_data_addr = (unsigned char *)wifi_data_process_buf;
                file_data_addr += (offset + DATA_START + 4);
      
                if((total_len == 4) && (dp_len == file_download_size)) {
                    //最后一包
                    ret = file_download_handle(file_data_addr,dp_len,0);
                    file_download_flag = 0;
                }
                else if((total_len - 4) <= file_package_size) {
                    ret = file_download_handle(file_data_addr,dp_len,total_len - 4);
                }else {
                    file_download_flag = 0;
                    ret = ERROR;
                }
      
                if(ret == SUCCESS) {
                    wifi_uart_write_frame(FILE_DOWNLOAD_TRANS_CMD, MCU_TX_VER, 0);
                }
            }
        break;
#endif
        
#ifdef MODULE_EXPANDING_SERVICE_ENABLE
        case MODULE_EXTEND_FUN_CMD:                             //模块拓展服务
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            open_module_time_serve_result((unsigned char *)(wifi_data_process_buf + offset + DATA_START), total_len);
        break;
#endif

#ifdef BLE_RELATED_FUNCTION_ENABLE
        case BLE_TEST_CMD:                                      //蓝牙功能性测试（扫描指定蓝牙信标）
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            BLE_test_result((unsigned char *)(wifi_data_process_buf + offset + DATA_START), total_len);
        break;
#endif

            
#ifdef VOICE_MODULE_PROTOCOL_ENABLE
        case GET_VOICE_STATE_CMD:                               //获取语音状态码
            result = wifi_data_process_buf[offset + DATA_START];
            get_voice_state_result(result);
        break;
        case MIC_SILENCE_CMD:                                   //MIC静音设置
            result = wifi_data_process_buf[offset + DATA_START];
            set_voice_MIC_silence_result(result);
        break;
        case SET_SPEAKER_VOLUME_CMD:                            //speaker音量设置
            result = wifi_data_process_buf[offset + DATA_START];
            set_speaker_voice_result(result);
        break;
        case VOICE_TEST_CMD:                                    //语音模组音频产测
            result = wifi_data_process_buf[offset + DATA_START];
            voice_test_result(result);
        break;
        case VOICE_AWAKEN_TEST_CMD:                             //语音模组唤醒产测
            result = wifi_data_process_buf[offset + DATA_START];
            voice_awaken_test_result(result);
        break;
        case VOICE_EXTEND_FUN_CMD:                              //语音模组扩展功能
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            voice_module_extend_fun((unsigned char *)(wifi_data_process_buf + offset + DATA_START), total_len);
        break;
#endif
        

        default:break;
    }
}

/**
 * @brief  判断串口接收缓存中是否有数据
 * @param  Null
 * @return 是否有数据
 */
unsigned char with_data_rxbuff(void)
{
    if(rx_buf_in != rx_buf_out)
        return 1;
    else
        return 0;
}

/**
 * @brief  读取队列1字节数据
 * @param  Null
 * @return Read the data
 */
unsigned char take_byte_rxbuff(void)
{
    unsigned char value;
    
    if(rx_buf_out != rx_buf_in) {
        //有数据
        if(rx_buf_out >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf))) {
            //数据已经到末尾
            rx_buf_out = (unsigned char *)(wifi_uart_rx_buf);
        }
        
        value = *rx_buf_out ++;   
    }
    
    return value;
}

