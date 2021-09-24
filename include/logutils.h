#ifndef LOGUTILS_H
#define LOGUTILS_H

typedef enum {
	CR_TEXT_COLOR_RED,
	CR_TEXT_COLOR_RESET
} cr_text_color_t;

void cr_log_error(char *message);
char *cr_text_color(cr_text_color_t color);

#endif
