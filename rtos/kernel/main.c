#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "syscode.h"

extern void *framebuffer_init(unsigned size1);

SemaphoreHandle_t xPrintfMutex;

void vRedTextTask(void *pvParameters) {
    (void)pvParameters;
    int line = 0;
    for(;;) {
        if(xSemaphoreTake(xPrintfMutex, portMAX_DELAY) == pdTRUE) {
            set_console_color(0xF800); // Red
            set_console_x(0);          // Left side
            printf("TASK RED RUNNING... line %d\n", line++);
            xSemaphoreGive(xPrintfMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vGreenTextTask(void *pvParameters) {
    (void)pvParameters;
    int line = 0;
    for(;;) {
        if(xSemaphoreTake(xPrintfMutex, portMAX_DELAY) == pdTRUE) {
            set_console_color(0x07E0); // Green
            set_console_x(120);        // Right side
            printf("TASK GREEN RUNNING... line %d\n", line++);
            xSemaphoreGive(xPrintfMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    sys_start();

    struct sys_display *disp = &sys_data.display;
    unsigned w = disp->w2, h = disp->h2;
    size_t size = w * h;
    uint8_t *p;
    p = malloc(size * 2 + 31);
    if (!p) {
        printf("malloc failed for framebuffer\n");
        return 1;
    }
    p += -(intptr_t)p & 31;
    void *fb = (void*)p;

    sys_framebuffer(fb);

    sys_brightness(50);

    if (fb) {
        for(unsigned i = 0; i < size; i++) {
            ((uint16_t*)fb)[i] = 0x0000;
        }
    }
    sys_start_refresh();

    xPrintfMutex = xSemaphoreCreateMutex();

    printf("Welcome to FreeRTOS on SC6531!\n");

    if (xPrintfMutex != NULL) {
        xTaskCreate(vRedTextTask, "RedTask", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
        xTaskCreate(vGreenTextTask, "GreenTask", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
    }

    vTaskStartScheduler();

    for(;;);

    return 0;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    (void)xTask;
    (void)pcTaskName;
    for(;;);
}

void vApplicationMallocFailedHook( void ) {
    for(;;);
}

void vApplicationIdleHook( void ) {
}

void vApplicationTickHook( void ) {
}

void _debug_msg(const char *msg) {
    printf("DEBUG: %s\n", msg);
}

void _sig_intdiv(void) {
    printf("Division by zero error!\n");
    for(;;);
}
