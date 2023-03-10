/*SPDX-License-Identifier: MIT
Copyright (C) 2023 by Michael Wei                          
michael@wei.email

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "NuMicro.h"
#include "mcu_sdk/wifi.h"

#define DP_LED0             10
#define DP_LED1             11
#define DP_LED2             12
#define DP_LED3             13
#define DP_LED4             14
#define DP_LED5             15
#define DP_LED6             16
#define DP_LED_MAIN_W       17
#define DP_LED_MAIN_R       18

#define DP_LED_FLAGS        20

#define DP_RELAY            32

#define DP_TRIAC_DELAY      40
#define DP_TRIAC_ON_TM      41
#define DP_TRIAC_FORCE_ON   42

#define DP_TEMP             50

const DOWNLOAD_CMD_S download_cmd[] =
{
  {DP_LED0, DP_TYPE_BOOL},
  {DP_LED1, DP_TYPE_BOOL},
  {DP_LED2, DP_TYPE_BOOL},
  {DP_LED3, DP_TYPE_BOOL},
  {DP_LED4, DP_TYPE_BOOL},
  {DP_LED5, DP_TYPE_BOOL},
  {DP_LED6, DP_TYPE_BOOL},
  {DP_LED_MAIN_W, DP_TYPE_BOOL},
  {DP_LED_MAIN_R, DP_TYPE_BOOL},
  {DP_LED_FLAGS, DP_TYPE_VALUE},
  {DP_RELAY, DP_TYPE_BOOL},
  {DP_TRIAC_DELAY, DP_TYPE_VALUE},
  {DP_TRIAC_ON_TM, DP_TYPE_VALUE},
  {DP_TRIAC_FORCE_ON, DP_TYPE_BOOL},
};



void uart_transmit_output(unsigned char value)
{
    while(UART_IS_TX_FULL(UART0)); 
    UART_WRITE(UART0, value);
}

int triac_force_on = 0;

uint32_t TIMER_GetClock(TIMER_T *timer)
{
    uint32_t u32Clk = TIMER_GetModuleClock(timer);
    return(u32Clk / (timer->CMP * ((timer->CTL & 0xFF) + 1)));
}

void all_data_update(void)
{
    // Construct the LED flags first so we only have to read GPIOs once...
    long led_flags =  !PA0 << 8 | !PA1 << 7 | !PC1 << 6 | !PF3 << 5 | !PB1 << 4 |!PB12 << 3 | !PB13 << 2 | !PA13 << 1 | !PA12;
	mcu_dp_bool_update(DP_LED0, !!(led_flags & (1)));
    mcu_dp_bool_update(DP_LED1, !!(led_flags & (1 << 1)));
    mcu_dp_bool_update(DP_LED2, !!(led_flags & (1 << 2)));
    mcu_dp_bool_update(DP_LED3, !!(led_flags & (1 << 3)));
    mcu_dp_bool_update(DP_LED4, !!(led_flags & (1 << 4)));
    mcu_dp_bool_update(DP_LED5, !!(led_flags & (1 << 5)));
    mcu_dp_bool_update(DP_LED6, !!(led_flags & (1 << 6)));
    mcu_dp_bool_update(DP_LED_MAIN_W, !!(led_flags & (1 << 7)));
    mcu_dp_bool_update(DP_LED_MAIN_R, !!(led_flags & (1 << 8)));
    mcu_dp_value_update(DP_LED_FLAGS, led_flags);
    mcu_dp_bool_update(DP_RELAY, !PA3);
    mcu_dp_value_update(DP_TEMP, ADC_GET_CONVERSION_DATA(ADC, 14));
    mcu_dp_value_update(DP_TRIAC_DELAY, TIMER_GetClock(TIMER2));
    mcu_dp_value_update(DP_TRIAC_ON_TM, TIMER_GetClock(TIMER3));
    mcu_dp_value_update(DP_TRIAC_FORCE_ON, triac_force_on);
}


unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
    unsigned char ret = 0;
    switch(dpid) {
            case DP_LED0:
                PA12 = !value[0];
                break;
            case DP_LED1:
                PA13 = !value[0];
                break;
            case DP_LED2:
                PB13 = !value[0];
                break;
            case DP_LED3:
                PB12 = !value[0];
                break;
            case DP_LED4:
                PB1 = !value[0];
                break;
            case DP_LED5:
                PF3 = !value[0];
                break;
            case DP_LED6:
                PC1 = !value[0];
                break;
            case DP_LED_MAIN_W:
                PA1 = !value[0];
                break;
            case DP_LED_MAIN_R:
                PA0 = !value[0];
                break;
            case DP_LED_FLAGS:
                PA12 = !(value[3] & 1);
                PA13 = !(value[3] >> 1 & 1);
                PB13 = !(value[3] >> 2 & 1);
                PB12 = !(value[3] >> 3 & 1);
                PB1 = !(value[3] >> 4 & 1);
                PF3 = !(value[3] >> 5 & 1);
                PC1 = !(value[3] >> 6 & 1);
                PA1 = !(value[3] >> 7 & 1);
                PA0 = !(value[2] & 1);
                break;
            case DP_RELAY:
                PA3 = !value[0];
                break;
            case DP_TRIAC_FORCE_ON:
                triac_force_on = value[0];
                break;
            case DP_TRIAC_DELAY:
                TIMER_Open(TIMER2, TIMER_ONESHOT_MODE, value[0] << 24 | value[1] << 16 | value[2] << 8 | value[3]);
                break;
            case DP_TRIAC_ON_TM:
                TIMER_Open(TIMER3, TIMER_ONESHOT_MODE, value[0] << 24 | value[1] << 16 | value[2] << 8 | value[3]);
                break;
        default:
        break;
    }

    all_data_update();
    return ret;
}

unsigned char get_download_cmd_total(void)
{
    return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}

