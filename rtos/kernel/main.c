#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "syscode.h"

extern void *framebuffer_init(unsigned size1);

static const uint8_t font_data[] = {
#include "../fpmenu/font8x16.h"
};

static unsigned console_y = 16;

static void draw_string(uint16_t *fb, unsigned fb_w, unsigned fb_h, unsigned x, const char *str, uint16_t color) {
    while (*str) {
        char c = *str++;

        if (c == '\n') {
            console_y += 16;
            if (console_y >= fb_h) {
                console_y = 0;
                for(unsigned i = 0; i < fb_w * fb_h; i++) {
                    fb[i] = 0x0000;
                }
            }
            continue;
        }

        if (c < 0x20 || c > 0x7E) c = '?';

        if (x + 8 > fb_w) {
            continue; // Basic truncation for this task test
        }

        const uint8_t *bm = font_data + (c - 0x20) * 16;
        for (unsigned fy = 0; fy < 16; fy++) {
            uint8_t row = bm[fy];
            for (unsigned fx = 0; fx < 8; fx++) {
                if (row & (0x80 >> fx)) {
                    fb[(console_y + fy) * fb_w + x + fx] = color;
                } else {
                    fb[(console_y + fy) * fb_w + x + fx] = 0x0000;
                }
            }
        }
        x += 8;
    }
}

SemaphoreHandle_t xPrintfMutex;

void vRedTextTask(void *pvParameters) {
    (void)pvParameters;
    int line = 1;
    uint16_t *fb = (uint16_t*)sys_data.framebuf;
    unsigned w = sys_data.display.w2;
    unsigned h = sys_data.display.h2;
    for(;;) {
        if(xSemaphoreTake(xPrintfMutex, portMAX_DELAY) == pdTRUE) {
            char buf[64];
            sprintf(buf, "RED %d\n", line++);
            draw_string(fb, w, h, 0, buf, 0xF800);
            sys_start_refresh();
            sys_wait_refresh();
            xSemaphoreGive(xPrintfMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vGreenTextTask(void *pvParameters) {
    (void)pvParameters;
    int line = 1;
    uint16_t *fb = (uint16_t*)sys_data.framebuf;
    unsigned w = sys_data.display.w2;
    unsigned h = sys_data.display.h2;
    for(;;) {
        if(xSemaphoreTake(xPrintfMutex, portMAX_DELAY) == pdTRUE) {
            char buf[64];
            sprintf(buf, "GREEN %d\n", line++);
            draw_string(fb, w, h, w / 2, buf, 0x07E0);
            sys_start_refresh();
            sys_wait_refresh();
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

    draw_string((uint16_t*)fb, w, h, 0, "Welcome to FreeRTOS on SC6531!\n", 0xFFFF);
    sys_start_refresh();
    sys_wait_refresh();

    xPrintfMutex = xSemaphoreCreateMutex();

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

void _sig_intovf(void) {
    printf("Integer overflow error!\n");
    for(;;);
}
