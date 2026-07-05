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
int fputc(int ch, FILE *f) { return ch; }
size_t fread(void *dst, size_t size, size_t count, FILE *f) { return 0; }
size_t fwrite(const void *src, size_t size, size_t count, FILE *f) { return 0; }
int fseek(FILE *f, long offset, int origin) { return -1; }
long ftell(FILE *f) { return -1; }
FILE *fopen(const char *name, const char *mode) { return NULL; }
int fclose(FILE *f) { return 0; }
void setbuf(FILE *f, char *buf) {}
int _argv_copy(char ***argvp, int argc, char *src) { return 0; }
int _argv_init(char ***argvp, int skip) { return 0; }

