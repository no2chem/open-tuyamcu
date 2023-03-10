/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2020, 涂鸦科技
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    mcu_api.h
 * @author  涂鸦综合协议开发组
 * @version v2.5.6
 * @date    2020.12.16
 * @brief   用户需要主动调用的函数都在该文件内
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#ifndef __MCU_API_H_
#define __MCU_API_H_


#ifdef MCU_API_GLOBAL
  #define MCU_API_EXTERN
#else
  #define MCU_API_EXTERN   extern
#endif

/**
 * @brief  hex转bcd
 * @param[in] {Value_H} 高字节
 * @param[in] {Value_L} 低字节
 * @return 转换完成后数据
 */
unsigned char hex_to_bcd(unsigned char Value_H,unsigned char Value_L);

/**
 * @brief  求字符串长度
 * @param[in] {str} 字符串地址
 * @return 数据长度
 */
unsigned long my_strlen(unsigned char *str);

/**
 * @brief  把src所指内存区域的前count个字节设置成字符c
 * @param[out] {src} 待设置的内存首地址
 * @param[in] {ch} 设置的字符
 * @param[in] {count} 设置的内存长度
 * @return 待设置的内存首地址
 */
void *my_memset(void *src,unsigned char ch,unsigned short count);

/**
 * @brief  内存拷贝
 * @param[out] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {count} 拷贝数据个数
 * @return 数据处理完后的源地址
 */
void *my_memcpy(void *dest, const void *src, unsigned short count);

/**
 * @brief  字符串拷贝
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @return 数据处理完后的源地址
 */
char *my_strcpy(char *dest, const char *src);

/**
 * @brief  字符串比较
 * @param[in] {s1} 字符串 1
 * @param[in] {s2} 字符串 2
 * @return 大小比较值
 * -         0:s1=s2
 * -         <0:s1<s2
 * -         >0:s1>s2
 */
int my_strcmp(char *s1 , char *s2);

/**
 * @brief  将int类型拆分四个字节
 * @param[in] {number} 4字节原数据
 * @param[out] {value} 处理完成后4字节数据
 * @return Null
 */
void int_to_byte(unsigned long number,unsigned char value[4]);

/**
 * @brief  将4字节合并为1个32bit变量
 * @param[in] {value} 4字节数组
 * @return 合并完成后的32bit变量
 */
unsigned long byte_to_int(const unsigned char value[4]);

/**
 * @brief  raw型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_raw_update(unsigned char dpid,const unsigned char value[],unsigned short len);

/**
 * @brief  bool型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_bool_update(unsigned char dpid,unsigned char value);

/**
 * @brief  value型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_value_update(unsigned char dpid,unsigned long value);

/**
 * @brief  string型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_string_update(unsigned char dpid,const unsigned char value[],unsigned short len);

/**
 * @brief  enum型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_enum_update(unsigned char dpid,unsigned char value);

/**
 * @brief  fault型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_fault_update(unsigned char dpid,unsigned long value);

#ifdef MCU_DP_UPLOAD_SYN
/**
 * @brief  raw型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_raw_update_syn(unsigned char dpid,const unsigned char value[],unsigned short len);

/**
 * @brief  bool型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_bool_update_syn(unsigned char dpid,unsigned char value);

/**
 * @brief  value型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_value_update_syn(unsigned char dpid,unsigned long value);

/**
 * @brief  string型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_string_update_syn(unsigned char dpid,const unsigned char value[],unsigned short len);

/**
 * @brief  enum型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_enum_update_syn(unsigned char dpid,unsigned char value);

/**
 * @brief  fault型dp数据同步上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_fault_update_syn(unsigned char dpid,unsigned long value);
#endif

/**
 * @brief  mcu获取bool型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len);

/**
 * @brief  mcu获取enum型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned char mcu_get_dp_download_enum(const unsigned char value[],unsigned short len);

/**
 * @brief  mcu获取value型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len);

/**
 * @brief  串口接收数据暂存处理
 * @param[in] {value} 串口收到的1字节数据
 * @return Null
 * @note   在MCU串口处理函数中调用该函数,并将接收到的数据作为参数传入
 */
void uart_receive_input(unsigned char value);

/**
 * @brief  串口接收多个字节数据暂存处理
 * @param[in] {value} 串口要接收的数据的源地址
 * @param[in] {data_len} 串口要接收的数据的数据长度
 * @return Null
 * @note   如需要支持一次多字节缓存，可调用该函数
 */
void uart_receive_buff_input(unsigned char value[], unsigned short data_len);

/**
 * @brief  wifi串口数据处理服务
 * @param  Null
 * @return Null
 * @note   在MCU主函数while循环中调用该函数
 */
void wifi_uart_service(void);

/**
 * @brief  协议串口初始化函数
 * @param  Null
 * @return Null
 * @note   在MCU初始化代码中调用该函数
 */
void wifi_protocol_init(void);

#ifndef WIFI_CONTROL_SELF_MODE
/**
 * @brief  MCU获取复位wifi成功标志
 * @param  Null
 * @return 复位标志
 * -           0(RESET_WIFI_ERROR):失败
 * -           1(RESET_WIFI_SUCCESS):成功
 * @note   1:MCU主动调用mcu_reset_wifi()后调用该函数获取复位状态
 *         2:如果为模块自处理模式,MCU无须调用该函数
 */
unsigned char mcu_get_reset_wifi_flag(void);

/**
 * @brief  MCU主动重置wifi工作模式
 * @param  Null
 * @return Null
 * @note   1:MCU主动调用,通过mcu_get_reset_wifi_flag()函数获取重置wifi是否成功
 *         2:如果为模块自处理模式,MCU无须调用该函数
 */
void mcu_reset_wifi(void);

/**
 * @brief  获取设置wifi状态成功标志
 * @param  Null
 * @return wifimode flag
 * -           0(SET_WIFICONFIG_ERROR):失败
 * -           1(SET_WIFICONFIG_SUCCESS):成功
 * @note   1:MCU主动调用mcu_set_wifi_mode()后调用该函数获取复位状态
 *         2:如果为模块自处理模式,MCU无须调用该函数
 */
unsigned char mcu_get_wifimode_flag(void);

/**
 * @brief  MCU设置wifi工作模式
 * @param[in] {mode} 进入的模式
 * @ref        0(SMART_CONFIG):进入smartconfig模式
 * @ref        1(AP_CONFIG):进入AP模式
 * @return Null
 * @note   1:MCU主动调用
 *         2:成功后,可判断set_wifi_config_state是否为TRUE;TRUE表示为设置wifi工作模式成功
 *         3:如果为模块自处理模式,MCU无须调用该函数
 */
void mcu_set_wifi_mode(unsigned char mode);

/**
 * @brief  MCU主动获取当前wifi工作状态
 * @param  Null
 * @return wifi work state
 * -          SMART_CONFIG_STATE: smartconfig配置状态
 * -          AP_STATE: AP配置状态
 * -          WIFI_NOT_CONNECTED: WIFI配置成功但未连上路由器
 * -          WIFI_CONNECTED: WIFI配置成功且连上路由器
 * -          WIFI_CONN_CLOUD: WIFI已经连接上云服务器
 * -          WIFI_LOW_POWER: WIFI处于低功耗模式
 * -          SMART_AND_AP_STATE: WIFI smartconfig&AP 模式
 * @note   如果为模块自处理模式,MCU无须调用该函数
 */
unsigned char mcu_get_wifi_work_state(void);
#endif

#ifdef SUPPORT_GREEN_TIME
/**
 * @brief  MCU获取格林时间
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_get_green_time(void);
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
/**
 * @brief  MCU获取系统时间,用于校对本地时钟
 * @param  Null
 * @return Null
 * @note   MCU主动调用完成后在mcu_write_rtctime函数内校对rtc时钟
 */
void mcu_get_system_time(void);
#endif

#ifdef WIFI_TEST_ENABLE
/**
 * @brief  mcu发起wifi功能测试
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_start_wifitest(void);
#endif

#ifdef WIFI_HEARTSTOP_ENABLE
/**
 * @brief  通知WIFI模组关闭心跳
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void wifi_heart_stop(void);
#endif

#ifdef GET_WIFI_STATUS_ENABLE
/**
 * @brief  获取当前wifi联网状态
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_get_wifi_connect_status(void);
#endif

#ifdef WIFI_STREAM_ENABLE
/**
 * @brief  流服务发送
 * @param[in] {id} ID号
 * @param[in] {buffer} 发送包的地址
 * @param[in] {buf_len} 发送包长度
 * @return 流服务传输结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   MCU需要自行实现该功能
 */
unsigned char stream_trans_send(unsigned int id, unsigned char *buffer, unsigned long buf_len);

/**
 * @brief  多地图流服务发送
 * @param[in] {id} 地图流服务会话ID
 * @param[in] {sub_id} 子地图ID
 * @param[in] {sub_id_pro_mode} 子地图ID数据处理方式
 * @ref       0x00: 继续累加
 * @ref       0x01: 清除上传的数据
 * @param[in] {buffer} 数据包发送的地址
 * @param[in] {buf_len} 数据包发送的长度
 * @return 流服务传输结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   MCU需要自行实现该功能
 */
unsigned char maps_stream_trans_send(unsigned int id, unsigned char sub_id, unsigned char sub_id_pro_mode, unsigned char *buffer, unsigned long buf_len);
#endif

#ifdef WIFI_CONNECT_TEST_ENABLE
/**
 * @brief  mcu发起wifi功能测试(连接指定路由)
 * @param[in] {ssid_buf} 存放路由器名称字符串数据的地址(ssid长度最大支持32个字节)
 * @param[in] {passwd_buffer} 存放路由器名称字符串数据的地址(passwd长度最大支持64个字节)
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_start_connect_wifitest(unsigned char *ssid_buf,unsigned char *passwd_buffer);
#endif

#ifdef GET_MODULE_MAC_ENABLE
/**
 * @brief  获取模块MAC
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_get_module_mac(void);
#endif

#ifdef IR_TX_RX_TEST_ENABLE
/**
 * @brief  mcu发起红外进入收发产测
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_start_ir_test(void);
#endif

#ifdef MODULE_EXPANDING_SERVICE_ENABLE
/**
 * @brief  打开模块时间服务通知
 * @param[in] {time_type} 时间类型
 * @ref       0x00: 格林时间
 * @ref       0x01: 本地时间
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void open_module_time_serve(unsigned char time_type);

/**
 * @brief  主动请求天气服务数据
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void request_weather_serve(void);

/**
 * @brief  打开模块重置状态通知
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void open_module_reset_state_serve(void);
#endif

#ifdef BLE_RELATED_FUNCTION_ENABLE
/**
 * @brief  mcu发起蓝牙功能性测试（扫描指定蓝牙信标）
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void mcu_start_BLE_test(void);
#endif

#ifdef VOICE_MODULE_PROTOCOL_ENABLE
/**
 * @brief  获取语音状态码
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void get_voice_state(void);

/**
 * @brief  MIC静音设置
 * @param[in] {set_val} 静音设置值
 * @ref       0x00: mic开启
 * @ref       0x01: mic静音
 * @ref       0xA0: 查询静音状态
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void set_voice_MIC_silence(unsigned char set_val);

/**
 * @brief  speaker音量设置
 * @param[in] {set_val} 音量设置值
 * @ref       0~10: 音量范围
 * @ref       0xA0: 查询音量值
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void set_speaker_voice(unsigned char set_val);

/**
 * @brief  音频产测
 * @param[in] {set_val} 音频产测值
 * @ref       0x00: 关闭音频产测
 * @ref       0x01: mic1音频环路测试
 * @ref       0x02: mic2音频环路测试
 * @ref       0xA0: 查询当前产测状态
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void voice_test(unsigned char test_val);

/**
 * @brief  唤醒产测
 * @param  Null
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void voice_awaken_test(void);

/**
 * @brief  语音模组MCU功能设置
 * @param[in] {play} 播放/暂停功能 1(播放) / 0(暂停)
 * @param[in] {bt_play} 蓝牙开关功能 1(开) / 0(关)
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void voice_mcu_fun_set(unsigned char play, unsigned char bt_play);
#endif

#endif