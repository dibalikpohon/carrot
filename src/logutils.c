#include <stdio.h>
#include "../include/logutils.h"

void carrot_log_error(char *message) {
	printf("%s%s%s    %s%s\n\n",
	       carrot_text_color(CR_TEXT_COLOR_RED),
	       ". . .",
	       "\n\n    Oops... Looks like we have a problem.\n",
	       carrot_text_color(CR_TEXT_COLOR_RESET),
	       message);
}

char *carrot_text_color(cr_text_color_t color) {
	switch (color) {
		case CR_TEXT_COLOR_RED:   return "\033[1;31m";
		case CR_TEXT_COLOR_RESET: return "\033[0m";
	}
	return "\033[0m";
}

