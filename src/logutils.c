#include <stdio.h>
#include "../include/logutils.h"

void carrot_log_error(char *message, char *filename, int line_number) {
	printf("%s%s%s%s%s\n\n",
	       ". . .",
	       carrot_text_style(CARROT_TEXT_ERROR_HEADING),
	       "\n\nOops... Looks like we have a problem.\n",
	       carrot_text_style(CARROT_TEXT_COLOR_RESET),
	       message);
	printf("Location:\nFile: %s\nLine: %d\n", filename, line_number);
}

char *carrot_text_style(carrot_text_color_t color) {
	switch (color) {
		case CARROT_TEXT_ERROR_HEADING: return "\033[1m\033[38;2;255;100;0m";
		case CARROT_TEXT_COLOR_RESET: return "\033[0m";
	}
	return "\033[0m";
}

