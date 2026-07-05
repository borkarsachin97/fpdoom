#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "syscode.h"

extern void *framebuffer_init(unsigned size1);
extern void fptest_task(void *pvParameters);

static const uint8_t font_data[] = {
#include "../fpmenu/font8x16.h"
};

static void draw_string(uint16_t *fb, unsigned fb_w, unsigned x, unsigned y, const char *str, uint16_t color) {
    while (*str) {
        char c = *str++;
        if (c < 0x20 || c > 0x7E) c = '?';
        const uint8_t *bm = font_data + (c - 0x20) * 16;
        for (unsigned fy = 0; fy < 16; fy++) {
            uint8_t row = bm[fy];
            for (unsigned fx = 0; fx < 8; fx++) {
                if (row & (0x80 >> fx)) {
                    fb[(y + fy) * fb_w + x + fx] = color;
                }
            }
        }
        x += 8;
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
        draw_string((uint16_t*)fb, w, 10, 10, "Welcome to FPDOOM RTOS!", 0xFFFF);
        draw_string((uint16_t*)fb, w, 10, 30, "System Booting...", 0x07E0);
    }
    sys_start_refresh();

    xTaskCreate(fptest_task, "fptest", 2048, NULL, tskIDLE_PRIORITY + 1, NULL);

    volatile uint32_t *timer = (volatile uint32_t *)0x8b000000;
    timer[0] = 26000;

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
