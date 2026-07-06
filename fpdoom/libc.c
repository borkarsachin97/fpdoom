#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

sighandler_t signal(int signum, sighandler_t handler) {
	(void)signum; (void)handler;
	return NULL;
}

#define PREFIX(name) name
#include "libc/printf.h"
#include "libc/malloc.h"
#undef PREFIX

int getchar(void) {
	return fgetc(stdin);
}

int putchar(int ch) {
	return fputc(ch, stdout);
}

int puts(const char *str) {
	size_t len = strlen(str);
	if (fwrite(str, 1, len, stdout) != len) return EOF;
	return putchar('\n');
}

int fputs(const char *str, FILE *f) {
	size_t len = strlen(str);
	return fwrite(str, 1, len, f) != len ? EOF : 0;
}

__attribute__((noreturn))
void sys_exit(void);

#ifndef ATEXIT_MAX
#define ATEXIT_MAX 0
#endif
#if ATEXIT_MAX
static int atexit_num;
typedef void (*atexit_fn_t)(void);
static atexit_fn_t atexit_buf[ATEXIT_MAX];
#endif

int atexit(void (*func)(void)) {
#if ATEXIT_MAX
	if (atexit_num >= ATEXIT_MAX) return -1;
	atexit_buf[atexit_num++] = func;
	return 0;
#else
	(void)func;
	return -1;
#endif
}

void exit(int status) {
#if ATEXIT_MAX
	int i;
	for (i = atexit_num; i > 0;)
		atexit_buf[--i]();
#endif
	(void)status;
	sys_exit();
}

FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

void _stdio_init(void) {
}

int fflush(FILE *f) { return 0; }
int fgetc(FILE *f) { return EOF; }

#include "syscode.h"

static const uint8_t font8x16[] = {
#include "../fpmenu/font8x16.h"
};

static int console_x = 0;
static int console_y = 0;
static uint16_t console_color = 0xffff;

void set_console_color(uint16_t color) {
	console_color = color;
}

void set_console_x(int x) {
	console_x = x;
}

int fputc(int ch, FILE *f) {
	if (!sys_data.framebuf) return ch;

	struct sys_display *disp = &sys_data.display;
	int w = disp->w1;
	int h = disp->h1;

	if (ch == '\n') {
		console_x = 0;
		console_y += 16;
		if (console_y >= h) {
			console_y = 0;
		}
		sys_start_refresh();
		sys_wait_refresh();
		return ch;
	}

	if (ch < 0x20 || ch > 0x7f) return ch;

	const uint8_t *glyph = &font8x16[(ch - 0x20) * 16];
	uint16_t *fb = (uint16_t*)sys_data.framebuf;

	if (console_x + 8 > w) {
		console_x = 0;
		console_y += 16;
		if (console_y >= h) {
			console_y = 0;
		}
	}

	for (int i = 0; i < 16; i++) {
		uint8_t row = glyph[i];
		for (int j = 0; j < 8; j++) {
			if (row & (1 << (7 - j))) {
				fb[(console_y + i) * w + console_x + j] = console_color;
			} else {
				fb[(console_y + i) * w + console_x + j] = 0x0000;
			}
		}
	}

	console_x += 8;
	return ch;
}

size_t fread(void *dst, size_t size, size_t count, FILE *f) { return 0; }
size_t fwrite(const void *src, size_t size, size_t count, FILE *f) { return 0; }
int fseek(FILE *f, long offset, int origin) { return -1; }
long ftell(FILE *f) { return -1; }
FILE *fopen(const char *name, const char *mode) { return NULL; }
int fclose(FILE *f) { return 0; }
void setbuf(FILE *f, char *buf) {}
int _argv_copy(char ***argvp, int argc, char *src) { return 0; }
int _argv_init(char ***argvp, int skip) { return 0; }

