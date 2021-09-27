#ifndef LOGUTILS_H
#define LOGUTILS_H

typedef enum {
	CARROT_TEXT_ERROR_HEADING,
	CARROT_TEXT_COLOR_RESET
} carrot_text_color_t;

void carrot_log_error(char *message);
char *carrot_text_style(carrot_text_color_t color);

#endif
