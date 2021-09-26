#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/logutils.h"

#define STB_DS_IMPLEMENTATION
#include "../lib/include/stb_ds.h"

void free_node(CarrotObj *node) {
	if (node->type == N_LIST) {
		int len;

		len = arrlen(node->list_items);
		for (int i = 0; i < len; i++) {
			free_node(&node->list_items[i]);
		}

		len = arrlen(node->interpreted_list_items);
		for (int i = 0; i < len; i++) {
			free_node(&node->interpreted_list_items[i]);
		}
		arrfree(node->list_items);
		arrfree(node->interpreted_list_items);
	}
}

Token parser_consume(Parser *parser) {
	Token token = parser->current_token;

	parser->i++;
	parser->current_token = parser->lexer.tokens[parser->i];
	return token;
}

void parser_init(Parser *parser, char *source) {
	lexer_init(&parser->lexer, source);
	lexer_lex(&parser->lexer);
	parser->i = 0;
	parser->current_token = parser->lexer.tokens[0];
	parser->node_cnt = 0;
}

Token parser_lookahed(Parser *parser) {
	return parser->lexer.tokens[parser->i + 1];
}

void parser_free(Parser *parser) {
	for (int i = 0; i < parser->node_cnt; i++) {
		if (parser->nodes[i].type == N_STATEMENTS) {
			arrfree(parser->nodes[i].statements);
		}
	}
}

CarrotObj parser_parse(Parser *parser) {
	return parser_parse_script(parser);
}

CarrotObj parser_parse_arith(Parser *parser) {
	CarrotObj left = parser_parse_term(parser);

	while (parser->current_token.tok_kind == T_PLUS) {
	}

	return left;
}

CarrotObj parser_parse_atom(Parser *parser) {
	Token tok = parser->current_token;

	if (tok.tok_kind == T_ID) {
		return parser_parse_identifier(parser);
	} else if (tok.tok_kind == T_STR) {
		CarrotObj obj;
		obj.type = N_STR;
		strcpy(obj.repr, tok.text);
		parser_consume(parser);
		return obj;
	}

	printf("ERROR: literal or expression expected");
	exit(1);
}


CarrotObj parser_parse_call(Parser *parser) {
	CarrotObj atom = parser_parse_atom(parser);
	if (parser->current_token.tok_kind == T_LPAREN) {
	}
	return atom;
}

CarrotObj parser_parse_com(Parser *parser) {
	// 1) parse NOT
	// ...

	// left term
	CarrotObj left = parser_parse_arith(parser);

	// 2) parse ==, >, <, >=, <=
	// ...
	
	return left;
}

CarrotObj parser_parse_expression(Parser *parser) {
	CarrotObj left = parser_parse_com(parser);

	/* and, or, ... */

	return left;
}

CarrotObj parser_parse_factor(Parser *parser) {
	CarrotObj left = parser_parse_power(parser);
	return left;
}

CarrotObj parser_parse_identifier(Parser *parser) {
	/* 3 possibilities:
	 * - variable declaration with dtype
	 * - variable declaration
	 * - variable access
	 */
	Token id_token = parser_consume(parser);
	Token next_token = parser->current_token; //parser_consume(parser);

	if (strcmp(next_token.text, "as") == 0) {
		parser_consume(parser);
		/* Handle variable declaration */

		// holding data type
		Token data_type_token = parser_consume(parser);

		if (parser->current_token.tok_kind == T_EQUAL) {
			/* The variable is initialized with some value 
			 * since we found equal char
			 */
			parser_consume(parser);
			Token var_value_token = parser_consume(parser);

			CarrotObj n;
			n.type = N_VAR_DEF;
			strcpy(n.var_name, id_token.text);

			// move these blocks inside parser_parse_variable_def
			if (strcmp(data_type_token.text, "str") == 0) {
				n.var_type = DT_STR;
				strcpy(n.str_val, var_value_token.text);
			} else if (strcmp(data_type_token.text, "int") == 0) {
				n.var_type = DT_INT;
				n.int_val = atoi(var_value_token.text);
			} else if (strcmp(data_type_token.text, "float") == 0) {
				n.var_type = DT_FLOAT;
				n.float_val = atof(var_value_token.text);
			}

			return n;
		} else {
			/* Otherwise, just return the node
			 * TODO: default value based on the type
			 */
			CarrotObj n;
			n.type = N_VAR_DEF;
			strcpy(n.var_name, id_token.text);
			return n;
		}

	} else if (next_token.tok_kind == T_LPAREN) {
		parser_consume(parser);
		/* Handle function call */

		CarrotObj *args = NULL;

		if (parser->current_token.tok_kind == T_RPAREN) {
			// handle call without args
			parser_consume(parser);
		} else {
			// handle call with args
			CarrotObj arg = parser_parse_expression(parser);
			arrput(args, arg);
			while (parser->current_token.tok_kind == T_COMMA) {
				parser_consume(parser);
				CarrotObj arg = parser_parse_expression(parser);
				arrput(args, arg);
			}
			if (parser->current_token.tok_kind != T_RPAREN) {
				printf("ERROR: \")\" expected.\n");
			}
			
			// consume R_PAREN
			parser_consume(parser);
		}

		CarrotObj obj;
		obj.type = N_FUNC_CALL;
		obj.func_args = args;
		strcpy(obj.func_name, id_token.text);
		return obj;
	} else if (next_token.tok_kind == T_EQUAL) {
		/* Handle assignment */
	} else {
		/* Handle variable access */
		CarrotObj obj;
		obj.type = N_VAR_ACCESS;
		strcpy(obj.var_name, id_token.text);
		return obj;
	}

	printf("ERROR: Invalid syntax.\n");
	printf("%s.\n", parser->current_token.text);
	exit(1);
}

CarrotObj parser_parse_keyword(Parser *parser) {
	exit(1);	
}

CarrotObj parser_parse_power(Parser *parser) {
	CarrotObj left = parser_parse_call(parser);

	return left;
}

CarrotObj parser_parse_script(Parser *parser) {
	CarrotObj *statements = NULL;
	while (parser->current_token.tok_kind != T_EOF) {
		CarrotObj stmt = parser_parse_statement(parser);
		arrput(statements, stmt);
	}

	CarrotObj list_node;
	list_node.type = N_LIST;
	list_node.list_items = statements;
	return list_node;
}

CarrotObj parser_parse_statement(Parser *parser) {
	return parser_parse_expression(parser);
}

CarrotObj parser_parse_statements(Parser *parser){
	CarrotObj *statements = NULL;

	CarrotObj statement = parser_parse_statement(parser);
	arrput(statements, statement);

	CarrotObj n;
	n.statements = statements;
	n.type = N_STATEMENTS;
	parser->nodes[parser->node_cnt++] = n;

	return n;
}

CarrotObj parser_parse_term(Parser *parser) {
	CarrotObj left = parser_parse_factor(parser);

	while (parser->current_token.tok_kind == T_MULT) {
	}

	return left;
}

CarrotObj parser_parse_value(Parser *parser) {
	exit(1);
}

CarrotObj parser_parse_variable_def(Parser *parser) {
	exit(1);
}

CarrotObj carrot_null() {
	CarrotObj obj;
	obj.type = N_NULL;
	return obj;
}

CarrotObj carrot_float(float float_val) {
	CarrotObj obj;
	obj.type = N_VALUE;
	obj.var_type = DT_FLOAT;
	obj.float_val = float_val;
	sprintf(obj.repr, "%f", float_val);
	return obj;
}

CarrotObj carrot_int(int int_val) {
	CarrotObj obj;
	obj.type = N_VALUE;
	obj.var_type = DT_INT;
	obj.int_val = int_val;
	sprintf(obj.repr, "%d", int_val);
	return obj;
}
CarrotObj carrot_str(char *str_val) {
	CarrotObj obj;
	obj.type = N_VALUE;
	obj.var_type = DT_STR;
	strcpy(obj.str_val, str_val);
	strcpy(obj.repr, str_val);
	return obj;
}

int  carrot_get_args_len(CarrotObj *args) {
	return arrlen(args);
}

void carrot_get_repr(CarrotObj obj, char *out) {
	if (strcmp(obj.repr, "") == 0) {
		printf("ERROR: unable to print representation");
		exit(1);
	}
	strcpy(out, obj.repr);
}
