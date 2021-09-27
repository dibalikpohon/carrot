#ifndef PARSER_H
#define PARSER_H

#include "../include/lexer.h"

#define MAX_NODE_NUM        2048
#define MAX_STR_LITERAL_LEN 512
#define MAX_STATEMENT_NUM   2048
#define MAX_VAR_NAME_LEN    255
#define MAX_VAR_TYPE_LEN    255

typedef struct CarrotObj_t CarrotObj;

typedef enum {
	/* program entry point */
	N_LIST,
	/* literals */
	N_VALUE, N_INT, N_FLOAT, N_STR,
	/* statement */
	N_STATEMENT, N_STATEMENTS,
	N_VAR_DEF, N_VAR_ACCESS, N_FUNC_DEF, N_FUNC_CALL,
	N_NULL,
} node_type_t;


typedef enum {
	DT_STR, DT_INT, DT_FLOAT, DT_NULL
} data_type_t;


typedef struct Symtable_t {
	char               *key;
	struct CarrotObj_t *value;
} Symtable;

typedef struct CarrotObj_t {
	node_type_t        type;
	char               repr[MAX_STR_LITERAL_LEN];

	/* reference to parent node and its corresponding
	 * local symtable
	 */
	struct CarrotObj_t *parent;
	Symtable           *sym_table;

	/* value node */
	int                int_val;
	float              float_val;
	char               str_val[MAX_STR_LITERAL_LEN];
	struct CarrotObj_t *obj_val;

	/* statements node */
	struct CarrotObj_t *statements;

	/* List node */
	struct CarrotObj_t *list_items;
	struct CarrotObj_t *interpreted_list_items;

	/* variable definition node */
	char               var_name[MAX_VAR_NAME_LEN];
	data_type_t        var_type;
	char               var_type_str[MAX_VAR_TYPE_LEN];
	struct CarrotObj_t *var_value; // shared with value node

	/* function definition node */
	char               func_name[MAX_VAR_NAME_LEN];
	int                is_builtin;
	struct CarrotObj_t *func_params;
	struct CarrotObj_t *func_body;

	/* function call node */
	struct CarrotObj_t (*builtin_func)(struct CarrotObj_t *args);
	struct CarrotObj_t *func_args;
	struct CarrotObj_t *func_interpreted_args;
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
Token parser_lookahed(Parser *parser);
CarrotObj parser_parse(Parser *parser);
CarrotObj parser_parse_arith(Parser *parser);
CarrotObj parser_parse_atom(Parser *parser);
CarrotObj parser_parse_call(Parser *parser);
CarrotObj parser_parse_comp(Parser *parser);
CarrotObj parser_parse_expression(Parser *parser);
CarrotObj parser_parse_factor(Parser *parser);
CarrotObj parser_parse_identifier(Parser *parser);
CarrotObj parser_parse_keyword(Parser *parser);
CarrotObj parser_parse_literal(Parser *parser);
CarrotObj parser_parse_power(Parser *parser);
CarrotObj parser_parse_script(Parser *parser);
CarrotObj parser_parse_statement(Parser *parser);
CarrotObj parser_parse_statements(Parser *parser);
CarrotObj parser_parse_term(Parser *parser);
CarrotObj parser_parse_value(Parser *parser);
CarrotObj parser_parse_variable_def(Parser *parser,
		                    Token id_token,
		                    char *var_type_str, 
				    Token var_value_token,
				    int initialized);

CarrotObj carrot_null();
CarrotObj carrot_int(int int_val);
CarrotObj carrot_float(float float_val);
CarrotObj carrot_str(char *str_val);

int  carrot_get_args_len(CarrotObj *args);
void carrot_get_repr(CarrotObj obj, char *out);

void carrot_type_check(tok_kind_t token_kind, char *data_type_str);

#endif
