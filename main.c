/*SPDX-License-Identifier: MIT
Copyright (C) 2023 by Michael Wei                          
michael@wei.email

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include "NuMicro.h"
#include "mcu_sdk/mcu_api.h"


#define PENDING_UPDATE_DP0 (1 << 0)
#define PENDING_UPDATE_DP1 (1 << 1)
#define PENDING_UPDATE_DP2 (1 << 2)

extern int triac_force_on;
uint8_t pending_updates = 0;

void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT5)) {
        // Start Zero-Crossing Delay
        TIMER_Start(TIMER2);
    }
    if (GPIO_GET_INT_FLAG(PB, BIT2))
    {
        pending_updates |= PENDING_UPDATE_DP0;
    }
    if (GPIO_GET_INT_FLAG(PA, BIT2)) {
       pending_updates |= PENDING_UPDATE_DP1;
    }

    // clear all PB interrupts.
    PA->INTSRC = 0xFFFFFFFF;
    PB->INTSRC = 0xFFFFFFFF;
}


void GPCDEF_IRQHandler(void)
{
    if(GPIO_GET_INT_FLAG(PF, BIT2))
    {
        GPIO_CLR_INT_FLAG(PF, BIT2);
        pending_updates |= PENDING_UPDATE_DP2;
    } 

    // clear all PF interrupts.
    PF->INTSRC = 0xFFFFFFFF;
}

void TMR2_IRQHandler(void)
{
    TIMER_Start(TIMER2);
    PB1 = 1;
    TIMER_ClearIntFlag(TIMER2);
}

void TMR3_IRQHandler(void)
{
    PB1 = triac_force_on ? 1 : 1;
    TIMER_ClearIntFlag(TIMER3);
}


void UART02_IRQHandler(void)
{
    if (UART_GET_INT_FLAG(UART0,UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))
    {
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
            uart_receive_input(UART_READ(UART0));	
        }
    }

    if (UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(UART0, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    } 
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock (Internal RC 48MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Select HCLK clock source as HIRC and HCLK clock divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set both PCLK0 and PCLK1 as HCLK */
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV1 | CLK_PCLKDIV_APB1DIV_DIV1;

    /* Select UART module clock source as HIRC and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable ADC module clock */
    CLK_EnableModuleClock(ADC_MODULE);

    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_PCLK1, 0);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_PCLK1, 0);

    /* ADC clock source is PCLK1, set divider to 1 */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL2_ADCSEL_PCLK1, CLK_CLKDIV0_ADC(256));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();

    // Configure MFP. See pinout descriptions in README.md for function configuration.

    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA1MFP_Msk | SYS_GPA_MFPL_PA2MFP_Msk | SYS_GPA_MFPL_PA3MFP_Msk)) |
                (SYS_GPA_MFPL_PA0MFP_GPIO | SYS_GPA_MFPL_PA1MFP_GPIO | SYS_GPA_MFPL_PA2MFP_GPIO | SYS_GPA_MFPL_PA3MFP_GPIO);

    SYS->GPA_MFPH = (SYS->GPA_MFPH & ~(SYS_GPA_MFPH_PA12MFP_Msk | SYS_GPA_MFPH_PA13MFP_Msk | SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk)) |
                (SYS_GPA_MFPH_PA12MFP_GPIO | SYS_GPA_MFPH_PA13MFP_GPIO | SYS_GPA_MFPH_PA14MFP_UART0_TXD | SYS_GPA_MFPH_PA15MFP_UART0_RXD);

    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk | SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk)) |
                (SYS_GPB_MFPL_PB0MFP_GPIO | SYS_GPB_MFPL_PB1MFP_GPIO | SYS_GPB_MFPL_PB2MFP_TM3 | SYS_GPB_MFPL_PB3MFP_GPIO  | SYS_GPB_MFPL_PB4MFP_GPIO | SYS_GPB_MFPL_PB5MFP_GPIO);

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk | SYS_GPB_MFPH_PB14MFP_Msk)) |
                (SYS_GPB_MFPH_PB12MFP_GPIO | SYS_GPB_MFPH_PB13MFP_GPIO | SYS_GPB_MFPH_PB14MFP_ADC0_CH14);

    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk)) |
                (SYS_GPC_MFPL_PC0MFP_GPIO | SYS_GPC_MFPL_PC1MFP_GPIO);

    SYS->GPF_MFPL = (SYS->GPF_MFPL & ~(SYS_GPF_MFPL_PF2MFP_Msk | SYS_GPF_MFPL_PF3MFP_Msk)) |
                (SYS_GPF_MFPL_PF2MFP_GPIO | SYS_GPF_MFPL_PF3MFP_GPIO);

}


int main()
{
    SYS_Init();

    wifi_protocol_init();

    // Configure the UART to 115200 baud. Set the RX IRQ FIFO threshold to 8 bytes (half).
    SYS_ResetModule(UART0_RST);
    UART_Open(UART0, 115200);
    UART0->FIFO = (UART0->FIFO & (~UART_FIFO_RFITL_Msk)) | UART_FIFO_RFITL_8BYTES;
    UART_SetTimeoutCnt(UART0, 0x3E);

    NVIC_EnableIRQ(UART02_IRQn);
    UART_EnableInt(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RLSIEN_Msk | UART_INTEN_RXTOIEN_Msk));

    // Configure GPIO, enable debounce and input IRQs.
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT3 | BIT12 | BIT13), GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, (BIT0 | BIT1 | BIT12 | BIT13), GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, (BIT1), GPIO_MODE_OUTPUT);
    GPIO_SetMode(PF, (BIT3), GPIO_MODE_OUTPUT);

    GPIO_SetMode(PA, (BIT2), GPIO_MODE_INPUT);
    GPIO_SetMode(PB, (BIT2 | BIT5), GPIO_MODE_INPUT);
    GPIO_SetMode(PF, (BIT2), GPIO_MODE_INPUT);

    GPIO_EnableInt(PA, 2, GPIO_INT_BOTH_EDGE);
    GPIO_EnableInt(PB, 2, GPIO_INT_BOTH_EDGE);
    GPIO_EnableInt(PB, 5, GPIO_INT_RISING);
    NVIC_EnableIRQ(GPIO_PAPBPGPH_IRQn);

    GPIO_EnableInt(PF, 2, GPIO_INT_BOTH_EDGE);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);

    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_ENABLE_DEBOUNCE(PA, BIT2);
    GPIO_ENABLE_DEBOUNCE(PB, BIT2);
    GPIO_ENABLE_DEBOUNCE(PF, BIT2);

    // Configure the ADC for TEMP readings. Continuous conversion. Don't bother with interrupts.
    ADC_POWER_ON(ADC);
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_CONTINUOUS, BIT14);
    ADC_START_CONV(ADC);

    // Configure the timers. 
    // Timer2 is Triac Turn-on delay from Zero crossing 
    // (so for 60Hz min is at 16.67ms-2us = 59.995Hz).
    // Timer3 is Triac Turn-on time. (500 KHz = 2us).
    TIMER_Open(TIMER2, TIMER_ONESHOT_MODE, 1);
    TIMER_EnableInt(TIMER2);
    NVIC_EnableIRQ(TMR2_IRQn);

    TIMER_Open(TIMER3, TIMER_ONESHOT_MODE, 500000);
    TIMER_EnableInt(TIMER3);
    NVIC_EnableIRQ(TMR3_IRQn);

    uint32_t i = 0;

    while(1) {
        // Run TuyaMCU main loop
        wifi_uart_service();
        // Handle any pending updates.
        if (pending_updates & PENDING_UPDATE_DP0) {
            mcu_dp_bool_update(0, !PB2);
            pending_updates = pending_updates & ~PENDING_UPDATE_DP0;
        }
        if (pending_updates & PENDING_UPDATE_DP1) {
            mcu_dp_bool_update(1, !PA2);
            pending_updates = pending_updates & ~PENDING_UPDATE_DP1;
        }
        if (pending_updates & PENDING_UPDATE_DP2) {
            mcu_dp_bool_update(2, !PF2);
            pending_updates = pending_updates & ~PENDING_UPDATE_DP2;
        }
        i++;
    }
}

