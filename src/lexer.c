#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/lexer.h"
#include "../include/logutils.h"


int is_keyword(char *s) {
	int n_keywords = 4;
	char keywords[4][20] = {
		#include "../spec/keywords"
	};

	int found = 0;
	for (int i = 0; i < n_keywords; i++) {
		if (strcmp(s, keywords[i]) == 0) {
			found = 1;
			break;
		}
	}
	return found;
}

Token create_token(tok_kind_t tok_kind, char *text) {
	Token t;
	t.tok_kind = tok_kind;
	strcpy(t.text, text);
	return t;
}

void make_identifier(Lexer *lexer) {
	int i = 0;
	char s[MAX_TOKEN_TEXT_LEN] = "";
	while (isalpha(lexer->c) || lexer->c == '_') {
		s[i++] = lexer->c;
		lexer_next(lexer);
	}
	s[i] = '\0';

	if (is_keyword(s)) {
		lexer_add_token(lexer, create_token(T_KEYWORD, s));
	} else {
		lexer_add_token(lexer, create_token(T_ID, s));
	}
}

void make_number(Lexer *lexer) {
	int i = 0;
	int num_dot = 0;
	char s[MAX_TOKEN_TEXT_LEN] = "";
	while (isdigit(lexer->c) || lexer->c == '.') {
		if (lexer->c == '.') num_dot++;
		s[i++] = lexer->c;
		lexer_next(lexer);
	}
	s[i] = '\0';

	if (num_dot > 1) {
		char msg[255];
		snprintf(msg, 255, "I saw an invalid number format in '%s' at line %d. "
			 "You typed: %s\n",
			 "foo",
			 lexer->line_num,
			 s);
		carrot_log_error(msg, "idklol", -1);
		exit(1);
	}

	if (num_dot == 1) {
		lexer_add_token(lexer, create_token(T_FLOAT, s));
	} else {
		lexer_add_token(lexer, create_token(T_INT, s));
	}
}

void make_single_char_token(Lexer *lexer, tok_kind_t kind, char *text) {
	lexer_add_token(lexer, create_token(kind, text));
	lexer_next(lexer);
}

void make_two_chars_token(Lexer *lexer, tok_kind_t kind, char *text) {
	lexer_add_token(lexer, create_token(kind, text));
	lexer_next(lexer);
	lexer_next(lexer);
}

void make_string(Lexer *lexer) {
	lexer_next(lexer);
	char s[MAX_TOKEN_TEXT_LEN] = "";
	int i = 0;
	while (lexer->c != '"') {
		if (lexer->c == '\\') {
			/* Capture escape sequence */
			lexer_next(lexer);
			switch (lexer->c) {
				case 'n':
					s[i++] = '\n';
					break;
				case 't':
					s[i++] = '\t';
					break;
				// TODO: more cases
			}
			lexer_next(lexer);
		} else {
			s[i++] = lexer->c;
			lexer_next(lexer);
		}
	}
	
	// found enclosing delimiter
	lexer_next(lexer);
	s[i] = '\0';

	lexer_add_token(lexer, create_token(T_STR, s));
}

void lexer_add_token(Lexer *lexer, Token t) {
	lexer->tokens[lexer->token_cnt++] = t;
}

void lexer_init(Lexer *lexer, char *source) {
	lexer->idx = 0;
	lexer->line_num = 1;
	lexer->source = source;
	lexer->c = lexer->source[0];
	lexer->token_cnt = 0;
}

void lexer_next(Lexer *lexer) {
	if (lexer->c != '\0')
		lexer->c = lexer->source[++lexer->idx];
}

void lexer_skip_comment(Lexer *lexer) {
	if (lexer->source[lexer->idx+1] == '-') {
		while (lexer->c != '\n') {
			lexer_next(lexer);
		}
		lexer->line_num++;
	}
}

void lexer_skip_whitespace(Lexer *lexer) {
	while (isspace(lexer->c)) {
		if (lexer->c == '\n') lexer->line_num++;
		lexer_next(lexer);
	}
}

/*===========================================================================
 *Lexical Analysis
 *===========================================================================*/
void lexer_lex(Lexer *lexer) {
	while (lexer->c != '\0') {
		if (isspace(lexer->c)) {
			lexer_skip_whitespace(lexer);
			continue;
		} else if (isalpha(lexer->c) || lexer->c == '_') {
			make_identifier(lexer);
			continue;
		} else if (isdigit(lexer->c)) {
			make_number(lexer);
			continue;
		} else if (lexer->c == '"') {
			make_string(lexer);
			continue;
		} else if (lexer->c == '=') {
			if (lexer->source[lexer->idx+1] == '=') {
				make_two_chars_token(lexer, T_EE, "==");
			} else {
				make_single_char_token(lexer, T_EQUAL, "=");
			}
			continue;
		} else if (lexer->c == '>') {
			if (lexer->source[lexer->idx+1] == '=') {
				make_two_chars_token(lexer, T_GE, ">=");
			} else {
				make_single_char_token(lexer, T_GT, ">");
			}
			continue;
		} else if (lexer->c == '<') {
			if (lexer->source[lexer->idx+1] == '=') {
				make_two_chars_token(lexer, T_LE, "<=");
			} else {
				make_single_char_token(lexer, T_LT, "<");
			}
			continue;
		} else if (lexer->c == '!') {
			if (lexer->source[lexer->idx+1] == '=') {
				make_two_chars_token(lexer, T_NE, "!=");
			} else {
				make_single_char_token(lexer, T_NOT, "!");
			}
			continue;
		} else if (lexer->c == '|') {
			if (lexer->source[lexer->idx+1] == '|') {
				make_two_chars_token(lexer, T_OR, "||");
			}
			continue;
		} else if (lexer->c == '&') {
			if (lexer->source[lexer->idx+1] == '&') {
				make_two_chars_token(lexer, T_AND, "&&");
			}
			continue;
		} else if (lexer->c == '(') {
			make_single_char_token(lexer, T_LPAREN, "(");
			continue;
		} else if (lexer->c == ')') {
			make_single_char_token(lexer, T_RPAREN, ")");
			continue;
		} else if (lexer->c == '[') {
			make_single_char_token(lexer, T_LBRACKET, "[");
			continue;
		} else if (lexer->c == ']') {
			make_single_char_token(lexer, T_RBRACKET, "]");
			continue;
		} else if (lexer->c == ',') {
			make_single_char_token(lexer, T_COMMA, ",");
			continue;
		} else if (lexer->c == '+') {
			make_single_char_token(lexer, T_PLUS, "+");
			continue;
		} else if (lexer->c == '*') {
			make_single_char_token(lexer, T_MULT, "*");
			continue;
		} else if (lexer->c == '/') {
			make_single_char_token(lexer, T_DIV, "/");
			continue;
		}else if (lexer->c == ':') {
			make_single_char_token(lexer, T_COLON, ":");
			continue;
		} else if (lexer->c == '@') {
			make_single_char_token(lexer, T_AT, "@");
			continue;
		} else if (lexer->c == '-') {
			if (lexer->source[lexer->idx+1] == '-') {
				lexer_skip_comment(lexer);
			} else if (lexer->source[lexer->idx+1] == '>') {
				make_two_chars_token(lexer, T_RARROW, "->");
			} else {
				make_single_char_token(lexer, T_MINUS, "-");
			}
			continue;
		} else {
			char msg[100];
			sprintf(msg,"Unexpected character: \"%c\"\n", lexer->c);
			carrot_log_error(msg, "idklol", lexer->line_num);
			exit(1);
		}
		lexer_next(lexer);
	}
	lexer_add_token(lexer, create_token(T_EOF, "EOF"));
}
