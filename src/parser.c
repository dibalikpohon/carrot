#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/logutils.h"

#define STB_DS_IMPLEMENTATION
#include "../lib/include/stb_ds.h"

Node **NODE_TRACKING_ARR = NULL;

Node *init_node() {
	Node *n = malloc(sizeof(Node));
	n->obj_val = NULL;
	n->list_items = NULL;
	n->statements = NULL;
	n->var_node = NULL;
	n->func_params = NULL;
	n->func_body = NULL;
	n->func_args = NULL;
	n->func_return_value = NULL;
	arrput(NODE_TRACKING_ARR, n);
	return n;
}

void free_node(Node *node) {
	int len = arrlen(NODE_TRACKING_ARR);
	for (int i = 0; i < len; i++) {
		Node *n = NODE_TRACKING_ARR[i];
		if (n->list_items != NULL) arrfree(n->list_items);
		if (n->func_args != NULL) arrfree(n->func_args);
		free(n);
	}
	arrfree(NODE_TRACKING_ARR);
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

Node *parser_parse(Parser *parser) {
	return parser_parse_script(parser);
}

Node *parser_parse_arith(Parser *parser) {
	Node *left = parser_parse_term(parser);

	while (parser->current_token.tok_kind == T_PLUS) {
	}

	return left;
}

Node *parser_parse_atom(Parser *parser) {
	tok_kind_t kind = parser->current_token.tok_kind;

	if (kind == T_ID) {
		return parser_parse_identifier(parser);
	} else if (kind == T_STR ||
		   kind == T_INT ||
		   kind == T_FLOAT) {
		Node *val_node = init_node();
		val_node->type = N_LITERAL;
		val_node->value_token = parser->current_token;
		if (kind == T_STR) {
			val_node->var_type = DT_STR;
			strcpy(val_node->str_val, val_node->value_token.text);
		} else if (kind == T_INT) {
			val_node->var_type = DT_INT;
			val_node->int_val = atoi(val_node->value_token.text);
		}
		else if (kind == T_FLOAT) {
			val_node->var_type = DT_FLOAT;
			val_node->float_val = atof(val_node->value_token.text);
		}
		parser_consume(parser);
		return val_node;
	} else if (kind == T_LBRACKET) {
		return parser_parse_list(parser);
	}
	printf("ERROR: literal or expression expected\n");
	exit(1);
}


Node *parser_parse_call(Parser *parser) {
	Node *atom = parser_parse_atom(parser);
	if (parser->current_token.tok_kind == T_LPAREN) {
	}
	return atom;
}

Node *parser_parse_com(Parser *parser) {
	// 1) parse NOT
	// ...

	// left term
	Node *left = parser_parse_arith(parser);

	// 2) parse ==, >, <, >=, <=
	// ...
	
	return left;
}

Node *parser_parse_expression(Parser *parser) {
	Node *left = parser_parse_com(parser);

	/* and, or, ... */

	return left;
}

Node *parser_parse_factor(Parser *parser) {
	Node *left = parser_parse_power(parser);
	return left;
}

Node *parser_parse_identifier(Parser *parser) {
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

			Node *vardef_node = init_node();
			vardef_node->type = N_VAR_DEF;
			Node *var_node = parser_parse_expression(parser);
			vardef_node->var_node = var_node;
			strcpy(vardef_node->var_name, id_token.text);
			strcpy(vardef_node->var_type_str, data_type_token.text);
			if (strcmp(data_type_token.text, "str") == 0) {
				vardef_node->var_type = DT_STR;
			} else if (strcmp(data_type_token.text, "int") == 0) {
				vardef_node->var_type = DT_INT;
			} else if (strcmp(data_type_token.text, "float") == 0) {
				vardef_node->var_type = DT_FLOAT;
			}


			return vardef_node;
		} 
	} else if (next_token.tok_kind == T_LPAREN) {
		parser_consume(parser);
		/* Handle function call */

		Node **args = NULL;

		if (parser->current_token.tok_kind == T_RPAREN) {
			// handle call without args
			parser_consume(parser);
		} else {
			// handle call with args
			Node *arg = parser_parse_expression(parser);
			arrput(args, arg);
			while (parser->current_token.tok_kind == T_COMMA) {
				parser_consume(parser);
				Node *arg = parser_parse_expression(parser);
				arrput(args, arg);
			}
			if (parser->current_token.tok_kind != T_RPAREN) {
				printf("ERROR: \")\" expected.\n");
			}
			
			// consume R_PAREN
			parser_consume(parser);
		}

		Node *obj = init_node();
		obj->type = N_FUNC_CALL;
		obj->func_args = args;
		strcpy(obj->func_name, id_token.text);
		return obj;
	} else if (next_token.tok_kind == T_EQUAL) {
		/* Handle assignment */
	} else {
		/* Handle variable access */
		Node *obj = init_node();
		obj->type = N_VAR_ACCESS;
		strcpy(obj->var_name, id_token.text);
		return obj;
	}

	printf("ERROR: Invalid syntax.\n");
	printf("%s.\n", parser->current_token.text);
	exit(1);
}

Node *parser_parse_keyword(Parser *parser) {
	exit(1);	
}

Node *parser_parse_list(Parser *parser) {
	parser_consume(parser);
	Node *obj = init_node();
	Node **list_items = NULL;
	if (parser->current_token.tok_kind == T_RBRACKET) {
		parser_consume(parser);
	} else {
		Node *item = parser_parse_expression(parser);
		arrput(list_items, item);
		while (parser->current_token.tok_kind == T_COMMA) {
			parser_consume(parser);
			Node *item = parser_parse_expression(parser);
			arrput(list_items, item);
		}

		if (parser->current_token.tok_kind != T_RBRACKET) {
			printf("ERROR: ] expected\n");
			exit(1);
		}
		parser_consume(parser);
	}
	obj->list_items = list_items;
	return obj;
}


Node *parser_parse_power(Parser *parser) {
	Node *left = parser_parse_call(parser);
	return left;
}

Node *parser_parse_script(Parser *parser) {
	Node **statements = NULL;
	while (parser->current_token.tok_kind != T_EOF) {
		Node *stmt = parser_parse_statement(parser);
		arrput(statements, stmt);
	}

	Node *list_node = init_node();
	list_node->type = N_STATEMENTS;
	list_node->list_items = statements;
	return list_node;
}

Node *parser_parse_statement(Parser *parser) {
	return parser_parse_expression(parser);
}

Node *parser_parse_statements(Parser *parser){
}

Node *parser_parse_term(Parser *parser) {
	Node *left = parser_parse_factor(parser);

	while (parser->current_token.tok_kind == T_MULT) {
	}

	return left;
}

Node *parser_parse_value(Parser *parser) {
	exit(1);
}

Node *parser_parse_variable_def(Parser *parser,
		                    Token id_token,
		                    char *var_type_str, 
				    Token var_value_token,
				    int initialized) {
	Node *obj = init_node();
	obj->type = N_VAR_DEF;
	strcpy(obj->var_name, id_token.text);
	
	if (strcmp(var_type_str, "str") == 0) {
		obj->var_type = DT_STR;
		if (initialized) strcpy(obj->str_val, var_value_token.text);
		else strcpy(obj->str_val, "");
	} else if (strcmp(var_type_str, "int") == 0) {
		obj->var_type = DT_INT;
		if (initialized) obj->int_val = atoi(var_value_token.text);
		else obj->int_val = 0;
	} else if (strcmp(var_type_str, "float") == 0) {
		obj->var_type = DT_FLOAT;
		if (initialized) obj->float_val = atof(var_value_token.text);
		else obj->float_val = 0.0;
	}  else if (strcmp(var_type_str, "null") == 0) {
		obj->var_type = DT_NULL;
	} else if (strcmp(var_type_str, "list") == 0) {
		if (parser->current_token.tok_kind != T_LBRACKET) {
			printf("[ expected");
			exit(1);
		}
		parser_consume(parser);

		obj->var_type = DT_LIST;
		Node **list_items = NULL;
		if (parser->current_token.tok_kind == T_RBRACKET) {
			parser_consume(parser);
		} else {
			Node *item = parser_parse_expression(parser);
			arrput(list_items, item);
			while (parser->current_token.tok_kind == T_COMMA) {
				parser_consume(parser);
				Node *item = parser_parse_expression(parser);
				arrput(list_items, item);
			}

			if (parser->current_token.tok_kind != T_RBRACKET) {
				printf("] expected");
			}

			parser_consume(parser);
		}
		obj->list_items = list_items;
	} else {
		printf("ERROR: unknown data type: %s\n", var_type_str);
		exit(1);
	}
	return obj;
}

int carrot_get_args_len(Node *args) {
	return arrlen(args);
}

#define string_equals(a, b) (strcmp(a, b) == 0)
void carrot_type_check(tok_kind_t token_kind, char *data_type_str) {
	// check builtin type
	if (string_equals("str", data_type_str)) {
		if (token_kind != T_STR) {
			printf("ERROR: expected string value\n");
			exit(1);
		}
	} 
	if (string_equals("int", data_type_str)) {
		if (token_kind != T_INT) {
			printf("ERROR: expected int value\n");
			exit(1);
		}
	} 
	if (string_equals("float", data_type_str)) {
		if (token_kind != T_FLOAT) {
			printf("ERROR: expected float value\n");
			exit(1);
		}
	} 
}
