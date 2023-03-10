/**********************************Copyright (c)**********************************
**                       版权所有 (C), 2015-2020, 涂鸦科技
**
**                             http://www.tuya.com
**
*********************************************************************************/
/**
 * @file    protocol.h
 * @author  涂鸦综合协议开发组
 * @version v2.5.6
 * @date    2020.12.16
 * @brief                
 *                       *******非常重要，一定要看哦！！！********
 *          1. 用户在此文件中实现数据下发/上报功能
 *          2. DP的ID/TYPE及数据处理函数都需要用户按照实际定义实现
 *          3. 当开始某些宏定义后需要用户实现代码的函数内部有#err提示,完成函数后请删除该#err
 */

/****************************** 免责声明 ！！！ *******************************
由于MCU类型和编译环境多种多样，所以此代码仅供参考，用户请自行把控最终代码质量，
涂鸦不对MCU功能结果负责。
******************************************************************************/

#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_


/******************************************************************************
                           用户相关信息配置
******************************************************************************/
/******************************************************************************
                       1:修改产品信息
******************************************************************************/
#define PRODUCT_KEY "s9wslmatgmp4dvms"    //开发平台创建产品后生成的16位字符产品唯一标识

#define MCU_VER "1.0.0"         //用户的软件版本,用于MCU固件升级,MCU升级版本需修改

/*  模块工作方式选择,只能三选一,推荐使用防误触模式  */
//#define CONFIG_MODE     CONFIG_MODE_DEFAULT             //默认工作模式
//#define CONFIG_MODE     CONFIG_MODE_LOWPOWER            //安全模式 (低功耗配网方式)
#define CONFIG_MODE     CONFIG_MODE_SPECIAL             //防误触模式(特殊配网方式)

/*  设置低功耗配网方式和特殊配网方式的配网模式打开时间,该宏处于注释状态将按三分钟处理,可以支持的设置数据范围: 3~10 分钟  */
//#define CONFIG_MODE_DELAY_TIME    10         //配网模式打开时间 单位:分钟

/*  选择smart模式和AP模式,该宏都注释将保持smart模式和AP模式互相切换  */
//#define CONFIG_MODE_CHOOSE        0         //模块同时支持AP连接配网和EZ配网无需用户切换,对应的配网状态0x06
//#define CONFIG_MODE_CHOOSE        1         //仅只有AP配网模式

/*  启用模块的红外功能并告知模块红外的收发脚使用那些IO口，没有该字段红外能力默认关闭  */
//#define ENABLE_MODULE_IR_FUN                  //启用模块的红外功能
#ifdef ENABLE_MODULE_IR_FUN
#define MODULE_IR_PIN_TX      5       //红外发送脚
#define MODULE_IR_PIN_RX      12      //红外接收脚
#endif

/*  模块是否开启保持长连接的低功耗模式，没有该字段低功耗模式默认关闭  */
//#define LONG_CONN_LOWPOWER        0         //关闭低功耗模式
//#define LONG_CONN_LOWPOWER        1         //打开低功耗模式

/******************************************************************************
                          2:MCU是否需要支固件升级
如需要支持MCU固件升级,请开启该宏
MCU可调用mcu_api.c文件内的mcu_firm_update_query()函数获取当前MCU固件更新情况
                        ********WARNING!!!**********
当前接收缓冲区为关闭固件更新功能的大小,固件升级包可选择，默认256字节大小
如需要开启该功能,串口接收缓冲区会变大
******************************************************************************/
//#define         SUPPORT_MCU_FIRM_UPDATE                 //开启MCU固件升级功能(默认关闭)
/*  Firmware package size selection  */
#ifdef SUPPORT_MCU_FIRM_UPDATE
#define PACKAGE_SIZE                   0        //包大小为256字节
//#define PACKAGE_SIZE                   1        //包大小为512字节
//#define PACKAGE_SIZE                   2        //包大小为1024字节
#endif
/******************************************************************************
                         3:定义收发缓存:
                    如当前使用MCU的RAM不够,可修改为24
******************************************************************************/
#ifndef SUPPORT_MCU_FIRM_UPDATE
#define WIFI_UART_RECV_BUF_LMT          16              //串口数据接收缓存区大小,如MCU的RAM不够,可缩小
#define WIFI_DATA_PROCESS_LMT           24              //串口数据处理缓存区大小,根据用户DP数据大小量定,必须大于24
#else
#define WIFI_UART_RECV_BUF_LMT          128             //串口数据接收缓存区大小,如MCU的RAM不够,可缩小

//请在此处选择合适的串口数据处理缓存大小（根据上面MCU固件升级包选择的大小和是否开启天气服务来选择开启多大的缓存）
#define WIFI_DATA_PROCESS_LMT           300             //串口数据处理缓存大小,如需MCU固件升级,若单包大小选择256,则缓存必须大于260,若开启天气服务,则需要更大
//#define WIFI_DATA_PROCESS_LMT           600             //串口数据处理缓存大小,如需MCU固件升级,若单包大小选择512,则缓存必须大于520,若开启天气服务,则需要更大
//#define WIFI_DATA_PROCESS_LMT           1200            //串口数据处理缓存大小,如需MCU固件升级,若单包大小选择1024,则缓存必须大于1030,若开启天气服务,则需要更大

#endif

#define WIFIR_UART_SEND_BUF_LMT         48              //根据用户DP数据大小量定,必须大于48
/******************************************************************************
                        4:定义模块工作方式
模块自处理:
          wifi指示灯和wifi复位按钮接在wifi模块上(开启WIFI_CONTROL_SELF_MODE宏)
          并正确定义WF_STATE_KEY和WF_RESET_KEY
MCU自处理:
          wifi指示灯和wifi复位按钮接在MCU上(关闭WIFI_CONTROL_SELF_MODE宏)
          MCU在需要处理复位wifi的地方调用mcu_api.c文件内的mcu_reset_wifi()函数,并可调用mcu_get_reset_wifi_flag()函数返回复位wifi结果
          或调用设置wifi模式mcu_api.c文件内的mcu_set_wifi_mode(WIFI_CONFIG_E mode)函数,并可调用mcu_get_wifi_work_state()函数返回设置wifi结果
******************************************************************************/
//#define         WIFI_CONTROL_SELF_MODE                       //wifi自处理按键及LED指示灯;如为MCU外界按键/LED指示灯请关闭该宏
#ifdef          WIFI_CONTROL_SELF_MODE                      //模块自处理
  #define     WF_STATE_KEY            14                    //wifi模块状态指示按键，请根据实际GPIO管脚设置
  #define     WF_RESERT_KEY           0                     //wifi模块重置按键，请根据实际GPIO管脚设置
#endif

/******************************************************************************
                      5:MCU是否需要支持校时功能
如需要请开启该宏,并在Protocol.c文件内mcu_write_rtctime实现代码
mcu_write_rtctime内部有#err提示,完成函数后请删除该#err
mcu在wifi模块正确联网后可调用mcu_get_system_time()函数发起校时功能
******************************************************************************/
//#define         SUPPORT_MCU_RTC_CHECK                //开启校时功能

/******************************************************************************
                      6:MCU是否需要支持wifi功能测试
如需要请开启该宏,并且mcu在需要wifi功能测试处调用mcu_api.c文件内mcu_start_wifitest
并在protocol.c文件wifi_test_result函数内查看测试结果,
wifi_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         WIFI_TEST_ENABLE                //开启WIFI产测功能（扫描指定路由）

/******************************************************************************
                      7:是否开启天气功能
如需要请开启该宏,并在protocol.c文件内weather_open_return_handle与weather_data_user_handle两个用户处理函数中实现显示等代码
此两函数内#err提示,完成函数后请删除该#err
开启天气功能，串口数据缓存区的大小要开大一些
******************************************************************************/
//#define         WEATHER_ENABLE                  //打开天气功能
#ifdef          WEATHER_ENABLE
/*  在protocol.c文件中weather_choose数组中可调整，然后将打开服务的类型数目写到此宏定义  */
#define         WEATHER_CHOOSE_CNT              4   //选择的需要天气服务类型的数目
/*  在打开天气服务时，可以设置此宏定义选择天气预报的天数，1表示当天天气(不需要预报可设置为1)，最多为7天(不可以设置成0或大于7)  */
#define         WEATHER_FORECAST_DAYS_NUM       1   //设置天气预报的天数
#endif

/******************************************************************************
                      8:是否开启WIFI模组心跳关闭功能
如需要请开启该宏,调用mcu_api.c文件内wifi_heart_stop函数即可停止心跳
******************************************************************************/
//#define         WIFI_HEARTSTOP_ENABLE           //开启心跳停止功能

/******************************************************************************
                      9:是否支持流服务功能
STREM_PACK_LEN为流服务传输一包的大小，目前模块串口最大可以缓存的数据部分可以达到 1024 字节，一包地图 
数据包数据部分不能超过 1024 字节，每包地图数据内容建议 512 字节每包。
******************************************************************************/
//#define         WIFI_STREAM_ENABLE              //支持流服务相关功能
#ifdef WIFI_STREAM_ENABLE
#define         STREM_PACK_LEN                 256
#endif

/******************************************************************************
                      10:MCU是否需要支持wifi功能测试(连接指定路由)
如需要请开启该宏,并且mcu在需要wifi功能测试处调用mcu_api.c文件内mcu_start_connect_wifitest
并在protocol.c文件wifi_connect_test_result函数内查看测试结果,
wifi_connect_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         WIFI_CONNECT_TEST_ENABLE                //开启WIFI产测功能（连接指定路由）

/******************************************************************************
                      11:MCU是否需要开启获取当前WIFI联网状态功能
如需要请开启该宏,并且mcu在需要获取当前WIFI联网状态处调用mcu_api.c文件内mcu_get_wifi_connect_status
并在protocol.c文件wifi_test_result函数内查看结果,
wifi_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         GET_WIFI_STATUS_ENABLE                  //开启获取当前WIFI联网状态功能

/******************************************************************************
                      12:MCU是否需要开启获取模块mac地址功能
如需要请开启该宏,并且mcu在需要获取模块mac地址处调用mcu_api.c文件内mcu_get_module_mac
并在protocol.c文件mcu_get_mac函数内查看结果,
mcu_get_mac内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         GET_MODULE_MAC_ENABLE                   //开启获取模块mac地址功能

/******************************************************************************
                      13:MCU是否需要支持获取格林时间功能
如需要请开启该宏,并且mcu在需要获取格林时间处调用mcu_api.c文件内mcu_get_green_time
并在protocol.c文件mcu_get_greentime函数内查看结果,
mcu_get_greentime内部有#err提示,完成函数后请删除该#err
mcu在wifi模块正确联网后可调用mcu_get_green_time()函数发起校时功能
******************************************************************************/
//#define         SUPPORT_GREEN_TIME                //开启格林时间功能

/******************************************************************************
                      14:MCU是否需要开启同步状态上报功能
1) 此命令为同步指令，MCU 数据状态上报后，需要等待模块返回结果；
2) 每次发送模组都会有响应，WIFI 模组未响应前不可多次上报；
3) 网络不好，数据难以及时上报时，模块会在 5 后返回失败，MCU 需要等待大于 5 秒。
******************************************************************************/
//#define         MCU_DP_UPLOAD_SYN                   //开启同步状态上报功能

/******************************************************************************
                      15:MCU是否需要开启红外状态通知功能
如需要请开启该宏,
并在protocol.c文件get_ir_status函数内查看结果并回复,
get_ir_status内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         GET_IR_STATUS_ENABLE                   //开启红外状态通知功能

/******************************************************************************
                      16:MCU是否需要开启红外进入收发产测功能
如需要请开启该宏,并且mcu在需要开启红外进入收发产测处调用mcu_api.c文件内mcu_start_ir_test
并在protocol.c文件ir_tx_rx_test_result函数内查看测试结果,
ir_tx_rx_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         IR_TX_RX_TEST_ENABLE                   //开启红外进入收发产测功能

/******************************************************************************
                      17:文件包下载功能
如需要请开启该宏,并且需要选择包大小
并在protocol.c文件file_download_handle函数处理数据,
file_download_handle内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         FILE_DOWNLOAD_ENABLE                   //开启文件包下载功能
//文件下载包大小选择
#ifdef FILE_DOWNLOAD_ENABLE
#define FILE_DOWNLOAD_PACKAGE_SIZE                   0        //包大小为256字节
//#define FILE_DOWNLOAD_PACKAGE_SIZE                   1        //包大小为512字节
//#define FILE_DOWNLOAD_PACKAGE_SIZE                   2        //包大小为1024字节
#endif

/******************************************************************************
                      18:MCU是否需要支持语音模组相关协议功能
此协议只适用语音模组VWXR2的通用对接，其他非语音模组的通用固件没有本目录下相关协议功能
如需要请开启该宏,并且mcu在需要语音模组相关协议功能处调用mcu_api.c文件内
get_voice_state/set_voice_MIC_silence/set_speaker_voice/voice_test/voice_awaken_test这几个函数
并在protocol.c文件相关结果处理函数内查看结果,
结果处理函数内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         VOICE_MODULE_PROTOCOL_ENABLE           //开启语音模组相关协议功能

/******************************************************************************
                      19:MCU是否需要支持模块拓展服务功能
如需要请开启该宏,并且mcu在需要模块拓展服务处调用mcu_api.c文件内open_module_time_serve
并在protocol.c文件open_module_time_serve_result函数内查看结果,
open_module_time_serve_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         MODULE_EXPANDING_SERVICE_ENABLE        //开启模块拓展服务功能

/******************************************************************************
                      20:MCU是否需要支持蓝牙相关功能
如需要请开启该宏,并且mcu在需要蓝牙相关功能处调用mcu_api.c文件内mcu_start_BLE_test
并在protocol.c文件BLE_test_result函数内查看测试结果,
BLE_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
//#define         BLE_RELATED_FUNCTION_ENABLE            //开启蓝牙相关功能




/******************************************************************************
                        1:dp数据点序列号重新定义
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/

//方向(可下发可上报)
#define DPID_DIRECTION_CONTROL             101

//刀片位置(可下发可上报)
#define DPID_BLADE_POSITION_SET            102

//刀片速度(可下发可上报)
#define DPID_BLADE_SPEED_SET               103

//范围长度
#define DPID_LENGTH                        104

//范围宽度
#define DPID_WIDTH                         105

//弓字模式开关
#define DPID_BOW_MODE_SWITCH               106

/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
void uart_transmit_output(unsigned char value);

/**
 * @brief  系统所有dp点信息上传,实现APP和muc数据同步
 * @param  Null
 * @return Null
 * @note   MCU必须实现该函数内数据上报功能
 */
void all_data_update(void);

/**
 * @brief  dp下发处理函数
 * @param[in] {dpid} dpid 序号
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {length} dp数据长度
 * @return dp处理结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   该函数用户不能修改
 */
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length);

/**
 * @brief  获取所有dp命令总和
 * @param[in] Null
 * @return 下发命令总和
 * @note   该函数用户不能修改
 */
unsigned char get_download_cmd_total(void);



#ifdef SUPPORT_MCU_FIRM_UPDATE
/**
 * @brief  升级包大小选择
 * @param[in] {package_sz} 升级包大小
 * @ref           0x00: 256byte (默认)
 * @ref           0x01: 512byte
 * @ref           0x02: 1024byte
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void upgrade_package_choose(unsigned char package_sz);

/**
 * @brief  MCU进入固件升级模式
 * @param[in] {value} 固件缓冲区
 * @param[in] {position} 当前数据包在于固件位置
 * @param[in] {length} 当前固件包长度(固件包长度为0时,表示固件包发送完成)
 * @return Null
 * @note   MCU需要自行实现该功能
 */
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length);
#endif

#ifdef SUPPORT_GREEN_TIME
/**
 * @brief  获取到的格林时间
 * @param[in] {time} 获取到的格林时间数据
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_get_greentime(unsigned char time[]);
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
/**
 * @brief  MCU校对本地RTC时钟
 * @param[in] {time} 获取到的格林时间数据
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_write_rtctime(unsigned char time[]);
#endif

#ifdef WIFI_TEST_ENABLE
/**
 * @brief  wifi功能测试反馈
 * @param[in] {result} wifi功能测试结果
 * @ref       0: 失败
 * @ref       1: 成功
 * @param[in] {rssi} 测试成功表示wifi信号强度/测试失败表示错误类型
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void wifi_test_result(unsigned char result,unsigned char rssi);
#endif

#ifdef WEATHER_ENABLE
/**
* @brief  mcu打开天气服务
 * @param  Null
 * @return Null
 */
void mcu_open_weather(void);

/**
 * @brief  打开天气功能返回用户自处理函数
 * @param[in] {res} 打开天气功能返回结果
 * @ref       0: 失败
 * @ref       1: 成功
 * @param[in] {err} 错误码
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void weather_open_return_handle(unsigned char res, unsigned char err);

/**
 * @brief  天气数据用户自处理函数
 * @param[in] {name} 参数名
 * @param[in] {type} 参数类型
 * @ref       0: int 型
 * @ref       1: string 型
 * @param[in] {data} 参数值的地址
 * @param[in] {day} 哪一天的天气  0:表示当天 取值范围: 0~6
 * @ref       0: 今天
 * @ref       1: 明天
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void weather_data_user_handle(char *name, unsigned char type, const unsigned char *data, char day);
#endif

#ifdef MCU_DP_UPLOAD_SYN
/**
 * @brief  状态同步上报结果
 * @param[in] {result} 结果
 * @ref       0: 失败
 * @ref       1: 成功
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_upload_syn_result(unsigned char result);
#endif

#ifdef GET_WIFI_STATUS_ENABLE
/**
 * @brief  获取 WIFI 状态结果
 * @param[in] {result} 指示 WIFI 工作状态
 * @ref       0x00: wifi状态 1 smartconfig 配置状态
 * @ref       0x01: wifi状态 2 AP 配置状态
 * @ref       0x02: wifi状态 3 WIFI 已配置但未连上路由器
 * @ref       0x03: wifi状态 4 WIFI 已配置且连上路由器
 * @ref       0x04: wifi状态 5 已连上路由器且连接到云端
 * @ref       0x05: wifi状态 6 WIFI 设备处于低功耗模式
 * @ref       0x06: wifi状态 7 WIFI 设备处于smartconfig&AP配置状态
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_wifi_status(unsigned char result);
#endif

#ifdef WIFI_STREAM_ENABLE
/**
 * @brief  流服务发送结果
 * @param[in] {result} 结果
 * @ref       0x00: 成功
 * @ref       0x01: 流服务功能未开启
 * @ref       0x02: 流服务器未连接成功
 * @ref       0x03: 数据推送超时
 * @ref       0x04: 传输的数据长度错误
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void stream_trans_send_result(unsigned char result);

/**
 * @brief  多地图流服务发送结果
 * @param[in] {result} 结果
 * @ref       0x00: 成功
 * @ref       0x01: 失败
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void maps_stream_trans_send_result(unsigned char result);
#endif

#ifdef WIFI_CONNECT_TEST_ENABLE
/**
 * @brief  路由信息接收结果通知
 * @param[in] {result} 模块是否成功接收到正确的路由信息
 * @ref       0x00: 失败
 * @ref       0x01: 成功
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void wifi_connect_test_result(unsigned char result);
#endif

#ifdef GET_MODULE_MAC_ENABLE
/**
 * @brief  获取模块mac结果
 * @param[in] {mac} 模块 MAC 数据
 * @ref       mac[0]: 为是否获取mac成功标志，0x00 表示成功，0x01 表示失败
 * @ref       mac[1]~mac[6]: 当获取 MAC地址标志位如果mac[0]为成功，则表示模块有效的MAC地址
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void mcu_get_mac(unsigned char mac[]);
#endif

#ifdef GET_IR_STATUS_ENABLE
/**
 * @brief  获取红外状态结果
 * @param[in] {result} 指示红外状态
 * @ref       0x00: 红外状态 1 正在发送红外码
 * @ref       0x01: 红外状态 2 发送红外码结束
 * @ref       0x02: 红外状态 3 红外学习开始
 * @ref       0x03: 红外状态 4 红外学习结束
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_ir_status(unsigned char result);
#endif

#ifdef IR_TX_RX_TEST_ENABLE
/**
 * @brief  红外进入收发产测结果通知
 * @param[in] {result} 模块是否成功接收到正确的信息
 * @ref       0x00: 失败
 * @ref       0x01: 成功
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void ir_tx_rx_test_result(unsigned char result);
#endif

#ifdef FILE_DOWNLOAD_ENABLE
/**
 * @brief  文件下载包大小选择
 * @param[in] {package_sz} 文件下载包大小
 * @ref       0x00: 256 byte (默认)
 * @ref       0x01: 512 byte
 * @ref       0x02: 1024 byte
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void file_download_package_choose(unsigned char package_sz);

/**
 * @brief  文件包下载模式
 * @param[in] {value} 数据缓冲区
 * @param[in] {position} 当前数据包在于文件位置
 * @param[in] {length} 当前文件包长度(长度为0时,表示文件包发送完成)
 * @return 数据处理结果
 * -           0(ERROR): 失败
 * -           1(SUCCESS): 成功
 * @note   MCU需要自行实现该功能
 */
unsigned char file_download_handle(const unsigned char value[],unsigned long position,unsigned short length);
#endif

#ifdef MODULE_EXPANDING_SERVICE_ENABLE
/**
 * @brief  打开模块时间服务通知结果
 * @param[in] {value} 数据缓冲区
 * @param[in] {length} 数据长度
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void open_module_time_serve_result(const unsigned char value[], unsigned short length);
#endif

#ifdef BLE_RELATED_FUNCTION_ENABLE
/**
 * @brief  蓝牙功能性测试结果
 * @param[in] {value} 数据缓冲区
 * @param[in] {length} 数据长度
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void BLE_test_result(const unsigned char value[], unsigned short length);
#endif

#ifdef VOICE_MODULE_PROTOCOL_ENABLE
/**
 * @brief  获取语音状态码结果
 * @param[in] {result} 语音状态码
 * @ref       0x00: 空闲
 * @ref       0x01: mic静音状态
 * @ref       0x02: 唤醒
 * @ref       0x03: 正在录音
 * @ref       0x04: 正在识别
 * @ref       0x05: 识别成功
 * @ref       0x06: 识别失败
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void get_voice_state_result(unsigned char result);

/**
 * @brief  MIC静音设置
 * @param[in] {result} 语音状态码
 * @ref       0x00: mic 开启
 * @ref       0x01: mic 静音
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void set_voice_MIC_silence_result(unsigned char result);

/**
 * @brief  speaker音量设置结果
 * @param[in] {result} 音量值
 * @ref       0~10: 音量范围
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void set_speaker_voice_result(unsigned char result);

/**
 * @brief  音频产测结果
 * @param[in] {result} 音频产测状态
 * @ref       0x00: 关闭音频产测
 * @ref       0x01: mic1音频环路测试
 * @ref       0x02: mic2音频环路测试
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void voice_test_result(unsigned char result);

/**
 * @brief  唤醒产测结果
 * @param[in] {result} 唤醒返回值
 * @ref       0x00: 唤醒成功
 * @ref       0x01: 唤醒失败(10s超时失败)
 * @return Null
 * @note   MCU需要自行调用该功能
 */
void voice_awaken_test_result(unsigned char result);

/**
 * @brief  语音模组扩展功能
 * @param[in] {value} 数据缓冲区
 * @param[in] {length} 数据长度
 * @return Null
 * @note   MCU需要自行实现该功能
 */
void voice_module_extend_fun(const unsigned char value[], unsigned short length);
#endif


#endif