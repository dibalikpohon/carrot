#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKEN_NUM 4096
#define MAX_TOKEN_TEXT_LEN 128

typedef enum {
	/* Primitive literal */
	T_INT, T_FLOAT, T_STR,
	/* Identifier and keyword */
	T_ID, T_KEYWORD,
	/* Arithmetic */
	T_PLUS, T_MINUS, T_MULT, T_DIV,
	/* Assignment */
	T_EQUAL,
	/* Parentheses, brackets, etc. */
	T_LPAREN, T_RPAREN,
	/* Misc */
	T_COMMA, T_EOF, T_UNKNOWN,
} tok_kind_t;

typedef struct TOKEN {
	tok_kind_t tok_kind;
	char       text[MAX_TOKEN_TEXT_LEN];

	/* Token coordinate information */
	int        line_num;
	int        col_num;
} Token;

typedef struct LEXER {
	char  c;
	int   idx;
	int   line_num;
	char  *source;
	int   token_cnt;
	Token tokens[MAX_TOKEN_NUM];
} Lexer;

int is_keyword(char *s);
char *tok_kind_to_str(tok_kind_t kind);

Token create_token(tok_kind_t tok_kind, char *text);
void make_identifier(Lexer *lexer);
void make_number(Lexer *lexer);
void make_single_char_token(Lexer *lexer, tok_kind_t kind, char *text);
void make_string(Lexer *lexer);

void lexer_add_token(Lexer *lexer, Token t);
void lexer_init(Lexer *lexer, char *filename);
void lexer_next(Lexer *lexer);
void lexer_skip_comment(Lexer *lexer);
void lexer_skip_whitespace(Lexer *lexer);

void lexer_lex(Lexer *lexer);

#endif

