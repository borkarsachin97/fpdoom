#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				1
#define configUSE_TICK_HOOK				1
#define configCPU_CLOCK_HZ				( ( unsigned long ) 60000000 )
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES			( 5 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 256 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 1024 * 64 ) )
#define configMAX_TASK_NAME_LEN			( 16 )
#define configUSE_TRACE_FACILITY		0
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			1
#define configUSE_MUTEXES				1
#define configQUEUE_REGISTRY_SIZE		8
#define configCHECK_FOR_STACK_OVERFLOW	2
#define configUSE_RECURSIVE_MUTEXES		1
#define configUSE_MALLOC_FAILED_HOOK	1
#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_COUNTING_SEMAPHORES	1

/* ARM_CA9 Port specific definitions */
#define configUNIQUE_INTERRUPT_PRIORITIES 32
#define configMAX_API_CALL_INTERRUPT_PRIORITY 18
#define configINTERRUPT_CONTROLLER_BASE_ADDRESS 0x80000000
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET 0x1000

/* ARM_CA9 Port specific hook */
#define configSETUP_TICK_INTERRUPT()    /* Implement if tick timer hardware is initialized in main */
#define configCLEAR_TICK_INTERRUPT()    /* Implement to clear the tick interrupt flag */

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

#endif /* FREERTOS_CONFIG_H */