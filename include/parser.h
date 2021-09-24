#ifndef PARSER_H
#define PARSER_H

#include "../include/lexer.h"

#define MAX_NODE_NUM        2048
#define MAX_STR_LITERAL_LEN 512
#define MAX_STATEMENT_NUM   2048
#define MAX_VAR_NAME_LEN    255

typedef enum {
	/* program entry point */
	N_LIST,
	/* literals */
	N_INT_LITERAL, N_FLOAT_LITERAL, N_STR_LITERAL,
	/* statement */
	N_STATEMENT, N_STATEMENTS,
	N_VAR_DEF, N_FUNC_DEF, N_FUNC_CALL,
	N_NULL,
} node_type_t;

typedef struct CarrotObj_t {
	node_type_t        type;

	/* value node */
	int                int_val;
	float              float_val;
	char               str_val[MAX_STR_LITERAL_LEN];
	struct CarrotObj_t *obj_val;

	/* statements node */
	struct CarrotObj_t *statements;

	/* List node */
	struct CarrotObj_t *list_items;

	/* variable definition node */
	char               var_name[MAX_VAR_NAME_LEN];
	char               *var_type;
	struct CarrotObj_t *var_value;

	/* function definition node */
	char               func_name[MAX_VAR_NAME_LEN];
	int                is_builtin;
	struct CarrotObj_t *func_params;
	struct CarrotObj_t *func_body;

	/* function call node */
	struct CarrotObj_t (*builtin_func)(struct CarrotObj_t *args);
	struct CarrotObj_t *func_args;
	struct CarrotObj_t *func_return_value;
} CarrotObj;

typedef struct PARSER {
	Token current_token;
	int   i;
	Lexer lexer;
	int   node_cnt;
	CarrotObj  nodes[MAX_NODE_NUM];
} Parser;

void free_node(CarrotObj *node);
Token parser_consume(Parser *parser);
void parser_free(Parser *parser);
void parser_init(Parser *parser, char *source);
CarrotObj parser_parse(Parser *parser);
Token parser_lookahed(Parser *parser);
CarrotObj parser_parse_compound(Parser *parser);
CarrotObj parser_parse_expression(Parser *parser);
CarrotObj parser_parse_identifier(Parser *parser);
CarrotObj parser_parse_keyword(Parser *parser);
CarrotObj parser_parse_literal(Parser *parser);
CarrotObj parser_parse_script(Parser *parser);
CarrotObj parser_parse_statement(Parser *parser);
CarrotObj parser_parse_statements(Parser *parser);
CarrotObj parser_parse_variable_def(Parser *parser);

#endif
