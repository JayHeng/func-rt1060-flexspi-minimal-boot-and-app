/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined(FUNC_BOOT)
#define APP_START 0x60400000U
 
uint32_t s_appStack = 0;
uint32_t s_appEntry = 0;
 
typedef void (*call_function_t)(void);
call_function_t s_callFunction = 0;
 
void jump_to_app(void)
{
    s_appStack = *(uint32_t *)(APP_START);
    s_appEntry = *(uint32_t *)(APP_START + 4);
 
    // Turn off interrupts.
    __disable_irq();
 
    // Set the VTOR to default.
    SCB->VTOR = APP_START;
 
    // Memory barriers for good measure.
    __ISB();
    __DSB();
 
    // Set main stack pointer and process stack pointer.
    __set_MSP(s_appStack);
    __set_PSP(s_appStack);
 
    // Jump to app entry point, does not return.
    s_callFunction = (call_function_t)s_appEntry;
    s_callFunction();
 
    while (1)
    {
    }
}

void print_secure_status(void)
{
    uint32_t sec_config = 0;
    sec_config = (OCOTP->CFG3 >> 1) & 0x1;
    sec_config |= OCOTP->CFG5 & 0x02;
    switch(sec_config)
    {
        case 0x0:
            PRINTF("chip in FAB mode.\r\n");
            break;
        case 0x1:
            PRINTF("chip in HAB open mode.\r\n");
            break;
        case 0x2:
        case 0x3:
        default:
            PRINTF("chip in HAB closed mode.\r\n");
            break;
    }
}
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    char ch;

    /* Init board hardware. */
    BOARD_InitHardware();

    PRINTF("\r\n----------------------\r\n");
#if defined(FUNC_BOOT)
    PRINTF("hello world from boot.\r\n");
    print_secure_status();
    PRINTF("get app info: PC = 0x%x, SP = 0x%x.\r\n", *(uint32_t *)(APP_START + 4), *(uint32_t *)(APP_START));
    PRINTF("jump to app.\r\n");
    jump_to_app();
#elif defined(FUNC_APP)
    PRINTF("hello world from app.\r\n");
#endif

    while (1)
    {
        ch = GETCHAR();
        PUTCHAR(ch);
    }
}
