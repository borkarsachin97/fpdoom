/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


/*-----------------------------------------------------------
* Components that can be compiled to either ARM or THUMB mode are
* contained in port.c  The ISR routines, which can only be compiled
* to ARM mode, are contained in this file.
*----------------------------------------------------------*/

/*
 *  Changes from V3.2.4
 *
 + The assembler statements are now included in a single asm block rather
 +    than each line having its own asm block.
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Constants required to handle interrupts. */
#define portTIMER_MATCH_ISR_BIT    ( ( uint8_t ) 0x01 )
#define portCLEAR_VIC_INTERRUPT    ( ( uint32_t ) 0 )

/* Constants required to handle critical sections. */
#define portNO_CRITICAL_NESTING    ( ( uint32_t ) 0 )
volatile uint32_t ulCriticalNesting = 9999UL;

/*-----------------------------------------------------------*/

/* ISR to handle manual context switches (from a call to taskYIELD()). */
void vPortYieldProcessor( void ) __attribute__( ( naked ) );

/*
 * The scheduler can only be started from ARM mode, hence the inclusion of this
 * function here.
 */
void vPortISRStartFirstTask( void ) __attribute__( ( naked ) );
/*-----------------------------------------------------------*/

#define portSAVE_CONTEXT_ASM \
    "STMDB  SP!, {R0} \n\t" \
    "STMDB  SP,{SP}^ \n\t" \
    "NOP \n\t" \
    "SUB    SP, SP, #4 \n\t" \
    "LDMIA  SP!,{R0} \n\t" \
    "STMDB  R0!, {LR} \n\t" \
    "MOV    LR, R0 \n\t" \
    "LDMIA  SP!, {R0} \n\t" \
    "STMDB  LR,{R0-LR}^ \n\t" \
    "NOP \n\t" \
    "SUB    LR, LR, #60 \n\t" \
    "MRS    R0, SPSR \n\t" \
    "STMDB  LR!, {R0} \n\t" \
    "LDR    R0, =ulCriticalNesting \n\t" \
    "LDR    R0, [R0] \n\t" \
    "STMDB  LR!, {R0} \n\t" \
    "LDR    R1, =pxCurrentTCB \n\t" \
    "LDR    R0, [R1] \n\t" \
    "STR    LR, [R0] \n\t"

#define portRESTORE_CONTEXT_ASM \
    "LDR    R0, =pxCurrentTCB \n\t" \
    "LDR    R0, [R0] \n\t" \
    "LDR    LR, [R0] \n\t" \
    "LDR    R0, =ulCriticalNesting \n\t" \
    "LDMFD  LR!, {R1} \n\t" \
    "STR    R1, [R0] \n\t" \
    "LDMFD  LR!, {R0} \n\t" \
    "MSR    SPSR_cxsf, R0 \n\t" \
    "LDMFD  LR, {R0-R14}^ \n\t" \
    "NOP \n\t" \
    "LDR    LR, [LR, #+60] \n\t" \
    "SUBS   PC, LR, #4 \n\t"

void vPortISRStartFirstTask( void )
{
    __asm volatile ( portRESTORE_CONTEXT_ASM );
}
/*-----------------------------------------------------------*/

/*
 * Called by portYIELD() or taskYIELD() to manually force a context switch.
 *
 * When a context switch is performed from the task level the saved task
 * context is made to look as if it occurred from within the tick ISR.  This
 * way the same restore context function can be used when restoring the context
 * saved from the ISR or that saved from a call to vPortYieldProcessor.
 */
void vPortYieldProcessor( void )
{
    /* Use a unified assembly block to guarantee GCC doesn't insert register-clobbering code. */
    __asm volatile (
        "ADD    LR, LR, #4 \n\t"
        portSAVE_CONTEXT_ASM
        "BL     vTaskSwitchContext \n\t"
        portRESTORE_CONTEXT_ASM
    );
}
/*-----------------------------------------------------------*/

/*
 * The ISR used for the scheduler tick depends on whether the cooperative or
 * the preemptive scheduler is being used.
 */

#if configUSE_PREEMPTION == 0

/* The cooperative scheduler requires a normal IRQ service routine to
 * simply increment the system tick. */
    void vNonPreemptiveTick( void ) __attribute__( ( interrupt( "IRQ" ) ) );
    void vNonPreemptiveTick( void )
    {
        /* Increment the tick count - which may wake some tasks but as the
         * preemptive scheduler is not being used any woken task is not given
         * processor time no matter what its priority. */
        xTaskIncrementTick();

        /* Clear the timer interrupt. */
        configCLEAR_TICK_INTERRUPT();
    }

#else /* if configUSE_PREEMPTION == 0 */

/* The preemptive scheduler is defined as "naked" as the full context is
 * saved on entry as part of the context switch. */
    void vPreemptiveTick( void ) __attribute__( ( naked ) );
    void vPreemptiveTick( void )
    {
        /* Use a unified assembly block to guarantee GCC doesn't insert register-clobbering code. */
        __asm volatile (
            portSAVE_CONTEXT_ASM
            "BL     xTaskIncrementTick \n\t"
            "CMP    R0, #0 \n\t"
            "BEQ    1f \n\t"
            "BL     vTaskSwitchContext \n\t"
            "1: \n\t"
            "BL     sys_clear_tick \n\t"
            portRESTORE_CONTEXT_ASM
        );
    }

#endif /* if configUSE_PREEMPTION == 0 */
/*-----------------------------------------------------------*/

/*
 * The interrupt management utilities can only be called from ARM mode.  When
 * THUMB_INTERWORK is defined the utilities are defined as functions here to
 * ensure a switch to ARM mode.  When THUMB_INTERWORK is not defined then
 * the utilities are defined as macros in portmacro.h - as per other ports.
 */
void vPortDisableInterruptsFromThumb( void ) __attribute__( ( naked ) );
void vPortEnableInterruptsFromThumb( void ) __attribute__( ( naked ) );

void vPortDisableInterruptsFromThumb( void )
{
    __asm volatile (
        "STMDB  SP!, {R0}       \n\t" /* Push R0.                                 */
        "MRS    R0, CPSR        \n\t" /* Get CPSR.                                */
        "ORR    R0, R0, #0x80   \n\t" /* Disable IRQ.                             */
        "MSR    CPSR_c, R0      \n\t" /* Write back modified value.               */
        "LDMIA  SP!, {R0}       \n\t" /* Pop R0.                                  */
        "BX     R14" );               /* Return back to thumb.                    */
}

void vPortEnableInterruptsFromThumb( void )
{
    __asm volatile (
        "STMDB  SP!, {R0}       \n\t" /* Push R0.                                 */
        "MRS    R0, CPSR        \n\t" /* Get CPSR.                                */
        "BIC    R0, R0, #0x80   \n\t" /* Enable IRQ.                              */
        "MSR    CPSR_c, R0      \n\t" /* Write back modified value.               */
        "LDMIA  SP!, {R0}       \n\t" /* Pop R0.                                  */
        "BX     R14" );               /* Return back to thumb.                    */
}


/* The code generated by the GCC compiler uses the stack in different ways at
 * different optimisation levels.  The interrupt flags can therefore not always
 * be saved to the stack.  Instead the critical section nesting level is stored
 * in a variable, which is then saved as part of the stack context. */
void vPortEnterCritical( void )
{
    /* Disable interrupts as per portDISABLE_INTERRUPTS();                          */
    __asm volatile (
        "STMDB  SP!, {R0}           \n\t" /* Push R0.                             */
        "MRS    R0, CPSR            \n\t" /* Get CPSR.                            */
        "ORR    R0, R0, #0x80       \n\t" /* Disable IRQ.                         */
        "MSR    CPSR_c, R0          \n\t" /* Write back modified value.           */
        "LDMIA  SP!, {R0}" );             /* Pop R0.                              */

    /* Now that interrupts are disabled, ulCriticalNesting can be accessed
     * directly.  Increment ulCriticalNesting to keep a count of how many times
     * portENTER_CRITICAL() has been called. */
    ulCriticalNesting++;
}

void vPortExitCritical( void )
{
    if( ulCriticalNesting > portNO_CRITICAL_NESTING )
    {
        /* Decrement the nesting count as we are leaving a critical section. */
        ulCriticalNesting--;

        /* If the nesting level has reached zero then interrupts should be
         * re-enabled. */
        if( ulCriticalNesting == portNO_CRITICAL_NESTING )
        {
            /* Enable interrupts as per portEXIT_CRITICAL().                    */
            __asm volatile (
                "STMDB  SP!, {R0}       \n\t" /* Push R0.                     */
                "MRS    R0, CPSR        \n\t" /* Get CPSR.                    */
                "BIC    R0, R0, #0x80   \n\t" /* Enable IRQ.                    */
                "MSR    CPSR_c, R0      \n\t" /* Write back modified value.     */
                "LDMIA  SP!, {R0}" );         /* Pop R0.                      */
        }
    }
}
